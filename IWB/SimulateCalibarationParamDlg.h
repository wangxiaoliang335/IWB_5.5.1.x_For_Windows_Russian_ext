#pragma once
#include "afxwin.h"


// CSimulateCalibarationParamDlg dialog

class CSimulateCalibarationParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CSimulateCalibarationParamDlg)

public:
	CSimulateCalibarationParamDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSimulateCalibarationParamDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SIMULATE_CALIBRATION };

	LPCTSTR GetAVIFilePath()const { return m_strAVIFilePath;}
	E_AutoCalibratePattern GetAutoCalibrateGridsNumber() const { return m_eAutoCalibrateGridsNumber;}
    UINT GetScreenWidth()const {return m_uCx;}
    UINT GetScreenHeight()const{return m_uCy;}
    BYTE GetImageAvgBrightness()const{return m_uImageAvgBrightness;}
    int GetSelectSensorId() const{ return m_nSelectSensorId;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowseForFilePath();
	CString m_strAVIFilePath;
	E_AutoCalibratePattern m_eAutoCalibrateGridsNumber;

	virtual BOOL OnInitDialog();
	CComboBox m_cbCalibrationType;

protected:
	virtual void OnOK();

protected:
    UINT m_uCx;//屏幕宽度
    UINT m_uCy;//屏幕高度
    afx_msg void OnBnClickedOk();
    // 校正时的画面亮度
    BYTE m_uImageAvgBrightness;
    // 选中的图像传感器
    int m_nSelectSensorId;
};
