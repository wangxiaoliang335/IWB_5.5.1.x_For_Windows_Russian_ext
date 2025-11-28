#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CameraFmtDialog dialog

class CameraFmtDialog : public CDialog
{
	DECLARE_DYNAMIC(CameraFmtDialog)

public:
	CameraFmtDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CameraFmtDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADANCEDSETTINGS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeCamerafmtCombo();
	afx_msg void OnBnClickedCheckHid();
	afx_msg void OnBnClickedCheckTuio();
	afx_msg void OnIpnFieldchangedIpaddressIp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPort();

	void    SetCameraResolution(std::vector<CAtlString>& CameraInfo, CAtlString  CurrentCameraInfo);
	CAtlString  GetSelectComboxvalue();
	bool    GetTouchHIDMode();
	bool    GetTouchTUIOMode();

	void   SetTUIOParams(DWORD IP, int nPort,int nScreenWindth ,int nScreenHeight);
	DWORD  GetIPAddress();
	int    GetPort();
	int    GetScreenWidth();
	int    GetScreenHeight();

	DECLARE_MESSAGE_MAP()

public:
	CComboBox  m_CCameraFmt;
	std::vector<CAtlString> CameraAllResution;
	CAtlString   m_sCurrentCameraResution;
	CString m_SelectedCameraFmtValue;

	BOOL m_bHIDMode;
	BOOL m_bTUIOMode;
	DWORD m_IPAddress;
	CString m_strPort;
	CIPAddressCtrl CIPAddress;
	CEdit CPortEdit;

	int m_nTUIOScreenWidth;
	int m_nTUIOScreenHeight;
	afx_msg void OnBnClickedButtonDefault();
};
