/*****************************************************************************************
 File:     CBPropertySheet.cpp
 Author:   Ovidiu Cucu - Microsoft MVP Viusal C++ 
                         Codeguru nickname: ovidiucucu
                         Homepage: www.codexpert.ro
 Updated:  January 02, 2010
 Contents: CCBPropertySheet class implementation.
 Remarks:  CCBPropertySheet is an MFC-extension class for creating properties sheets
           which are using a custom font.
******************************************************************************************/
#include "stdafx.h"

#include "CBPropertySheet.h"
#include "ScrollablePropertyPage.h"
#include <AFXPRIV.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WORD __declspec(thread) CCBPropertySheet::m_wFontSize = 0;
LPCTSTR __declspec(thread) CCBPropertySheet::m_pszFontFaceName = NULL;

IMPLEMENT_DYNAMIC(CCBPropertySheet, CPropertySheet)

/*****************************************************************************************
 Function:   CCBPropertySheet::CCBPropertySheet
 Purpose:    Constructs a CCBPropertySheet object.
 Parameters: - nIDCaption: Resource ID of the caption to be used for the property sheet.
             - pParentWnd: Parent window (default NULL).
             - iSelectPage: Index of the page that will initially be on top (default 0).
******************************************************************************************/
CCBPropertySheet::CCBPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

/*****************************************************************************************
 Function:   CCBPropertySheet::CCBPropertySheet
 Purpose:    Constructs a CCBPropertySheet object.
 Parameters: - pszCaption: The string to be displayed on the property sheet caption. 
             - pParentWnd: Parent window (default NULL).
             - iSelectPage: Index of the page that will initially be on top (default 0).
******************************************************************************************/
CCBPropertySheet::CCBPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

/*****************************************************************************************
 Function:   CCBPropertySheet::~CCBPropertySheet
 Purpose:    Destructs a CCBPropertySheet object.
******************************************************************************************/
CCBPropertySheet::~CCBPropertySheet()
{
}

/*****************************************************************************************
 Function:   CCBPropertySheet::PropSheetProc
 Purpose:    Application-defined callback function that the system calls when the property 
             sheet is being created and initialized
 Parameters: - hWndDlg: Handle to the property sheet dialog box.
             - uMsg: Message being received
             - lParam: Additional information about the message.
 Remarks:    If uMsg is PSCB_PRECREATE, lParam is the address of a dialog template in 
             memory. This template is in the form of a DLGTEMPLATE or DLGTEMPLATEEX 
             structure followed by one or more DLGITEMTEMPLATE structures.
******************************************************************************************/
int CALLBACK CCBPropertySheet::PropSheetProc(HWND hWndDlg, UINT uMsg, LPARAM lParam)
{
   switch(uMsg)
   {
   case PSCB_PRECREATE://Property Sheet Callback prior to creation
      {
         if((m_wFontSize > 0) && (NULL != m_pszFontFaceName))
         {
            
            LPDLGTEMPLATE pResource = (LPDLGTEMPLATE)lParam;

            CDialogTemplate dlgTemplate(pResource);
            dlgTemplate.SetFont(m_pszFontFaceName, m_wFontSize);
            
			memmove((void*)lParam, dlgTemplate.m_hTemplate, dlgTemplate.m_dwTemplateSize);
         }
		
      }
      break;
   }
   return 0;
}

/*****************************************************************************************
 Function:   CCBPropertySheet::Create
 Purpose:    Creates a modeless property sheet.
 Parameters: - pszFontFaceName: Font face name.
             - wFontSize: Font size.
             - pParentWnd: parent window (default NULL).
             - dwStyle: style (default -1).
             - dwExStyle: extended style (default 0).
 Remarks:    Call this function for creating modeless property sheets with CUSTOM FONT.
******************************************************************************************/
BOOL CCBPropertySheet::Create(LPCTSTR pszFontFaceName, WORD wFontSize, CWnd* pParentWnd, 
                              DWORD dwStyle, DWORD dwExStyle)
{
   Init(pszFontFaceName, wFontSize);
   return CPropertySheet::Create(pParentWnd, dwStyle, dwExStyle);
}

/*****************************************************************************************
 Function:   CCBPropertySheet::Create
 Purpose:    Creates a modeless property sheet.
 Parameters: - pParentWnd: parent window (default NULL).
             - dwStyle: style (default -1).
             - dwExStyle: extended style (default 0).
 Remarks:    Call this function for creating modeless property sheets with DEFAULT FONT.
******************************************************************************************/
BOOL CCBPropertySheet::Create(CWnd* pParentWnd, DWORD dwStyle, DWORD dwExStyle)
{
   Init(NULL, 0);
   return CPropertySheet::Create(pParentWnd, dwStyle, dwExStyle);
}

/*****************************************************************************************
 Function:   CCBPropertySheet::DoModal
 Purpose:    Creates a modal property sheet.
 Parameters: - pszFontFaceName: Font face name.
             - wFontSize: Font size.
 Remarks:    Call this function for creating modal property sheets with CUSTOM FONT.
******************************************************************************************/
DOMODAL_RET CCBPropertySheet::DoModal(LPCTSTR pszFontFaceName, WORD wFontSize)
{
   Init(pszFontFaceName, wFontSize);
   return CPropertySheet::DoModal();
}

/*****************************************************************************************
 Function:   CCBPropertySheet::DoModal
 Purpose:    Creates a modal property sheet.
 Remarks:    Call this function for creating modal property sheets with DEFAULT FONT.
******************************************************************************************/
DOMODAL_RET CCBPropertySheet::DoModal()
{
   Init(NULL, 0);
   return CPropertySheet::DoModal();
}

/*****************************************************************************************
 Function:   CCBPropertySheet::Init
 Purpose:    Initializes font info as well as PROPSHEETHEADER structure in order to 
             use application-defined callback function (CCBPropertySheet::PropSheetProc).
 Parameters: - pszFontFaceName: Font face name.
             - wFontSize: Font size.
 Remarks:    Called either from Create and DoModal functions.
             If m_wFontSize = 0 or pszFontFaceName is NULL then default font will be used.
******************************************************************************************/
void CCBPropertySheet::Init(LPCTSTR pszFontFaceName, WORD wFontSize)
{
   m_pszFontFaceName = pszFontFaceName;
   m_wFontSize = wFontSize;

   if((m_wFontSize > 0) && (NULL != m_pszFontFaceName))
   {
      m_psh.pfnCallback = &CCBPropertySheet::PropSheetProc;
      m_psh.dwFlags |= PSH_USECALLBACK;
   }
}

/*****************************************************************************************
 Function: CCBPropertySheet::BuildPropPageArray
 Purpose:  Overrides CPropertySheet::BuildPropPageArray
******************************************************************************************/
void CCBPropertySheet::BuildPropPageArray()
{
  // CPropertySheet::BuildPropPageArray();

   // delete existing prop page array
	free((void*)m_psh.ppsp);
	m_psh.ppsp = NULL;

	// determine size of PROPSHEETPAGE array
	int i;
	int nBytes = 0;
	for (i = 0; i < m_pages.GetSize(); i++)
	{
		CPropertyPage* pPage = GetPage(i);
		nBytes += pPage->m_psp.dwSize;
	}

	// build new PROPSHEETPAGE array
	PROPSHEETPAGE* ppsp = (PROPSHEETPAGE*)malloc(nBytes);
	BYTE* ppspOrigByte = reinterpret_cast<BYTE*>(ppsp);
	if (ppsp == NULL)
		AfxThrowMemoryException();
	BYTE* pPropSheetPagesArrEnd = ppspOrigByte + nBytes;
	ENSURE(pPropSheetPagesArrEnd >= ppspOrigByte);
	m_psh.ppsp = ppsp;
	BOOL bWizard = (m_psh.dwFlags & (PSH_WIZARD | PSH_WIZARD97));
	for (i = 0; i < m_pages.GetSize(); i++)
	{
		//CPropertyPage* pPage = GetPage(i);

		//权宜之计
		CScrollablePropertyPage* pScrollPage = reinterpret_cast<CScrollablePropertyPage*>(GetPage(i));

		BYTE* ppspByte = reinterpret_cast<BYTE*>(ppsp);
		ENSURE_THROW(ppspByte >= ppspOrigByte && ppspByte <= pPropSheetPagesArrEnd, AfxThrowMemoryException());
		Checked::memcpy_s(ppsp, pPropSheetPagesArrEnd - reinterpret_cast<BYTE*>(ppsp), &pScrollPage->m_psp, pScrollPage->m_psp.dwSize);

		/*
		if (!pPage->m_strHeaderTitle.IsEmpty())
		{
			ppsp->pszHeaderTitle = pPage->m_strHeaderTitle;
			ppsp->dwFlags |= PSP_USEHEADERTITLE;
		}
		if (!pPage->m_strHeaderSubTitle.IsEmpty())
		{
			ppsp->pszHeaderSubTitle = pPage->m_strHeaderSubTitle;
			ppsp->dwFlags |= PSP_USEHEADERSUBTITLE;
		}
		*/
		pScrollPage->PreProcessPageTemplate(*ppsp, bWizard, m_pszFontFaceName, m_wFontSize);



		(BYTE*&)ppsp += ppsp->dwSize;
	}
	m_psh.nPages = (int)m_pages.GetSize();

   /*
   //1.先设置字体
   if ((m_wFontSize > 0) && (NULL != m_pszFontFaceName))
   {
	   LPCPROPSHEETPAGE ppsp = m_psh.ppsp;
	   const int nSize = static_cast<int>(m_pages.GetSize());

	   for (int nPage = 0; nPage < nSize; nPage++)
	   {
		   //const DLGTEMPLATE* pResource = ppsp->pResource;
		   CDialogTemplate dlgTemplate(pResource);


		   dlgTemplate.SetFont(m_pszFontFaceName, m_wFontSize);

		   SIZE sizeInPixel;
		   dlgTemplate.GetSizeInPixels(&sizeInPixel);

		   SIZE sizeInDialogUnits;
		   dlgTemplate.GetSizeInDialogUnits(&sizeInDialogUnits);


		   //<<在Win10 Version 1703 pResource指向的内存区域为只读, Write Memory Access Violation
		   //因为其指向的模板数据存放在代码段中，不可写。
		   //if (!IsBadWritePtr((void*)pResource, dlgTemplate.m_dwTemplateSize))
		   //{
			   //memmove((void*)pResource, dlgTemplate.m_hTemplate, dlgTemplate.m_dwTemplateSize);
		   //}

		   //>>
		   ppsp->pResource = (DLGTEMPLATE*)dlgTemplate.Detach();

		   (BYTE*&)ppsp += ppsp->dwSize;
	   }
   }
   */
   return;


 
}