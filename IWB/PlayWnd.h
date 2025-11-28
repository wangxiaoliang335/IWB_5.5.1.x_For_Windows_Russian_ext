#pragma once


// CPlayWnd

class CPlayWnd : public CWnd
{
	DECLARE_DYNAMIC(CPlayWnd)

public:
	CPlayWnd();
	virtual ~CPlayWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


