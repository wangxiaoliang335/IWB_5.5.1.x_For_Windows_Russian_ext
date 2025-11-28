#include "stdafx.h"

CScreenRecognition::CScreenRecognition(COLORREF clrText)
{
#if 0
	int nCxVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int nCyVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int nXVScreen = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int nYVScreen = GetSystemMetrics(SM_YVIRTUALSCREEN);
#else
    int nCxVScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyVScreen = GetSystemMetrics(SM_CYSCREEN);
    int nXVScreen = 0;
    int nYVScreen = 0;

#endif

	m_rcWndArea.left = nXVScreen;
	m_rcWndArea.right = nXVScreen + nCxVScreen;

	m_rcWndArea.top = nYVScreen;
	m_rcWndArea.bottom = nYVScreen + nCyVScreen;


	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = (m_rcWndArea.bottom - m_rcWndArea.top);
	lf.lfWidth = 0;
	lf.lfWeight = FW_BOLD;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

	m_hFont = CreateFontIndirect(&lf);

	m_clrText = clrText;
	//m_hNotifyWnd = hNotifyWnd;
	///m_uNotifyMsg = uNotifyMsg;

	static int s_InstanceCount = 1;
	CAtlString strClassName;
	strClassName.Format(_T("Countdown Timer Window %d"), s_InstanceCount++);

	//m_hbrBackground = ::CreateSolidBrush(RGB(0,0,0));
	m_hbrBackground = ::CreateSolidBrush(RGB(0, 0, 0));
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof wndclass;
	wndclass.style = CS_SAVEBITS;
	wndclass.lpfnWndProc = StaticWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	m_hInst = wndclass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);;
	wndclass.hIcon = 0;
	wndclass.hCursor = 0;
	wndclass.hbrBackground = m_hbrBackground;
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = strClassName;
	wndclass.hIconSm = 0;
	ATOM  atom = RegisterClassEx(&wndclass);

	HDC hDCScreen = ::GetDC(GetDesktopWindow());
	m_hMemDC = ::CreateCompatibleDC(hDCScreen);
	m_hBitmap = ::CreateCompatibleBitmap(hDCScreen, m_rcWndArea.right - m_rcWndArea.left, m_rcWndArea.bottom - m_rcWndArea.top);

	::SetTextColor(m_hMemDC, m_clrText);
	SetBkMode(m_hMemDC, TRANSPARENT);

	::ReleaseDC(GetDesktopWindow(), hDCScreen);

	m_hBitmapOld = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
	m_hFontOld = (HFONT)::SelectObject(m_hMemDC, m_hFont);

    LOGPEN logPen;
    
    logPen.lopnStyle = PS_DASHDOTDOT;
//  logPen.lopnStyle = PS_DASH;
    logPen.lopnWidth = {DEFAULT_PEN_WIDTH, 0 };
    logPen.lopnColor = m_clrText;
    m_hPen = ::CreatePenIndirect(&logPen);

    m_hPenOld = (HPEN)::SelectObject(m_hMemDC, m_hPen);

	m_hWnd = ::CreateWindowEx(
		WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,//分层窗体。
		strClassName,
		strClassName,//windows name
		WS_POPUP,
		m_rcWndArea.left,
		m_rcWndArea.top,
		m_rcWndArea.right - m_rcWndArea.left,
		m_rcWndArea.bottom - m_rcWndArea.top,
		NULL,//owner window
		0,//Menu ID
		m_hInst,
		(LPVOID)this);

	SetLayeredWindowAttributes(
		this->m_hWnd, //A handle to the layered window.
		RGB(0, 0, 0), //specifies the transparency color key
		255,//the opacity of the layered window. 
		LWA_ALPHA |   //Use bAlpha to determine the opacity of the layered window.
		LWA_COLORKEY  //se crKey as the transparency color
	);




	//搜索收集屏幕信息
	//m_oDispMonitorFinder.SearchDisplayDev();
	theApp.GetMonitorFinder().SearchDisplayDev();

}

CScreenRecognition::~CScreenRecognition()
{
	::DestroyWindow(m_hWnd);//销毁
	FreeThunk(&CScreenRecognition::WndProc, this);

    ::SelectObject(m_hMemDC, m_hPenOld);
	::SelectObject(m_hMemDC, m_hBitmapOld);
	::SelectObject(m_hMemDC, m_hFontOld);
    
    ::DeleteObject(m_hPen);
	::DeleteObject(m_hFont);
	::DeleteDC(m_hMemDC);
	::DeleteObject(m_hbrBackground);

}


LRESULT  CScreenRecognition::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//AtlTrace(_T("msg=0x%x\n"), uMsg);

	switch (uMsg)
	{

	case WM_NCDESTROY:
	{


		break;
	}

	case WM_CREATE:
	{

		break;
	}

	case WM_SHOWWINDOW:

		break;

	case WM_ERASEBKGND:

		return 1;//erases the background
		break;

	case WM_NCPAINT:

		return 0;
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);

		BitBlt(
			hdc,
			0,
			0,
			rcClient.right - rcClient.left,
			rcClient.bottom - rcClient.top,
			m_hMemDC,
			0,
			0,
			SRCCOPY);


		EndPaint(hWnd, &ps);
		return 0;

		break;
	}

	case WM_TIMER:

		break;

	case WM_KEYDOWN:
		//ShowWindow(this->m_hWnd, SW_HIDE);
		break;

	case WM_LBUTTONDOWN:
		//ShowWindow(this->m_hWnd, SW_HIDE);

		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CScreenRecognition::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* lpcs = reinterpret_cast< CREATESTRUCT*>(lParam);

		SetWindowLong(
			hWnd,
			GWL_WNDPROC,
			(LONG)MF2GF<WNDPROC>(&CScreenRecognition::WndProc, (CScreenRecognition*)lpcs->lpCreateParams));
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void CScreenRecognition::DoRecoginition(const CIWBSensorManager* pIWBSensorManager)
{

	if (pIWBSensorManager == NULL) return;

	ShowWindow(this->m_hWnd, SW_SHOW);

	//for(int i=0; i < m_oDispMonitorFinder.GetDisplayDevCount(); i++)
    //
    int nSensorCount = pIWBSensorManager->GetSensorCount();


	for (int i = 0; i < nSensorCount;i++)
	{
		const CIWBSensor* pSensor = pIWBSensorManager->GetSensor(i);
		TSensorConfig sensorCfg = pSensor->GetCfgData();
		TCHAR szText[256];
		_stprintf_s(szText, _countof(szText), _T("%d"), i + 1);
		//const DisplayDevInfo* pDisplayDevInfo = m_oDispMonitorFinder.GetMointorInfo(i);
		//const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);
		
		RECT rcMonitor;
		BOOL bRet = pSensor->GetAttachedScreenArea(rcMonitor);

		if (!bRet) continue;

		FillRect(m_hMemDC, &rcMonitor, m_hbrBackground);

		DrawText(
			m_hMemDC,
			szText,
			_tcsclen(szText),
			&rcMonitor,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        //用虚线绘制屏幕分割线
        if (i != nSensorCount-1)
        {
            int nMonitorWidth  = rcMonitor.right  - rcMonitor.left;
            int nMonitorHeight = rcMonitor.bottom - rcMonitor.top;
            
            ::MoveToEx(m_hMemDC, rcMonitor.right - DEFAULT_PEN_WIDTH, rcMonitor.top, NULL);
            ::LineTo(m_hMemDC, rcMonitor.right - DEFAULT_PEN_WIDTH, rcMonitor.bottom);
        }

	}//for


	UpdateWindow(this->m_hWnd);

}
