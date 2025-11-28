#pragma once
#include "../../inc/ImageFrame.h"
#include "../../inc/NewFrameSink.h"
class CScreenAreaDetectorImpl:public INewFrameEventSink
{
public:
	CScreenAreaDetectorImpl();
	~CScreenAreaDetectorImpl();

	//@功 能:开启侦测,同时显示全屏窗口
	//@参 数:hNotifyWnd, 接收侦测结束通知的窗体句柄
	//@返回值;	
	BOOL StartDetect(HWND hNotifyWnd);
	
	//@功  能:结束侦测功能,
	//@参  数:
	//@返回指:
	BOOL End();


	//@功能:返回屏幕区域屏蔽位图
	const CImageFrame& GetMaskFrame()const;


	//@功能:新的图片帧事件响应
	virtual void OnNewFrame(CImageFrame* pFrame);



protected:
	//BOOL m_bIsDetecting;//正在侦测标志
	//白板窗体相关函数
	BOOL (CreateWindow)();

	//@功能:判断白板窗体是否可见
	BOOL IsBoardWindowVisible();

	HRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//状态机状态
	enum E_STAGE
	{
		E_STAGE_INITIAL = 0      ,//初始阶段
        E_STAGE_WHITE_FRAME      ,//白屏开始
		E_STAGE_BLACK_FRAME      ,//黑屏阶段
		E_STAGE_CALC             ,//计算阶段
		E_STAGE_END              ,//侦测结束
	}m_eStage;

	int  m_nStageCount;//阶段计数器
	static const int S_WAITCOUNT = 50;//

	BYTE m_cBrightness;//全屏窗体像素亮度, 0~255.

	//CImageFrame m_oGayFrame   ;//灰度图片帧
	CImageFrame m_oBlackFrame    ;//黑屏图片帧
	CImageFrame m_oWiteFrame     ;//白屏图片帧


	CImageFrame m_oMaskFrame    ;//屏蔽位图
	HWND        m_hBoardWnd     ;//
	HWND        m_hNotifyWnd    ;//消息通知窗口
	HINSTANCE   m_hInst         ;//资源实例句柄

	const TCHAR* m_szClassName   ;//窗体类名

};