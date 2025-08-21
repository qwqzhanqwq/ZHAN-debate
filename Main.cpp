// 链接渐变填充库和GDI+库
// Link gradient fill and GDI+ libraries
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "gdiplus.lib")

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "DebateTimer.h" 
#include <sstream>        
#include <gdiplus.h>

using namespace std; 
using namespace Gdiplus;

// --- 全局变量定义与初始化 ---
// --- Global variable definition and initialization ---

// 辩论阶段配置数据，每个阶段包含类型、标题、总时长、发言人、颜色
// Debate stage configuration, each stage includes type, title, total time, speaker, color
vector<DebateStage> stages = 
{
    // 阶段                   标题              时间  发言人            颜色
    // Phase                 Title            Time  Speaker           Color
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
// State variables
int currentStage = 0;         // 当前阶段索引 // Current stage index
int timeLeft = 0;             // 当前阶段剩余时间（秒） // Remaining time in current stage (seconds)
bool isRunning = false;       // 计时器是否运行 // Is timer running
int zhengRemain = 300;        // 自由辩论正方剩余时间 // Remaining time for pro side in free debate
int fanRemain = 300;          // 自由辩论反方剩余时间 // Remaining time for con side in free debate
int currentSpeechTime = 0;    // 当前发言剩余时间（自由辩论用） // Remaining speech time in free debate
bool isZhengTurn = true;      // 当前是否正方发言 // Is it pro side's turn

// 窗口控件句柄
// Window control handles
HWND hStage = NULL, hSpeaker = NULL, hTime = NULL, hStartBtn = NULL, hPauseBtn = NULL, hResetBtn = NULL, hSkipBtn = NULL;
HFONT hFont = NULL;

// 窗口消息处理函数
// Window message handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
    switch (msg) 
    {
    case WM_CREATE:
        // 初始化控件和状态 // Initialize controls and state
        InitControls(hWnd);
        currentStage = 0;
        timeLeft = stages[currentStage].totalTime;
        UpdateTimeDisplay(); // 初始显示时间 // Initial time display

        // 设置窗口为分层窗口（Layered Window），实现整体半透明
        // Set window as layered for overall transparency
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        // 设置整体窗口透明度（200为不透明度，0~255，数值越小越透明）
        // Set window alpha (200 = less transparent, 0~255)
        SetLayeredWindowAttributes(hWnd, 0, 230, LWA_ALPHA);
        break;

    case WM_COMMAND: 
        {
        int wmId = LOWORD(wParam);
        switch (wmId) 
        {
        case ID_START:
            // 开始计时 // Start timer
            if (!isRunning) 
            {
                isRunning = true;
                SetTimer(hWnd, ID_TIMER, 1000, NULL);
            }
            break;
        case ID_SKIP:
            // 跳过当前阶段 // Skip current stage
            if (stages[currentStage].phase == PHASE_FREE) 
            {
                // 扣除未用完的发言时间 // Deduct unused speech time
                if (isZhengTurn) zhengRemain -= currentSpeechTime - timeLeft;
                else fanRemain -= currentSpeechTime - timeLeft;
            }
            NextStage(hWnd);
            if (isRunning) SetTimer(hWnd, ID_TIMER, 1000, NULL);
            break;
        case ID_PAUSE:
            // 暂停计时 // Pause timer
            KillTimer(hWnd, ID_TIMER);
            isRunning = false;
            break;
        case ID_RESET:
            // 重置所有状态 // Reset all state
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
        // 定时器消息，每秒触发一次 // Timer message, triggered every second
        if (stages[currentStage].phase == PHASE_FREE) 
        {
            // 自由辩论阶段 // Free debate stage
            if (timeLeft > 0) 
            {
                timeLeft--;
                if (isZhengTurn) zhengRemain--; else fanRemain--;
                currentSpeechTime--;
                UpdateTimeDisplay();
                InvalidateRect(hWnd, NULL, TRUE);
                // 当前发言时间或方用完，切换 // Switch when speech time or side time is up
                if (currentSpeechTime <= 0 || (isZhengTurn && zhengRemain <= 0) || (!isZhengTurn && fanRemain <= 0)) {
                    NextStage(hWnd);
                }
            }
        }
        else 
        {
            // 普通阶段 // Normal stage
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

    case WM_PAINT: 
        {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);

        // 使用 GDI+ 绘制半透明背景 // Draw semi-transparent background with GDI+
        Graphics graphics(hdc);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);

        // 半透明浅灰色背景（ARGB: 128为半透明） // Semi-transparent light gray background (ARGB: 128 is semi-transparent)
        SolidBrush semiTransBrush(Color(128, 240, 240, 240));
        graphics.FillRectangle(&semiTransBrush, 
            static_cast<INT>(rc.left), 
            static_cast<INT>(rc.top), 
            static_cast<INT>(rc.right - rc.left), 
            static_cast<INT>(rc.bottom - rc.top));

        // 绘制圆形进度条 // Draw circular progress bar
        DrawProgress(hdc, rc);

        // 绘制当前阶段标题和发言人 // Draw current stage title and speaker
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        SelectObject(hdc, hFont);
        if (stages[currentStage].phase == PHASE_OPENING)
        {
            SetWindowTextW(hStage, stages[currentStage].title);
            SetWindowTextW(hSpeaker, stages[currentStage].speaker);
        }

        // 自由辩论阶段显示剩余时间 // Show remaining time in free debate
        if (stages[currentStage].phase == PHASE_FREE) 
        {
            wchar_t stageText[100];
            swprintf(stageText, 100, L"自由辩论-%s（正剩%03d秒 反剩%03d秒）",
                isZhengTurn ? L"正方" : L"反方", zhengRemain, fanRemain);
            SetWindowTextW(hStage, stageText);

            wchar_t speakerText[50];
            swprintf(speakerText, 50, L"%s发言（剩%02d秒）",
                isZhengTurn ? L"正方" : L"反方", currentSpeechTime);
            SetWindowTextW(hSpeaker, speakerText);
        }
        else 
        {
            SetWindowTextW(hStage, stages[currentStage].title);
            SetWindowTextW(hSpeaker, stages[currentStage].speaker);
        }

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        // 程序退出时清理资源 // Clean up resources on exit
        KillTimer(hWnd, ID_TIMER);
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;

    default:
        // 其他消息默认处理 // Default message handling
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// 程序入口
// Program entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) 
{
    // 初始化GDI+库 // Initialize GDI+ library
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 注册窗口类 // Register window class
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"DebateTimerClass";
    RegisterClassExW(&wcex);

    // 创建主窗口 // Create main window
    HWND hWnd = CreateWindowW(L"DebateTimerClass", L"辩论赛计时器",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 600, 400, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 消息循环 // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 关闭GDI+库 // Shutdown GDI+ library
    GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}