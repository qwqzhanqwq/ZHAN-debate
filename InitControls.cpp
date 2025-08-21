#include "DebateTimer.h" 

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