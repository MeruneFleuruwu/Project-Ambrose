/*
 * Project Ambrose by Imjustchico
 * One connected game client, on the network side a session like the login server's and on the game side the thing the world owns: what arrives is queued by the network thread that read it and run later by the world thread that owns the game state, so a handler never touches the world from two threads at once, and the account and character it belongs to are carried here for every later system to read. No message has a handler yet, so one that arrives is counted and reported rather than acted on; milestone 4.05 gives the game server its own handler table and this starts dispatching.
 */

#ifndef AMBROSE_GAMESESSION_H
#define AMBROSE_GAMESESSION_H

#include "SessionBase.h"

#include <atomic>
#include <memory>
#include <string>

class GameSession : public SessionBase
{
public:
    GameSession(asio::ip::tcp::socket&& socket, FrameLimits limits, std::shared_ptr<SessionContext> context);

    uint64 GetAccountId() const noexcept { return _accountId.load(std::memory_order_relaxed); }
    void SetAccountId(uint64 accountId) noexcept { _accountId.store(accountId, std::memory_order_relaxed); }

    uint64 GetCharacterId() const noexcept { return _characterId.load(std::memory_order_relaxed); }
    void SetCharacterId(uint64 characterId) noexcept { _characterId.store(characterId, std::memory_order_relaxed); }

    std::size_t DrainQueue(std::size_t limit = MaxQueuedMessages);

    uint64 GetUnhandledMessageCount() const noexcept { return _unhandled.load(std::memory_order_relaxed); }

protected:
    void OnMessage(DmlMessageData& message) override;

private:
    std::atomic<uint64> _accountId{ 0 };
    std::atomic<uint64> _characterId{ 0 };
    std::atomic<uint64> _unhandled{ 0 };
};

#endif
