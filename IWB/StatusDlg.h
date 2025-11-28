#pragma once


// CStatusDlg dialog

class CStatusDlg : public CDialog
{
	DECLARE_DYNAMIC(CStatusDlg)

public:
	CStatusDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatusDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void UpdateFPSInfo(float fAvgFPS, float fRealFps);
};
