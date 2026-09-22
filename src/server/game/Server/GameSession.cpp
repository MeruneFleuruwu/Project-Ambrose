/*
 * Project Ambrose by Imjustchico
 * Draining is what this adds to a session, and counting what it cannot yet answer: the world calls DrainQueue on its own thread and the queued work runs there, bounded so one talkative client cannot hold the tick, and every later handler is written knowing it runs on that thread and nowhere else.
 */

#include "GameSession.h"
#include "Frame.h"
#include "Log.h"

#include <utility>

GameSession::GameSession(asio::ip::tcp::socket&& socket, FrameLimits limits, std::shared_ptr<SessionContext> context)
    : SessionBase(std::move(socket), limits, std::move(context))
{
}

std::size_t GameSession::DrainQueue(std::size_t limit)
{
    return ProcessQueuedMessages(limit);
}

void GameSession::OnMessage(DmlMessageData& message)
{
    _unhandled.fetch_add(1, std::memory_order_relaxed);
    LOG_DEBUG("server.gamesession", "Session {} sent service {} order {}, which the game server has no handler for yet",
        GetSessionId(), message.ServiceId, message.Order);
}
