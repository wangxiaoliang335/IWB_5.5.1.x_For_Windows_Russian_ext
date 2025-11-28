// AddExceptionDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"
#include "AddExceptionDlg.h"


// CAddExceptionDlg dialog

IMPLEMENT_DYNAMIC(CAddExceptionDlg, CDialog)

CAddExceptionDlg::CAddExceptionDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAddExceptionDlg::IDD, pParent)
, m_procName(_T(""))
, m_appName(_T(""))
, m_pCurProcNameArry(NULL)
, m_nCurProcNames(0)
{

}

CAddExceptionDlg::~CAddExceptionDlg()
{
}

void CAddExceptionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    /*DDX_Text(pDX, IDC_EDIT_PROCNAME, m_procName);
    DDX_Text(pDX, IDC_EDIT_APPNAME, m_appName);*/
    DDX_Text(pDX, IDC_EDIT_APPNAME, m_appName);
    DDX_Control(pDX, IDC_COMBO_PROCNAMES, m_combProcNames);
}


BEGIN_MESSAGE_MAP(CAddExceptionDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CAddExceptionDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CAddExceptionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CAddExceptionDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    //
    InitProcNamesComboBox();

    m_pCurProcNameArry = NULL;
    m_nCurProcNames = 0;

    // TODO: Add extra initialization here    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// CAddExceptionDlg message handlers

void CAddExceptionDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    UpdateData();

    m_combProcNames.GetWindowText(m_procName);

    m_procName.MakeLower();    

    if (!m_procName.IsEmpty())
    {                
        if (m_procName.Find(_T(".exe")) == -1)
        {
            if (m_appName.IsEmpty())
            {
                m_appName = m_procName;              
            }

            m_procName += _T(".exe");
        }
        else
        {
            if (m_appName.IsEmpty())
            {
                CString tmpStr  = m_procName;
                m_appName = tmpStr.TrimRight(_T(".exe"));                
            }
        }

        UpdateData(FALSE);
    }

    OnOK();
}

void CAddExceptionDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    OnCancel();
}

void CAddExceptionDlg::SetProcessInfo(const char (*procNameArry)[256], const int &procNameCount)
{    
    m_pCurProcNameArry = procNameArry;
    m_nCurProcNames = procNameCount;
}

void CAddExceptionDlg::InitProcNamesComboBox()
{
    if (NULL == m_pCurProcNameArry || m_nCurProcNames == 0) return;

    for (int i = 0; i < m_nCurProcNames; i++)
    {
        if (strcmp("", m_pCurProcNameArry[i]))
        {
            m_combProcNames.AddString(CA2T(m_pCurProcNameArry[i]));
        }
    }    
}