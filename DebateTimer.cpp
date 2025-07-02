// ���ӽ�������
#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "DebateTimer.h"  // ����ͷ�ļ�
#include <sstream>        // wstringstream ���ڴ��ļ�ʹ��

// ���ӽ�������
#pragma comment(lib, "msimg32.lib")

using namespace std; 

// --- ȫ�ֱ����������ʼ�� ---

// ���۽׶���������
vector<DebateStage> stages = {
    // �׶�                   ����              ʱ��  ������            ��ɫ
    {PHASE_OPENING,        L"�����˿���",      120, L"�����˷���",     RGB(128,128,128)},
    {PHASE_ZHENGLUN1,      L"���۽׶�-����",  180, L"����һ�緢��",   RGB(0, 0, 255)},
    {PHASE_FANLUN1,        L"���۽׶�-����",  180, L"����һ�緢��",   RGB(255, 0, 0)},
    {PHASE_BOLUN2,         L"���۽׶�-����",  120, L"�������緢��",   RGB(0, 0, 200)},
    {PHASE_FANBOLUN2,      L"���۽׶�-����",  120, L"�������緢��",   RGB(200, 0, 0)},
    {PHASE_ZHIXUN3,        L"��ѯ�׶�-����",  120, L"����������ѯ",   RGB(0, 0, 150)},
    {PHASE_FANZHIXUN3,     L"��ѯ�׶�-����",  120, L"����������ѯ",   RGB(150, 0, 0)},
    {PHASE_FREE,           L"���ɱ���",         600, L"���ɱ���׼��",   RGB(100, 0, 100)},
    {PHASE_SUMMARY_FAN,    L"�ܽ�´�-����",  180, L"�����ı��ܽ�",   RGB(200, 0, 0)},
    {PHASE_SUMMARY_ZHENG,  L"�ܽ�´�-����",  180, L"�����ı��ܽ�",   RGB(0, 0, 200)},
    {PHASE_END,            L"��������",          0,  L"",                RGB(128,128,128)}
};

// ״̬����
int currentStage = 0;
int timeLeft = 0;
bool isRunning = false;
int zhengRemain = 300;
int fanRemain = 300;
int currentSpeechTime = 0;
bool isZhengTurn = true;

// ���ڿؼ����
HWND hStage = NULL, hSpeaker = NULL, hTime = NULL, hStartBtn = NULL, hPauseBtn = NULL, hResetBtn = NULL, hSkipBtn = NULL;
HFONT hFont = NULL;

// --- ����ʵ�� ---

// ��ʼ������ؼ�
void InitControls(HWND hWnd) {
    RECT rc;
    GetClientRect(hWnd, &rc);

    hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");

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

    hStartBtn = CreateWindowW(L"BUTTON", L"��ʼ",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX, btnY, btnWidth, 35,
        hWnd, (HMENU)ID_START, NULL, NULL);

    hPauseBtn = CreateWindowW(L"BUTTON", L"��ͣ",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX + (btnWidth + btnSpacing), btnY,
        btnWidth, 35, hWnd, (HMENU)ID_PAUSE, NULL, NULL);

    hResetBtn = CreateWindowW(L"BUTTON", L"����",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX + 2 * (btnWidth + btnSpacing), btnY,
        btnWidth, 35, hWnd, (HMENU)ID_RESET, NULL, NULL);

    hSkipBtn = CreateWindowW(L"BUTTON", L"����",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX + 3 * (btnWidth + btnSpacing), btnY,
        btnWidth, 35, hWnd, (HMENU)ID_SKIP, NULL, NULL);

    SendMessage(hStage, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hSpeaker, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hTime, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// ����ʱ����ʾ����ʽ��MM:SS��
void UpdateTimeDisplay() {
    wstringstream ss;
    ss << timeLeft / 60 << L":"
        << (timeLeft % 60 < 10 ? L"0" : L"") << timeLeft % 60;
    SetWindowTextW(hTime, ss.str().c_str());
}

// �л�����һ�׶�
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

// ����Բ�ν�����
void DrawProgress(HDC hdc, RECT& rc) {
    DebateStage& stage = stages[currentStage];
    int diameter = min(rc.right, rc.bottom) - 40; // ������ֱ��
    int x = (rc.right - diameter) / 2;  // Բ��X����
    int y = (rc.bottom - diameter) / 2; // Բ��Y����

    // ���Ʊ���Բ
    HBRUSH hBr = CreateSolidBrush(RGB(230, 230, 230));
    SelectObject(hdc, hBr);
    Ellipse(hdc, x, y, x + diameter, y + diameter);

    // ��������������
    HPEN hPen = CreatePen(PS_SOLID, 15, stage.color);
    SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH)); // �����

    // ������Ȱٷֱ�
    double progress = 0;
    if (stage.phase == PHASE_FREE) {
        // ���ɱ����ܽ��ȼ���
        int totalUsed = 600 - (zhengRemain + fanRemain);
        progress = totalUsed / 600.0;
    }
    else {
        // ����׶ν��ȼ���
        progress = (stage.totalTime - timeLeft) / (double)stage.totalTime;
    }

    // ����Բ������12�㷽��˳ʱ����ƣ�
    int sweepAngle = (int)(3600 * progress); // 360��=3600��λ
    Arc(hdc, x + 15, y + 15, x + diameter - 15, y + diameter - 15,
        x + diameter / 2, y + 15,  // �����12�㷽��
        x + diameter / 2 + (int)(diameter / 2 * cos(sweepAngle * 3.14159 / 1800)),
        y + diameter / 2 + (int)(diameter / 2 * sin(sweepAngle * 3.14159 / 1800)));

    // ������Դ
    DeleteObject(hPen);
    DeleteObject(hBr);
}

// ������Ϣ������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        InitControls(hWnd);
        currentStage = 0;
        timeLeft = stages[currentStage].totalTime;
        UpdateTimeDisplay(); // ��ʼ��ʾʱ��
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

        //DrawBackground(hdc, rc); // ���Ʊ���
        //void DrawProgress(HDC hdc, RECT & rc);


        if (stages[currentStage].phase == PHASE_FREE) {
            wchar_t stageText[100];
            swprintf(stageText, 100, L"���ɱ���-%s����ʣ%03d�� ��ʣ%03d�룩",
                isZhengTurn ? L"����" : L"����", zhengRemain, fanRemain);
            SetWindowTextW(hStage, stageText);

            wchar_t speakerText[50];
            swprintf(speakerText, 50, L"%s���ԣ�ʣ%02d�룩",
                isZhengTurn ? L"����" : L"����", currentSpeechTime);
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

// �������
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

    HWND hWnd = CreateWindowW(L"DebateTimerClass", L"��������ʱ��",
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