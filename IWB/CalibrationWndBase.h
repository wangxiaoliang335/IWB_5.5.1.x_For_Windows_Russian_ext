#pragma once
class CCalibrationWndBase
{
public:
	CCalibrationWndBase();

	virtual ~CCalibrationWndBase();

	//窗体生成函数
	virtual BOOL Create();


	//@功能:全屏显示函数
	void FullScreen(BOOL bFull);

	BOOL IsFullScreen()const{ return m_bFullScreen;}

	HWND GetNotifyWnd()const;

	void SetNotifyWnd(HWND hWnd);

	static LRESULT CALLBACK StaticWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LRESULT  WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	operator HWND()const{return m_hWnd;}


protected:
	static const TCHAR* m_szClassName;
	static ATOM  m_atomClass;
	static long   m_lInstanceCount;
	HWND m_hWnd;
	HWND m_hNotifyWnd;//通知窗体,负责接收校正结束的消息。

	WINDOWPLACEMENT m_OldWndPlacement;//窗体原始位置
	BOOL m_bFullScreen;//全屏标志
	
};