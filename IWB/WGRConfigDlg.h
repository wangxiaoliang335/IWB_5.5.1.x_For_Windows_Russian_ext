#pragma once


// CWGRConfigDlg dialog

// CWGRConfigDlg dialog
class CWGRConfigDlg : public CDialog
{
    // Construction
public:
    CWGRConfigDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_WGRCONFIGTESTDEMO_DIALOG };

public:
    DWORD GetGeneralPermition() {return m_generalPermission;}

    bool HasGeneralPermissionChanged() {return m_hasGeneralPermitChanged;}

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    //////////////////////////////////////////////////////////////////////////
    afx_msg void OnBnClickedRadioGeneral();
    afx_msg void OnBnClickedRadioExceptional();
    afx_msg void OnBnClickedCheckButton();

    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedChkPall();
    afx_msg void OnBnClickedChkPclswnd();
    afx_msg void OnBnClickedChkPtitlewnds();
    afx_msg void OnBnClickedChkPmovwnd();
    afx_msg void OnBnClickedChkPrefwnd();
    afx_msg void OnBnClickedChkFall();
    afx_msg void OnBnClickedChkFmouwheel();
    afx_msg void OnBnClickedChkFdisdesktop();
    afx_msg void OnBnClickedChkFmaxicurwnd();
    afx_msg void OnBnClickedChkFminicurwnd();
    afx_msg void OnBnClickedChkFswitchwnd();
    afx_msg void OnLvnItemchangedListException(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnAddexception();
    afx_msg void OnBnClickedBtnDelexception();
    afx_msg void OnNMClickListException(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedChkLog();

    afx_msg void OnEnChangeEditMultPalm();

private:
    void InitRadioButton(void); //初始化单选按钮

    void InitOtherSetting(void);

    void InitExceptionList(void);   //初始化Exception列表控件
    void UpdateExceptionListState(void); //更新列表控件状态   

    void UpdateAddExceptionButtonState(void); //更新增加Exception按钮的状态
    void UpdateDelExceptionButtonState(void); //更新增加Exception按钮的状态

    void UpdateCheckStateByPermission(const DWORD &perm);

    void UpdateCheckStateByGeneralPermission(void);
    void UpdateGeneralPermissionByCheckState(void);

    void UpdateCheckStateByCurSelExceptionPermission(void);
    void UpdateCurSelExceptionByCheckState(void);

    void EnableAllCheckButton(bool bEnable);

    void MakeExceptionListCtrlSingleChecked(NMHDR *pNMHDR, LRESULT *pResult);
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

    void AfterTheCheckButtonClicked(DWORD theBtnID, E_TRIGGEREVENT evt)
    {
        DWORD perm = m_generalPermission;

        int nCheck = m_radioExceptional.GetCheck();
        if (nCheck == 1) perm = m_curSelExceptionPermission;

        CWGRPermissionParse::ConvertToPermissionSet(evt, IsTheCheckButtonChecked(theBtnID), perm); 

        if (nCheck == 1)
        {
            m_curSelExceptionPermission = perm;
        }
        else
        {
            m_generalPermission = perm;
        }

        UpdateCheckStateByPermission(perm);
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
    CListCtrl       m_exceptionList; 
    int             m_nCurCheckedItem;

    CButton         m_radioGeneral;
    CButton         m_radioExceptional;    

    CButton         m_btnAddException;
    CButton         m_btnDeletException;
    bool            m_radioGeneralHasChecked;

    /*
    WGR配置相关
    */
    DWORD           m_generalPermission;

    int             m_curSelException;
    std::string     m_curSelExceptionProcName;
    DWORD           m_curSelExceptionPermission;

    //int             m_nMultPalm;                //手掌满足手掌时，光斑质量与标准采样光斑质量的比例倍数
    //CSpinButtonCtrl m_sMultPalm;

    bool            m_bInited; 

    bool            m_hasGeneralPermitChanged; //用户是否更改了General Permission

    /*bool            m_bGeneralHasChanged;
    bool            m_bExceptionalHasChanged;*/    
};