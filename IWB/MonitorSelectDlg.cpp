// MonitorSelectDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"
#include "MonitorSelectDlg.h"


// CMonitorSelectDlg dialog

IMPLEMENT_DYNAMIC(CMonitorSelectDlg, CDialog)

CMonitorSelectDlg::CMonitorSelectDlg(const CYUY2Frame&  captureFrame, CWnd* pParent)
	: CDialog(CMonitorSelectDlg::IDD, pParent)
{
    m_dib.Create(captureFrame.Width(), captureFrame.Height());
    //YUY2ToDIB(pYUY2Frame, nWidth, nHeight, m_oRealFrameDib.GetBits());
//	if(m_oRealFrameDib.GetWidth() != nWidth|| m_oRealFrameDib.GetHeight() != nHeight)
//	{
//		m_oRealFrameDib.Create(nWidth, nHeight);
//	}
    
    YUY2ToDIB((const unsigned char*)captureFrame.GetData(), captureFrame.Width(), captureFrame.Height(), m_dib.GetBits());
}

CMonitorSelectDlg::~CMonitorSelectDlg()
{
}

void CMonitorSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMonitorSelectDlg, CDialog)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CMonitorSelectDlg message handlers

void CMonitorSelectDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    // Do not call CDialog::OnPaint() for painting messages
    CRect rcDrawArea;
    GetDlgItem(IDC_STATIC_CAMERA_CAPTURE_IMAGE)->GetWindowRect(&rcDrawArea);

    ScreenToClient(&rcDrawArea);

    m_dib.Draw(
        dc.GetSafeHdc(),
        rcDrawArea.Width(),
        rcDrawArea.Height(),
        rcDrawArea.left,
        rcDrawArea.top);

}
