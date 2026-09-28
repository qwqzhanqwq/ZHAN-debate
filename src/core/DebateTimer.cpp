#include "DebateTimer.h"

#include <algorithm>
#include <cstdio>
#include <utility>

namespace debate
{

DebateTimer::DebateTimer(Rules rules)
    : rules_(std::move(rules))
{
    // 保证至少有一个结束阶段，避免越界 // Always keep an End stage so indices stay valid
    if (rules_.stages.empty() || rules_.stages.back().kind != StageKind::End)
        rules_.stages.push_back({StageKind::End, Side::Neutral, "比赛结束", "End", "", 0});
    enterStage(0);
}

void DebateTimer::start()
{
    if (isOver() || state_ == RunState::Finished)
        return;
    state_ = RunState::Running;
}

void DebateTimer::pause()
{
    if (state_ == RunState::Running)
        state_ = RunState::Paused;
}

void DebateTimer::toggle()
{
    switch (state_)
    {
    case RunState::Idle:
    case RunState::Paused:
        start();
        break;
    case RunState::Running:
        pause();
        break;
    case RunState::Finished:
        nextStage();
        break;
    }
}

void DebateTimer::tick(std::int64_t elapsedMs)
{
    if (state_ != RunState::Running || elapsedMs <= 0)
        return;

    if (!isFreeDebate())
    {
        remainingMs_ = std::max<std::int64_t>(0, remainingMs_ - elapsedMs);
        if (remainingMs_ == 0)
            state_ = RunState::Finished;
        return;
    }

    // 自由辩论：一次 tick 可能跨过换人时刻，剩余的时间继续记到下一方
    // Free debate: one tick may cross a turn change; the leftover time goes to the next speaker
    while (elapsedMs > 0 && state_ == RunState::Running)
    {
        std::int64_t& own = sideMs(active_);
        std::int64_t step = std::min({elapsedMs, own, turnMs_});
        own -= step;
        turnMs_ -= step;
        elapsedMs -= step;
        if (own <= 0 || turnMs_ <= 0)
            endTurn();
    }
}

void DebateTimer::nextStage()
{
    if (index_ + 1 < stageCount())
        enterStage(index_ + 1);
}

void DebateTimer::previousStage()
{
    if (index_ > 0)
        enterStage(index_ - 1);
}

void DebateTimer::goToStage(int index)
{
    if (index >= 0 && index < stageCount())
        enterStage(index);
}

void DebateTimer::resetStage()
{
    enterStage(index_);
}

void DebateTimer::resetAll()
{
    enterStage(0);
}

bool DebateTimer::switchSide()
{
    if (!canSwitchSide())
        return false;
    beginTurn(opposite(active_));
    return true;
}

bool DebateTimer::canSwitchSide() const
{
    return isFreeDebate() && state_ != RunState::Finished && sideRemainingMs(opposite(active_)) > 0;
}

std::int64_t DebateTimer::remainingMs() const
{
    if (isFreeDebate())
        return sideRemainingMs(active_);
    return remainingMs_;
}

double DebateTimer::progress() const
{
    std::int64_t total = durationMs();
    if (total <= 0)
        return 1.0;
    if (isFreeDebate())
        return 1.0 - static_cast<double>(proMs_ + conMs_) / (2.0 * total);
    return 1.0 - static_cast<double>(remainingMs_) / total;
}

bool DebateTimer::isWarning() const
{
    std::int64_t remaining = remainingMs();
    return !isOver() && remaining > 0 && remaining <= rules_.warningSec * 1000LL;
}

std::int64_t DebateTimer::sideRemainingMs(Side side) const
{
    return side == Side::Con ? conMs_ : proMs_;
}

void DebateTimer::enterStage(int index)
{
    index_ = index;
    state_ = RunState::Idle;
    remainingMs_ = durationMs();
    proMs_ = conMs_ = 0;
    turnMs_ = 0;
    turnLimited_ = false;
    active_ = Side::Pro;

    if (isFreeDebate())
    {
        // 双方各自拥有完整时长，默认正方先发言 // Each side gets the full duration; Pro speaks first
        proMs_ = conMs_ = durationMs();
        beginTurn(Side::Pro);
    }
}

void DebateTimer::beginTurn(Side side)
{
    active_ = side;
    std::int64_t own = sideMs(side);
    std::int64_t limit = rules_.freeDebateTurnLimitSec * 1000LL;
    // 对方已用完时不再轮换，本方可一次用完剩余时间
    // Once the other side is out of time there is no more alternation: this side may use all it has left
    turnLimited_ = limit > 0 && sideMs(opposite(side)) > 0;
    turnMs_ = turnLimited_ ? std::min(limit, own) : own;
}

void DebateTimer::endTurn()
{
    Side other = opposite(active_);
    if (sideMs(other) > 0)
        beginTurn(other);
    else if (sideMs(active_) > 0)
        beginTurn(active_);
    else
        state_ = RunState::Finished;  // 双方都用完 // Both sides are out of time
}

Side opposite(Side side)
{
    return side == Side::Pro ? Side::Con : Side::Pro;
}

std::string formatClock(std::int64_t ms)
{
    std::int64_t totalSec = ms > 0 ? (ms + 999) / 1000 : 0;
    char buffer[48];
    std::snprintf(buffer, sizeof(buffer), "%02lld:%02lld",
        static_cast<long long>(totalSec / 60), static_cast<long long>(totalSec % 60));
    return buffer;
}

}  // namespace debate
