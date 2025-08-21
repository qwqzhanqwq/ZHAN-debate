#include <windows.h>
#include <vector>
#include "DebateTimer.h" 

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