// GenAutoStartupShortcut.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
	


#include "GenAutoStartupShortcut.h"
#include "../IWB/Shortcut.h"
#include <string>

#include "CmdLineParser.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void DisplayError(DWORD dwError)
{
    LPVOID lpvMessageBuffer;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, 
        dwError, 
        MAKELANGID(LANG_NEUTRAL, 
        SUBLANG_DEFAULT), 
        (LPWSTR)&lpvMessageBuffer,
        0, 
        NULL);

    TCHAR text[1024];

    _stprintf_s(
         text,
         _countof(text),
         _T("Error 0x%x:%s"),
         dwError,
        (LPCTSTR)lpvMessageBuffer
         );

    MessageBox(NULL, text,_T("Error"), MB_ICONEXCLAMATION);

    LocalFree(lpvMessageBuffer);
    

}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    CoInitialize(NULL);
	UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

   // MessageBox(NULL, lpCmdLine, _T("Information"), MB_ICONINFORMATION);

    CmdLineParser<> cmdParser;
    cmdParser.Parse(lpCmdLine);

    TCHAR text[1024];

    if(cmdParser.GetArgsCount() == 0 )
    {
       
     _stprintf_s(
         text,
         _countof(text),
         _T("Usage:IWBCreateShortcut.exe /app-path <path> /arguments <arguments>   /shortcut-path <path> /work-directory <path> /description <description>")
         );

        MessageBox(NULL, text,_T("Usage"), MB_ICONINFORMATION);
        return 0;
    }



    CString strModuleName    = _T("");
    CString strDescription   = _T("");
    CString strWorkDirectory = _T("");
    CString strShortcutPath  = _T("");
    CString strCmdLine       = _T("");

    if(cmdParser.GeArgsValue(_T("app-path"), text, _countof(text)))
    {
        strModuleName = text;
    }


    if(cmdParser.GeArgsValue(_T("shortcut-path"), text, _countof(text)))
    {
        strShortcutPath = text;
    }

    if(cmdParser.GeArgsValue(_T("work-directory"), text, _countof(text)))
    {
        strWorkDirectory = text;
    }

    if(cmdParser.GeArgsValue(_T("description"), text, _countof(text)))
    {
        strDescription = text;
    }

    if(cmdParser.GeArgsValue(_T("arguments"), text, _countof(text)))
    {
        strCmdLine = text;
    }

    HRESULT hr =
            CreateShortcut(
            strModuleName,//快捷方式指向的模块名称,
            strDescription,//Description, 快捷方式备注
            strWorkDirectory,//工作路径
            strShortcutPath,//
            strCmdLine);

    if(hr != S_OK)
    {
        DisplayError(hr);
    }

    CoUninitialize();

    return 0;
}

