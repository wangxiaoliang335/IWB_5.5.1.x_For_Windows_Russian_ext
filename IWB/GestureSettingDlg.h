#pragma once

#include "ScrollablePropertyPage.h"
// CGestureSettingDlg dialog

class CGestureSettingDlg : public CScrollablePropertyPage
{
	DECLARE_DYNAMIC(CGestureSettingDlg)

public:
	CGestureSettingDlg();   // standard constructor
	virtual ~CGestureSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_GESTURE_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    afx_msg void OnEnChangeEditMultPalm();
    afx_msg void OnEnChangeEditMultEraser();
    afx_msg void OnBnClickedCheckSallwgesture();
    afx_msg void OnBnClickedCheckSallglbgesture();
    afx_msg void OnBnClickedButtonWgrmore();
    afx_msg void OnBnClickedButtonGlbgrmore();

public:
   virtual BOOL OnApply();

private:
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

    void SetTheCheckButtonChecked(DWORD theBtnID, bool bCheck)
    {
        CWnd *pWnd = GetDlgItem(theBtnID);        

        if (pWnd)
        {
            int nCheck = bCheck ? 1 : 0;
            ((CButton *)pWnd)->SetCheck(bCheck);
        }       
    }

public:
    BOOL           m_isWGRPermitted;
    BOOL           m_isGLBGRPeritted;
    DWORD          m_dwWGRPermition;

    int            m_nPalmMulti;
    int            m_nEraserMulti;

    CSpinButtonCtrl m_sPalmMulti;
    CSpinButtonCtrl m_sEraserMulti;

    bool            m_bInited;       
};
