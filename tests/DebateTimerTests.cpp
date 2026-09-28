// 计时核心的单元测试，无第三方依赖 // Unit tests for the timer core, no third-party dependencies
#include "DebateTimer.h"

#include <cstdio>
#include <functional>
#include <string>
#include <vector>

using namespace debate;

namespace
{

int failures = 0;

#define CHECK(expr)                                                              \
    do                                                                           \
    {                                                                            \
        if (!(expr))                                                             \
        {                                                                        \
            std::printf("  FAILED %s:%d: %s\n", __FILE__, __LINE__, #expr);      \
            ++failures;                                                          \
        }                                                                        \
    } while (0)

#define CHECK_EQ(a, b)                                                           \
    do                                                                           \
    {                                                                            \
        auto va = (a);                                                           \
        auto vb = (b);                                                           \
        if (!(va == vb))                                                         \
        {                                                                        \
            std::printf("  FAILED %s:%d: %s == %s (%lld vs %lld)\n", __FILE__,  \
                __LINE__, #a, #b, static_cast<long long>(va),                    \
                static_cast<long long>(vb));                                     \
            ++failures;                                                          \
        }                                                                        \
    } while (0)

constexpr int kFreeStage = 7;

// 走到自由辩论阶段 // Jump to the free debate stage
DebateTimer freeDebate(int turnLimitSec = 60)
{
    Rules rules = Rules::standard();
    rules.freeDebateTurnLimitSec = turnLimitSec;
    DebateTimer timer(rules);
    timer.goToStage(kFreeStage);
    return timer;
}

// 当前方说 seconds 秒 // Let the active side speak for the given seconds
void speak(DebateTimer& timer, int seconds)
{
    for (int i = 0; i < seconds; ++i)
        timer.tick(1000);
}

// 一秒一秒推进，直到本阶段结束，返回用时 // Tick second by second until the stage ends; returns seconds used
int runUntilFinished(DebateTimer& timer)
{
    int seconds = 0;
    while (timer.state() == RunState::Running && seconds < 100000)
    {
        timer.tick(1000);
        ++seconds;
    }
    return seconds;
}

void standardRules()
{
    DebateTimer timer;
    CHECK_EQ(timer.stageCount(), 11);
    CHECK(timer.stage().kind == StageKind::Speech);
    CHECK_EQ(timer.durationMs(), 120000);
    CHECK(timer.rules().stages[kFreeStage].kind == StageKind::FreeDebate);
    CHECK_EQ(timer.rules().stages[kFreeStage].durationSec, 300);
    CHECK(timer.rules().stages.back().kind == StageKind::End);
}

void formatting()
{
    CHECK(formatClock(180000) == "03:00");
    CHECK(formatClock(179001) == "03:00");
    CHECK(formatClock(179000) == "02:59");
    CHECK(formatClock(1) == "00:01");
    CHECK(formatClock(0) == "00:00");
    CHECK(formatClock(-500) == "00:00");
    CHECK(formatClock(600000) == "10:00");
}

void speechCountdown()
{
    DebateTimer timer;
    CHECK(timer.state() == RunState::Idle);

    // 未开始时不计时 // Nothing counts before start
    timer.tick(5000);
    CHECK_EQ(timer.remainingMs(), 120000);

    timer.toggle();
    CHECK(timer.state() == RunState::Running);
    timer.tick(1500);
    CHECK_EQ(timer.remainingMs(), 118500);

    // 暂停后不计时 // Nothing counts while paused
    timer.toggle();
    CHECK(timer.state() == RunState::Paused);
    timer.tick(10000);
    CHECK_EQ(timer.remainingMs(), 118500);

    // 超出剩余时间：停在 0 并进入时间到 // Overshooting stops at 0 and enters Finished
    timer.start();
    timer.tick(200000);
    CHECK_EQ(timer.remainingMs(), 0);
    CHECK(timer.state() == RunState::Finished);
    CHECK(timer.progress() == 1.0);

    // 时间到后不能再开始；空格进入下一阶段但不自动计时
    // Cannot restart after time up; Space moves to the next stage without starting it
    timer.start();
    CHECK(timer.state() == RunState::Finished);
    timer.toggle();
    CHECK_EQ(timer.stageIndex(), 1);
    CHECK(timer.state() == RunState::Idle);
    CHECK_EQ(timer.remainingMs(), 180000);
}

void progressAndWarning()
{
    DebateTimer timer;
    timer.start();
    timer.tick(60000);
    CHECK(timer.progress() == 0.5);

    timer.tick(29000);  // 剩 31 秒 // 31 s left
    CHECK(!timer.isWarning());
    timer.tick(1000);   // 剩 30 秒 // 30 s left
    CHECK(timer.isWarning());
    timer.tick(30000);  // 归零 // Zero
    CHECK(!timer.isWarning());
}

void navigation()
{
    DebateTimer timer;
    timer.previousStage();
    CHECK_EQ(timer.stageIndex(), 0);

    timer.start();
    timer.tick(3000);
    timer.nextStage();
    CHECK_EQ(timer.stageIndex(), 1);
    CHECK(timer.state() == RunState::Idle);
    CHECK_EQ(timer.remainingMs(), 180000);

    timer.goToStage(-1);
    timer.goToStage(99);
    CHECK_EQ(timer.stageIndex(), 1);

    timer.start();
    timer.tick(10000);
    timer.resetStage();
    CHECK(timer.state() == RunState::Idle);
    CHECK_EQ(timer.remainingMs(), 180000);

    timer.goToStage(timer.stageCount() - 1);
    CHECK(timer.isOver());
    timer.nextStage();
    CHECK_EQ(timer.stageIndex(), timer.stageCount() - 1);
    timer.toggle();
    CHECK(timer.state() == RunState::Idle);
    CHECK(timer.progress() == 1.0);

    timer.resetAll();
    CHECK_EQ(timer.stageIndex(), 0);
    CHECK(timer.state() == RunState::Idle);
}

void freeDebateDefaultFlow()
{
    // 全程不交换：每轮 60 秒自动换人，双方 300 秒都应用完
    // No manual switching: turns alternate every 60 s and both sides use all 300 s
    DebateTimer timer = freeDebate();
    CHECK(timer.activeSide() == Side::Pro);
    CHECK(timer.turnLimited());
    CHECK_EQ(timer.turnRemainingMs(), 60000);

    timer.start();
    speak(timer, 60);
    CHECK(timer.activeSide() == Side::Con);
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 240000);

    int used = 60 + runUntilFinished(timer);
    CHECK_EQ(used, 600);
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 0);
    CHECK_EQ(timer.sideRemainingMs(Side::Con), 0);
    CHECK(timer.state() == RunState::Finished);
    CHECK(timer.progress() == 1.0);
}

void freeDebateLargeTick()
{
    // 一次推进很长时间，跨越多次换人 // One huge tick crossing many turn changes
    DebateTimer timer = freeDebate();
    timer.start();
    timer.tick(10 * 60 * 1000 + 5000);
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 0);
    CHECK_EQ(timer.sideRemainingMs(Side::Con), 0);
    CHECK(timer.state() == RunState::Finished);
}

void freeDebateManualSwitch()
{
    DebateTimer timer = freeDebate();
    timer.start();
    speak(timer, 20);
    CHECK(timer.switchSide());
    CHECK(timer.activeSide() == Side::Con);
    CHECK_EQ(timer.turnRemainingMs(), 60000);
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 280000);
    CHECK(timer.state() == RunState::Running);

    speak(timer, 5);
    CHECK_EQ(timer.sideRemainingMs(Side::Con), 295000);
    CHECK_EQ(timer.remainingMs(), 295000);
}

void freeDebateOneSideRunsOut()
{
    // 反方每轮 20 秒就交棒，正方说满 60 秒：正方先用完后，反方应能一次用完剩下的 220 秒
    // Con passes after 20 s, Pro speaks full turns: once Pro is out, Con keeps its remaining 220 s in one go
    DebateTimer timer = freeDebate();
    timer.start();
    for (int round = 0; round < 4; ++round)
    {
        speak(timer, 60);  // 正方 // Pro
        speak(timer, 20);  // 反方 // Con
        CHECK(timer.switchSide());
    }
    speak(timer, 60);  // 正方最后 60 秒 // Pro's last 60 s
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 0);
    CHECK(timer.activeSide() == Side::Con);
    CHECK(!timer.turnLimited());
    CHECK_EQ(timer.turnRemainingMs(), 220000);

    // 对方已无时间，不能交换 // The other side has no time, so no switching
    CHECK(!timer.canSwitchSide());
    CHECK(!timer.switchSide());

    CHECK_EQ(runUntilFinished(timer), 220);
    CHECK_EQ(timer.sideRemainingMs(Side::Con), 0);
}

void freeDebateSwitchBeforeStart()
{
    // 开始前可以改由反方先发言 // Before starting, Con can be chosen to speak first
    DebateTimer timer = freeDebate();
    CHECK(timer.switchSide());
    CHECK(timer.activeSide() == Side::Con);
    CHECK(timer.state() == RunState::Idle);
    timer.start();
    speak(timer, 10);
    CHECK_EQ(timer.sideRemainingMs(Side::Con), 290000);
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 300000);
}

void freeDebateUnlimitedTurns()
{
    // 不限单次时长：不交换就一直是正方 // No turn limit: Pro keeps the floor until switching
    DebateTimer timer = freeDebate(0);
    CHECK(!timer.turnLimited());
    timer.start();
    speak(timer, 120);
    CHECK(timer.activeSide() == Side::Pro);
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 180000);
    speak(timer, 180);
    CHECK(timer.activeSide() == Side::Con);
    CHECK_EQ(timer.turnRemainingMs(), 300000);
}

void freeDebateResetAndReenter()
{
    DebateTimer timer = freeDebate();
    timer.start();
    speak(timer, 90);
    timer.resetStage();
    CHECK_EQ(timer.sideRemainingMs(Side::Pro), 300000);
    CHECK_EQ(timer.sideRemainingMs(Side::Con), 300000);
    CHECK(timer.activeSide() == Side::Pro);

    timer.nextStage();
    CHECK(!timer.isFreeDebate());
    CHECK(!timer.canSwitchSide());
    CHECK(!timer.switchSide());
}

void rulesWithoutEndStage()
{
    // 自定义赛制缺少结束阶段时自动补上 // A custom format without an End stage gets one appended
    Rules rules;
    rules.stages = {{StageKind::Speech, Side::Pro, "立论", "Constructive", "一辩", 10}};
    DebateTimer timer(rules);
    CHECK_EQ(timer.stageCount(), 2);
    timer.nextStage();
    CHECK(timer.isOver());
}

}  // namespace

int main()
{
    const std::vector<std::pair<const char*, std::function<void()>>> tests = {
        {"standardRules", standardRules},
        {"formatting", formatting},
        {"speechCountdown", speechCountdown},
        {"progressAndWarning", progressAndWarning},
        {"navigation", navigation},
        {"freeDebateDefaultFlow", freeDebateDefaultFlow},
        {"freeDebateLargeTick", freeDebateLargeTick},
        {"freeDebateManualSwitch", freeDebateManualSwitch},
        {"freeDebateOneSideRunsOut", freeDebateOneSideRunsOut},
        {"freeDebateSwitchBeforeStart", freeDebateSwitchBeforeStart},
        {"freeDebateUnlimitedTurns", freeDebateUnlimitedTurns},
        {"freeDebateResetAndReenter", freeDebateResetAndReenter},
        {"rulesWithoutEndStage", rulesWithoutEndStage},
    };

    for (const auto& [name, test] : tests)
    {
        int before = failures;
        test();
        std::printf("%s %s\n", failures == before ? "[ OK ]" : "[FAIL]", name);
    }
    std::printf("%d failure(s)\n", failures);
    return failures == 0 ? 0 : 1;
}
