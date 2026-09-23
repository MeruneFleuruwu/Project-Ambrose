/*
 * Project Ambrose by Imjustchico
 * The gameserver saying it is alive. Every Realm.HeartbeatInterval seconds it writes its own realmlist row with the time and how many players it holds, so the login server reads a figure at most one beat old and a server that stopped beating falls out of the list without anyone marking it down. Stopping writes one last row that marks the realm offline rather than erasing when it last beat, because when a server was last alive stays true after it has gone and a player is kept away by the mark rather than by the loss of a fact. The beat is driven by the world tick rather than a timer of its own, because a server whose tick has stopped is not one a player should be sent to, and saying so by going quiet is the truth rather than a report of it. Which realm this is comes from the operator's Realm.Name, and a server that names no realm beats for nothing and says so once.
 */

#ifndef AMBROSE_REALMHEARTBEAT_H
#define AMBROSE_REALMHEARTBEAT_H

#include "Types.h"

#include <chrono>
#include <functional>
#include <string>

class ConfigMgr;

struct RealmHeartbeatSettings
{
    static constexpr uint32 DefaultIntervalSeconds = 30;

    std::string RealmName;
    uint32 IntervalSeconds = DefaultIntervalSeconds;

    static RealmHeartbeatSettings Load(ConfigMgr const& config);
};

class RealmHeartbeat
{
public:
    using Writer = std::function<void(std::string const& realm, uint32 population, int64 heartbeatEpoch, bool online)>;
    using Counter = std::function<uint32()>;

    RealmHeartbeat() = default;

    RealmHeartbeat(RealmHeartbeat const&) = delete;
    RealmHeartbeat& operator=(RealmHeartbeat const&) = delete;

    void Configure(RealmHeartbeatSettings settings, Writer writer, Counter counter);
    void Update(std::chrono::milliseconds diff);
    void BeatNow();
    void Stop();

    bool Beating() const noexcept { return _beating; }
    uint64 Beats() const noexcept { return _beats; }

private:
    RealmHeartbeatSettings _settings;
    Writer _writer;
    Counter _counter;
    std::chrono::milliseconds _since{ 0 };
    bool _beating = false;
    uint64 _beats = 0;
};

#endif
