#include <windows.h>
#include <iostream>
#include <string>
#include "DebateTimer.h" 
#include <sstream>        

// ����ʱ����ʾ����ʽ��MM:SS��
// Update time display (format: MM:SS)
void UpdateTimeDisplay()
{
    wstringstream ss;
    ss << timeLeft / 60 << L":"
        << (timeLeft % 60 < 10 ? L"0" : L"") << timeLeft % 60;
    SetWindowTextW(hTime, ss.str().c_str());
}