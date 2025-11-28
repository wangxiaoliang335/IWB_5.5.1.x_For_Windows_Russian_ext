#include "StdAfx.h"
#include "ProcessUtility.h"
#include "..\..\inc\PerfDetector.h"
#include <WtsApi32.h>
#include <map>

#pragma comment(lib, "WtsApi32.lib")

bool CProcessUtility::m_hasProcessDetectionStarted = false;
CProcessDetection CProcessUtility::m_processDetection;

CProcessUtility::CProcessUtility(void)
{
}

CProcessUtility::~CProcessUtility(void)
{
}

int CProcessUtility::GetCurrentProcessInfo(char (*ppProcNameArry)[256], const int &nMaxArrySize)
{
	if (!m_hasProcessDetectionStarted) {m_hasProcessDetectionStarted = true; m_processDetection.StartDetection();}

    return m_processDetection.GetCurrentProcessInfo(ppProcNameArry, nMaxArrySize);
	//return m_processDetection.GetProcessInfoMap();
}

std::string CProcessUtility::GetProcessNameFromPoint(const POINT &pt, bool isFindSpecProcName)
{
    /*CPerfDetector perf(_T("GetProcessNameFromPoint(POINT pt)"));*/
	if (!m_hasProcessDetectionStarted) {m_hasProcessDetectionStarted = true; m_processDetection.StartDetection();}

	//const std::map<DWORD, std::string> &processInfoMap = m_processDetection.GetProcessInfoMap();
	
    std::string procName = "";
    DWORD dwProcessId = NULL;

    do 
    {
        HWND hWnd = WindowFromPoint(pt);

        if (NULL == hWnd) break;

        {
            /*CPerfDetector perf(_T("GetWindowThreadProcessId(hWnd, &dwProcessId)"));*/
            GetWindowThreadProcessId(hWnd, &dwProcessId);   
        }  


        char procNameArry[256];
        memset(procNameArry, 0, 256);

        bool bRet = m_processDetection.GetProcessNameWithPID(dwProcessId, procNameArry, 256, isFindSpecProcName);

        if (bRet) procName = procNameArry;
       
    } while (false);    

    return procName;
}

bool CProcessUtility::IsTheSpecWatchProcessExisted()
{
    if (!m_hasProcessDetectionStarted) {m_hasProcessDetectionStarted = true; m_processDetection.StartDetection();}

    return m_processDetection.IsTheSpecialWatchProcessExsit();
}

bool CProcessUtility::IsPointInTheSpecialWatchProcess(const POINT &pt)
{
    if (!m_hasProcessDetectionStarted) {m_hasProcessDetectionStarted = true; m_processDetection.StartDetection();}

    do 
    {
        HWND hWnd = WindowFromPoint(pt);

        if (NULL == hWnd) break;  

        //{
        //    /*CPerfDetector perf(_T("GetWindowThreadProcessId(hWnd, &dwProcessId)"));*/
            std::string procName = "";
            DWORD dwProcessId = 0;
            GetWindowThreadProcessId(hWnd, &dwProcessId);   
        //}  

        return m_processDetection.IsTheSpecProcessWithPID(dwProcessId);

    } while (false); 

    return false;
}

//bool CProcessUtility::IsPointOnTheSpecialWndWithTitle(const char *wndTitle, const POINT &pt, HWND &hWnd, bool isFindSpecProcName = true)
//{
//    if(!isFindSpecProcName) {hWnd = 0; return false;}
//
//    HWND hWnd = WindowFromPoint(pt);
//
//    if (NULL == hWnd) return false;
//
//    TCHAR wndText[512];
//    memset(0, wndText, sizeof(wndText) * 512);
//    GetWindowText(hWnd, wndText, 512);
//
//    if ()
//    {
//    }
//}

void CProcessUtility::AddSpecialWatchProcess(const std::string &procName)
{
    m_processDetection.AddSpecialWatchProcName(procName);
}