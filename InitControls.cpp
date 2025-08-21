#include "DebateTimer.h" 

// ��ʼ������ؼ�
// Initialize UI controls
void InitControls(HWND hWnd)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    // �������� // Create font
    hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");

    // ������ʾ�׶εľ�̬�ı��ؼ� // Create static text control for stage
    hStage = CreateWindowW(L"STATIC", L"",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 10, rc.right - 20, 30, hWnd, NULL, NULL, NULL);

    // ������ʾ�����˵ľ�̬�ı��ؼ� // Create static text control for speaker
    hSpeaker = CreateWindowW(L"STATIC", L"",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 50, rc.right - 20, 30, hWnd, NULL, NULL, NULL);

    // ������ʾʱ��ľ�̬�ı��ؼ� // Create static text control for time
    hTime = CreateWindowW(L"STATIC", L"00:00",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 100, rc.right - 20, 40, hWnd, NULL, NULL, NULL);

    // ���㰴ť���� // Calculate button layout
    int btnY = 200;
    int btnWidth = 80;
    int btnSpacing = 10;
    int totalBtnWidth = 4 * btnWidth + 3 * btnSpacing;
    int startX = (rc.right - totalBtnWidth) / 2;

    // ��������ʼ����ť // Create "Start" button
    hStartBtn = CreateWindowW(L"BUTTON", L"��ʼ",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        50, 200, 100, 40, hWnd, (HMENU)ID_START, NULL, NULL);

    // ��������ͣ����ť // Create "Pause" button
    hPauseBtn = CreateWindowW(L"BUTTON", L"��ͣ",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        160, 200, 100, 40, hWnd, (HMENU)ID_PAUSE, NULL, NULL);

    // ���������á���ť // Create "Reset" button
    hResetBtn = CreateWindowW(L"BUTTON", L"����",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        270, 200, 100, 40, hWnd, (HMENU)ID_RESET, NULL, NULL);

    // ��������������ť // Create "Skip" button
    hSkipBtn = CreateWindowW(L"BUTTON", L"����",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        380, 200, 100, 40, hWnd, (HMENU)ID_SKIP, NULL, NULL);

    // �������� // Set font
    SendMessage(hStage, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hSpeaker, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hTime, WM_SETFONT, (WPARAM)hFont, TRUE);
}