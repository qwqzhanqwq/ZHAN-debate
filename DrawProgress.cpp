#include <windows.h>
#include <vector>
#include "DebateTimer.h" 

// ����Բ�ν�����
// Draw circular progress bar
void DrawProgress(HDC hdc, RECT& rc)
{
    DebateStage& stage = stages[currentStage];
    int diameter = min(rc.right, rc.bottom) - 40; // ������ֱ�� // Progress bar diameter
    int x = (rc.right - diameter) / 2;  // Բ��X���� // Center X
    int y = (rc.bottom - diameter) / 2; // Բ��Y���� // Center Y

    // ���Ʊ���Բ // Draw background circle
    HBRUSH hBr = CreateSolidBrush(RGB(230, 230, 230));
    SelectObject(hdc, hBr);
    Ellipse(hdc, x, y, x + diameter, y + diameter);

    // �������������� // Create pen for progress bar
    HPEN hPen = CreatePen(PS_SOLID, 15, stage.color);
    SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH)); // ����� // No fill

    // ������Ȱٷֱ� // Calculate progress percent
    double progress = 0;
    if (stage.phase == PHASE_FREE)
    {
        // ���ɱ����ܽ��ȼ��� // Free debate total progress
        int totalUsed = 600 - (zhengRemain + fanRemain);
        progress = totalUsed / 600.0;
    }
    else
    {
        // ����׶ν��ȼ��� // Normal stage progress
        progress = (stage.totalTime - timeLeft) / (double)stage.totalTime;
    }

    // ����Բ������12�㷽��˳ʱ����ƣ�
    // Draw arc (clockwise from 12 o'clock)
    int sweepAngle = (int)(3600 * progress); // 360��=3600��λ // 360 deg = 3600 units
    Arc(hdc, x + 15, y + 15, x + diameter - 15, y + diameter - 15,
        x + diameter / 2, y + 15,  // �����12�㷽�� // Start at 12 o'clock
        x + diameter / 2 + (int)(diameter / 2 * cos(sweepAngle * 3.14159 / 1800)),
        y + diameter / 2 + (int)(diameter / 2 * sin(sweepAngle * 3.14159 / 1800)));

    // ������Դ // Clean up resources
    DeleteObject(hPen);
    DeleteObject(hBr);
}