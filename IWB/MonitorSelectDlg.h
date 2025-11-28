#pragma once


// CMonitorSelectDlg dialog

class CMonitorSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CMonitorSelectDlg)

public:
	CMonitorSelectDlg(const CYUY2Frame&  CaptureFrame,CWnd* pParent = NULL);   // standard constructor
	virtual ~CMonitorSelectDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECT_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    //CYUY2Frame m_oCaptureFrame;
    CDib m_dib;
public:
    afx_msg void OnPaint();
};
