#pragma once
#include "afxwin.h"

// CSpotSettingDlg dialog

class CSpotSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpotSettingDlg)

public:
	CSpotSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpotSettingDlg();

// Dialog Data
	enum { IDD = IDD_SPOTSETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	HWND m_hwnd;
	int m_x_Spot;
	int m_y_Spot;
	long m_s_Spot;
};
