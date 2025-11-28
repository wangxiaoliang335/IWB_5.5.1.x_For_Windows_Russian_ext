// DegugVideoLoader.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"
#include "DebugVideoLoaderDlg.h"


// CDebugVideoLoaderDlg dialog

IMPLEMENT_DYNAMIC(CDebugVideoLoaderDlg, CDialog)

CDebugVideoLoaderDlg::CDebugVideoLoaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugVideoLoaderDlg::IDD, pParent)
    , m_strVideoPath1(_T(""))
    , m_strVideoPath2(_T(""))
{

}

CDebugVideoLoaderDlg::~CDebugVideoLoaderDlg()
{
}

void CDebugVideoLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_strVideoPath1);
    DDX_Text(pDX, IDC_EDIT2, m_strVideoPath2);
}


BEGIN_MESSAGE_MAP(CDebugVideoLoaderDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_FILE_SELECT_01, &CDebugVideoLoaderDlg::OnBnClickedButtonFileSelect01)
    ON_BN_CLICKED(IDC_BUTTON_FILE_SELECT_2, &CDebugVideoLoaderDlg::OnBnClickedButtonFileSelect2)

    ON_BN_CLICKED(IDOK, &CDebugVideoLoaderDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDebugVideoLoaderDlg message handlers

void CDebugVideoLoaderDlg::OnBnClickedButtonFileSelect01()
{
    // TODO: Add your control notification handler code here
     CFileDialog fileDlg(TRUE, _T(".avi"), NULL, 4|2, _T("AVI FIle(*.avi)|*.avi|"), this);
     if(fileDlg.DoModal() == IDOK)
     {
        m_strVideoPath1 =  fileDlg.GetPathName();
        UpdateData(FALSE);
     }

}

void CDebugVideoLoaderDlg::OnBnClickedButtonFileSelect2()
{
    // TODO: Add your control notification handler code here
     CFileDialog fileDlg(TRUE, _T(".avi"), NULL, 4|2, _T("AVI FIle(*.avi)|*.avi|"), this);
     if(fileDlg.DoModal() == IDOK)
     {
        m_strVideoPath2 =  fileDlg.GetPathName();
        UpdateData(FALSE);
     }
}


void CDebugVideoLoaderDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}
