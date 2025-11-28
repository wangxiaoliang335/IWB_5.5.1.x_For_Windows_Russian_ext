#pragma once

#include "../inc/EEPROM/UVCToEEPROM.h"
// CUpdateFirmwareDlg dialog

class CUpdateFirmwareDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateFirmwareDlg)

public:
	CUpdateFirmwareDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpdateFirmwareDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UPDATE_FIRMWARE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowserFirmwareFile();
	afx_msg void OnBnClickedButtonWriteFirmware();
	afx_msg void OnBnClickedButtonReadFirmware();

	void AppendResultMsg(LPCTSTR lpszMsg);

	CUVCToEEPROM m_oUVCToEEPROM;
	UINT m_uMsgID;

	afx_msg void OnBnClickedButtonClearResult();
};
