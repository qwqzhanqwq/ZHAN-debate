#include <windows.h>
#include <vector>
#include "DebateTimer.h" 

// 切换至下一阶段
// Switch to next stage
void NextStage(HWND hWnd)
{
    if (currentStage >= stages.size() - 1) return;

    // 自由辩论阶段特殊处理 // Special handling for free debate stage
    if (stages[currentStage].phase == PHASE_FREE)
    {
        int selfRemain = isZhengTurn ? zhengRemain : fanRemain;
        int otherRemain = isZhengTurn ? fanRemain : zhengRemain;

        // 对方还有时间：切换发言方，分配发言时间
        // Other side still has time: switch side, assign speech time
        if (otherRemain > 0)
        {
            isZhengTurn = !isZhengTurn;
            // 当前方时间已用完时不再轮换，对方可一次用完剩余时间
            // If the current side has run out, there is no more alternation: the other side may use all its remaining time
            currentSpeechTime = selfRemain > 0 ? min(60, otherRemain) : otherRemain;
            timeLeft = currentSpeechTime;
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateTimeDisplay();
            if (isRunning) SetTimer(hWnd, ID_TIMER, 1000, NULL);
            return;
        }

        // 对方时间已用完（当前方用完或主动跳过剩余时间），进入下一阶段
        // Other side has no time left (current side ran out or skips the rest): go to next stage
        currentStage++;
        timeLeft = stages[currentStage].totalTime;
    }
    else
    {
        // 普通阶段直接进入下一阶段 // Normal stage, go to next
        currentStage++;
        timeLeft = stages[currentStage].totalTime;
    }

    // 进入自由辩论阶段时初始化双方时间
    // Initialize both sides' time when entering free debate
    if (stages[currentStage].phase == PHASE_FREE)
    {
        zhengRemain = 300;
        fanRemain = 300;
        isZhengTurn = true;
        currentSpeechTime = 60;
        timeLeft = currentSpeechTime;
    }

    // 比赛结束时停止计时器
    // Stop timer when debate ends
    if (currentStage >= stages.size() - 1)
    {
        KillTimer(hWnd, ID_TIMER);
        isRunning = false;
    }

    InvalidateRect(hWnd, NULL, TRUE);
    UpdateTimeDisplay();
}