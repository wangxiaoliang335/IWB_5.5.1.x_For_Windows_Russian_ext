#pragma once


// CScrollablePropertyPage dialog

class CScrollablePropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScrollablePropertyPage)

public:
	explicit CScrollablePropertyPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	virtual ~CScrollablePropertyPage();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void ScrollClientArea(int nBar, int nPos);

	int m_nLastScrollPosX;
	int m_nLastScrollPosY;

	//void PreProcessPageTemplate(PROPSHEETPAGE& psp, BOOL bWizard);
	void PreProcessPageTemplate(PROPSHEETPAGE& psp, BOOL bWizard, LPCTSTR lpszFontFaceName, WORD wFontSize);

	friend class CCBPropertySheet;
};
