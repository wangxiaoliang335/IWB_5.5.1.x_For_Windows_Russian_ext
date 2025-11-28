#pragma once
#include "Config.h"


// CAttachToScreen dialog

class CAttachToScreenDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttachToScreenDlg)

public:
	CAttachToScreenDlg(BOOL bAutoAttach, BOOL bDoubleScreenTouchMerge, int nMonitorId, EMonitorAreaType eMonitorAreaType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAttachToScreenDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ATTACH_SCREEN };
#endif

	BOOL IsAutoAttachMode() const { return m_bAutoAttach; }
	int GetMonitorId() const { return m_nMonitorId; }
	EMonitorAreaType GetMonitorAreaType() const 
	{
		return m_eMonitorAreaType;
	}
	BOOL IsDoubleScreenTouchMerge() const { return m_bDoubleScreenTouchMerge; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	
public:
	BOOL m_bAutoAttach;
	BOOL m_bDoubleScreenTouchMerge;
	int m_nMonitorId;
	EMonitorAreaType m_eMonitorAreaType;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckAutoAttachScreen();
	afx_msg void OnBnClickedCheckDoubleScreenTouchMerge();
};
