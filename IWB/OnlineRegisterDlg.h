#pragma once

#define FEATURE_TOUCH_TYPE 1
#define FEATURE_SCREEN_TYPE 2
// COnlineRegisterDlg dialog

class COnlineRegisterDlg : public CDialog
{
	DECLARE_DYNAMIC(COnlineRegisterDlg)

public:
	COnlineRegisterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineRegisterDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ONLINE_REGISTER };

    BOOL IsRegisteredOk() const { return m_bRegisteredOk;}

    EDeviceTouchType GetTouchType() const { return m_eTouchType;}
    EScreenMode GetScreenMode() const { return m_eScreenMode;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetDefaultButton(UINT uID);
	DECLARE_MESSAGE_MAP()

    BOOL m_bRegisteredOk;//注册成功标志
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonOnlineRegister();
protected:
    CString m_strSN;
    CString m_strText;
    EDeviceTouchType m_eTouchType;
    EScreenMode      m_eScreenMode;
	virtual void OnCancel();
};
