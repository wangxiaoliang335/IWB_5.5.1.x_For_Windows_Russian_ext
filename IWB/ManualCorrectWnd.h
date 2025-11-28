#pragma once
//自动校正人工补正窗体
#include "CalibrationWndBase.h"
#include "CalibrationDataDef.h"

//
class CManualCorrectWnd:public CCalibrationWndBase
{
public:
	CManualCorrectWnd();
	virtual ~CManualCorrectWnd();


	BOOL StartCorrect(HWND hNotifyWnd);


	BOOL EndCorrect(BOOL bSuccess);



	static BOOL LoadArrowBitmaps();


	const std::vector<CalibratePair>& GetCorrectData()const { return m_vecScreen2Image;}
	
protected:
	virtual LRESULT  WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnPaint();

	void DrawArrow(HDC hDC);
	void DrawFlashCircle(HDC hDC, int nRadius);
	enum ECornerPos
	{
		E_LEFT_TOP_CORNER     = 0,
		E_RIGHT_TOP_CORNER    = 1,
		E_LEFT_BOTTOM_CORNER  = 2,
		E_RIGHT_BOTTOM_CORNER = 3,
		E_CORNER_COUNT       =  4
	};


	enum E_CorrectStage
	{
		E_BEIGN_CORRECT,
		E_IS_CORRECTING,
		E_CORRECT_DONE,		
	}m_eCorrectStage;//校正阶段

	
	LRESULT OnManualCalibrateData(WPARAM wParam,LPARAM lParam);

	static const int ARROW_COUNT = 4;
	static HBITMAP m_aryBmpArrows[ARROW_COUNT];
	static BOOL    m_bInit;


	int   m_nCurrentCornerNo;
	POINT m_ptLastPos               ;//上一次的输入位置
	int   m_nInputWaitCount         ;//输入等待次数
	HDC   m_hMemDC                  ;//MemDC

	
	static const int MAX_ALLOWED_X_OFFSET = 5;
	static const int MAX_ALLOWED_Y_OFFSET = 5;

	static const int MAX_INPUT_WAIT_TIMES = 12;
	static const int MINIMUM_CORNER_DISTANCE = 100;
    static const int FALSH_SPOT_RADIUS = 5;//闪烁红点的半径
	
	std::vector<CalibratePair> m_vecScreen2Image;//计算机屏幕角点对应的视频坐标
    static const COLORREF BAKGND_COLOR = RGB(0,0,128);
};