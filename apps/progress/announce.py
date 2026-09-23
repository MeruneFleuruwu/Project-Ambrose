# Project Ambrose by Imjustchico
# Keeps one Discord message up to date with the generated progress, replacing its card on every edit rather than adding one, because a message holds only ten attachments: it edits the message it posted last time, remembering its id in a state file, and posts a new one only when there is none or Discord says the old one is gone, as one embed carrying a rendered card and the headline percentage, the milestone and check counts, the contributor totals and the phases that moved since the previous commit, saying what it would post and exiting zero when no webhook is configured.

import argparse
import json
import os
import subprocess
import sys
import mimetypes
import secrets
import urllib.error
import urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DATA = "doc/progress/progress.json"
CARD_URL = "https://raw.githubusercontent.com/Justchicoo/Project-Ambrose/main/doc/progress/progress.svg"
REPOSITORY_URL = "https://github.com/Justchicoo/Project-Ambrose"
BOARD_URL = "https://justchicoo.github.io/Project-Ambrose/"
ROADMAP_URL = REPOSITORY_URL + "/blob/main/doc/ROADMAP.md"
GOLD = 0xE4B457
ATTACHMENT = "progress.png"
NEWLINE = chr(10)


def load(root):
    with open(os.path.join(root, DATA), "r", encoding="utf-8") as handle:
        return json.load(handle)


def previous(root):
    try:
        out = subprocess.run(["git", "-C", root, "show", f"HEAD~1:{DATA}"], capture_output=True, text=True, check=True)
        return json.loads(out.stdout)
    except (OSError, subprocess.CalledProcessError, json.JSONDecodeError):
        return None


def moved(now, before):
    if before is None:
        return []
    was = {phase["phase"]: phase["milestones_done"] for phase in before.get("phases", [])}
    lines = []
    for phase in now["phases"]:
        gained = phase["milestones_done"] - was.get(phase["phase"], 0)
        if gained > 0:
            lines.append(f'Phase {phase["phase"]:02d} {phase["title"]}: {gained} more, now {phase["milestones_done"]} of {phase["milestones_total"]}')
    return lines


def stamp():
    try:
        out = subprocess.run(["git", "log", "-1", "--format=%cs %h"], capture_output=True, text=True, check=True)
        date, commit = out.stdout.strip().split()
        return f"{date} · {commit}"
    except (OSError, subprocess.CalledProcessError, ValueError):
        return "counted from the roadmap itself"


def embed(now, before):
    milestones = now["milestones"]
    checks = now["checks"]
    track = now["contributor_track"]
    gained = milestones["done"] - before["milestones"]["done"] if before else 0
    lines = moved(now, before)

    description = [f'**{milestones["percent"]}% of the plan built** · {milestones["done"]} of {milestones["total"]} milestones · '
                   f'{checks["done"]} of {checks["total"]} acceptance checks']
    if gained > 0:
        description.append(f'{gained} milestone{"s" if gained > 1 else ""} finished since the last update.')
    if lines:
        description.append(NEWLINE.join("· " + line for line in lines[:4]))
    description.append(f'[What is free to take]({BOARD_URL}) · [The plan]({ROADMAP_URL}) · '
                       f'{track["merged"]} contributor items merged, {track["open"]} open')

    return {
        "username": "Project Ambrose",
        "embeds": [{
            "title": "Project Ambrose",
            "url": BOARD_URL,
            "description": (NEWLINE + NEWLINE).join(description),
            "color": GOLD,
            "image": {"url": "attachment://" + ATTACHMENT},
            "footer": {"text": stamp()},
        }],
    }


def reason(failure):
    try:
        body = failure.read().decode("utf-8", "replace").strip()
    except (OSError, AttributeError):
        body = ""
    return f"{failure} {body[:600]}" if body else str(failure)


def post(url, payload, attachment=None, method="POST"):
    if attachment is None:
        request = urllib.request.Request(url, data=json.dumps(payload).encode("utf-8"), method=method,
                                         headers={"Content-Type": "application/json", "User-Agent": "Project-Ambrose"})
    else:
        boundary = "ambrose" + secrets.token_hex(12)
        kind = mimetypes.guess_type(attachment)[0] or "application/octet-stream"
        with open(attachment, "rb") as handle:
            blob = handle.read()
        line = "\r\n"
        head = (f"--{boundary}{line}"
                f'Content-Disposition: form-data; name="payload_json"{line}'
                f"Content-Type: application/json{line}{line}")
        middle = (f"{line}--{boundary}{line}"
                  f'Content-Disposition: form-data; name="files[0]"; filename="{ATTACHMENT}"{line}'
                  f"Content-Type: {kind}{line}{line}")
        tail = f"{line}--{boundary}--{line}"
        parts = [head.encode("utf-8"), json.dumps(payload).encode("utf-8"), middle.encode("utf-8"), blob, tail.encode("utf-8")]
        request = urllib.request.Request(url, data=b"".join(parts), method=method,
                                         headers={"Content-Type": f"multipart/form-data; boundary={boundary}", "User-Agent": "Project-Ambrose"})
    with urllib.request.urlopen(request, timeout=30) as answer:
        return answer.read().decode("utf-8", "replace"), answer.status


def remembered(path):
    if not path or not os.path.exists(path):
        return None
    try:
        with open(path, "r", encoding="utf-8") as handle:
            return json.load(handle).get("message_id")
    except (OSError, json.JSONDecodeError):
        return None


def remember(path, message_id):
    if not path:
        return
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as handle:
        json.dump({"message_id": message_id}, handle, indent=2)
        handle.write("\n")


def carried(body):
    try:
        return len(json.loads(body).get("attachments", []))
    except (json.JSONDecodeError, AttributeError, TypeError):
        return 0


def send(url, payload, attachment, message_id):
    if message_id:
        edited = dict(payload)
        if attachment is not None:
            edited["attachments"] = [{"id": "0", "filename": ATTACHMENT}]
        try:
            body, status = post(f"{url}/messages/{message_id}?wait=true", edited, attachment, method="PATCH")
            return message_id, status, "edited", carried(body)
        except urllib.error.HTTPError as failure:
            if failure.code not in (404, 401, 403):
                raise
    body, status = post(f"{url}?wait=true", payload, attachment)
    fresh = None
    try:
        fresh = json.loads(body).get("id")
    except (json.JSONDecodeError, AttributeError):
        pass
    return fresh, status, "posted", carried(body)


def main(argv=None):
    parser = argparse.ArgumentParser(description="Project Ambrose progress announcer")
    parser.add_argument("--root", default=ROOT)
    parser.add_argument("--webhook-env", default="DISCORD_PROGRESS_WEBHOOK", help="the environment variable holding the Discord webhook URL")
    parser.add_argument("--state", default="doc/progress/discord-message.json", help="where the id of the message to keep updating is remembered")
    parser.add_argument("--attach", help="a rendered PNG of the card to post with the embed")
    parser.add_argument("--dry-run", action="store_true", help="print the payload instead of posting it")
    args = parser.parse_args(argv)
    root = os.path.abspath(args.root)

    now = load(root)
    payload = embed(now, previous(root))
    url = os.environ.get(args.webhook_env, "").strip()
    if args.dry_run or not url:
        print(json.dumps(payload, indent=2))
        print("no webhook configured, nothing posted" if not url else "dry run, nothing posted")
        return 0
    if not url.startswith("https://discord.com/api/webhooks/") and not url.startswith("https://discordapp.com/api/webhooks/"):
        print("the webhook must be a Discord webhook URL", file=sys.stderr)
        return 1
    try:
        attachment = args.attach if args.attach and os.path.exists(args.attach) else None
        if args.attach and attachment is None:
            print(f"{args.attach}: not found, posting without the card", file=sys.stderr)
            payload["embeds"][0].pop("image", None)
        message_id, status, what, carried = send(url, payload, attachment, remembered(args.state))
    except urllib.error.HTTPError as failure:
        print(f"the webhook refused the post: {reason(failure)}", file=sys.stderr)
        return 1
    except (urllib.error.URLError, OSError) as failure:
        print(f"the webhook refused the post: {failure}", file=sys.stderr)
        return 1
    if message_id:
        remember(args.state, message_id)
    print(f"{what} message {message_id}, Discord answered {status}, the message now carries {carried} attachment(s)")
    if attachment is not None and carried != 1:
        print(f"the message should carry exactly one card and carries {carried}: more than one means an edit added rather than "
              "replaced, and none means the edit dropped the card it was uploading", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
