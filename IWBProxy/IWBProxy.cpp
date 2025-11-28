// IWBProxy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "IWBProxy.h"
#include "IWBProxyLog.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL GetWorkingDirectory(HINSTANCE hInstance, TCHAR* szWorkingDirectory, UINT bufCount)
{
	TCHAR szModuleFileName[MAX_PATH];

	DWORD dwRet = GetModuleFileName(hInstance, szModuleFileName, _countof(szModuleFileName));

	if (dwRet == 0)
	{
		
		return FALSE;
	}


	ZeroMemory(szWorkingDirectory, bufCount *sizeof(TCHAR));


	_tcscpy_s(szWorkingDirectory, bufCount , szModuleFileName);

	TCHAR* pLastBackslash = _tcsrchr(szWorkingDirectory, _T('\\'));

	if(pLastBackslash)*pLastBackslash = _T('\0');
	return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //UNREFERENCED_PARAMETER(hPrevInstance);
    //UNREFERENCED_PARAMETER(lpCmdLine);

    //// TODO: Place code here.

    //// Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_IWBPROXY, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);

    //// Perform application initialization:
    //if (!InitInstance (hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IWBPROXY));

    //MSG msg;

    //// Main message loop:
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}
	TCHAR szWorkingDirectory[MAX_PATH];
	GetWorkingDirectory(hInstance, szWorkingDirectory, _countof(szWorkingDirectory));

	

	const TCHAR* module_names[] = {
	_T("IWB.exe"),
	_T("AlgoLib.dll"),
		_T("CameraModel.dll"),
			_T("CHS_RES.dll"),
				_T("CHT_RES.dll"),
					_T("DEU_RES.dll"),
						_T("EasiSDKReg_Interface.dll"),
							_T("EASI_TOUCHSCREEN_INTERFACE.dll"),
								_T("ESP_RES.dll"),
									_T("FRA_RES.dll"),
										_T("ITA_RES.dll"),
											_T("IWB_Interface.dll"),
												_T("KOR_RES.dll"),
													_T("libifcoremd.dll"),
														_T("libmmd.dll"),
															_T("MJPG_Decoder.dll"),
																_T("MJPG_Encoder.dll"),
																	_T("MorphologyAlgo.dll"),
																		_T("OpticalPen_Interface.dll"),
																			_T("ProcessDetection.dll"),
																				_T("PTG_RES.dll"),
																					_T("Rockey2.dll"),
																						_T("RUS_RES.dll"),
																							_T("ShareMemoryRegion.dll"),
																								_T("THAI_RES.dll"),
																									_T("THA_RES.dll"),
																										_T("Video.dll"),
																											_T("VIT_RES.dll"),
																												_T("WinGestureRecognition.dll") };


	for (int i = 0; i < _countof(module_names); i++)
	{
		TCHAR szModulePath[MAX_PATH];
		_stprintf_s(szModulePath, _countof(szModulePath), _T("%s\\%s"), szWorkingDirectory, module_names[i]);

		if (!PathFileExists(szModulePath))
		{
			LOG_ERR("%s is missing.", (CHAR*)CT2CA(szModulePath));
			TCHAR szModuleBackupPath[MAX_PATH];
			_stprintf_s(szModuleBackupPath, _countof(szModuleBackupPath), _T("%s\\backup\\%s"), szWorkingDirectory, module_names[i]);

			if (PathFileExists(szModuleBackupPath))
			{
				//BOOL WINAPI CopyFile(
				//	_In_ LPCTSTR lpExistingFileName,
				//	_In_ LPCTSTR lpNewFileName,
				//	_In_ BOOL    bFailIfExists
				//);
				BOOL bRet = CopyFile(szModuleBackupPath, szModulePath, TRUE);
				if (!bRet)
				{
					DWORD dwError = GetLastError();

					LOG_ERR("Copy File %s to %s failed with error 0x%x.", 
						(CHAR*)CT2CA(szModuleBackupPath),
						(CHAR*)CT2CA(szModulePath),
						dwError);
				}
				else
				{
					LOG_INF("Copy File %s to %s succeeded.",
						(CHAR*)CT2CA(szModuleBackupPath),
						(CHAR*)CT2CA(szModulePath));

				}
			}//if
			else
			{

				LOG_ERR("%s is missing.", (CHAR*)CT2CA(szModuleBackupPath));
			}//else

		}//if

	}//for
	

	TCHAR szAppName[MAX_PATH];
	DWORD dwRet = GetModuleFileName(hInstance, szAppName, _countof(szAppName));
	
	TCHAR* lpszModuleFileName = PathFindFileName(szAppName);	
	TCHAR* pProxyChars = _tcsstr(lpszModuleFileName, _T("Proxy"));
	if (pProxyChars)
	{
		_tcscpy_s(pProxyChars, 5, _T(".exe"));
	}

	const TCHAR* lpszAppName = PathFindFileName(szAppName);


	TCHAR szAppPath[MAX_PATH];
	_stprintf_s(szAppPath, _countof(szAppPath), _T("%s\\%s"), szWorkingDirectory, lpszAppName);

	//开启客户端进程
	PROCESS_INFORMATION processInfo;
	STARTUPINFO si;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);



	BOOL bResult = CreateProcess(
		szAppPath,
		lpCmdLine,
		NULL,//Process handle not inheritable
		NULL,//Thread handle not inheritable.
		TRUE,//Set handle inheritance to TRUE
		0,   //no creation flag
		NULL,//Use parent's environment block. 
		szWorkingDirectory,//Use parent's starting directory
		&si, // Pointer to STARTUPINFO structure.
		&processInfo);// Pointer to PROCESS_INFORMATION structure.

	if (!bResult)
	{
		DWORD dwErr = GetLastError();

		if (dwErr == ERROR_ELEVATION_REQUIRED)
		{
			LOG_ERR("Create Process %s failed!Error Code, ", (CHAR*)CT2CA(szAppPath));
		}
		else
		{
			LOG_ERR("Create Process %s failed!Error Code:0x%x", (CHAR*)CT2CA(szAppPath), GetLastError());

		}
		return -1; //开启客户端进程失败，直接退出
	}



	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread); 
	
	return 0; 
}



/*
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IWBPROXY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IWBPROXY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
*/