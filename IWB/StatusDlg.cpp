// StatusDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"


// CStatusDlg dialog

IMPLEMENT_DYNAMIC(CStatusDlg, CDialog)

CStatusDlg::CStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusDlg::IDD, pParent)
{

}

CStatusDlg::~CStatusDlg()
{
}

void CStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStatusDlg, CDialog)
END_MESSAGE_MAP()


// CStatusDlg message handlers

void CStatusDlg::UpdateFPSInfo(float fAvgFPS, float fRealFps)
{
	CString strText;
	fAvgFPS = fRealFps * 2.0f;
	strText.Format(_T("%2.1f"), fAvgFPS);
	GetDlgItem(IDC_STATIC_AVG_FPS)->SetWindowText(strText);

	fRealFps = fRealFps * 2.0f;
	strText.Format(_T("%2.2f"), fRealFps);
	GetDlgItem(IDC_STATIC_REALTIME_FPS)->SetWindowText(strText);

}
