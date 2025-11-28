#pragma once
#include "afxwin.h"
#include "ScrollablePropertyPage.h"
#include "afxcmn.h"
// CAdvancedSettingsDlg dialog
class CAdvancedSettingsDlg : public CScrollablePropertyPage
{
	DECLARE_DYNAMIC(CAdvancedSettingsDlg)

public:
	//CAdvancedSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	//CAdvancedSettingsDlg(const TAdvancedSettings* pSettings, CWnd* pParent = NULL);
    CAdvancedSettingsDlg();
    /*CAdvancedSettingsDlg(CIWBSensor* pSensor, const GlobalSettings& globalSettings, CWnd* pParent = NULL, LPCTSTR lpszAdditionalCaption=_T(""));*/
	virtual ~CAdvancedSettingsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADVANCED_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditAutoMaskInflateRadius();
	
	//int m_nMaskAreaInflationRadius;
	//int m_nGuideline1YPos;
	//int m_nGuideline2YPos;
	CSpinButtonCtrl m_spnDilateRadius;
	CSpinButtonCtrl m_spnGuideline1YPos;
	CSpinButtonCtrl m_spnGuideline2YPos;
	BOOL m_bGuideline1Show;
	BOOL m_bGuideline2Show;
	BOOL m_bInitDone;
	afx_msg void OnBnClickedButtonDefaultSettings();

	//int m_nSpotProportion;
	CSpinButtonCtrl m_ctlSpotProportion;

    //CSpinButtonCtrl m_ctlXCoordOffset;
    //CSpinButtonCtrl m_ctlYCoordOffset;

	//int m_nMultEraser;
	CSpinButtonCtrl m_cltMultEraser;

	//int m_nFixedBlobDefinedTime;
	CSpinButtonCtrl m_ctlFixedBlobDefinedTime;

	//int m_nNormalUseBrightness;
	CSpinButtonCtrl m_ctlNormalUserBrightness;

	//int m_nAutoCalibrationAveBrightness;
	CSpinButtonCtrl m_ctlAutoCalibrationAveBrightness;

	//int m_nAutoCalibrateHiLightGray;
	CSpinButtonCtrl m_ctlAutoCalibrateHiLightGray;

	//IDC_SPIN_VIDEODISPLAYDELAY
	CSpinButtonCtrl  m_ctlVideoIsplayDelay;


	CSpinButtonCtrl m_ctlInstallBrightness;
	CSpinButtonCtrl m_ctlInstallGramma;

    //<<added by toxuke@gmail.com, 2012/08/20
	//int m_nCalibrationDeviationXCompensation;
	//int m_nCalibrationDeviationYCompensation;

	//CSpinButtonCtrl m_spnCalibrationHorzCompensation;
	//CSpinButtonCtrl m_spnCalibrationVertCompensation;
    //added by toxuke@gmail.com, 2012/08/20>>

	afx_msg void OnBnClickedButtonManualCollectSpot();
    //BOOL m_bEnableGestureRecognition;
 //   afx_msg void OnBnClickedCheckGestureRecognition();
	afx_msg void OnEnChangeEditFixedBlobSetTimeValue();

	afx_msg void OnEnChangeEditSpotProportion();

    afx_msg void OnEnChangeEditXCoordOffset();
    afx_msg void OnEnChangeEditYCoordOffset();

	afx_msg void OnEnChangeEditMultEraser();
	afx_msg void OnEnChangeEditSetNormalUserBrightness();
	afx_msg void OnEnChangeEditAutomaskdetectthreshold();
	afx_msg void OnBnClickedRadioFingerTouch();
	afx_msg void OnBnClickedRadioPenTouch();

	afx_msg void OnBnClickedCheckRecordVideo();

	afx_msg void  OnBnClickedRadioWallMode();
	afx_msg void  OnBnClickedRadioDeskTopMode();

	virtual BOOL OnApply();

	//int m_nEnableGestureTouch;

	CSpinButtonCtrl m_sAutoMaskDetectThreshold;
	//int m_nAutoMaskDetectThreshold;
	//int m_nThreshold;                  //亮度门限

	//BOOL m_bRecordVideo;
	//BOOL m_bDoubleScreen;
	//BOOL m_bRearProjector;


	afx_msg void OnEnChangeEditSetAutocalibrateAverageBrightness();
	afx_msg void OnEnChangeEditAutocalibrateHilightGray();
	afx_msg void OnBnClickedCheckRearProjector();
    //afx_msg void OnBnClickedScreenSelect();
    afx_msg void OnBnClickRadioLensType(UINT uID);

    const TSensorConfig&   GetSensorConfig()const;
    const GlobalSettings& GetGlobalSettings()const;

	afx_msg void OnBnClickedButtonAttachToAScreen();
	afx_msg void OnBnClickedCheckDynamicmaskframecontrol();
	afx_msg void OnBnClickedCheckAntijammingcontrol();

	afx_msg void OnBnClickedCheckSinglepointmode();
	afx_msg void OnBnClickedCheckDisablereflectionpoint();
	afx_msg void OnEnChangeEditVideodisplaydelay();
	afx_msg void OnChangeEditInstallbrightness();
	afx_msg void OnChangeEditInstallgamma();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    TSensorConfig m_tSensorConfig;
	GlobalSettings m_tGlobalSettings;	  
 


    //CComboBox m_ctlMonitorList;
    afx_msg void OnCbnSelchangeComboMonitorList();

    //CListBox m_ctlListMonitors;
    CListCtrl m_ctlListMonitors;

	CString   m_strAdditionalCaption;
  
    CScreenRecognition m_oScreenRecognition;//屏幕识别类。
    CIWBSensor*        m_pSensor           ;//指向传感器的指针
    
	//CSliderCtrl m_ctrlSliderSmooth;
	CEdit m_ctrlEditSmoothCoef;
	CSpinButtonCtrl m_ctrlSpinSmoothCoef;
};
