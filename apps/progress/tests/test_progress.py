# Project Ambrose by Imjustchico
# Self-tests for the two Discord announcers: that editing a message replaces its card rather than adding another, since a message holds only ten attachments, that a message id is remembered and reused, that a message Discord no longer has is posted again, and that a refusal reports what Discord said rather than only its status code.
import io
import json
import os
import sys
import tempfile
import unittest
import urllib.error

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import announce


class FakePost:
    def __init__(self, answer=None, failure=None):
        self.calls = []
        self.answer = answer or json.dumps({"id": "1552", "attachments": [{"id": "9", "filename": "progress.png"}]})
        self.failure = failure

    def __call__(self, url, payload, attachment=None, method="POST"):
        self.calls.append({"url": url, "payload": payload, "attachment": attachment, "method": method})
        if self.failure is not None and len(self.calls) == 1:
            raise self.failure
        return self.answer, 200


class AnnouncerTests(unittest.TestCase):
    def setUp(self):
        self.original = announce.post
        self.payload = {"embeds": [{"title": "Progress update"}]}

    def tearDown(self):
        announce.post = self.original

    def test_editing_replaces_the_card_rather_than_adding_another(self):
        announce.post = FakePost()
        identifier, status, what, held = announce.send("https://discord.invalid/hook", self.payload, "progress.png", "1552")
        self.assertEqual((identifier, status, what, held), ("1552", 200, "edited", 1))
        call = announce.post.calls[0]
        self.assertEqual(call["method"], "PATCH")
        self.assertIn("/messages/1552", call["url"])
        self.assertEqual(call["payload"]["attachments"], [{"id": "0", "filename": announce.ATTACHMENT}])

    def test_editing_without_a_card_declares_no_attachments(self):
        announce.post = FakePost()
        announce.send("https://discord.invalid/hook", self.payload, None, "1552")
        self.assertNotIn("attachments", announce.post.calls[0]["payload"])

    def test_the_first_post_is_a_post_and_its_id_comes_back(self):
        announce.post = FakePost()
        identifier, _status, what, _held = announce.send("https://discord.invalid/hook", self.payload, "progress.png", None)
        self.assertEqual((identifier, what), ("1552", "posted"))
        self.assertEqual(announce.post.calls[0]["method"], "POST")
        self.assertNotIn("attachments", announce.post.calls[0]["payload"])

    def test_a_message_discord_no_longer_has_is_posted_again(self):
        gone = urllib.error.HTTPError("u", 404, "Not Found", None, None)
        announce.post = FakePost(failure=gone)
        identifier, _status, what, _held = announce.send("https://discord.invalid/hook", self.payload, None, "1552")
        self.assertEqual((identifier, what), ("1552", "posted"))
        self.assertEqual([call["method"] for call in announce.post.calls], ["PATCH", "POST"])

    def test_any_other_refusal_is_raised_rather_than_posting_a_second_message(self):
        refused = urllib.error.HTTPError("u", 400, "Bad Request", None, None)
        announce.post = FakePost(failure=refused)
        with self.assertRaises(urllib.error.HTTPError):
            announce.send("https://discord.invalid/hook", self.payload, "progress.png", "1552")

    def test_a_refusal_reports_what_discord_said(self):
        body = io.BytesIO(json.dumps({"message": "Maximum number of allowed attachments in a message reached (10).", "code": 30015}).encode("utf-8"))
        failure = urllib.error.HTTPError("u", 400, "Bad Request", None, body)
        said = announce.reason(failure)
        self.assertIn("30015", said)
        self.assertIn("attachments", said)

    def test_the_remembered_id_survives_a_round_trip_and_a_broken_file(self):
        folder = tempfile.mkdtemp()
        path = os.path.join(folder, "state.json")
        self.assertIsNone(announce.remembered(path))
        announce.remember(path, "1552")
        self.assertEqual(announce.remembered(path), "1552")
        with io.open(path, "w", encoding="utf-8", newline="\n") as handle:
            handle.write("{not json")
        self.assertIsNone(announce.remembered(path))


if __name__ == "__main__":
    unittest.main(verbosity=1)
