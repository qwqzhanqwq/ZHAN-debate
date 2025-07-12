#pragma once

// 链接渐变填充库
#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>

// 控件ID定义
#define ID_TIMER        1     // 定时器ID
#define ID_START        1001  // 开始按钮
#define ID_PAUSE        1002  // 暂停按钮
#define ID_RESET        1003  // 重置按钮
#define ID_SKIP         1004  // 跳过按钮

using namespace std;

// 辩论阶段枚举定义
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

// 辩论阶段结构体
struct DebateStage 
{
    PHASE phase;
    const wchar_t* title;
    int totalTime;
    const wchar_t* speaker;
    COLORREF color;
};

// 声明全局变量（在cpp里定义）
extern vector<DebateStage> stages;
extern int currentStage;
extern int timeLeft;
extern bool isRunning;
extern int zhengRemain;
extern int fanRemain;
extern int currentSpeechTime;
extern bool isZhengTurn;

// 窗口控件句柄
extern HWND hStage, hSpeaker, hTime, hStartBtn, hPauseBtn, hResetBtn, hSkipBtn;
extern HFONT hFont;

// 函数声明
void InitControls(HWND hWnd);
void UpdateTimeDisplay();
void NextStage(HWND hWnd);
VOID ProgressBar(HDC hdc,RECT& rc);
void DrawProgress(HDC hdc, RECT& rc);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);