#pragma once

// CGLBGRSettingDlg dialog

class CGLBGRSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CGLBGRSettingDlg)

public:
	CGLBGRSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGLBGRSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_GLBGRSETING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
    virtual BOOL OnInitDialog();
    afx_msg void OnEnChangeEditMultEraser();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedCheckStartglbgr();

private:
    void UpdateCheckBtnStateByGLBGRPermission();
    void InitOtherSetting(void);

private:
    void UpdateTheCheckButtonState(DWORD theBtnID, int nCheck)
    {
        CWnd *pWnd = GetDlgItem(theBtnID);

        if (pWnd)
        {
            ((CButton *)pWnd)->SetCheck(nCheck);
        }
    }

    bool IsTheCheckButtonChecked(DWORD theBtnID)
    {
        bool bChecked = false;

        CWnd *pWnd = GetDlgItem(theBtnID);

        if (pWnd)
        {
            bChecked = (((CButton *)pWnd)->GetCheck() == 1);
        }

        return bChecked;
    }

    void AfterTheCheckButtonClicked(DWORD theBtnID/*, E_TRIGGEREVENT evt*/)
    {        
        m_bGREnabled = IsTheCheckButtonChecked(IDC_CHECK_STARTGLBGR);
        EnableTheButton(IDOK, true);
    }

    void EnableTheCheckButton(DWORD theBtnID, bool bEnable)
    {
        EnableTheButton(theBtnID, bEnable);
    }

    void EnableTheButton(DWORD theBtnID, bool bEnable)
    {
        CWnd *pWnd = GetDlgItem(theBtnID);

        if (pWnd)
        {
            pWnd->EnableWindow(bEnable);
        }
    }

private:
    int             m_nMultEraser;                //手掌满足手掌时，光斑质量与标准采样光斑质量的比例倍数
    CSpinButtonCtrl m_sMultEraser;

    bool            m_bGREnabled;

    bool            m_bIsLogPermit;

    bool            m_bInited;     
};
