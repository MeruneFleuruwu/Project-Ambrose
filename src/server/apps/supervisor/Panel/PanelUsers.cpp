/*
 * Project Ambrose by Imjustchico
 * Hashes with libsodium's Argon2id at its interactive cost, which is what a sign-in can afford to wait for, and keeps only the hash string it produces, which carries its own parameters so an older row still opens after the cost is raised; a sign-in that names nobody, or a disabled account, still spends one verify against a hash made at start, so an attacker cannot tell the three refusals apart by how long they took, and every refusal answers the same way.
 */

#include "PanelUsers.h"
#include "StringUtil.h"

#include <sodium.h>

#include <fmt/format.h>

#include <mutex>
#include <utility>

namespace
{
    std::once_flag SodiumReady;
    std::string DecoyHash;

    void StartSodium()
    {
        std::call_once(SodiumReady, []
        {
            if (sodium_init() < 0)
                return;
            char hash[crypto_pwhash_STRBYTES] = {};
            constexpr char const* Nothing = "there is no such user";
            if (crypto_pwhash_str(hash, Nothing, std::char_traits<char>::length(Nothing),
                    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) == 0)
                DecoyHash.assign(hash);
        });
    }
}

PanelUserResult PanelPasswordPolicy::Check(std::string_view username, std::string_view password) const
{
    switch (Ambrose::AccountText::CheckPassword(password, std::max(MinLength, FloorLength)))
    {
        case Ambrose::AccountText::TextProblem::TooLong: return PanelUserResult::PasswordTooLong;
        case Ambrose::AccountText::TextProblem::Invalid: return PanelUserResult::PasswordInvalid;
        case Ambrose::AccountText::TextProblem::TooShort: return PanelUserResult::PasswordTooShort;
        case Ambrose::AccountText::TextProblem::Ok: break;
    }
    if (!username.empty() && Ambrose::EqualsIgnoreCase(username, password))
        return PanelUserResult::PasswordIsTheName;
    return PanelUserResult::Ok;
}

PanelUsers::PanelUsers(PanelStore& store) : _store(store)
{
    StartSodium();
}

void PanelUsers::SetPolicy(PanelPasswordPolicy policy)
{
    policy.MinLength = std::max(policy.MinLength, PanelPasswordPolicy::FloorLength);
    _policy = policy;
}

std::string PanelUsers::Fold(std::string_view username)
{
    return Ambrose::ToLower(username);
}

std::string_view PanelUsers::Explain(PanelUserResult result) noexcept
{
    switch (result)
    {
        case PanelUserResult::Ok: return "that is fine";
        case PanelUserResult::NameTooShort: return "the name is shorter than the panel takes";
        case PanelUserResult::NameTooLong: return "the name is longer than the panel takes";
        case PanelUserResult::NameInvalid: return "a name holds letters, digits, underscore, dash and dot, and nothing else";
        case PanelUserResult::NameTaken: return "another panel user already has that name";
        case PanelUserResult::PasswordTooShort: return "the password is shorter than the policy asks for";
        case PanelUserResult::PasswordTooLong: return "the password is longer than the panel takes";
        case PanelUserResult::PasswordInvalid: return "a password is text with no control characters in it";
        case PanelUserResult::PasswordIsTheName: return "the password cannot be the name";
        case PanelUserResult::UnknownUser:
        case PanelUserResult::Disabled:
        case PanelUserResult::WrongPassword: return "that name and password do not sign in";
        case PanelUserResult::HashFailed: return "the password could not be hashed";
        case PanelUserResult::StoreFailed: break;
    }
    return "the panel store refused the change";
}

bool PanelUsers::HashPassword(std::string_view password, std::string& hash, std::string& error)
{
    StartSodium();
    char made[crypto_pwhash_STRBYTES] = {};
    if (crypto_pwhash_str(made, password.data(), password.size(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        error = "the password could not be hashed; the machine may be out of memory";
        return false;
    }
    hash.assign(made);
    return true;
}

bool PanelUsers::PasswordMatches(std::string const& hash, std::string_view password)
{
    StartSodium();
    if (hash.empty() || hash.size() >= crypto_pwhash_STRBYTES)
        return false;
    return crypto_pwhash_str_verify(hash.c_str(), password.data(), password.size()) == 0;
}

PanelUser PanelUsers::Read(PanelStore::Statement const& row)
{
    PanelUser user;
    user.Id = row.Int64(0);
    user.Username = row.Text(1);
    user.DisplayName = row.Text(2);
    user.Email = row.Text(3);
    user.Generation = row.Int64(4);
    user.Disabled = row.Int64(5) != 0;
    user.MustChange = row.Int64(6) != 0;
    user.IsOwner = row.Int64(7) != 0;
    user.CreatedEpochMs = row.Int64(8);
    user.PasswordSetEpochMs = row.Int64(9);
    if (!row.IsNull(10))
        user.SignedInEpochMs = row.Int64(10);
    return user;
}

namespace
{
    constexpr std::string_view UserColumns =
        "id, username, COALESCE(display_name, ''), COALESCE(email, ''), generation, disabled, must_change, is_owner, created_epoch_ms, password_set_epoch_ms, signed_in_epoch_ms";
}

bool PanelUsers::IsEmpty(std::string& error)
{
    std::optional<PanelStore::Statement> rows = _store.Prepare("SELECT COUNT(*) FROM panel_user", error);
    if (!rows || !rows->Step(error))
        return false;
    return rows->Int64(0) == 0;
}

std::optional<PanelUser> PanelUsers::Find(std::string_view username, std::string& error)
{
    std::optional<PanelStore::Statement> rows = _store.Prepare(fmt::format("SELECT {} FROM panel_user WHERE username_folded = ?", UserColumns), error);
    if (!rows)
        return std::nullopt;
    std::string const folded = Fold(username);
    rows->Bind(1, folded);
    if (!rows->Step(error))
        return std::nullopt;
    return Read(*rows);
}

std::optional<PanelUser> PanelUsers::FindById(int64 id, std::string& error)
{
    std::optional<PanelStore::Statement> rows = _store.Prepare(fmt::format("SELECT {} FROM panel_user WHERE id = ?", UserColumns), error);
    if (!rows)
        return std::nullopt;
    rows->Bind(1, id);
    if (!rows->Step(error))
        return std::nullopt;
    return Read(*rows);
}

std::vector<PanelUser> PanelUsers::List(std::string& error)
{
    std::vector<PanelUser> users;
    std::optional<PanelStore::Statement> rows = _store.Prepare(fmt::format("SELECT {} FROM panel_user ORDER BY username_folded", UserColumns), error);
    if (!rows)
        return users;
    while (rows->Step(error))
        users.push_back(Read(*rows));
    return users;
}

PanelUserResult PanelUsers::Create(std::string_view username, std::string_view password, bool owner, bool mustChange, int64* id, std::string& error)
{
    switch (Ambrose::AccountText::CheckUsername(username, Ambrose::AccountText::DefaultUsernameMinLength))
    {
        case Ambrose::AccountText::TextProblem::TooLong: return PanelUserResult::NameTooLong;
        case Ambrose::AccountText::TextProblem::Invalid: return PanelUserResult::NameInvalid;
        case Ambrose::AccountText::TextProblem::TooShort: return PanelUserResult::NameTooShort;
        case Ambrose::AccountText::TextProblem::Ok: break;
    }
    if (PanelUserResult const problem = _policy.Check(username, password); problem != PanelUserResult::Ok)
        return problem;
    if (std::optional<PanelUser> const taken = Find(username, error); taken)
        return PanelUserResult::NameTaken;
    if (!error.empty())
        return PanelUserResult::StoreFailed;

    std::string hash;
    if (!HashPassword(password, hash, error))
        return PanelUserResult::HashFailed;

    std::optional<PanelStore::Statement> insert = _store.Prepare(
        "INSERT INTO panel_user (username, username_folded, password_hash, password_set_epoch_ms, must_change, is_owner, created_epoch_ms) VALUES (?, ?, ?, ?, ?, ?, ?)", error);
    if (!insert)
        return PanelUserResult::StoreFailed;
    int64 const now = PanelStore::NowEpochMs();
    insert->Bind(1, username);
    insert->Bind(2, Fold(username));
    insert->Bind(3, hash);
    insert->Bind(4, now);
    insert->Bind(5, mustChange ? int64{ 1 } : int64{ 0 });
    insert->Bind(6, owner ? int64{ 1 } : int64{ 0 });
    insert->Bind(7, now);
    if (!insert->Run(error))
        return PanelUserResult::StoreFailed;
    if (id)
        *id = _store.LastInsertId();
    return PanelUserResult::Ok;
}

PanelUserResult PanelUsers::Authenticate(std::string_view username, std::string_view password, PanelUser& user, std::string& error)
{
    std::optional<PanelUser> const found = Find(username, error);
    if (!found)
    {
        if (!error.empty())
            return PanelUserResult::StoreFailed;
        PasswordMatches(DecoyHash, password);
        return PanelUserResult::UnknownUser;
    }

    std::optional<PanelStore::Statement> rows = _store.Prepare("SELECT password_hash FROM panel_user WHERE id = ?", error);
    if (!rows)
        return PanelUserResult::StoreFailed;
    rows->Bind(1, found->Id);
    if (!rows->Step(error))
        return PanelUserResult::StoreFailed;
    std::string const hash = rows->Text(0);
    rows.reset();

    bool const matches = PasswordMatches(hash, password);
    if (found->Disabled)
        return PanelUserResult::Disabled;
    if (!matches)
        return PanelUserResult::WrongPassword;
    user = *found;
    return PanelUserResult::Ok;
}

PanelUserResult PanelUsers::SetPassword(int64 id, std::string_view password, bool mustChange, std::string& error)
{
    std::optional<PanelUser> const found = FindById(id, error);
    if (!found)
        return error.empty() ? PanelUserResult::UnknownUser : PanelUserResult::StoreFailed;
    if (PanelUserResult const problem = _policy.Check(found->Username, password); problem != PanelUserResult::Ok)
        return problem;

    std::string hash;
    if (!HashPassword(password, hash, error))
        return PanelUserResult::HashFailed;

    std::optional<PanelStore::Statement> update = _store.Prepare(
        "UPDATE panel_user SET password_hash = ?, password_set_epoch_ms = ?, must_change = ?, generation = generation + 1 WHERE id = ?", error);
    if (!update)
        return PanelUserResult::StoreFailed;
    update->Bind(1, hash);
    update->Bind(2, PanelStore::NowEpochMs());
    update->Bind(3, mustChange ? int64{ 1 } : int64{ 0 });
    update->Bind(4, id);
    if (!update->Run(error))
        return PanelUserResult::StoreFailed;
    return PanelUserResult::Ok;
}

bool PanelUsers::SetDisabled(int64 id, bool disabled, std::string& error)
{
    std::optional<PanelStore::Statement> update = _store.Prepare("UPDATE panel_user SET disabled = ?, generation = generation + 1 WHERE id = ?", error);
    if (!update)
        return false;
    update->Bind(1, disabled ? int64{ 1 } : int64{ 0 });
    update->Bind(2, id);
    if (!update->Run(error))
        return false;
    return _store.Changed() == 1;
}

bool PanelUsers::RecordSignIn(int64 id, std::string& error)
{
    std::optional<PanelStore::Statement> update = _store.Prepare("UPDATE panel_user SET signed_in_epoch_ms = ? WHERE id = ?", error);
    if (!update)
        return false;
    update->Bind(1, PanelStore::NowEpochMs());
    update->Bind(2, id);
    return update->Run(error);
}
