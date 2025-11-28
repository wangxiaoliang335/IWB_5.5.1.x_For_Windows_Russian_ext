#pragma once
#include <atlstr.H>
//#include <pathcch.h>
//#pragma comment(lib, "Pathcch.lib")
#include  <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <shellapi.h>
#include <winerror.h>
#pragma comment(lib, "shell32.lib")

inline BOOL CreateFullDirectory(LPCTSTR lpszPath)
{

	TCHAR path[MAX_PATH];
	memset(path, 0 ,sizeof(path));

	const TCHAR* pData = lpszPath;
	int l = 0;

	do
	{
		if(*pData == _T('\\') || *pData ==_T('/') || *pData == _T('\0'))
		{
			if(l!=0)
			{
				BOOL bRet = PathIsRoot(path);
				if(!bRet)
				{
					bRet = PathFileExists(path);

					if(!bRet)
					{
						bRet = CreateDirectory(path, NULL);
						if(!bRet)
						{
							DWORD dwError = GetLastError();

							if(dwError != ERROR_ALREADY_EXISTS)
							{
								return FALSE;
							}
						
						}
					}
				}


			}//if(l)
	   }//if found path sepearator symbol
		if(*pData == _T('\0'))
		{
			break;
		}		
		path[l++] = *pData++;

	}while(1);

	return TRUE;
}





inline CAtlString CheckAndCreatePath(LPCTSTR szPath)
{	
	CAtlString strAbsPath;

	TCHAR absPathBuf[MAX_PATH];

	if(PathIsRelative(szPath))
	{
		GetCurrentDirectory(_countof(absPathBuf), absPathBuf);

		_tcscat_s(absPathBuf, _countof(absPathBuf), _T("\\"));
		_tcscat_s(absPathBuf, _countof(absPathBuf), szPath);

		strAbsPath = absPathBuf;
	}
	else
	{

		strAbsPath = szPath;
	}

	PathCanonicalize(absPathBuf, strAbsPath);
    //PathCchCanonicalize(absPathBuf, _countof(absPathBuf), strAbsPath);

	strAbsPath = absPathBuf;


	PathRemoveFileSpec(absPathBuf);

	CreateFullDirectory(absPathBuf);

	return strAbsPath;
}


//
inline LPCTSTR GetSHFileOperationErrorInfo(int nErrorCode)
{
    LPCTSTR lpszInfo = NULL;
    switch(nErrorCode)
    {
    case 0x71/*DE_SAMEFILE*/:        
         lpszInfo =_T("The source and destination files are the same file.");
         break;

    case 0x72/*DE_MANYSRC1DEST*/:
        lpszInfo = _T("Multiple file paths were specified in the source buffer, but only one destination file path.");
        break;

    case 0x73/*DE_DIFFDIR*/:
        lpszInfo = _T("Rename operation was specified but the destination path is a different directory. Use the move operation instead.");
        break;

    case 0x74/*DE_ROOTDIR*/:
        lpszInfo = _T("The source is a root directory, which cannot be moved or renamed.");
        break;

    case 0x75/*DE_OPCANCELLED*/:
        lpszInfo = _T("The operation was cancelled by the user, or silently cancelled if the appropriate flags were supplied to SHFileOperation.");
        break;

    case 0x76/*DE_DESTSUBTREE*/:
        lpszInfo = _T("The destination is a subtree of the source.");
        break;

    case 0x78/*DE_ACCESSDENIEDSRC*/:
        lpszInfo = _T(" Security settings denied access to the source.");
        break;

    case 0x79/*DE_PATHTOODEEP*/:
        lpszInfo = _T("The source or destination path exceeded or would exceed MAX_PATH.");
        break;

    case 0x7A/*DE_MANYDEST*/:
        lpszInfo = _T("The operation involved multiple destination paths, which can fail in the case of: a move operation.");
        break;

    case 0x7C/*DE_INVALIDFILES*/:
        lpszInfo = _T("The path in the source or destination or both was invalid.");
        break;

    case 0x7D/*DE_DESTSAMETREE*/:
        lpszInfo =_T("The source and destination have the same parent folder.");
        break;

    case 0x7E/*DE_FLDDESTISFILE*/:
        lpszInfo =_T("The destination path is an existing file.");
        break;

    case 0x80/*DE_FILEDESTISFLD*/:
        lpszInfo =_T("The destination path is an existing folder.");
        break;

    case 0x81/*DE_FILENAMETOOLONG*/:
        lpszInfo =_T("The name of the file exceeds MAX_PATH.");
        break;

    case 0x82/*DE_DEST_IS_CDROM*/:
        lpszInfo =_T("The destination is a read-only CD-ROM, possibly unformatted.");
        break;

    case 0x83/*DE_DEST_IS_DVD*/:
        lpszInfo =_T("The destination is a read-only DVD, possibly unformatted.");
        break;

    case 0x84/*DE_DEST_IS_CDRECORD*/:
        lpszInfo =_T("The destination is a writable CD-ROM, possibly unformatted.");
        break;

    case 0x85/*DE_FILE_TOO_LARGE*/:
        lpszInfo =_T("The file involved in the operation is too large for the destination media or file system.");
        break;

    case 0x86/*DE_SRC_IS_CDROM*/:
        lpszInfo =_T("The source is a read-only CD-ROM, possibly unformatted.");
        break;

    case 0x87/*DE_SRC_IS_DVD*/:
        lpszInfo =_T("The source is a read-only DVD, possibly unformatted.");
        break;

    case 0x88/*DE_SRC_IS_CDRECORD*/:
        lpszInfo =_T("The source is a writable CD-ROM, possibly unformatted.");
        break;

    case 0xB7/*DE_ERROR_MAX*/:
        lpszInfo =_T("MAX_PATH was exceeded during the operation.");
        break;

    case 0x10000/*ERRORONDEST*/:
        lpszInfo =_T("An unspecified error occurred on the destination.");
        break;

    case 0x74|0x10000/*DE_ROOTDIR: | ERRORONDEST*/:
        lpszInfo =_T("Destination is a root directory and cannot be renamed.");
        break;

    default:
        lpszInfo =_T("An unknown error occurred. This is typically due to an invalid path in the source or destination. This error does not occur on Windows Vista and later.");
        break;

    }//switch
    return lpszInfo;
}


//@功能:递归删除指定文件夹
inline BOOL DeleteDirectory(LPCTSTR lpszDirPath)
{
    SHFILEOPSTRUCT fileOp;
    memset(&fileOp, 0, sizeof(fileOp));

    //A window handle to the dialog box to display information about the status of the file operation.
    fileOp.hwnd = NULL;
    //A value that indicates which operation to perform. One of the following values:
    //FO_COPY
    //Copy the files specified in the pFrom member to the location specified in the pTo member.
    //FO_DELETE
    //Delete the files specified in pFrom.
    //FO_MOVE
    //Move the files specified in pFrom to the location specified in pTo.
    //FO_RENAME
    //Rename the file specified in pFrom. You cannot use this flag to rename multiple files with a single function call. Use FO_MOVE instead.
    fileOp.wFunc = FO_DELETE;

    TCHAR lpszDirPathTemp[MAX_PATH];
    memset(lpszDirPathTemp, 0, sizeof(lpszDirPathTemp));
   _tcscpy_s(lpszDirPathTemp, _countof(lpszDirPathTemp), lpszDirPath);
   lpszDirPathTemp[_tcslen(lpszDirPathTemp) + 1] = _T('\0');


    fileOp.pFrom  = lpszDirPathTemp;

    fileOp.pTo    = NULL;
    fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;


    int nRet = SHFileOperation(&fileOp);

    return (nRet == 0?TRUE:FALSE);
}


inline void SaveMemDCToBitmapNoPalete(HDC hMemDC,HBITMAP hBitmap, const TCHAR* szFileName)
{
 
 int   wBitCount = 32;
 
 BITMAP   Bitmap;  
 BITMAPFILEHEADER   bmfHdr;    
 BITMAPINFOHEADER   bi;
 
 GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap); 
 
 bi.biSize          = sizeof(BITMAPINFOHEADER);
 bi.biWidth         = Bitmap.bmWidth;
 bi.biHeight        = Bitmap.bmHeight;
 bi.biPlanes        = 1;
 bi.biBitCount      = wBitCount;
 bi.biCompression   = BI_RGB;
 bi.biSizeImage     = 0;
 bi.biXPelsPerMeter = 0;
 bi.biYPelsPerMeter = 0;
 bi.biClrUsed      =  0;
 bi.biClrImportant =  0; 
 
 DWORD dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight; 
 
 HANDLE hDib = GlobalAlloc(GHND,dwBmBitsSize+sizeof(BITMAPINFOHEADER)); 
 LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
 *lpbi = bi;
 
 GetDIBits(
   hMemDC, 
   hBitmap, 
   0,
  (UINT)Bitmap.bmHeight,  
  (LPSTR)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO *)lpbi, DIB_RGB_COLORS);    
 
 HANDLE fh = CreateFile(szFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
  FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);   
 //ASSERT(fh != INVALID_HANDLE_VALUE);
 
 DWORD dwDIBSize   =   sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmBitsSize;
 
 bmfHdr.bfType = 0x4D42;    
 bmfHdr.bfSize = dwDIBSize;  
 bmfHdr.bfReserved1 = 0;    
 bmfHdr.bfReserved2 = 0;    
 bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);    
 
 DWORD dwWritten;
 
 WriteFile(fh,   (LPSTR)&bmfHdr,   sizeof(BITMAPFILEHEADER),   &dwWritten,   NULL);    
 WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);    
 GlobalUnlock(hDib);    
 GlobalFree(hDib);    
 CloseHandle(fh);

}


inline RECT UnionRect(const RECT& rc1, const RECT& rc2)
{
    RECT rcNew;
    rcNew = rc1;

    if(rcNew.left > rc2.left)
    {
        rcNew.left  = rc2.left;
    }

    if(rcNew.right  < rc2.right)
    {
        rcNew.right = rc2.right;
    }

    if(rcNew.top > rc2.top)
    {
        rcNew.top  = rc2.top;
    }

    if(rcNew.bottom < rc2.bottom)
    {
        rcNew.bottom = rc2.bottom;
    }

    return rcNew;

}


inline CAtlString GetErrorMessage(DWORD dwError)
{
    CAtlString strError = _T("");

    LPVOID lpMsgBuf = NULL;

  int nRet = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,//lpSource
        dwError,
        0,//MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, 
        NULL
        );

   
  if(nRet)
  {
    strError.Format(_T("%s"), lpMsgBuf);
    LocalFree(lpMsgBuf);
  }
  else
  {
    dwError = GetLastError();
  }
 
    return strError;

}


inline const TCHAR* GetWindowsVersionFromRegistry()
{
	const TCHAR* szWinVersion = _T("");

	HKEY hKey = NULL;
	LONG lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
		0,
		KEY_QUERY_VALUE,
		&hKey);
	if (ERROR_SUCCESS == lResult)
	{
		static  TCHAR szValue[256] = { 0 };
		
		DWORD dwBufSize = sizeof(szValue); 
		DWORD dwType = 0;
		lResult = RegQueryValueEx(
			hKey, 
			_T("CurrentVersion"), 
			0,//plReserved
			&dwType,
			(LPBYTE)&szValue[0], 
			&dwBufSize);		
		if (ERROR_SUCCESS == lResult)
		{
			//ERROR_FILE_NOT_FOUND.
			RegCloseKey(hKey);
			return szValue;
		}

	}
	if (hKey)RegCloseKey(hKey);

	return szWinVersion;

}

inline BOOL GetWindowsVersionNumberFromRegistry(DWORD& dwMajor, DWORD& dwMinor)
{
	HKEY hKey = NULL;
	LONG lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
		0,
		KEY_QUERY_VALUE,
		&hKey);
	if (ERROR_SUCCESS != lResult) return FALSE;

	DWORD dwBufSize = sizeof(dwMajor);
	DWORD dwType = 0;
	lResult = RegQueryValueEx(
		hKey,
		_T("CurrentMajorVersionNumber"),
		0,//plReserved
		&dwType,
		(LPBYTE)&dwMajor,
		&dwBufSize);
	if (ERROR_SUCCESS != lResult)
	{
		RegCloseKey(hKey);
		return FALSE;
	}


	lResult = RegQueryValueEx(
		hKey,
		_T("CurrentMinorVersionNumber"),
		0,//plReserved
		&dwType,
		(LPBYTE)&dwMinor,
		&dwBufSize);
	if (ERROR_SUCCESS != lResult)
	{
		RegCloseKey(hKey);
		return FALSE;
	}



	if (hKey)RegCloseKey(hKey);

	return TRUE;

}

//Win8.1和Win10从HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion项的CurrrentVersion键的值都为6.3。
//Win10新添加了CurrentMajorVersionNumber和CurrentMinorVersionNumber两个键值。
inline BOOL IsWin10OrGreater()
{
	DWORD dwMajor = 0, dwMinor = 0;
	BOOL bResult = GetWindowsVersionNumberFromRegistry(dwMajor, dwMinor);
	if (!bResult) return FALSE;
	if (dwMajor >= 10u && dwMinor >= 0u) return TRUE;
	return FALSE;
}

inline BOOL IsWin7OrGreater()
{
	CONST TCHAR* szWinVer = GetWindowsVersionFromRegistry();
	if (_tcscmp(szWinVer, _T("6.1")) >= 0)
	{
		return TRUE;
	}
	{
		return FALSE;
	}
}


inline HBITMAP BitmapFromIcon(HICON hIcon)
{
    HDC hDCDestop = GetDC(GetDesktopWindow());
    HDC hDC = CreateCompatibleDC(hDCDestop);
    HBITMAP hBitmap = CreateCompatibleBitmap(hDCDestop, GetSystemMetrics(SM_CXSMICON ), GetSystemMetrics(SM_CYSMICON ));
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
    DrawIcon(hDC, 0, 0, hIcon);
    SelectObject(hDC, hOldBitmap);
    DeleteDC(hDC);
    ReleaseDC(GetDesktopWindow(), hDCDestop);
    return hBitmap;
}

/*
inline BOOL ReplaceDIBColor(HBITMAP &hDIB, COLORREF oldColor, COLORREF newColor)
{
    BOOL bRet=FALSE;
    //get color information
    DIBSECTION ds;

    if (!GetObject(hDIB, sizeof(DIBSECTION), &ds)) return FALSE;

    if (ds.dsBmih.biBitCount>8) return FALSE; //must be 8 bpp max

	HDC hDC=CreateCompatibleDC(NULL);

	if (!hDC) return FALSE;
	
	HBITMAP hbmpOld=(HBITMAP)::SelectObject(hDC, hDIB);

	//allocate color table
	UINT nColors = ds.dsBmih.biClrUsed ? ds.dsBmih.biClrUsed : 1<<ds.dsBmih.biBitCount; //bpp to UINT
	
	RGBQUAD* ptbl=(RGBQUAD*)CoTaskMemAlloc(nColors*sizeof(RGBQUAD));
	if (ptbl)
	{
	    if (GetDIBColorTable(hDC, 0, nColors, ptbl))
	    {
	        //replace color table entries
	        UINT i;
	        for (i=0; i<nColors ; i++)
	        {
	            if (oldColor==RGB(ptbl[i].rgbRed, ptbl[i].rgbGreen, ptbl[i].rgbBlue))
	            {
	                ptbl[i].rgbRed=GetRValue(newColor);
	                ptbl[i].rgbGreen=GetGValue(newColor);
	                ptbl[i].rgbBlue=GetBValue(newColor);
	                bRet=TRUE;
	            }
	        }
	        //set new table
	        if (bRet)
	            if (!SetDIBColorTable(hDC, 0, nColors, ptbl)) bRet=FALSE;
	    }
	    //cleanup
	    CoTaskMemFree(ptbl);
	    ptbl=NULL;
	    bRet=TRUE;
	}
	else bRet=FALSE;
	
	hDIB=(HBITMAP)::SelectObject(hDC, hbmpOld);
	DeleteDC(hDC);  return bRet;  
}
*/

#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) \
                                 | ((Color << 16) & 0xff0000)

//-------------------------------------------------------------------------------
// ReplaceColor
//
// Author    : Dimitri Rochette drochette@coldcat.fr
// Specials Thanks to Joe Woodbury for his comments and code corrections
//
// Includes  : Only <windows.h>

//
// hBmp         : Source Bitmap
// cOldColor : Color to replace in hBmp
// cNewColor : Color used for replacement
// hBmpDC    : DC of hBmp ( default NULL ) could be NULL if hBmp is not selected
//
// Retcode   : HBITMAP of the modified bitmap or NULL for errors
//
//-------------------------------------------------------------------------------
inline HBITMAP ReplaceColor(HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC)
{
    HBITMAP RetBmp=NULL;
    if (hBmp)
    {
        HDC BufferDC=CreateCompatibleDC(NULL);    // DC for Source Bitmap
        if (BufferDC)
        {
            HBITMAP hTmpBitmap = (HBITMAP) NULL;
            if (hBmpDC)
            if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
            {
                hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
                SelectObject(hBmpDC, hTmpBitmap);
            }

            HGDIOBJ PreviousBufferObject=SelectObject(BufferDC,hBmp);
            // here BufferDC contains the bitmap
            
            HDC DirectDC=CreateCompatibleDC(NULL); // DC for working
            if (DirectDC)
            {
                // Get bitmap size
                BITMAP bm;
                GetObject(hBmp, sizeof(bm), &bm);

                // create a BITMAPINFO with minimal initilisation 
                // for the CreateDIBSection
                BITMAPINFO RGB32BitsBITMAPINFO; 
                ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
                RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
                RGB32BitsBITMAPINFO.bmiHeader.biWidth=bm.bmWidth;
                RGB32BitsBITMAPINFO.bmiHeader.biHeight=bm.bmHeight;
                RGB32BitsBITMAPINFO.bmiHeader.biPlanes=1;
                RGB32BitsBITMAPINFO.bmiHeader.biBitCount=32;

                // pointer used for direct Bitmap pixels access
                UINT * ptPixels;    

                HBITMAP DirectBitmap = CreateDIBSection(DirectDC, 
                                       (BITMAPINFO *)&RGB32BitsBITMAPINFO, 
                                       DIB_RGB_COLORS,
                                       (void **)&ptPixels, 
                                       NULL, 0);
                if (DirectBitmap)
                {
                    // here DirectBitmap!=NULL so ptPixels!=NULL no need to test
                    HGDIOBJ PreviousObject=SelectObject(DirectDC, DirectBitmap);
                    BitBlt(DirectDC,0,0,
                                   bm.bmWidth,bm.bmHeight,
                                   BufferDC,0,0,SRCCOPY);

                       // here the DirectDC contains the bitmap

                    // Convert COLORREF to RGB (Invert RED and BLUE)
                    cOldColor=COLORREF2RGB(cOldColor);
                    cNewColor=COLORREF2RGB(cNewColor);

                    // After all the inits we can do the job : Replace Color
                    for (int i=((bm.bmWidth*bm.bmHeight)-1);i>=0;i--)
                    {
                        if (ptPixels[i]==cOldColor) ptPixels[i]=cNewColor;
                    }
                    // little clean up
                    // Don't delete the result of SelectObject because it's 
                    // our modified bitmap (DirectBitmap)
                       SelectObject(DirectDC,PreviousObject);

                    // finish
                    RetBmp=DirectBitmap;
                }
                // clean up
                DeleteDC(DirectDC);
            }            
            if (hTmpBitmap)
            {
                SelectObject(hBmpDC, hBmp);
                DeleteObject(hTmpBitmap);
            }
            SelectObject(BufferDC,PreviousBufferObject);
            // BufferDC is now useless
            DeleteDC(BufferDC);
        }
    }
    return RetBmp;
}

#include <Ctfutb.h>
//系统语言栏窗口类名和标题名
//static TCHAR* langbar_window_class_name = _T("CiceroUIWndFrame");
//static TCHAR* langbar_window_title_name = _T("TF_FloatingLangBar_WndTitle");

inline void ShowLanguageBar(BOOL bShow)
{
    //HWND hWnd = ::FindWindow(langbar_window_class_name, TF_FLOATINGLANGBAR_WNDTITLE);
    //BOOL bRet = FALSE;
    //if(NULL == hWnd)
    //{
    //    //bRet = ShowWindow(hWnd, bShow?SW_RESTORE:SW_FORCEMINIMIZE);
    //    //bRet = ShowWindow(hWnd, bShow?SW_SHOW:SW_HIDE);
    //    //bRet = ShowWindow(hWnd, bShow?SW_SHOW:SW_HIDE);
    //    return ;
    //}


    HRESULT hr;
    ITfLangBarMgr *pLangBarMgr;

    hr = CoCreateInstance(CLSID_TF_LangBarMgr,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_ITfLangBarMgr,
                              (void**)&pLangBarMgr);
    if(hr == S_OK)
    {
        hr = pLangBarMgr->ShowFloating(bShow?TF_SFT_SHOWNORMAL:TF_SFT_HIDDEN);

         pLangBarMgr->Release();
    }

   
}

inline BOOL IsLanguageBarVisible()
{
    //HWND hWnd = ::FindWindow(langbar_window_class_name, langbar_window_title_name);
    //if(hWnd)
    //{
    //    return ::IsWindowVisible(hWnd);
    //}

	DWORD dwFlags;
    HRESULT hr;
    ITfLangBarMgr *pLangBarMgr;

    hr = CoCreateInstance(CLSID_TF_LangBarMgr,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_ITfLangBarMgr,
                              (void**)&pLangBarMgr);
    if(hr == S_OK)
    {
        hr = pLangBarMgr->GetShowFloatingStatus(&dwFlags);


		if(TF_SFT_SHOWNORMAL&dwFlags)
		{
			return TRUE;
		}

         pLangBarMgr->Release();
    }


    return FALSE;
}

//@功能:判断任务栏是否可见
inline  BOOL IsTaskBarVisible()
{

    HWND hWnd = ::FindWindow(_T("Shell_traywnd"),_T(""));
    if(hWnd)
    {
        return ::IsWindowVisible(hWnd);
    }
	return FALSE;

}

//@功能:隐藏/显示任务栏
inline void ShowTaskBar(BOOL bShow)
{
    HWND hWnd = ::FindWindow(_T("Shell_TrayWnd"),_T(""));
    if(hWnd)
    {
			ShowWindow(hWnd, bShow?SW_SHOW:SW_HIDE);
    }

}


inline BOOL operator !=(const RECT& a, const RECT& b)
{
	if (a.left != b.left
		||
		a.right != b.right
		||
		a.top != b.top
		||
		a.bottom != b.bottom
		)
	{
		return TRUE;
	}
	return FALSE;
}

inline BOOL IsEmptyRect(const RECT& rc)
{
	int nWidth  = rc.right  - rc.left;
	int nHeight = rc.bottom - rc.top;
	if(nWidth <= 0 || nHeight <=0)
		return TRUE;
	return FALSE;
}

class CHalfToneBrush
{
public:
    CHalfToneBrush()
        :
    m_hBrushHalfTone(NULL)
    {
        WORD grayPattern[8];
        for (int i = 0; i < 8; i++)
        {
            grayPattern[i] = (WORD)(0x5555 << (i & 1));
        }

        m_hGrayBitmap = CreateBitmap(8, 8, 1, 1, grayPattern);

        if (m_hGrayBitmap != NULL)
        {
            m_hBrushHalfTone = ::CreatePatternBrush(m_hGrayBitmap);
        }

    }
    ~CHalfToneBrush()
    {
        if (m_hBrushHalfTone)
        {
            DeleteObject(m_hBrushHalfTone);
        }
    }

    operator HBRUSH()
    {
        return m_hBrushHalfTone;
    }

protected:
    HBITMAP m_hGrayBitmap;

    HBRUSH  m_hBrushHalfTone;
};

inline HBRUSH GetHalfToneBrush()
{
    static CHalfToneBrush s_HalfToneBrush;

    return (HBRUSH)s_HalfToneBrush;



    //WORD grayPattern[8];
    //for (int i = 0; i < 8; i++)
    //{
    //    grayPattern[i] = (WORD)(0x5555 << (i & 1));
    //}

    //HBITMAP hGrayBitmap = CreateBitmap(8, 8, 1, 1, grayPattern);

    //HBRUSH hBrushHalfTone = 0;
    //if (hGrayBitmap != NULL)
    //{
    //    hBrushHalfTone = ::CreatePatternBrush(hGrayBitmap);
    //}

    //return hBrushHalfTone;

}	


