#include <windows.h>
#include <vector>
#include <gdiplus.h>
#include "DebateTimer.h" 

using namespace Gdiplus;

// 绘制圆形进度条
// Draw circular progress bar
void DrawProgress(HDC hdc, RECT& rc)
{
    DebateStage& stage = stages[currentStage];
    int diameter = min(rc.right, rc.bottom) - 40; // 进度条直径 // Progress bar diameter
    int x = (rc.right - diameter) / 2;  // 外接矩形左上角X坐标 // Bounding box left
    int y = (rc.bottom - diameter) / 2; // 外接矩形左上角Y坐标 // Bounding box top

    // 绘制背景圆 // Draw background circle
    HBRUSH hBr = CreateSolidBrush(RGB(230, 230, 230));
    HGDIOBJ hOldBrush = SelectObject(hdc, hBr);
    Ellipse(hdc, x, y, x + diameter, y + diameter);
    // 先选回旧画刷再删除，仍被选入DC的对象无法删除，会造成GDI泄漏
    // Restore the old brush before deleting; an object still selected into a DC cannot be deleted and leaks
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBr);

    // 计算进度百分比 // Calculate progress percent
    double progress = 0;
    if (stage.phase == PHASE_FREE)
    {
        // 自由辩论总进度计算 // Free debate total progress
        int totalUsed = 600 - (zhengRemain + fanRemain);
        progress = totalUsed / 600.0;
    }
    else if (stage.totalTime > 0)
    {
        // 常规阶段进度计算 // Normal stage progress
        progress = (stage.totalTime - timeLeft) / (double)stage.totalTime;
    }
    else
    {
        // 比赛结束阶段时长为0，显示满圈，避免除以0 // End stage has no duration: show full ring, avoid dividing by 0
        progress = 1.0;
    }
    progress = max(0.0, min(1.0, progress));

    // 绘制圆弧：GDI+角度以3点方向为0度、顺时针为正，-90度即12点方向
    // Draw arc: GDI+ angles start at 3 o'clock and grow clockwise, so -90 is 12 o'clock
    if (progress > 0)
    {
        Graphics graphics(hdc);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        Pen pen(Color(255, GetRValue(stage.color), GetGValue(stage.color), GetBValue(stage.color)), 15.0f);
        graphics.DrawArc(&pen, x + 15, y + 15, diameter - 30, diameter - 30,
            -90.0f, (REAL)(360.0 * progress));
    }
}