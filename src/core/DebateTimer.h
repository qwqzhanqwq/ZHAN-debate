#pragma once

#include "DebateRules.h"

#include <cstdint>
#include <string>

namespace debate
{

// 计时状态 // Run state
enum class RunState
{
    Idle,     // 本阶段尚未开始 // Stage not started yet
    Running,  // 计时中 // Counting down
    Paused,   // 已暂停 // Paused
    Finished  // 本阶段时间到 // Stage time is up
};

// 辩论赛计时器状态机，不依赖任何界面库，时间由外部通过 tick() 推进
// Debate timer state machine. It has no UI dependency; time is advanced from outside via tick()
class DebateTimer
{
public:
    explicit DebateTimer(Rules rules = Rules::standard());

    // --- 操作 // Actions ---
    void start();
    void pause();
    // 空格键：未开始/暂停 → 计时；计时 → 暂停；时间到 → 进入下一阶段（不自动开始）
    // Space: idle/paused → run; running → pause; time up → go to next stage (not started)
    void toggle();
    // 推进时间，仅在计时状态下生效 // Advance time; only has effect while running
    void tick(std::int64_t elapsedMs);

    void nextStage();
    void previousStage();
    void goToStage(int index);
    void resetStage();
    void resetAll();
    // 自由辩论交换发言方；对方已无剩余时间时返回 false
    // Pass the turn in free debate; returns false if the other side has no time left
    bool switchSide();

    // --- 查询 // Queries ---
    const Rules& rules() const { return rules_; }
    const Stage& stage() const { return rules_.stages[index_]; }
    int stageIndex() const { return index_; }
    int stageCount() const { return static_cast<int>(rules_.stages.size()); }
    RunState state() const { return state_; }
    bool isFreeDebate() const { return stage().kind == StageKind::FreeDebate; }
    bool isOver() const { return stage().kind == StageKind::End; }

    // 主倒计时：普通阶段为本阶段剩余，自由辩论为当前发言方剩余
    // Main countdown: stage remaining for speeches, active side remaining in free debate
    std::int64_t remainingMs() const;
    std::int64_t durationMs() const { return stage().durationSec * 1000LL; }
    // 本阶段进度 0~1，自由辩论按双方合计 // Stage progress 0..1; free debate counts both sides
    double progress() const;
    // 剩余时间进入警示区间 // Remaining time is within the warning window
    bool isWarning() const;

    // --- 自由辩论 // Free debate ---
    Side activeSide() const { return active_; }
    std::int64_t sideRemainingMs(Side side) const;
    // 本轮剩余：有单次上限时为本轮剩余，否则等于当前方剩余
    // Time left in this turn: capped turn if a limit applies, otherwise the active side's remaining time
    std::int64_t turnRemainingMs() const { return turnMs_; }
    bool turnLimited() const { return turnLimited_; }
    bool canSwitchSide() const;

private:
    void enterStage(int index);
    void beginTurn(Side side);
    void endTurn();
    std::int64_t& sideMs(Side side) { return side == Side::Con ? conMs_ : proMs_; }

    Rules rules_;
    int index_ = 0;
    RunState state_ = RunState::Idle;
    std::int64_t remainingMs_ = 0;  // 普通阶段剩余 // Remaining time of a speech stage
    std::int64_t proMs_ = 0;        // 自由辩论正方剩余 // Pro side remaining in free debate
    std::int64_t conMs_ = 0;        // 自由辩论反方剩余 // Con side remaining in free debate
    std::int64_t turnMs_ = 0;       // 本轮剩余 // Remaining time of the current turn
    bool turnLimited_ = false;
    Side active_ = Side::Pro;
};

// 对方 // The opposing side
Side opposite(Side side);

// 格式化为 MM:SS，不足一秒向上取整，保证倒计时在归零前不会显示 00:00
// Format as MM:SS, rounding partial seconds up so the countdown never shows 00:00 before it ends
std::string formatClock(std::int64_t ms);

}  // namespace debate
