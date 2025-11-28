// ColoredStatusBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "StatusBarEx.h"
struct TAttribute
{
	const TCHAR* pName;
	UINT   uNameLength;
	const TCHAR* pValue;
	UINT   uValueLength;
};

struct TColorMapEntry
{
	const TCHAR* szColorName;
	COLORREF clr;
};

static const TColorMapEntry colorMaps[] = {

	{_T("black"   ), RGB(0,     0,    0)},
	{_T("red"     ), RGB(255,   0,    0)},
	{ _T("green"  ), RGB(0,   255,    0)},
	{ _T("blue"   ), RGB(0,     0,  255)},
	{ _T("yellow" ), RGB(255,   0,  255)},
	{ _T("cyan"   ), RGB(0,   255,  255)},
	{ _T("magenta"), RGB(255,   0,  255)},
	{ _T("white"  ), RGB(255, 255,  255)},
};

//@参数:
//     szColor, 表示颜色的字符串
//     nLength, 表示颜色的字符串的长度
//     clr, 返回的颜色
//
//@功能:
//		成功, 返回TRUE
//      失败，返回FALSE
BOOL GetColorFromString(const TCHAR* szColor, int nLength, COLORREF& clr)
{
	if (-1 == nLength)
	{
		nLength = _tcslen(szColor);
	}

	for (int i = 0; i < _countof(colorMaps); i++)
	{
		if (0 == _tcsnicmp(szColor, colorMaps[i].szColorName, nLength))
		{
			clr = colorMaps[i].clr;
			return TRUE;
		}

	}//for

	return FALSE;
}
//"content text/color:green/bgcolor:black"
//szAttributes, e.g. "color:green/bgcolor:black"
void ParseAttributes(const TCHAR* szAttributes, TAttribute* attributes, int* pSize)
{
	const TCHAR* pCursor = szAttributes;
	int mode = 0;//0:key name, 1: key value
	int nFound = 0;

	const TCHAR* pName = pCursor;
	const TCHAR* pValue = pCursor;
	TCHAR KEY_VALUE_SEPARATOR = _T(':');
	TCHAR ATTRIBUTE_SEPARATOR = _T('/');

	while (*pCursor)
	{
		switch (mode)
		{
			case 0://key name				
				for (; ;)
				{
					if (KEY_VALUE_SEPARATOR == *pCursor || _T('\x0') == *pCursor)
					{
						if(nFound < *pSize)
						{ 
							attributes[nFound].pName = pName;
							attributes[nFound].uNameLength = pCursor - pName;
							
						}
						

						if (_T('\x0') != *pCursor)
						{
							mode = 1;
							pCursor++;
							pValue = pCursor;
						}
					
						break;
					}
					pCursor++;
				}

				

				break;

			case 1://key value

				for (; ;)
				{
					if (ATTRIBUTE_SEPARATOR == *pCursor || _T('\x0') == *pCursor)
					{
						attributes[nFound].pValue = pValue;
						attributes[nFound].uValueLength = pCursor - pValue;

						if (_T('\x0') != *pCursor)
						{
							pCursor++;
							pName = pCursor;
							mode = 0;//search key name
						}
						nFound++;
						
						break;
					}
					pCursor++;
				}

				break;
		}//switch

	}//while

	*pSize = nFound;

}
/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx

CStatusBarEx::CStatusBarEx()
{

}

CStatusBarEx::~CStatusBarEx()
{
}


BEGIN_MESSAGE_MAP(CStatusBarEx, CStatusBar)
	//{{AFX_MSG_MAP(CStatusBarEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx message handlers

void CStatusBarEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Attach to a CDC object
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	dc.SetBkMode(TRANSPARENT);

	// Get the pane rectangle and calculate text coordinates
	CRect rect(&lpDrawItemStruct->rcItem);
	
	CString strText = GetPaneText(lpDrawItemStruct->itemID);

	//字符串示例:
	//"<sometext>/color:red/bgcolor:black"
	//通过附加的属性指定文本颜色和文本颜色.
	//后续可以通过"/icon:0xxxxx"指定图标id
	//分隔符'/'
	const TCHAR* pText = strText;
	int nTextLength = strText.GetLength();

	const TCHAR* pAttribute = NULL;

	const TCHAR* pCursor = pText;

	while (*pCursor)
	{
		if (*pCursor == _T('/'))
		{
			pAttribute  = pCursor + 1;
			nTextLength = pCursor - pText;
			break;
		}

		pCursor++;
	}//while



	BOOL bChangeTextColor = FALSE;
	BOOL bChangeBkColor   = FALSE;
	COLORREF clrTextColor, clrBgColor;

	if (pAttribute)
	{
		const int MAX_ATTRIBUTE_COUNT = 4;
		TAttribute attrs[MAX_ATTRIBUTE_COUNT];
		int nCount = _countof(attrs);
		ParseAttributes(pAttribute, attrs, &nCount);	
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; i++)
			{

				if (0 == _tcsnicmp(_T("color"), attrs[i].pName, attrs[i].uNameLength))
				{
					if (GetColorFromString(attrs[i].pValue, attrs[i].uValueLength, clrTextColor))
					{
						bChangeTextColor = TRUE;
					}


				}
				else if (0 == _tcsnicmp(_T("bgcolor"), attrs[i].pName, attrs[i].uNameLength))
				{

					if (GetColorFromString(attrs[i].pValue, attrs[i].uValueLength, clrBgColor))
					{
						bChangeBkColor = TRUE;
					}
				}

			}//for

		}//if
	}//if

	if (bChangeTextColor)
	{
		dc.SetTextColor(clrTextColor);
	}
	
	if (bChangeBkColor)
	{
		dc.SetBkColor(clrBgColor);
	}
	
	CFont* pFont = dc.GetCurrentFont();

	LOGFONT LogFont;
	CFont  fontBold;
	CFont* fontOld;

		//  Get log font for the given window
		if ((pFont != NULL) && (pFont->GetLogFont(&LogFont)))
		{
			//  Change the weight to bold
			LogFont.lfWeight = FW_BOLD;

			//  Set the bold font for the given window
			fontBold.CreateFontIndirect(&LogFont);

			fontOld = dc.SelectObject(&fontBold);
		}


	dc.TextOut(rect.left + 2, rect.top, strText, nTextLength);

	dc.SelectObject(&fontOld);
	// Detach from the CDC object, otherwise the hDC will be
	// destroyed when the CDC object goes out of scope
	dc.Detach();
}
