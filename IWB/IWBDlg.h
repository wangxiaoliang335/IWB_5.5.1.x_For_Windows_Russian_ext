// OpticalPenDlg.h : header file
//

#pragma once

//#include "FilterGraphBuilder.h"

#include "afxcmn.h"
#include "afxwin.h"
#include "StatusBarEx.h"
#include "InstallTip/TipProcServer.h"
#include "CameraFmtDialog.h"

class COwnerDrawMenu:public COwnerDrawMenuBase
{
public:
	//@功能:绘制自定义菜单项
	virtual void OnDrawMenuItem(LPDRAWITEMSTRUCT lpDrawItem)
	{
		BOOL bRet = FALSE;
		DWORD dwError = 0;
		RECT rcText;
		RECT rcImage;

		HMENU hMenu = GetMenuHandle(this->m_hMenu, lpDrawItem->itemID);
		if(hMenu == NULL) return;

		int  nItemWidth = lpDrawItem->rcItem.right  - lpDrawItem->rcItem.left;
		int nItemHeight = lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;

		SIZE sizeText   = GetMenuCaptionSize(lpDrawItem->itemID);
		SIZE sizeBitmap = GetItemBitmapSize(lpDrawItem->itemID);

		int cxMenuCheck = GetSystemMetrics(SM_CXMENUCHECK);
		int cyMenuCheck = GetSystemMetrics(SM_CYMENUCHECK);

		rcText.left   = lpDrawItem->rcItem.left    + cxMenuCheck;
		rcText.top    = lpDrawItem->rcItem.top     + (nItemHeight - sizeText.cy)/2;
		rcText.right  = rcText.left + sizeText.cx;
		rcText.bottom = lpDrawItem->rcItem.bottom  - (nItemHeight - sizeText.cy)/2;


		rcImage.right = lpDrawItem->rcItem.right;
		rcImage.left  = rcImage.right - sizeBitmap.cx;
		rcImage.top   = lpDrawItem->rcItem.top     + (nItemHeight - sizeBitmap.cy)/2;
		rcImage.bottom= lpDrawItem->rcItem.bottom  - (nItemHeight - sizeBitmap.cy)/2;
		

		TCHAR menuText[256];
		memset(menuText, 0, sizeof(menuText));
		GetMenuString(m_hMenu, lpDrawItem->itemID, menuText, _countof(menuText), MF_BYCOMMAND);

		HBITMAP hItemBitmap = GetMenuItemBitmap(lpDrawItem->itemID);
		HBITMAP hBmpOld = NULL;

        if(lpDrawItem->itemState & ODS_DISABLED)
        {
            SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_GRAYTEXT));
        }
        else
        {
            SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_MENUTEXT));
        }

		switch(lpDrawItem->itemAction)
		{
			case ODA_DRAWENTIRE:// This bit is set when the entire control needs to be drawn.

				DrawText(lpDrawItem->hDC, menuText, _tcslen(menuText), &rcText, DT_CENTER | DT_VCENTER);
				
				if(hItemBitmap)
				{
					hBmpOld = (HBITMAP)SelectObject(this->m_hMemDC, hItemBitmap);
			

					bRet = BitBlt(
						lpDrawItem->hDC,
						rcImage.left,
						rcImage.top,
						sizeBitmap.cx,
						sizeBitmap.cy,
						m_hMemDC,
						0,
						0,
						SRCCOPY);

				

					if(!bRet)
					{
						dwError = GetLastError();
					}

					SelectObject(this->m_hMemDC, hBmpOld);
				}

				TRACE(_T("OnDrawMenuItem itemAction = ODA_DRAWENTIRE\n"));
			break;

			case ODA_FOCUS:// This bit is set when the control gains or loses input focus. The itemState member should be checked to determine whether the control has focus.
				TRACE(_T("OnDrawMenuItem itemAction = ODA_FOCUS\n"));
			break;


			case ODA_SELECT://This bit is set when only the selection status has changed. The itemState member should be checked to determine the new selection state.
				
				{

					if(lpDrawItem->itemState & ODS_SELECTED)
					{
						HBRUSH  hSelectBrush = ::CreateSolidBrush(RGB(49, 106, 197));
						//背景填充
						FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hSelectBrush);
						INT oldBkMode = SetBkMode(lpDrawItem->hDC, TRANSPARENT);
						COLORREF clrOld = SetTextColor(lpDrawItem->hDC, RGB(255,255,255));

						//绘制文字
						DrawText(lpDrawItem->hDC, menuText, _tcslen(menuText), &rcText, DT_CENTER | DT_VCENTER);
						SetTextColor(lpDrawItem->hDC, clrOld);
						 SetBkMode(lpDrawItem->hDC, oldBkMode);
						::DeleteObject(hSelectBrush);

						//绘制图片
						if(hItemBitmap)
						{
							hBmpOld = (HBITMAP)SelectObject(this->m_hMemDC, hItemBitmap);
					

							bRet = TransparentBlt(
								lpDrawItem->hDC,
								rcImage.left,
								rcImage.top,
								sizeBitmap.cx,
								sizeBitmap.cy,
								m_hMemDC,
								0,
								0,
								sizeBitmap.cx,
								sizeBitmap.cy,
								GetSysColor(COLOR_MENU));

						

							if(!bRet)
							{
								dwError = GetLastError();
							}

							SelectObject(this->m_hMemDC, hBmpOld);
						}//


					}
					else
					{
						
						HBRUSH  hBackgroundBrush = ::CreateSolidBrush(GetSysColor(COLOR_MENU));
						//背景填充
						FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBackgroundBrush);



						//绘制文字
						DrawText(lpDrawItem->hDC, menuText, _tcslen(menuText), &rcText, DT_CENTER | DT_VCENTER);
						::DeleteObject(hBackgroundBrush);
						

						//绘制图片
						if(hItemBitmap)
						{
							hBmpOld = (HBITMAP)SelectObject(this->m_hMemDC, hItemBitmap);
					
							bRet = BitBlt(
								lpDrawItem->hDC,
								rcImage.left,
								rcImage.top,
								sizeBitmap.cx,
								sizeBitmap.cy,
								m_hMemDC,
								0,
								0,
								SRCCOPY);


							if(!bRet)
							{
								dwError = GetLastError();
							}

							SelectObject(this->m_hMemDC, hBmpOld);
						}//

					}
			
					
				}
			
				TRACE(_T("OnDrawMenuItem itemAction = ODA_SELECT\n"));
			break;


		}//switch

		//lpDrawItem
		switch(lpDrawItem->itemState)
		{
			case ODS_CHECKED://The menu item is to be checked. This bit is used only in a menu.
				TRACE(_T("OnDrawMenuItem itemState=ODS_CHECKED\n"));		
				break;

			case ODS_COMBOBOXEDIT://The drawing takes place in the selection field (edit control) of an owner-drawn combo box.
				TRACE(_T("OnDrawMenuItem itemState=ODS_COMBOBOXEDIT\n"));
				break;

			case ODS_DEFAULT://The item is the default item.
				TRACE(_T("OnDrawMenuItem itemState=ODS_DEFAULT\n"));
				break;

			case ODS_DISABLED://The item is to be drawn as disabled.
				TRACE(_T("OnDrawMenuItem itemState=ODS_DISABLED\n"));
				break;

			case ODS_FOCUS://The item has the keyboard focus.
				TRACE(_T("OnDrawMenuItem itemState=ODS_FOCUS\n"));
				break;

			case ODS_GRAYED://The item is to be grayed. This bit is used only in a menu.
				TRACE(_T("OnDrawMenuItem itemState=ODS_GRAYED\n"));
				break;

			case ODS_HOTLIGHT://Windows 98/Me, Windows 2000/XP: The item is being hot-tracked, that is, the item will be highlighted when the mouse is on the item.
				TRACE(_T("OnDrawMenuItem itemState=ODS_HOTLIGHT\n"));
				break;

			case ODS_INACTIVE://Windows 98/Me, Windows 2000/XP: The item is inactive and the window associated with the menu is inactive.
				TRACE(_T("OnDrawMenuItem itemState=ODS_INACTIVE\n"));
				break;

			case ODS_NOACCEL://Windows 2000/XP: The control is drawn without the keyboard accelerator cues.
				TRACE(_T("OnDrawMenuItem itemState=ODS_NOACCEL\n"));
				break;

			case ODS_NOFOCUSRECT://	Windows 2000/XP: The control is drawn without focus indicator cues.
				TRACE(_T("OnDrawMenuItem itemState=ODS_NOFOCUSRECT\n"));
				break;

			case ODS_SELECTED://The menu item's status is selected.
				TRACE(_T("OnDrawMenuItem itemState=ODS_SELECTED\n"));
				break;

		}//switch
	}

	//@功能:计算菜单项的尺寸
	virtual void OnMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItem)
	{
		BOOL bRet = FALSE;
		DWORD dwErr = 0;

		HMENU hMenu = GetMenuHandle(this->m_hMenu, lpMeasureItem->itemID);
		if(hMenu == NULL) return;

		SIZE sizeText   = GetMenuCaptionSize(lpMeasureItem->itemID);
		SIZE sizeBitmap = GetItemBitmapSize(lpMeasureItem->itemID);

		int cxMenuCheck = GetSystemMetrics(SM_CXMENUCHECK);
		int cyMenuCheck = GetSystemMetrics(SM_CYMENUCHECK);

		lpMeasureItem->itemWidth = cxMenuCheck + sizeText.cx + sizeBitmap.cx;
		lpMeasureItem->itemHeight = cyMenuCheck;

		if(long(lpMeasureItem->itemHeight)  < sizeText.cy)
		{
			lpMeasureItem->itemHeight = sizeText.cy;
		}

		if(long(lpMeasureItem->itemHeight)  < sizeBitmap.cy)
		{
			lpMeasureItem->itemHeight = sizeBitmap.cy;
		}
	}

protected:


};


// CIWBDlg dialog
class CIWBDlg : public CDialog
{
// Construction
public:
	CIWBDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OPTICALPEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


protected:
	 void   afx_msg OnSize(UINT nType, int cx, int cy);
	HRESULT afx_msg OnTrayNotifyMsg(WPARAM wParam,LPARAM lParam);
	HRESULT afx_msg OnManualCalibrationDone (WPARAM wParam,LPARAM lParam);
	//HRESULT afx_msg OnAbortManualCalibrating (WPARAM wParam,LPARAM lParam);
	HRESULT afx_msg OnDisplayChange  (WPARAM wParam,LPARAM lParam);
	HRESULT afx_msg OnSetDetectThreshold(WPARAM wParam, LPARAM lParam);

	HRESULT  afx_msg OnBreakSpotSetting(WPARAM wParam, LPARAM lParam);       //zhaown
    HRESULT  afx_msg OnFinshSpotSetting(WPARAM wParam, LPARAM lParam);
//	HRESULT  afx_msg OnSetDetectSpot(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnClose(WPARAM wParam,LPARAM lParam);
	//afx_msg void OnBnClickedButtonExit();
	afx_msg void OnMenuExit();
	afx_msg void OnCtxmenuShowMainWindow();
	afx_msg void OnCtxMenuManualCalibrate();

	afx_msg void OnMenuRun();
	afx_msg void OnMenuStop();
	//afx_msg void OnMenuManualCalibrate();
//	afx_msg void OnUpdateMenuStop(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateMenuRun(CCmdUI *pCmdUI);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnMenuAbout();
    afx_msg void OnMenuOnlineRegister();
	afx_msg void OnMenuStatus();
	afx_msg void OnMenuDisableOpticalPenControl();	
	//afx_msg void OnOpenCloseTest();
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	afx_msg void OnProperyPage();
	afx_msg void OnLangSelect();
	//afx_msg void OnMenuAutoCalibrate(UINT uID);
	afx_msg void OnStartMenuAutoCalibrate();
	afx_msg void OnCtxmenuAutorunAtSystemStartup();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMenuParameterSettings();

	//图像传感器快捷菜单处理函数
	afx_msg void OnSensorCtxMenu(UINT uID);
    afx_msg void OnGestureSettingMenu(UINT uID);

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//afx_msg void OnMaskAreaSpecify();
	//afx_msg void OnDisableManualSreenArea();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnAddMaskArea(UINT uID);
	afx_msg void OnEraseMaskArea(UINT uID);

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnMaskAreaEditActionRedo();
	afx_msg void OnMaskAreaEditActionUndo();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);

    afx_msg void OnVideoTuningMode();
    afx_msg void OnNormalUsageMode();
//	afx_msg void OnAutoMaskClutterTest();
	afx_msg void OnControlOn();
	afx_msg void OnControlOff();
	afx_msg void OnLaserTunningModel();
	afx_msg void OnMenuManualCalibrate();
	afx_msg void OnMenuManualCalibrate25();
	afx_msg void OnMenuManualCalibrate36();

	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

    afx_msg void OnOperationLightspotsampling();
    afx_msg void OnInstallationanddebuggingMouse();
    afx_msg void OnInstallationanddebuggingTouchpad();

    afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnMenuAutoAddScreenMask();
	afx_msg void OnNcDestroy();

	afx_msg void OnSpotCollectionSetting();
	afx_msg void OnSimulationManualCalibration();
	afx_msg void OnSpotCalibrationSetting();
 	afx_msg void OnEndManualMaskAreaEdit();
	afx_msg void OnClearMaskArea();
	afx_msg void OnCtxmenuAutoCalibrate();
    afx_msg void OnSwapSensorImage();
    afx_msg void OnSwapImageWithSensor(UINT uID);
    afx_msg void OnSwitchToFusionScreenMode(UINT uID);

	afx_msg void OnChangeTouchScreenAspectRatio(UINT uID);

	afx_msg void OnInstallationanddebuggingUpdatefirmware();
	afx_msg void OnEndSession(BOOL bEnding);

	afx_msg void OnInstallationanddebuggingEnableinterpolate();
	afx_msg void OnMenuAdvancessetting();

	afx_msg void OnMenuStartDrawOnlineScreenArea();
	afx_msg void OnMenuClearDrawOnlineScreenArea();

	afx_msg void OnMenuEnableDrawOnlineScreenArea();

	//触屏布局编辑工具
	afx_msg void OnMenuTouchScreenLayoutDesigner();

    void OnChangeSensorAreaNo(UINT uID);
//=====函数======
	void AdjustStatusBar(int cx, int cy);
	BOOL StartRunning();
	BOOL StopRunning();
	void DoAutoCalibrate();

    //@功能:载入配置文件
    //@参数:lpszConfigFilePath, 配置文件的完整路路径
    BOOL LoadConfig();

    //@功能:保存配置文件
    //@参数:lpszConfigFilePath, 配置文件的完成路径
    BOOL SaveConfig();
    void Exit();

//	afx_msg void OnBnClickedButtonStartStop();
	HRESULT OnFpsNotify         (WPARAM wParam,LPARAM lParam);
    HRESULT OnCameraStatusNotify(WPARAM wParam,LPARAM lParam);
//	HRESULT OnCalcAdjustMatrix(WPARAM wParam, LPARAM lParam);
	HRESULT OnDeviceChange(WPARAM wParam, LPARAM lParam);

	HRESULT OnAutoCalibrateDone(WPARAM wParam, LPARAM lParam);

	HRESULT OnAppCommMsg(WPARAM wParam, LPARAM lParam);

	//@功能:最小化到托盘中
	void MinimizeToTray();

	//@功能:取消屏幕矩形区域编辑模式
	void CancelScreenAreadEditMode();

	//@功能:更新手动指定的屏幕区域
	//void UpdateScreenMaskArea();

	//@功能:生成自绘光标
	void CreateOwnerCursor();

	virtual void OnCancel();
	virtual void OnOK();


	virtual BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnInitialStateDisableOpticalPenForClutter(WPARAM, LPARAM lParam);
	LRESULT OnChangeGuidelineYPos(WPARAM wParam, LPARAM lParam);
	LRESULT OnShowGuideline(WPARAM wParam, LPARAM lParam);

	LRESULT OnChangeSpotProportion(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeMultEraser(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeFixedbolbsettime(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeNormalUserBrightness(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeAutoMaskDetectThreshold(WPARAM wParam, LPARAM lParam);

	LRESULT OnSpotCollectionSetting_Par(WPARAM wParam, LPARAM lParam);
	LRESULT OnStateDisableOpticalPenForClutter(WPARAM, LPARAM lParam);

    LRESULT OnGraphNotify(WPARAM wParam, LPARAM lParam);

    //<<added by toxuke@gmail.com, 2013/04/19
 //   LRESULT OnChangeEnableGestrueRecognition(WPARAM wParam, LPARAM lParam);
 //	LRESULT OnChangeEnableGestureTouch(WPARAM wParam, LPARAM lParam);
 //	LRESULT OnChangeEnablePenTouch(WPARAM wParam, LPARAM lParam);
	LRESULT OnRecordvideo(WPARAM wParam, LPARAM lParam);

	LRESULT OnChangeAutoCalibrateAveBrightness(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeAutoCalibrateLightGray(WPARAM wParam, LPARAM lParam);
	LRESULT OnRearProjection(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeVideoDisplayDelay(WPARAM wParam, LPARAM lParam);


	//afx_msg void OnMenuAutoCalibrationWithHumanIntervention();
	//afx_msg void OnAutoSearchScreenArea();

	LRESULT OnSearchScreenAreaDone(WPARAM wParam, LPARAM lParam);
	//LRESULT OnManualCorrectDone(WPARAM wParam, LPARAM lParam);
	//LRESULT OnDisPlayWindow(WPARAM wParam, LPARAM lParam);


	//void Undo();
	//void Redo();

   //@功能;响应任务栏重新生成消息。
    LRESULT OnTaskbarRestartMsg(WPARAM, LPARAM lParam);

    //<<added by toxuke@gmail.com, 2014/12/18
    void OnAdvancedSettings(CIWBSensor* pSensor);
    //>>

    //<<added by jiqw, 2015/04/08
    void OnStartIntallerWithTips(CIWBSensor* pSensor);

    /*
    @功能：更改视频的显示窗口，主要是当安装调试模式时，视频会绘在另一个指定窗口上
    @参数：wParam，指定窗口的句柄
    */
    LRESULT OnChangeDisplayWindow(WPARAM wParam, LPARAM lParam);
    //>>

    /*
    @功能：在激光器模式下，检测是否挡板的模块
    */
    HRESULT OnProcessDetectBackSplashData(WPARAM wParam,LPARAM lParam);

    /*
    @功能：在安装向导过程中，开启/停止挡板是否消失
    */
    HRESULT OnStartDetectBackSplashVanished(WPARAM wParam,LPARAM lParam);
    HRESULT OnStopDetectBackSplashVanished(WPARAM wParam,LPARAM lParam);

    /*
    @功能：禁用或使能某控件
    */
    HRESULT OnEnableOrDisableTheWindow(WPARAM wParam, LPARAM lParam);


    void OnDisplayChangeHelper(const SIZE& sizeNewScreen);


	//@功能:应用传感器的配置
	HRESULT OnApplySensorConfig(WPARAM wParam, LPARAM lParam);


    //@功能:屏幕布局设计按钮事件响应函数
	//@参数:wParam, 虚拟按钮ID, 
    HRESULT OnScreenLayoutDesignBtnEvent(WPARAM wParam, LPARAM lParam);


    //@功能:”结束4点标定“消息响应函数
    HRESULT OnEnd4BasePointCalibrate(WPARAM wParam, LPARAM lParam);

	//2018/01/26
	//更新加密锁的状态
	void UpdateInfoAboutDongle();
	//
	BOOL LoadResolutionConfig();



	HRESULT OnPowerBroadcast(WPARAM wParam, LPARAM lParam);
//====变量===========


     BOOL m_bAllWndCreated  ;
	 CTrayImpl m_oTray      ;
	 HICON m_hAppIcon       ;
	//CString m_strProfilePath;//配置文件路径
	//CString m_strProfilePathSpot ;//设置模拟采集光斑的视频的文件的路径zhaown
	//BYTE    m_YThreshold    ;//检测器亮度门限
    


    
    //BOOL LoadCfgData();
	//BOOL SaveCfgData();

    //@功能:保存校正参数到配置文件中去
    //BOOL SaveCalibParams(const TCalibParams& calibParams, const TCHAR* szProfileName);

    //@功能:从配置文件中读取校正参数
    //BOOL LoadCalibParams(TCalibParams& calibParams, const TCHAR* szProfileName);



	//CSliderCtrl m_sldYDetectThreshold;
	//afx_msg void OnNMReleasedcaptureSliderYDetectThreshold(NMHDR *pNMHDR, LRESULT *pResult);
	//BOOL m_bEnableOpticalPenControl;//使能光笔控制
	
	
	//功能:视频捕获设备列表
	//void UpdateVideoCaptureDevicesList();

	////@功能:根据名称获取视频捕获设备数据实例
	//const TCaptureDeviceInstance* GetCaptureDevByName(LPCTSTR lpszCapDevName)const;


	////@功能:根据设备ID获取视频捕获设备数据实例
	//const TCaptureDeviceInstance* GetCaptureDevById(LPCTSTR lpszCapDevID)const;

	////@功能:根据设备的ID和Frendily名称查找设备实例
	//const TCaptureDeviceInstance* GetCaptureDevByIdAndFrendilyName(LPCTSTR lpszCapDevID, LPCTSTR lpszCapDevName)const;


	//@功能:根据视频格式名称获取指定设备实例的格式
	//const VideoMediaType* GetVideoFormat(const TCaptureDeviceInstance& devInst, LPCTSTR lpszVideoFormatName);


	////功能:获取第一个找到的符合缺省VID和PID的设备实例
	//const TCaptureDeviceInstance* GetDefaultUSBDev()const;


	////@功能:查找设备实例
	//const TCaptureDeviceInstance* FindDevIntance()const;


	////@功能:判断指定USB是否存在
	//BOOL IsUSBCameraInstanceExisted(const TCaptureDeviceInstance& devInst);

	
	//CaptuerDeviceInstanceSet m_videoCaptureSet             ;
	//CString                  m_strCaptureDeviceFriendlyName;
	//CString                  m_strCaptureDeviceID          ;
	//CString                  m_strFavoriteDevicePath       ;//优先选用的设备路径
	//CString                  m_strFavoriteFormatName       ;//优先选用的视频格式
	CUsbCameraDeviceList     m_oUSBCameraDeviceList        ;//视频设备列表
	CMenu                    m_oMenu                       ;
    CMenu                    m_oSubMenuMergMode;
	CMenu                    m_mnuManualScreenAreaSettings ;//2011/11/03,手动屏幕区域指定快捷菜单。
	CIWBSensor*              m_pSelectedSensor             ;//选中的图像传感器对象指针

	
	//CStatusDlg               m_ctlStatusBar                   ;//状态对话框
	CUSBDevDetector*         m_pUSBDevDetector             ;//USB即插即用检测
	CUSBDevDetector*         m_pUSBDevDetector_HID         ;
	UINT_PTR                 m_uOpenCloseTestTimer         ;//USB摄像头打开关闭测试定时器
	//CAutoCalibrator          m_oAutoCalibrator             ;
	//CScreenAreaDetector      m_oScreenAreaDetector         ;//屏幕区域自动侦测
	BOOL                     m_bVisible                    ;//对话框可见标志

	CPoint                   m_ptScreenAreaStart           ;//	
	CPoint                   m_ptScreenAreaEnd             ;//
	BOOL                     m_bScreenMaskAreaSelectDragging  ;//屏幕区域矩形拖拽中
	

	//屏蔽区域编辑相关变量和函数
	enum E_BMP_TYPE
	{
		e_BMP_BRUSH_1X,
		e_BMP_BRUSH_1D5X,
		e_BMP_BRUSH_2X,
		e_BMP_BRUSH_4X,
		e_BMP_ERASE_1X,
		e_BMP_ERASE_1D5X,
		e_BMP_ERASE_2X,
		e_BMP_ERASE_3X,
		e_BMP_NUMBER = 8
	};

	enum E_CURSOR_TYPE
	{
		e_CURSOR_BRUSH_1X,
		e_CURSOR_BRUSH_1D5X,
		e_CURSOR_BRUSH_2X,
		e_CURSOR_BRUSH_3X,
		e_CURSOR_ERASE_1X,
		e_CURSOR_ERASE_1D5X,
		e_CURSOR_ERASE_2X,
		e_CURSOR_ERASE_3X,
		e_CURSOR_NUMBER = 8
	};


    //程序启动时使用的摄像头视频参数类型
  //  enum E_VideoParamsMode
  //  {
  //      E_VIDEO_PARAMS_MODE_NORMAL_USAGE    ,//正常使用时的参数
  //      E_VIDEO_PARAMS_MODE_INTALLATION     ,//安装调试时使用的参数
		//E_VIDEO_PARAMS_MODE_INTALLATION1    ,//安装调试打板时使用的参数

  //      E_VIDEO_PARAMS_MODE_AUTO_CALIBRATION,//自动校正时使用的参数
  //      E_VIDEO_PARAMS_MODE_AUTO_MASKING    ,//自动屏蔽时时使用的参数
  //      E_VIDEO_PARAMS_MODE_NUMBER          ,//参数类型数目

  //  }m_eVideoParamsMode;


	CBitmap        m_aryMenuBmp  [e_BMP_NUMBER] ;//画刷位图数组。
	CColorCursor   m_aryCursor   [e_BMP_NUMBER] ;//光标数组

	HCURSOR        m_hScreenMaskAreaSelectCursor;//手动屏幕区域指定鼠标
	HCURSOR        m_hArrowCursor               ;//
    HCURSOR        m_hHandCursor                ;//手形光标

	COwnerDrawMenu m_oOwnerDrawMenu             ;//画刷自画菜单
	COwnerDrawMenu m_oOwnerDrawEaraseMenu       ;//橡皮擦自画菜单

	enum EScreenMaskAreaEditMode
	{
		e_SCREEN_MASK_AREA_EDIT_MODE_IDLE,              //空闲状态
		//e_SCREEN_MASK_AREA_EDIT_MODE_SELECT_SCREEN_AREA,//选取屏幕区域状态
		e_SCREEN_MASK_AREA_EDIT_MODE_ADD_MASK          ,//增加屏幕遮蔽区域
		e_SCREEN_MASK_AREA_EDIT_MODE_ERASE_MASK        ,//删除屏幕遮蔽区域
	}m_eScreenMaskAreaEditMode;

	enum EManualCalibrateMode
	{
		e_MANUALCALIBRATE_MODE_5,        //25点的手动校正
		e_MANUALCALIBRATE_MODE_6,        //36点的手动校正

	}m_eManualCalibrateMode;

	HCURSOR                 m_hCurrentEditToolCursor;
	E_AutoCalibratePattern  m_AutoCalibrateColsType;

	SIZE m_EraseSize;
	SIZE m_FillSize;

//	CPlayWnd m_PlayWnd;


	//屏蔽区域编辑操作撤销，重做相关变量和函数
	enum EScreenMaskAreaEditActionType
	{
		e_SCREEN_MASK_AREA_EDIT_ACTION_TYPE_FILL,//屏蔽区域填充操作
		e_SCREEN_MASK_AREA_EDIT_ACTION_TYPE_ERASE//屏蔽区域删除操作

	};
	struct TMaskAreaEditActions
	{
		EScreenMaskAreaEditActionType eScreenMaskAreaEditActionType;
		std::vector<RECT> vecArea;
	};

	std::stack<TMaskAreaEditActions> m_stackRedo;
	std::stack<TMaskAreaEditActions> m_stackUndo;

	//激光笔模式相关变量和函数
	ATOM                     m_atomLaserPwrOffHotkey       ;//光笔电源关闭指示热键
	ATOM                     m_atomLaserPwrOnHotkey        ;//光笔电源打开指示热键 
	ATOM                     m_atomTogglePenContolHotkey   ;//切换光笔控制热键
	ATOM                     m_atomTogglePenContolHotkey_2 ;//切换光笔控制热键2

	TMaskAreaEditActions m_tMaskEditAction;

	//afx_msg LRESULT OnClutterDetectionDone(WPARAM wParam, LPARAM lParam);



	BYTE* m_pRawInputBuf;
	UINT  m_nRawInputBufSize;
	//CManualCorrectWnd m_oManualCorrentWnd;

	//BOOL m_bNeedHumanIntervention;//需要人工干预标志
	//BOOL m_bNeedManualCalibrate  ;//需要手动校正
    //int  m_nAutoCalibrateTryTimes;//自动校正尝试次数
    //static const int MAX_AUTOCALIBRATE_TRY_TIMES = 2;//自动校正尝试次数。
	int m_nAutoMaskDetectThreshold;//自动屏蔽时的门限值
	BOOL m_bIsRecordVideo;
	BOOL m_bIsOpenNoProjectorMode;
	BOOL m_bIsRearProjection     ;//是否是背投模式


    struct TOldState
    {
       BOOL bEnableOpticalPen;//使能光笔的状态。
    }m_tOldState;
	

	//=======2013/04/13=============
	//Added by toxuke@gmail.com, 2013/04/03
	//<<
	//@功能:设置校正时的摄像头参数
	//void SetAutoCalibrationCameraParams();

	//@功能:设置正常使用时的摄像头参数
	//void SetNormalCameraParams();

	//@功能:设置添加屏蔽时的摄像头参数
	//void SetAutoMaskCameraParams();

    //@功能:设置安装调试时的摄像头参数
    //void SetAutoInstallationCameraParams();
    //@功能:设置安装挡板时的摄像头参数
	//void SetAutoInstallationCameraParams1();
	//>>

protected:
    void InitMenu();
    void AppendScreenModeSubmenu();
    void InsertMenuItem2TheMenu(CMenu *pMenu, const int &nInsertIndex, const DWORD &dwIMenuItemID, LPTSTR lpszMenuItemName);
    void InsertParamSettingMenuItem(CMenu *pMenu, const int &nInsertIndex);
    void InitDeviceUseModeMenuItemWithMenu(CMenu *pMenu);

    //@功能:其他运行实例启动时, 检测到已有实例存在，要求既有实例显示主窗体。
    HRESULT OnBetweenInstanceMsg(WPARAM wParam, LPARAM lParam);

    //@功能:判断是否是合法的USBkey
    BOOL IsEligibleUSBKey(UINT uVID, UINT uPID);
public:
    //<<aded by toxuke@gmail.com, 2013/05/06
    //任务栏重新生成事件消息
    static UINT m_uTaskbarRestartMsg;

    //自动校正过程中变更摄像头参数的回调函数
    static BOOL  OnAutoCalibChangeCameraParams(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1, BYTE param2);

    CIWBSensorManager m_oIWBSensorManager;//CMOS传感器管理者
    CStatusBarEx         m_ctlStatusBar;//状态栏
    CScreenRecognition m_oScreenRecognition                       ;//屏幕识别器
    UINT_PTR           m_uScreenRecognitionCloseTimer             ;//屏幕识别器关闭定时器
    static const int   TIMER_EVENT_SCREEN_RECOGNITION_CLOSE   = 1 ;//屏幕识别器关闭定时事件
    static const int   SCREEN_RECOGNITION_CLOSE_TIMEOUT = 1000*10 ;//10秒钟延迟

	//<<20170925
	static const int TIMER_FOR_SENSOR_MANAGER = 2                  ;//传感器管理器定时器
	static const int TIMER__FOR_SENSOR_MANAGER_TIMEOUT = 1000 * 1  ;//1秒钟定时
	//20170925>>


    //////////////////////////////////////////////////////////////////////////
    //安装提示处理模块
    CTipProcServer     m_tipProcServer;
    CIWBSensor         *m_pCurInstalledSensor;

    //<added by jiqw:20150416
    //<added reason:安装模式下，显示视频的窗口画在指定的窗口上
    HWND               m_hDispWnd;
    //RECT               m_rcDispWnd;
    //>
    
    //ICONINFO m_shieldIconInfo;

    HBITMAP m_hUCShieldBitmap;
	static UINT m_uAppCommMsg;
	///add by vera_zhao 2019.10.28
	bool    m_bStartDrawOnlineScreenArea;
	bool    m_bPreGuideRectangleVisible ;
	int     m_nDrawOnlineAreaCount      ;
	int     m_nActiveDetectCameraId     ;

    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnOperationFourpointcalibration();

	std::vector<CAtlString>      m_aryCandidateResolution;


    afx_msg void OnMenuCircleScreenManualCalibrate();
};
