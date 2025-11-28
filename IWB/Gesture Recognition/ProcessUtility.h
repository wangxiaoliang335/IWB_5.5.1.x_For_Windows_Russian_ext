#pragma once
#include <string>

//#include "ProcessDetection.h"
#include "../../ProcessDetection/ProcessDetection.h"


class CProcessUtility
{
private:
    CProcessUtility(void);
    ~CProcessUtility(void);

public:
    static std::string GetProcessNameFromPoint(const POINT &pt, bool isFindSpecProcName = false);    
    //static bool IsPointOnTheSpecialWndWithTitle(const char *wndTitle, const POINT &pt, HWND &hWnd, bool isFindSpecProcName = true);
    static bool IsTheSpecWatchProcessExisted();
    static bool IsPointInTheSpecialWatchProcess(const POINT &pt);
    //static int GetCurrentProcessInfo(std::string *procNameArry, const int &arrySize);
    static int GetCurrentProcessInfo(char (*ppProcNameArry)[256], const int &nMaxArrySize);
    static void AddSpecialWatchProcess(const std::string &procName);

private:
	static bool m_hasProcessDetectionStarted;
	static CProcessDetection m_processDetection;
};
