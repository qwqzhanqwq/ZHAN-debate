#pragma once

// ���ӽ�������
#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>

// �ؼ�ID����
#define ID_TIMER        1     // ��ʱ��ID
#define ID_START        1001  // ��ʼ��ť
#define ID_PAUSE        1002  // ��ͣ��ť
#define ID_RESET        1003  // ���ð�ť
#define ID_SKIP         1004  // ������ť

using namespace std;

// ���۽׶�ö�ٶ���
enum PHASE 
{
    PHASE_OPENING,
    PHASE_ZHENGLUN1,
    PHASE_FANLUN1,
    PHASE_BOLUN2,
    PHASE_FANBOLUN2,
    PHASE_ZHIXUN3,
    PHASE_FANZHIXUN3,
    PHASE_FREE,
    PHASE_SUMMARY_FAN,
    PHASE_SUMMARY_ZHENG,
    PHASE_END
};

// ���۽׶νṹ��
struct DebateStage 
{
    PHASE phase;
    const wchar_t* title;
    int totalTime;
    const wchar_t* speaker;
    COLORREF color;
};

// ����ȫ�ֱ�������cpp�ﶨ�壩
extern vector<DebateStage> stages;
extern int currentStage;
extern int timeLeft;
extern bool isRunning;
extern int zhengRemain;
extern int fanRemain;
extern int currentSpeechTime;
extern bool isZhengTurn;

// ���ڿؼ����
extern HWND hStage, hSpeaker, hTime, hStartBtn, hPauseBtn, hResetBtn, hSkipBtn;
extern HFONT hFont;

// ��������
void InitControls(HWND hWnd);
void UpdateTimeDisplay();
void NextStage(HWND hWnd);
VOID ProgressBar(HDC hdc,RECT& rc);
void DrawProgress(HDC hdc, RECT& rc);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);