// ScrollablePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "IWB.h"
#include "ScrollablePropertyPage.h"
#include "afxdialogex.h"


int  GetNewScrollPos(HWND hwnd, int scrollbar, UINT code)
{
	SCROLLINFO si = {};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	GetScrollInfo(hwnd, scrollbar, &si);

	const int minPos = si.nMin;
	const int maxPos = si.nMax - (si.nPage - 1);

	int result = -1;

	switch (code)
	{
	case SB_LINEUP /*SB_LINELEFT*/:
		result = max(si.nPos - 1, minPos);
		break;

	case SB_LINEDOWN /*SB_LINERIGHT*/:
		result = min(si.nPos + 1, maxPos);
		break;

	case SB_PAGEUP /*SB_PAGELEFT*/:
		result = max(si.nPos - (int)si.nPage, minPos);
		break;

	case SB_PAGEDOWN /*SB_PAGERIGHT*/:
		result = min(si.nPos + (int)si.nPage, maxPos);
		break;

	case SB_THUMBPOSITION:
		// do nothing
		break;

	case SB_THUMBTRACK:
		result = si.nTrackPos;
		break;

	case SB_TOP /*SB_LEFT*/:
		result = minPos;
		break;

	case SB_BOTTOM /*SB_RIGHT*/:
		result = maxPos;
		break;

	case SB_ENDSCROLL:
		// do nothing
		break;
	}

	return result;
}

// CScrollablePropertyPage dialog

IMPLEMENT_DYNAMIC(CScrollablePropertyPage, CPropertyPage)

CScrollablePropertyPage::CScrollablePropertyPage(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize)
	: CPropertyPage(nIDTemplate, nIDCaption, dwSize)
	, m_nLastScrollPosX(1)
	, m_nLastScrollPosY(1)
{

}

CScrollablePropertyPage::~CScrollablePropertyPage()
{
}

void CScrollablePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScrollablePropertyPage, CPropertyPage)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CScrollablePropertyPage message handlers


BOOL CScrollablePropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rcClient;
	GetClientRect(&rcClient);

	int cx = rcClient.Width();
	int cy = rcClient.Height();


	SCROLLINFO si = {};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nPos = si.nMin = 1;

	si.nMax =  cx;//宽度范围上限
	si.nPage = cx;//可视页面宽度
	
	//设置水平滚动条的初始滚动信息
	SetScrollInfo(SB_HORZ, &si, FALSE);


	si.nMax  = cy;//宽度范围上限
	si.nPage = cy;//可视页面宽度

	//设置垂直滚动条的初始滚动信息
	SetScrollInfo(SB_VERT, &si, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CScrollablePropertyPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (nType != SIZE_RESTORED && nType != SIZE_MAXIMIZED)
		return;

	SCROLLINFO si = {};
	si.cbSize = sizeof(SCROLLINFO);

	const int bar[] = { SB_HORZ, SB_VERT };
	const int page[] = { cx, cy };

	for (size_t i = 0; i < ARRAYSIZE(bar); ++i)
	{
		si.fMask = SIF_PAGE;
		si.nPage = page[i];

		SetScrollInfo(bar[i], &si, TRUE);

		si.fMask = SIF_RANGE | SIF_POS;
		GetScrollInfo(bar[i], &si);

		const int maxScrollPos = si.nMax - (page[i] - 1);

		// Scroll client only if scroll bar is visible and window's
		// content is fully scrolled toward right and/or bottom side.
		// Also, update window's content on maximize.
		//已经到了ScrollBar的最大调节位置，
		const bool bNeedToScroll =
			(si.nPos != si.nMin && si.nPos == maxScrollPos) ||
			(nType == SIZE_MAXIMIZED);

		if (bNeedToScroll)
		{
			ScrollClientArea(bar[i], si.nPos);
		}

	}//for
}


void CScrollablePropertyPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	const int nNewScrollPos = GetNewScrollPos(this->GetSafeHwnd(), SB_HORZ, nSBCode);

	if (nNewScrollPos == -1)
		return;

	//设置滚动滑块的位置
	SetScrollPos(SB_HORZ, nNewScrollPos, TRUE);

	//滚动客户区域
	ScrollClientArea(SB_HORZ, nNewScrollPos);

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CScrollablePropertyPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	const int nNewScrollPos = GetNewScrollPos(this->GetSafeHwnd(), SB_VERT, nSBCode);

	if (nNewScrollPos == -1)
		return;

	//设置滚动滑块的位置
	SetScrollPos(SB_VERT, nNewScrollPos, TRUE);

	//滚动客户区域
	ScrollClientArea(SB_VERT, nNewScrollPos);

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CScrollablePropertyPage::ScrollClientArea(int nBar, int nPos)
{
	int cx = 0, cy = 0;
	
	int& delta = nBar == SB_HORZ ? cx : cy;
	int& lastPos = nBar == SB_HORZ ? m_nLastScrollPosX : m_nLastScrollPosY;

	delta = lastPos - nPos;//窗体滚动方向于滑块移动方向相反

	lastPos = nPos;

	if (cx || cy)
	{
		ScrollWindow(cx, cy, NULL, NULL);
	}

}



void CScrollablePropertyPage::PreProcessPageTemplate(PROPSHEETPAGE& psp, BOOL bWizard, LPCTSTR lpszFontFaceName, WORD wFontSize)
{
	const DLGTEMPLATE* pTemplate;

	if (psp.dwFlags & PSP_DLGINDIRECT)
	{
		pTemplate = psp.pResource;
	}
	else
	{
		HRSRC hResource = ::FindResource(psp.hInstance, psp.pszTemplate, RT_DIALOG);
		if (hResource == NULL)
		{
			AfxThrowResourceException();
		}
		HGLOBAL hTemplate = LoadResource(psp.hInstance, hResource);
		if (hTemplate == NULL)
		{
			AfxThrowResourceException();
		}
		pTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);
		if (pTemplate == NULL)
		{
			AfxThrowResourceException();
		}
	}

	// if the dialog could contain OLE controls, deal with them now
	if (afxOccManager != NULL)
		pTemplate = InitDialogInfo(pTemplate);

	// set font of property page to same font used by property sheet
	//HGLOBAL hTemplate = _AfxChangePropPageFont(pTemplate, bWizard);

	CDialogTemplate dlgTemplate(pTemplate);
	dlgTemplate.SetFont(lpszFontFaceName, wFontSize);

	HGLOBAL hTemplate = (DLGTEMPLATE*)dlgTemplate.Detach();
	if (m_hDialogTemplate != NULL)
	{
		GlobalFree(m_hDialogTemplate);
		m_hDialogTemplate = NULL;
	}

	if (hTemplate != NULL)
	{
		pTemplate = (LPCDLGTEMPLATE)hTemplate;
		m_hDialogTemplate = hTemplate;
	}
	psp.pResource = pTemplate;
	psp.dwFlags |= PSP_DLGINDIRECT;
}