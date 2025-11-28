#pragma once
//@类名, 自动校正窗口
class CAutoCalibrateWnd
{
public:
    CAutoCalibrateWnd(IAutoCalibratorInterface* pCalibrateInterface)
        :
    m_ptrAutoCalibrateInterface(pCalibrateInterface),
        m_szClassName(_T("OpticalPenAutoCalibrateWindow")),
        m_hWnd(NULL),
        m_hInst(NULL),
        m_bTimeout(TRUE),
		m_liLastTickCount(0)
    {
        WNDCLASSEX wndclass;
        wndclass.cbSize = sizeof wndclass;
        wndclass.style = 0;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        m_hInst = wndclass.hInstance = _AtlBaseModule.m_hInst;
        wndclass.hIcon = 0;
        wndclass.hCursor = 0;
        wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndclass.lpszMenuName = 0;
        wndclass.lpszClassName = m_szClassName;
        wndclass.hIconSm = 0;
        ATOM atom = RegisterClassEx(&wndclass);
    }

    ~CAutoCalibrateWnd()
    {
        FreeThunk(&CAutoCalibrateWnd::InternalWndProc, this);
        UnregisterClass(m_szClassName, m_hInst);
    }

    static HRESULT _stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);   
    }

    HRESULT InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);

                EndPaint(hWnd, &ps);
            }
            break;


        case WM_AUTO_CALIBRATE_DATA:
            {
				m_liLastTickCount = GetTickCount64();
                m_bTimeout = FALSE;
                //CYUY2Frame* pFrame = reinterpret_cast<CYUY2Frame*>(lParam);
                CImageFrame* pFrame = reinterpret_cast<CImageFrame*>(lParam);

                //AtlTrace(_T("WM_AUTO_CALIBRATE_DATA.\r\n"));
                if(m_ptrAutoCalibrateInterface)
                {
                    m_ptrAutoCalibrateInterface->FeedImage(pFrame);
                }


            }
            break;

        case WM_KEYUP:
            {
                switch(wParam)
                {
                case VK_ESCAPE:
                    if(IsVisible())
                    {
                        this->ShowWindow(SW_HIDE);
                        if(m_ptrAutoCalibrateInterface)
                        {
                            m_ptrAutoCalibrateInterface->Finish();
                        }
                    }
                    break;
                }

                //AtlTrace(_T("virtual key code=0x%x\r\n"), wParam);

            }

        case WM_TIMER:
            if(m_bTimeout)
            {
                if(m_ptrAutoCalibrateInterface)
                {
                    m_ptrAutoCalibrateInterface->Finish();
                }
            }
            else
            {
				LONGLONG liNow = GetTickCount64();
				LONGLONG liDelta = liNow - m_liLastTickCount;
				if ((int)liDelta > TIMEOUT_CONSTANT)
				{
                	m_bTimeout = TRUE;//进入下一次超时判断
				}
            }

            break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        };

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    BOOL (CreateWindow)(const RECT& rcPosition,BOOL bShow = TRUE)
    {
        BOOL bRet = TRUE;
        DWORD dwErr = 0;

        m_hWnd = ::CreateWindowEx(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,//dwExStyle,
            m_szClassName,
            m_szClassName,//windows name
            WS_POPUP,
            0,0,0,0,
            NULL,
            0,//Menu ID
            m_hInst,
            0);

        if(m_hWnd == NULL)
        {

            dwErr = GetLastError();
        }


        //int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
        //int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
        //RECT rcPosition;
        //rcPosition.left = 0;
        //rcPosition.top = 0;
        //rcPosition.right = nCxScreen;
        //rcPosition.bottom = nCyScreen;

        SetWindowPos(
            m_hWnd,
            HWND_TOPMOST,
            //HWND_TOP,
            rcPosition.left,
            rcPosition.top,
            rcPosition.right  - rcPosition.left,
            rcPosition.bottom - rcPosition.top,
            bShow?SWP_SHOWWINDOW:SWP_HIDEWINDOW);
        

        SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)MF2GF<WNDPROC>(&CAutoCalibrateWnd::InternalWndProc, this));

		m_rcPositionWnd = rcPosition;
        return bRet;
    };

    operator HWND() const
    {

        return m_hWnd;
    }

    HWND SetFocus()
    {
        if(m_hWnd)
        {
            return ::SetFocus(m_hWnd);
        }

        return NULL;
    }

    BOOL IsVisible()
    {
        return ::IsWindowVisible(m_hWnd);
    }

    BOOL ShowWindow(int nCmdShow)
    {
        return ::ShowWindow(m_hWnd, nCmdShow);
    }

    BOOL DestroyWindow()
    {
        BOOL bRet = ::DestroyWindow(m_hWnd);
        m_hWnd = NULL;
        return bRet;

    }

    void ResizeToFullScreen()
    {
        int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
        int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
        RECT rcPosition;
        rcPosition.left = 0;
        rcPosition.top = 0;
        rcPosition.right = nCxScreen;
        rcPosition.bottom = nCyScreen;

        SetWindowPos(
            m_hWnd,
            HWND_TOPMOST,
            //HWND_TOP,
            rcPosition.left,
            rcPosition.top,
            rcPosition.right  - rcPosition.left,
            rcPosition.bottom - rcPosition.top,
            SWP_SHOWWINDOW);
    }


    void MoveWindow(const RECT& rcPosition)
    {
        SetWindowPos(
            m_hWnd,
            HWND_TOPMOST,
            //HWND_TOP,
            rcPosition.left,
            rcPosition.top,
            rcPosition.right  - rcPosition.left,
            rcPosition.bottom - rcPosition.top,
            SWP_SHOWWINDOW);
    }

    //@参数:开启超时检测
    void StartTimeoutDetect()
    {
        if(m_uTimerID)
        {
            KillTimer(m_hWnd, 1);
            m_uTimerID = 0;
        }

        m_uTimerID = SetTimer(m_hWnd, 1, TIMEOUT_CONSTANT, NULL);
        m_bTimeout = FALSE;
		m_liLastTickCount = GetTickCount64();
    }

    void CloseTimeoutDetect()
    {
        if(m_uTimerID)
        {
            KillTimer(m_hWnd, 1);
            m_uTimerID = 0;
            m_bTimeout = FALSE;
        }

    }

	BOOL GetWindowsRect(RECT& rcMonitor)
	{
		rcMonitor = m_rcPositionWnd;
		return TRUE;
	}


protected:
    HWND m_hWnd;
    HINSTANCE m_hInst;
    const TCHAR* m_szClassName;
    IAutoCalibratorInterface* m_ptrAutoCalibrateInterface;

	static const INT TIMEOUT_CONSTANT = 1000;
    UINT m_uTimerID;//
    BOOL m_bTimeout;//超时标志
	LONGLONG m_liLastTickCount;

	RECT m_rcPositionWnd;
};