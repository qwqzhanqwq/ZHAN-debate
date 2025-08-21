#include <windows.h>
#include <vector>
#include "DebateTimer.h" 

// �л�����һ�׶�
// Switch to next stage
void NextStage(HWND hWnd)
{
    if (currentStage >= stages.size() - 1) return;

    // ���ɱ��۽׶����⴦�� // Special handling for free debate stage
    if (stages[currentStage].phase == PHASE_FREE)
    {
        bool zhengExhausted = (zhengRemain <= 0 && isZhengTurn);
        bool fanExhausted = (fanRemain <= 0 && !isZhengTurn);

        // �������˫��������ʱ���ǰ������ʱ�䣬������һ�׶�
        // If both sides used up time or current side used up, go to next stage
        if ((zhengRemain <= 0 && fanRemain <= 0) || zhengExhausted || fanExhausted)
        {
            currentStage++;
            timeLeft = stages[currentStage].totalTime;
        }
        else
        {
            // �л����Է������䷢��ʱ�� // Switch side, assign speech time
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
        // ��ͨ�׶�ֱ�ӽ�����һ�׶� // Normal stage, go to next
        currentStage++;
        timeLeft = stages[currentStage].totalTime;
    }

    // �������ɱ��۽׶�ʱ��ʼ��˫��ʱ��
    // Initialize both sides' time when entering free debate
    if (stages[currentStage].phase == PHASE_FREE)
    {
        zhengRemain = 300;
        fanRemain = 300;
        isZhengTurn = true;
        currentSpeechTime = 60;
        timeLeft = currentSpeechTime;
    }

    // ��������ʱֹͣ��ʱ��
    // Stop timer when debate ends
    if (currentStage >= stages.size() - 1)
    {
        KillTimer(hWnd, ID_TIMER);
        isRunning = false;
    }

    InvalidateRect(hWnd, NULL, TRUE);
    UpdateTimeDisplay();
}