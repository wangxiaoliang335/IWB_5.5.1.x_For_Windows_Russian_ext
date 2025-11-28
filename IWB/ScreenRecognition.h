#pragma once
//@功能:在各个屏幕上通过显示数字来识别不同的屏幕。
class CScreenRecognition
{
public:
    //@参数:clrText, 缺省的文字颜色。
	CScreenRecognition(COLORREF clrText = RGB(255, 0, 0));
	~CScreenRecognition();

	LRESULT  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	void DoRecoginition(const CIWBSensorManager* pIWBSensorManager);

    void StopRecognition()
    {
        ShowWindow(this->m_hWnd, SW_HIDE);
    }


protected:
    HBRUSH m_hbrBackground;
    HWND  m_hWnd          ;


    int   m_nStepTimeout  ;//倒计数定时间隔
    RECT  m_rcWndArea     ;//
    HFONT m_hFont         ;//计数显示的字体
    HFONT m_hFontOld      ;//
    COLORREF m_clrText    ;//字体颜色

    //static 	const TCHAR* m_szClassName;

    //static const int DEFAULT_WIDTH  = 100;
    //static const int DEFAULT_HEIGHT = 100;
    HINSTANCE m_hInst;
    HDC       m_hMemDC;
    HBITMAP   m_hBitmap;
    HBITMAP   m_hBitmapOld;
    HPEN      m_hPen;
    HPEN      m_hPenOld;
    //CDispMonitorFinder m_oDispMonitorFinder;
    static const LONG DEFAULT_PEN_WIDTH = 1;

};