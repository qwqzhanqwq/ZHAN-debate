// 链接渐变填充库
#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "DebateTimer.h"  // 包含头文件
#include <sstream>        // wstringstream 仅在此文件使用

// 链接渐变填充库
#pragma comment(lib, "msimg32.lib")

using namespace std; 

// --- 全局变量定义与初始化 ---

// 辩论阶段配置数据
vector<DebateStage> stages = {
    // 阶段                   标题              时间  发言人            颜色
    {PHASE_OPENING,        L"主持人开场",      120, L"主持人发言",     RGB(128,128,128)},
    {PHASE_ZHENGLUN1,      L"立论阶段-正方",  180, L"正方一辩发言",   RGB(0, 0, 255)},
    {PHASE_FANLUN1,        L"立论阶段-反方",  180, L"反方一辩发言",   RGB(255, 0, 0)},
    {PHASE_BOLUN2,         L"驳论阶段-正方",  120, L"正方二辩发言",   RGB(0, 0, 200)},
    {PHASE_FANBOLUN2,      L"驳论阶段-反方",  120, L"反方二辩发言",   RGB(200, 0, 0)},
    {PHASE_ZHIXUN3,        L"质询阶段-正方",  120, L"正方三辩质询",   RGB(0, 0, 150)},
    {PHASE_FANZHIXUN3,     L"质询阶段-反方",  120, L"反方三辩质询",   RGB(150, 0, 0)},
    {PHASE_FREE,           L"自由辩论",         600, L"自由辩论准备",   RGB(100, 0, 100)},
    {PHASE_SUMMARY_FAN,    L"总结陈词-反方",  180, L"反方四辩总结",   RGB(200, 0, 0)},
    {PHASE_SUMMARY_ZHENG,  L"总结陈词-正方",  180, L"正方四辩总结",   RGB(0, 0, 200)},
    {PHASE_END,            L"比赛结束",          0,  L"",                RGB(128,128,128)}
};

// 状态变量
int currentStage = 0;
int timeLeft = 0;
bool isRunning = false;
int zhengRemain = 300;
int fanRemain = 300;
int currentSpeechTime = 0;
bool isZhengTurn = true;

// 窗口控件句柄
HWND hStage = NULL, hSpeaker = NULL, hTime = NULL, hStartBtn = NULL, hPauseBtn = NULL, hResetBtn = NULL, hSkipBtn = NULL;
HFONT hFont = NULL;

// --- 函数实现 ---

// 初始化界面控件
void InitControls(HWND hWnd) {
    RECT rc;
    GetClientRect(hWnd, &rc);

    hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");

    hStage = CreateWindowW(L"STATIC", L"",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 10, rc.right - 20, 30, hWnd, NULL, NULL, NULL);

    hSpeaker = CreateWindowW(L"STATIC", L"",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 50, rc.right - 20, 30, hWnd, NULL, NULL, NULL);

    hTime = CreateWindowW(L"STATIC", L"00:00",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 100, rc.right - 20, 40, hWnd, NULL, NULL, NULL);

    int btnY = 200;
    int btnWidth = 80;
    int btnSpacing = 10;
    int totalBtnWidth = 4 * btnWidth + 3 * btnSpacing;
    int startX = (rc.right - totalBtnWidth) / 2;

    hStartBtn = CreateWindowW(L"BUTTON", L"开始",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX, btnY, btnWidth, 35,
        hWnd, (HMENU)ID_START, NULL, NULL);

    hPauseBtn = CreateWindowW(L"BUTTON", L"暂停",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX + (btnWidth + btnSpacing), btnY,
        btnWidth, 35, hWnd, (HMENU)ID_PAUSE, NULL, NULL);

    hResetBtn = CreateWindowW(L"BUTTON", L"重置",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX + 2 * (btnWidth + btnSpacing), btnY,
        btnWidth, 35, hWnd, (HMENU)ID_RESET, NULL, NULL);

    hSkipBtn = CreateWindowW(L"BUTTON", L"跳过",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX + 3 * (btnWidth + btnSpacing), btnY,
        btnWidth, 35, hWnd, (HMENU)ID_SKIP, NULL, NULL);

    SendMessage(hStage, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hSpeaker, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hTime, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// 更新时间显示（格式：MM:SS）
void UpdateTimeDisplay() {
    wstringstream ss;
    ss << timeLeft / 60 << L":"
        << (timeLeft % 60 < 10 ? L"0" : L"") << timeLeft % 60;
    SetWindowTextW(hTime, ss.str().c_str());
}

// 切换至下一阶段
void NextStage(HWND hWnd) {
    if (currentStage >= stages.size() - 1) return;

    if (stages[currentStage].phase == PHASE_FREE) {
        bool zhengExhausted = (zhengRemain <= 0 && isZhengTurn);
        bool fanExhausted = (fanRemain <= 0 && !isZhengTurn);

        if ((zhengRemain <= 0 && fanRemain <= 0) || zhengExhausted || fanExhausted) {
            currentStage++;
            timeLeft = stages[currentStage].totalTime;
        }
        else {
            isZhengTurn = !isZhengTurn;
            currentSpeechTime = min(60, isZhengTurn ? zhengRemain : fanRemain);
            timeLeft = currentSpeechTime;
            InvalidateRect(hWnd, NULL, TRUE);
            if (isRunning) SetTimer(hWnd, ID_TIMER, 1000, NULL);
            return;
        }
    }
    else {
        currentStage++;
        timeLeft = stages[currentStage].totalTime;
    }

    if (stages[currentStage].phase == PHASE_FREE) {
        zhengRemain = 300;
        fanRemain = 300;
        isZhengTurn = true;
        currentSpeechTime = 60;
        timeLeft = currentSpeechTime;
    }

    if (currentStage >= stages.size() - 1) {
        KillTimer(hWnd, ID_TIMER);
        isRunning = false;
    }

    InvalidateRect(hWnd, NULL, TRUE);
    UpdateTimeDisplay();
}

// 绘制圆形进度条
void DrawProgress(HDC hdc, RECT& rc) {
    DebateStage& stage = stages[currentStage];
    int diameter = min(rc.right, rc.bottom) - 40; // 进度条直径
    int x = (rc.right - diameter) / 2;  // 圆心X坐标
    int y = (rc.bottom - diameter) / 2; // 圆心Y坐标

    // 绘制背景圆
    HBRUSH hBr = CreateSolidBrush(RGB(230, 230, 230));
    SelectObject(hdc, hBr);
    Ellipse(hdc, x, y, x + diameter, y + diameter);

    // 创建进度条画笔
    HPEN hPen = CreatePen(PS_SOLID, 15, stage.color);
    SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH)); // 无填充

    // 计算进度百分比
    double progress = 0;
    if (stage.phase == PHASE_FREE) {
        // 自由辩论总进度计算
        int totalUsed = 600 - (zhengRemain + fanRemain);
        progress = totalUsed / 600.0;
    }
    else {
        // 常规阶段进度计算
        progress = (stage.totalTime - timeLeft) / (double)stage.totalTime;
    }

    // 绘制圆弧（从12点方向顺时针绘制）
    int sweepAngle = (int)(3600 * progress); // 360度=3600单位
    Arc(hdc, x + 15, y + 15, x + diameter - 15, y + diameter - 15,
        x + diameter / 2, y + 15,  // 起点在12点方向
        x + diameter / 2 + (int)(diameter / 2 * cos(sweepAngle * 3.14159 / 1800)),
        y + diameter / 2 + (int)(diameter / 2 * sin(sweepAngle * 3.14159 / 1800)));

    // 清理资源
    DeleteObject(hPen);
    DeleteObject(hBr);
}

// 窗口消息处理函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        InitControls(hWnd);
        currentStage = 0;
        timeLeft = stages[currentStage].totalTime;
        UpdateTimeDisplay(); // 初始显示时间
        break;

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case ID_START:
            if (!isRunning) {
                isRunning = true;
                SetTimer(hWnd, ID_TIMER, 1000, NULL);
            }
            break;
        case ID_SKIP:
            if (stages[currentStage].phase == PHASE_FREE) {
                if (isZhengTurn) zhengRemain -= currentSpeechTime - timeLeft;
                else fanRemain -= currentSpeechTime - timeLeft;
            }
            NextStage(hWnd);
            if (isRunning) SetTimer(hWnd, ID_TIMER, 1000, NULL);
            break;
        case ID_PAUSE:
            KillTimer(hWnd, ID_TIMER);
            isRunning = false;
            break;
        case ID_RESET:
            KillTimer(hWnd, ID_TIMER);
            currentStage = 0;
            timeLeft = stages[currentStage].totalTime;
            zhengRemain = 300;
            fanRemain = 300;
            isRunning = false;
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateTimeDisplay();
            break;
        }
        break;
    }

    case WM_TIMER:
        if (stages[currentStage].phase == PHASE_FREE) {
            if (timeLeft > 0) {
                timeLeft--;
                if (isZhengTurn) zhengRemain--; else fanRemain--;
                currentSpeechTime--;
                UpdateTimeDisplay();
                InvalidateRect(hWnd, NULL, TRUE);
                if (currentSpeechTime <= 0 || (isZhengTurn && zhengRemain <= 0) || (!isZhengTurn && fanRemain <= 0)) {
                    NextStage(hWnd);
                }
            }
        }
        else {
            if (timeLeft > 0) {
                timeLeft--;
                UpdateTimeDisplay();
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else {
                NextStage(hWnd);
            }
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);

        //DrawBackground(hdc, rc); // 绘制背景
        //void DrawProgress(HDC hdc, RECT & rc);


        if (stages[currentStage].phase == PHASE_FREE) {
            wchar_t stageText[100];
            swprintf(stageText, 100, L"自由辩论-%s（正剩%03d秒 反剩%03d秒）",
                isZhengTurn ? L"正方" : L"反方", zhengRemain, fanRemain);
            SetWindowTextW(hStage, stageText);

            wchar_t speakerText[50];
            swprintf(speakerText, 50, L"%s发言（剩%02d秒）",
                isZhengTurn ? L"正方" : L"反方", currentSpeechTime);
            SetWindowTextW(hSpeaker, speakerText);
        }

        else {
            SetWindowTextW(hStage, stages[currentStage].title);
            SetWindowTextW(hSpeaker, stages[currentStage].speaker);
        }

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        KillTimer(hWnd, ID_TIMER);
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// 程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"DebateTimerClass";
    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(L"DebateTimerClass", L"辩论赛计时器",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 600, 400, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}