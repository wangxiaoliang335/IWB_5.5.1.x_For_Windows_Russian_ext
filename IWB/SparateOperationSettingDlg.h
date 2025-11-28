#pragma once

// CSparateOperationSettingDlg dialog
#include "ScrollablePropertyPage.h"
#include "afxwin.h"

class CSparateOperationSettingDlg : public CScrollablePropertyPage
{
	DECLARE_DYNAMIC(CSparateOperationSettingDlg)

public:
	CSparateOperationSettingDlg();   // standard constructor
	virtual ~CSparateOperationSettingDlg();


// Dialog Data
	enum { IDD = IDD_DIALOG_SEPARATE_OPERATION };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedCheckAiroperation();
	afx_msg void OnBnClickedRadioClick();
	afx_msg void OnBnClickedRadioDoubleclick();

public:
	virtual BOOL OnApply();

public:
	BOOL       m_IsAirOperationPermitted;
	int        m_nAPClick;
	bool       m_bInited;

};
