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

// --- 函数实现 ---
// --- Function implementation ---

// 初始化界面控件
// Initialize UI controls
void InitControls(HWND hWnd) 
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    // 创建字体 // Create font
    hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");

    // 创建显示阶段的静态文本控件 // Create static text control for stage
    hStage = CreateWindowW(L"STATIC", L"",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 10, rc.right - 20, 30, hWnd, NULL, NULL, NULL);

    // 创建显示发言人的静态文本控件 // Create static text control for speaker
    hSpeaker = CreateWindowW(L"STATIC", L"",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 50, rc.right - 20, 30, hWnd, NULL, NULL, NULL);

    // 创建显示时间的静态文本控件 // Create static text control for time
    hTime = CreateWindowW(L"STATIC", L"00:00",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 100, rc.right - 20, 40, hWnd, NULL, NULL, NULL);

    // 计算按钮布局 // Calculate button layout
    int btnY = 200;
    int btnWidth = 80;
    int btnSpacing = 10;
    int totalBtnWidth = 4 * btnWidth + 3 * btnSpacing;
    int startX = (rc.right - totalBtnWidth) / 2;

    // 创建“开始”按钮 // Create "Start" button
    hStartBtn = CreateWindowW(L"BUTTON", L"开始",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        50, 200, 100, 40, hWnd, (HMENU)ID_START, NULL, NULL);

    // 创建“暂停”按钮 // Create "Pause" button
    hPauseBtn = CreateWindowW(L"BUTTON", L"暂停",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        160, 200, 100, 40, hWnd, (HMENU)ID_PAUSE, NULL, NULL);

    // 创建“重置”按钮 // Create "Reset" button
    hResetBtn = CreateWindowW(L"BUTTON", L"重置",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        270, 200, 100, 40, hWnd, (HMENU)ID_RESET, NULL, NULL);

    // 创建“跳过”按钮 // Create "Skip" button
    hSkipBtn = CreateWindowW(L"BUTTON", L"跳过",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        380, 200, 100, 40, hWnd, (HMENU)ID_SKIP, NULL, NULL);

    // 设置字体 // Set font
    SendMessage(hStage, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hSpeaker, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hTime, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// 更新时间显示（格式：MM:SS）
// Update time display (format: MM:SS)
void UpdateTimeDisplay() 
{
    wstringstream ss;
    ss << timeLeft / 60 << L":"
        << (timeLeft % 60 < 10 ? L"0" : L"") << timeLeft % 60;
    SetWindowTextW(hTime, ss.str().c_str());
}

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

// 绘制圆形进度条
// Draw circular progress bar
void DrawProgress(HDC hdc, RECT& rc) 
{
    DebateStage& stage = stages[currentStage];
    int diameter = min(rc.right, rc.bottom) - 40; // 进度条直径 // Progress bar diameter
    int x = (rc.right - diameter) / 2;  // 圆心X坐标 // Center X
    int y = (rc.bottom - diameter) / 2; // 圆心Y坐标 // Center Y

    // 绘制背景圆 // Draw background circle
    HBRUSH hBr = CreateSolidBrush(RGB(230, 230, 230));
    SelectObject(hdc, hBr);
    Ellipse(hdc, x, y, x + diameter, y + diameter);

    // 创建进度条画笔 // Create pen for progress bar
    HPEN hPen = CreatePen(PS_SOLID, 15, stage.color);
    SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH)); // 无填充 // No fill

    // 计算进度百分比 // Calculate progress percent
    double progress = 0;
    if (stage.phase == PHASE_FREE) 
    {
        // 自由辩论总进度计算 // Free debate total progress
        int totalUsed = 600 - (zhengRemain + fanRemain);
        progress = totalUsed / 600.0;
    }
    else 
    {
        // 常规阶段进度计算 // Normal stage progress
        progress = (stage.totalTime - timeLeft) / (double)stage.totalTime;
    }

    // 绘制圆弧（从12点方向顺时针绘制）
    // Draw arc (clockwise from 12 o'clock)
    int sweepAngle = (int)(3600 * progress); // 360度=3600单位 // 360 deg = 3600 units
    Arc(hdc, x + 15, y + 15, x + diameter - 15, y + diameter - 15,
        x + diameter / 2, y + 15,  // 起点在12点方向 // Start at 12 o'clock
        x + diameter / 2 + (int)(diameter / 2 * cos(sweepAngle * 3.14159 / 1800)),
        y + diameter / 2 + (int)(diameter / 2 * sin(sweepAngle * 3.14159 / 1800)));

    // 清理资源 // Clean up resources
    DeleteObject(hPen);
    DeleteObject(hBr);
}

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