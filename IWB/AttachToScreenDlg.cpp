// AttachToScreen.cpp : implementation file
//

#include "stdafx.h"
#include "IWB.h"
#include "AttachToScreenDlg.h"
#include "afxdialogex.h"


// CAttachToScreenDlg dialog

IMPLEMENT_DYNAMIC(CAttachToScreenDlg, CDialog)

CAttachToScreenDlg::CAttachToScreenDlg(BOOL bAutoAttach, BOOL bDoubleScreenTouchMerge, int nMonitorId, EMonitorAreaType eMonitorAreaType, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_ATTACH_SCREEN, pParent)
	, m_bAutoAttach(bAutoAttach)
	, m_bDoubleScreenTouchMerge(bDoubleScreenTouchMerge)
	, m_nMonitorId(nMonitorId)
	, m_eMonitorAreaType(eMonitorAreaType)
{

}

CAttachToScreenDlg::~CAttachToScreenDlg()
{
}

void CAttachToScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_AUTO_ATTACH_SCREEN, m_bAutoAttach);
	DDX_Radio(pDX, IDC_RADIO_MONITOR01, m_nMonitorId);
	DDX_Radio(pDX, IDC_RADIO_AREA_FULL, (int&)m_eMonitorAreaType);
	DDX_Check(pDX, IDC_CHECK_DOUBLE_SCREEN_TOUCH_MERGE, m_bDoubleScreenTouchMerge);
}


BEGIN_MESSAGE_MAP(CAttachToScreenDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_AUTO_ATTACH_SCREEN, &CAttachToScreenDlg::OnBnClickedCheckAutoAttachScreen)
	ON_BN_CLICKED(IDC_CHECK_DOUBLE_SCREEN_TOUCH_MERGE, &CAttachToScreenDlg::OnBnClickedCheckDoubleScreenTouchMerge)
END_MESSAGE_MAP()


// CAttachToScreenDlg message handlers


BOOL CAttachToScreenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if (m_bAutoAttach)
	{
		GetDlgItem(IDC_RADIO_MONITOR01)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MONITOR02)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MONITOR03)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MONITOR04)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_AREA_FULL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AREA_LEFT_HALF)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AREA_RIGHT_HALF)->EnableWindow(FALSE);
	}
	else
	{
		//系统的实际屏监视器目少于4个, 则只使能存在的监视器
		int nMonitorCount = theApp.GetMonitorFinder().GetDisplayDevCount();

		if (nMonitorCount < 4)
		{
			for (int i = nMonitorCount; i < 4; i++)
			{
				GetDlgItem(IDC_RADIO_MONITOR01 + i)->EnableWindow(FALSE);
			}
		}


	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CAttachToScreenDlg::OnBnClickedCheckAutoAttachScreen()
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked(IDC_CHECK_AUTO_ATTACH_SCREEN))
	{

		GetDlgItem(IDC_RADIO_MONITOR01)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MONITOR02)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MONITOR03)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MONITOR04)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_AREA_FULL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AREA_LEFT_HALF)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_AREA_RIGHT_HALF)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_RADIO_MONITOR01)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MONITOR02)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MONITOR03)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MONITOR04)->EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_AREA_FULL)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_AREA_LEFT_HALF)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_AREA_RIGHT_HALF)->EnableWindow(TRUE);

		int nMonitorCount = theApp.GetMonitorFinder().GetDisplayDevCount();
		if (nMonitorCount < 4)
		{
			for (int i = nMonitorCount; i < 4; i++)
			{
				GetDlgItem(IDC_RADIO_MONITOR01 + i)->EnableWindow(FALSE);
			}
		}


	}
}


void CAttachToScreenDlg::OnBnClickedCheckDoubleScreenTouchMerge()
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked(IDC_CHECK_DOUBLE_SCREEN_TOUCH_MERGE))
	{

	}
	else
	{

	}
}
