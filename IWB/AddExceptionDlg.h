#pragma once
#include <string>
#include <map>

// CAddExceptionDlg dialog

class CAddExceptionDlg : public CDialog
{
    DECLARE_DYNAMIC(CAddExceptionDlg)

public:
    CAddExceptionDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CAddExceptionDlg();

    // Dialog Data
    enum { IDD = IDD_DLG_ADDEXCEPTION };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    void SetProcessInfo(const char (*procNameArry)[256], const int &procNameCount);

private:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();

    void InitProcNamesComboBox();

public:
    CString m_procName;
    CString m_appName;   

private:
    CComboBox m_combProcNames;
    //const std::map<DWORD, std::string> *m_pProcInfoMap;    

    //const std::string *m_pCurProcNameArry;
    const char (*m_pCurProcNameArry)[256];
    int         m_nCurProcNames;

};
