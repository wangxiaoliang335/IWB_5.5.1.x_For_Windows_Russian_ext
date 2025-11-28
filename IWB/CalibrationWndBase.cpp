#include "stdafx.h"
#include "CalibrationWndBase.h"
#include "../inc/Mf2StdGf.h"
ATOM CCalibrationWndBase::m_atomClass = 0;
long CCalibrationWndBase::m_lInstanceCount = 0;
const TCHAR* CCalibrationWndBase::m_szClassName = _T("CalibrationWnd");
CCalibrationWndBase::CCalibrationWndBase()
:
m_hWnd(NULL),
m_hNotifyWnd(NULL),
m_bFullScreen(FALSE)
{
	if(m_atomClass == 0)
	{
		WNDCLASSEX wndclass;
		wndclass.cbSize = sizeof wndclass;
		wndclass.style = CS_SAVEBITS;
		wndclass.lpfnWndProc = StaticWndProc;  
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);;
		wndclass.hIcon = 0;
		wndclass.hCursor = 0;
		wndclass.hbrBackground = NULL;//m_hbrBackground;
		wndclass.lpszMenuName = 0;
		wndclass.lpszClassName = m_szClassName;
		wndclass.hIconSm = 0;
		 m_atomClass = RegisterClassEx(&wndclass);
	}
	InterlockedIncrement(&m_lInstanceCount);
}


CCalibrationWndBase::~CCalibrationWndBase()
{
	DestroyWindow(m_hWnd);

}

BOOL CCalibrationWndBase::Create()
{
	m_hWnd = ::CreateWindowEx(
		WS_EX_TOOLWINDOW,//dwExStyle,
		m_szClassName,
		m_szClassName,//windows name
		WS_POPUP,
		0,0,0,0,
		NULL,
		0,//Menu ID
		_AtlBaseModule.m_hInst,
		LPVOID(this));

	
	GetWindowPlacement(m_hWnd,&m_OldWndPlacement);
	return (m_hWnd != NULL);
}

//@功能:全屏显示函数
void CCalibrationWndBase::FullScreen(BOOL bFull)
{
	if(bFull)
	{
		GetWindowPlacement(m_hWnd,&m_OldWndPlacement);
		int nCxScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int nCyScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		RECT rcPosition;
		rcPosition.left   = 0;
		rcPosition.top    = 0;
		rcPosition.right  = nCxScreen;
		rcPosition.bottom = nCyScreen;	

        if(NULL == m_hWnd)
        {
            Create();
        }
		SetWindowPos(
			m_hWnd,
			HWND_TOPMOST, 
			//HWND_TOP, 
			rcPosition.left,
			rcPosition.top,
			rcPosition.right,
			rcPosition.bottom,
			SWP_SHOWWINDOW);


		////隐藏主框架
		//ShowWindow(m_hWnd,SW_HIDE);
		m_bFullScreen = TRUE;

		//SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);

	}
	else
	{
		SetWindowPlacement(m_hWnd,&m_OldWndPlacement);
		ShowWindow(m_hWnd,SW_HIDE);
		m_bFullScreen = FALSE;
	}

}



LRESULT CALLBACK CCalibrationWndBase::StaticWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
		if(uMsg == WM_CREATE)
		{
			 CREATESTRUCT* lpcs = reinterpret_cast< CREATESTRUCT*>(lParam);

			SetWindowLong(
				hWnd, 
              GWL_WNDPROC,
			  (LONG)MF2GF<WNDPROC>(&CCalibrationWndBase::WndProc, (CCalibrationWndBase*)lpcs->lpCreateParams));

		}

		return DefWindowProc(hWnd,uMsg,wParam,lParam);

}

LRESULT  CCalibrationWndBase::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

		return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


HWND CCalibrationWndBase::GetNotifyWnd()const
{
	return m_hNotifyWnd;
}

void CCalibrationWndBase::SetNotifyWnd(HWND hWnd)
{
	m_hNotifyWnd = hWnd;
}