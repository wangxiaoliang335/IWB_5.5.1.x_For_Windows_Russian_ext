#pragma once

class CStatusBarEx : public CStatusBar
{
// Construction
public:
	CStatusBarEx();
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBarEx)
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusBarEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusBarEx)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	

};


