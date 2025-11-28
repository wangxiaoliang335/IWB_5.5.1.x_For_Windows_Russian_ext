#pragma once
#include "MyGDI.h"
//自画菜单
class COwnerDrawMenuBase
{
public:
	explicit COwnerDrawMenuBase(HMENU hMenu = NULL)
	{
		m_hMenu = hMenu;

		HDC hDesktopDC = GetDC(GetDesktopWindow());
		m_hMemDC = ::CreateCompatibleDC(hDesktopDC);
		ReleaseDC(GetDesktopWindow(), hDesktopDC);
	}

	virtual ~COwnerDrawMenuBase()
	{

		DeleteDC(m_hMemDC);	

	}

	//@功能:关联一个菜单句柄
	BOOL Attach (HMENU hMenu)
	{
		m_hMenu = hMenu;
		return TRUE;
	}

	//功能:与关联的菜单句柄分离， 并返回该菜单句柄
	HMENU Detach()
	{
		return m_hMenu;
	}

	//@功能:设定自画菜单项关联的位图句柄
	//@参数:uID, 菜单项ID
	//      hBmp, 菜单项关联的位图句柄。
	//    clrTransparent, 透明颜色
	//@说明: 函数将菜单项设为自画。
	BOOL SetMenuOwnerDrawBitmap(UINT uID, HBITMAP hBmp, COLORREF clrTransparent)
	{

		 HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return FALSE;
		//填充图片背景颜色为菜单背景颜色
		COLORREF clrMenu = GetSysColor(COLOR_MENU); 
		ChangeBmpPixelColor(hBmp, clrTransparent, clrMenu);

		//设置菜单属性
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask  = MIIM_FTYPE/* |MIIM_ID|MIIM_BITMAP*/;
		

		BOOL bRet = TRUE;
		bRet = GetMenuItemInfo(hMenu, uID,  FALSE, &mii);
		if(!bRet) return FALSE;

		mii.fMask |=  MIIM_FTYPE | MIIM_DATA;/*| MIIM_BITMAP*/;//Retrieves or sets the fType and dwTypeData members

		
		mii.dwItemData = DWORD(hBmp);
		mii.fType   |=  MFT_OWNERDRAW;

		//MFT_OWNERDRAW
		//If this value is specified, the dwTypeData member contains an application-defined value.


		bRet = SetMenuItemInfo(hMenu, uID, FALSE, &mii);
		if(!bRet) 
		{
			DWORD dwError = GetLastError(); //0x00000057, 参数不正确

			return FALSE;
		}

		return bRet;
	}

		//@功能:绘制自定义菜单项
	virtual void OnDrawMenuItem(LPDRAWITEMSTRUCT lpDrawItem)
	{
		//lpDrawItem
	}

	//@功能:
	virtual void OnMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItem)
	{

	}

	HMENU GetMenuHandle()const
	{
		return m_hMenu;
	}

protected:


	SIZE GetMenuCaptionSize(UINT uID)
	{
		BOOL bRet = FALSE;
		DWORD dwErr = 0;



		//1.计算菜单项尺寸
		//读取菜单文字
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		SIZE sizeText    = {0,0};
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return sizeText;
		do
		{
			//fMask, Members to retrieve or set
			mii.fMask  = MIIM_STRING ;
		
			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}


			if(mii.cch == 0)//字符串长度为0
			{
				break;
			}

			TCHAR szItemText[MAX_PATH];
			memset(szItemText, 0, sizeof(szItemText));

			
			mii.dwTypeData = szItemText;
			mii.cch = mii.cch + 1;

			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}

			//TRACE(_T("menu %s\n"), szItemText);

			//
			NONCLIENTMETRICS ncm;
			memset(&ncm, 0, sizeof(ncm));
	

			//Windows Server 2003 and Windows XP/2000:  
			//If an application that is compiled for Windows Server 2008 or Windows Vista must also run on Windows Server 2003 or Windows XP/2000,
	        //use the GetVersionEx function to check the operating system version at run time and, if the application is running on Windows Server 
	        //2003 or Windows XP/2000, subtract the size of the iPaddedBorderWidth member from the cbSize member of the NONCLIENTMETRICS structure 
		    //before calling the SystemParametersInfo function.
			OSVERSIONINFO osvi;
		    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		    GetVersionEx(&osvi);
			if(osvi.dwMajorVersion <= 5)
			{
				ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth); //why? See http://msdn.microsoft.com/en-us/library/ms724506(VS.85).aspx
			}
			else
			{
				ncm.cbSize = sizeof(NONCLIENTMETRICS);
			}

			
			bRet = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}

			HFONT hFontMenu= CreateFontIndirect(&ncm.lfMenuFont);
			HDC   hDCDesktop = GetDC(GetDesktopWindow());
			HDC   hDCMem = ::CreateCompatibleDC(hDCDesktop);
			HFONT hFontOld = (HFONT)SelectObject(hDCMem,hFontMenu); 

			 

			::GetTextExtentPoint32(hDCMem, szItemText, _tcslen(szItemText),&sizeText);
			
			SelectObject(hDCMem, hFontOld);			
			DeleteObject(hFontMenu);
			DeleteDC(hDCMem);
			ReleaseDC(GetDesktopWindow(), hDCDesktop);
		}while(0);

		return sizeText;
	}


	SIZE GetItemBitmapSize(UINT uID)
	{
		BOOL bRet = FALSE;
		DWORD dwErr = 0;

		SIZE sizeBitmap  = {0,0};
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return sizeBitmap;

		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		do
		{
			//fMask, Members to retrieve or set
			mii.fMask  = MIIM_DATA;
		

			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			
			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}


			//读取关联的图片的尺寸
			if(mii.dwItemData)
			{
				BITMAP bitmap;
				
				bRet = GetObject(HBITMAP(mii.dwItemData), sizeof(BITMAP), &bitmap);

				if(!bRet)
				{
					dwErr = GetLastError();

				}

				sizeBitmap.cx = bitmap.bmWidth;
				sizeBitmap.cy = bitmap.bmHeight;
			}

		}while(0);

		return sizeBitmap;
	}


	HBITMAP GetMenuItemBitmap(UINT uID)
	{

		HBITMAP hBitmap = NULL;
		BOOL bRet = FALSE;
		DWORD dwErr = 0;
		
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return NULL;

		SIZE sizeBitmap  = {0,0};
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		do
		{
			//fMask, Members to retrieve or set
			mii.fMask  = MIIM_DATA;
		

			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			
			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}


			hBitmap = (HBITMAP)mii.dwItemData;


		}while(0);

		return hBitmap;
	}


	BOOL  HasMenuID(UINT uID)
	{
		
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
	
		return hMenu != NULL;
	}


	//@功能:判断菜单项是否在菜单及其子菜单中存在。
	//@参数: hMenu, 开始搜索的菜单句柄
	//       uID, 菜单项ID
	HMENU  GetMenuHandle(HMENU hSearchMenu, UINT uID)
	{

		int nCount = GetMenuItemCount(hSearchMenu);
		BOOL bFound = FALSE;
		for(int nPos = 0; nPos < nCount; nPos ++)
		{
			if(GetMenuItemID(hSearchMenu, nPos) == uID)
			{
				return hSearchMenu;				
			}
			else 
			{
				HMENU hSubmenu = GetSubMenu(hSearchMenu, nPos);

				if(hSubmenu)
				{

					HMENU hMenuFound = GetMenuHandle(hSubmenu, uID);
					if(hMenuFound != NULL)
					{
						return hMenuFound;
					}
					
				}


			}

		}//for_each(menu item)


		return NULL;
	}




protected:
	HMENU m_hMenu;
	HDC   m_hMemDC;


};