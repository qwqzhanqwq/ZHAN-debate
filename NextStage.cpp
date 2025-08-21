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
        bool zhengExhausted = (zhengRemain <= 0 && isZhengTurn);
        bool fanExhausted = (fanRemain <= 0 && !isZhengTurn);

        // 如果正反双方都用完时间或当前方用完时间，进入下一阶段
        // If both sides used up time or current side used up, go to next stage
        if ((zhengRemain <= 0 && fanRemain <= 0) || zhengExhausted || fanExhausted)
        {
            currentStage++;
            timeLeft = stages[currentStage].totalTime;
        }
        else
        {
            // 切换发言方，分配发言时间 // Switch side, assign speech time
            isZhengTurn = !isZhengTurn;
            currentSpeechTime = min(60, isZhengTurn ? zhengRemain : fanRemain);
            timeLeft = currentSpeechTime;
            InvalidateRect(hWnd, NULL, TRUE);
            if (isRunning) SetTimer(hWnd, ID_TIMER, 1000, NULL);
            return;
        }
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