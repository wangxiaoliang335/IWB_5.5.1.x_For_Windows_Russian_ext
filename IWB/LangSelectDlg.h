#pragma once
#include "afxwin.h"


// CLangSelectDlg 对话框

class CLangSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CLangSelectDlg)

public:
	//CLangSelectDlg(LCID lcid, CWnd* pParent = NULL);   // 标准构造函数
    CLangSelectDlg(const TCHAR* lpszLangCode, CWnd* pParent = NULL);
	virtual ~CLangSelectDlg();

// 对话框数据
	enum { IDD = IDD_LANGUAGE_SELECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    
    virtual BOOL OnInitDialog();

	//LCID GetSelectLangLCID();

	BOOL IsLangChanged();

    const TCHAR* GetSelectedLangCode();
protected:
    virtual void OnOK();

    CListBox m_lstLangs;
 //   LCID     m_lcidOld;
	//LCID    m_lcidNew;
    CString m_strOldLangCode;
    CString m_strNewLangCode;
};

#define ID_SYS_LANG_SELECT 0xEFFFF
BOOL AddLangSelectToSystemMenu(HWND hWnd);
BOOL SaveResDllDefaultLang(HINSTANCE hResDll );
//HINSTANCE LoadResDll();
