// PlayWnd.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"


// CPlayWnd

IMPLEMENT_DYNAMIC(CPlayWnd, CWnd)

CPlayWnd::CPlayWnd()
{

}

CPlayWnd::~CPlayWnd()
{
}


BEGIN_MESSAGE_MAP(CPlayWnd, CWnd)
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CPlayWnd message handlers



LRESULT CPlayWnd::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//TRACE(_T("CPlayWnd::OnNcHitTest\n"));
	return HTTRANSPARENT;
	return CWnd::OnNcHitTest(point);
}

BOOL CPlayWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	//TRACE(_T("CPlayWnd::OnSetCursor\n"));
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
