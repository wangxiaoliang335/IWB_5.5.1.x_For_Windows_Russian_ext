#include "stdafx.h"
#include "headers.h"

#define FLASH_TIMER 1

CManualCorrectWnd::CManualCorrectWnd()
:
m_eCorrectStage(E_BEIGN_CORRECT),
m_nCurrentCornerNo(0)
{
	if(!m_bInit)
	{
		LoadArrowBitmaps();
		m_bInit = TRUE;
	}

	m_vecScreen2Image.resize(E_CORNER_COUNT);

	int nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    static const int OFFSET = 10;

	m_vecScreen2Image[E_LEFT_TOP_CORNER].first.x = OFFSET;
	m_vecScreen2Image[E_LEFT_TOP_CORNER].first.y = OFFSET;

	m_vecScreen2Image[E_RIGHT_TOP_CORNER].first.x = nScreenWidth - OFFSET;
	m_vecScreen2Image[E_RIGHT_TOP_CORNER].first.y = OFFSET;


	m_vecScreen2Image[E_LEFT_BOTTOM_CORNER].first.x = OFFSET;
	m_vecScreen2Image[E_LEFT_BOTTOM_CORNER].first.y = nScreenHeight - OFFSET;


	m_vecScreen2Image[E_RIGHT_BOTTOM_CORNER].first.x = nScreenWidth  - OFFSET;
	m_vecScreen2Image[E_RIGHT_BOTTOM_CORNER].first.y = nScreenHeight - OFFSET;


	m_hMemDC = ::CreateCompatibleDC(GetDC(GetDesktopWindow()));

}

CManualCorrectWnd::~CManualCorrectWnd()
{
	::DestroyWindow(m_hWnd);

	::DeleteDC(m_hMemDC);
}


BOOL CManualCorrectWnd::StartCorrect(HWND hNotifyWnd)
{

	if(this->m_hWnd == NULL)
	{
		this->Create();
	}

	m_eCorrectStage = E_BEIGN_CORRECT;
	m_nCurrentCornerNo = 0;//在调用Create函数和FullScreen函数时产生WM_PAINT消息, 处理WM_PAINT消息时需要用到变量m_nCurrentCornerNo。


	this->FullScreen(TRUE);

	this->SetNotifyWnd(hNotifyWnd);



	SetTimer(m_hWnd, FLASH_TIMER, 500, NULL);


	return TRUE;

}


//@参数:bSuccess, 成功/失败标志
BOOL CManualCorrectWnd::EndCorrect(BOOL bSuccess)
{

	this->FullScreen(FALSE);
	PostMessage(this->m_hNotifyWnd, WM_MANUAL_CORRECT_DONE, (WPARAM)0, (LPARAM)bSuccess/*成功*/);
	::DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	return TRUE;

}


LRESULT  CManualCorrectWnd::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

	switch(uMsg)
	{
	case WM_PAINT:
		OnPaint();
		break;

	case WM_MANUAL_CALIBRATE_DATA:		
		return OnManualCalibrateData(wParam,lParam);
		break;

	case  WM_KEYDOWN:
		if(wParam == VK_ESCAPE)
		{

			EndCorrect(FALSE);
		}
		break;

    case WM_COMMAND:
        {
            WORD wID = LOWORD(wParam); 
            if(wID == ID_ABORT_MANUAL_CORRECT)
            {
                int nRet = MessageBox(this->m_hWnd, g_oResStr[IDS_STRING427],g_oResStr[IDS_STRING105], MB_YESNO|MB_ICONINFORMATION);
                if(nRet == IDYES)
                {
                    EndCorrect(FALSE);
                }

            }

        }

        break;

    case WM_CONTEXTMENU:
        {
            //HMENU hMenu = LoadMenu(_AtlBaseModule.m_hInst, MAKEINTRESOURCE(IDR_MENU_MANUAL_CORRECT_CTXMENU));
            HMENU hMenu = LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MENU_MANUAL_CORRECT_CTXMENU));
            HMENU hCtxMenu = GetSubMenu(hMenu, 0);

            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);

            TrackPopupMenu(hCtxMenu, 
            TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
            pt.x, pt.y, 0, this->m_hWnd, NULL); 
 
            DestroyMenu(hMenu);
        }

        break;


	case WM_TIMER:
		{
			HDC hDC = GetDC(m_hWnd);

			DrawArrow(hDC);

			static  int s_flashFlag = 1;

			if(s_flashFlag)
			{
				DrawFlashCircle(hDC,5);
				s_flashFlag = 0;
			}
			else
			{
				s_flashFlag = 1;
			}

			ReleaseDC(m_hWnd, hDC);
		}
		break;
	}


	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void CManualCorrectWnd::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(m_hWnd,&ps);

	//HBRUSH hBrush = ::CreateSolidBrush(RGB(0,0,128));
    HBRUSH hBrush = ::CreateSolidBrush(BAKGND_COLOR);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfHeight   = - MulDiv(8/*pt*/, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	_tcscpy_s(lf.lfFaceName, _countof( lf.lfFaceName), PROFILE::GetDisplayFontName()); 


	HFONT hFont = ::CreateFontIndirect(&lf);
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);


	RECT rcClient;
	GetClientRect(m_hWnd, & rcClient);


	FillRect(hDC, &rcClient, hBrush);



	DrawArrow(hDC);
	//DrawFlashCircle(hDC);



	RECT rcBound;
	memset(&rcBound, 0, sizeof(RECT));

    SetTextColor(hDC, RGB(255,255,0));
	int nRet = DrawText(hDC, g_oResStr[IDS_STRING426], _tcslen(g_oResStr[IDS_STRING426]), &rcBound, DT_CALCRECT);



	int nTextWidth  = rcBound.right  - rcBound.left;
	int nTextHeight = rcBound.bottom - rcBound.top;

	int nWindowWidth  = rcClient.right  - rcClient.left;
	int nWindowHeight = rcClient.bottom - rcClient.top;

	rcBound.top    = (nWindowHeight - nTextHeight)/2;
	rcBound.bottom = (nWindowHeight + nTextHeight)/2;
	rcBound.left   = (nWindowWidth  - nTextWidth )/2;
	rcBound.right  = (nWindowWidth  + nTextWidth )/2;

    SetBkMode(hDC, TRANSPARENT);
	DrawText(hDC, g_oResStr[IDS_STRING426], _tcslen(g_oResStr[IDS_STRING426]), &rcBound, DT_CENTER|DT_VCENTER);


	SelectObject(hDC, hFontOld);
	DeleteObject(hFont);

	DeleteObject(hBrush);
	EndPaint(m_hWnd, &ps);
}


void CManualCorrectWnd::DrawArrow(HDC hDC)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);


	BITMAP bmpInfo;
	GetObject(this->m_aryBmpArrows[m_nCurrentCornerNo],sizeof(BITMAP), &bmpInfo);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(m_hMemDC, this->m_aryBmpArrows[m_nCurrentCornerNo]);


    //用背景颜色更改位图背景
    //COLORREF clrBmpBkgnd = RGB(255,255,255);/*GetPixel(m_hMemDC, 0, 0);*/
    int nDeviationRange = 40;
    for(int r=0; r<bmpInfo.bmHeight; r++)
    {
        for(int c=0; c<bmpInfo.bmWidth; c++)
        {
            COLORREF clr = GetPixel(m_hMemDC, c, r);

            if( abs(int(GetRValue(clr)) - int(GetGValue(clr))) < nDeviationRange
                &&
                abs(int(GetGValue(clr)) - int(GetBValue(clr))) < nDeviationRange
                &&
               abs(int(GetBValue(clr)) - int(GetRValue(clr))) < nDeviationRange)
            {
                SetPixel(m_hMemDC, c, r, BAKGND_COLOR);
            }

        }
    }



	if(0 == m_nCurrentCornerNo)//左上角
	{
        /*    
		BitBlt(hDC,
			m_vecScreen2Image[E_LEFT_TOP_CORNER].first.x,
			m_vecScreen2Image[E_LEFT_TOP_CORNER].first.y,
			bmpInfo.bmWidth, bmpInfo.bmHeight,
			m_hMemDC,
			0,0,
			SRCCOPY);*/


       BitBlt(hDC,
			m_vecScreen2Image[E_LEFT_TOP_CORNER].first.x /*- bmpInfo.bmWidth  + FALSH_SPOT_RADIUS*/,
			m_vecScreen2Image[E_LEFT_TOP_CORNER].first.y /*- bmpInfo.bmHeight + FALSH_SPOT_RADIUS*/,
			bmpInfo.bmWidth, bmpInfo.bmHeight,
			m_hMemDC,
			0,0,
			SRCCOPY);


	}
	else if(1 == m_nCurrentCornerNo)//右上角
	{
		//BitBlt(hDC,
		//	m_vecScreen2Image[E_RIGHT_TOP_CORNER].first.x - bmpInfo.bmWidth, 
		//	m_vecScreen2Image[E_RIGHT_TOP_CORNER].first.y,
		//	bmpInfo.bmWidth, bmpInfo.bmHeight,
		//	m_hMemDC,
		//	0,0,
		//	SRCCOPY);

		BitBlt(hDC,
			m_vecScreen2Image[E_RIGHT_TOP_CORNER].first.x - bmpInfo.bmWidth/*- FALSH_SPOT_RADIUS*/ , 
			m_vecScreen2Image[E_RIGHT_TOP_CORNER].first.y /*- bmpInfo.bmHeight + FALSH_SPOT_RADIUS*/,
			bmpInfo.bmWidth, bmpInfo.bmHeight,
			m_hMemDC,
			0,0,
			SRCCOPY);

	}
	else if(2 == m_nCurrentCornerNo)//左下角
	{
		//BitBlt(hDC,
		//	m_vecScreen2Image[E_LEFT_BOTTOM_CORNER].first.x, 
		//	m_vecScreen2Image[E_LEFT_BOTTOM_CORNER].first.y - bmpInfo.bmHeight,
		//	bmpInfo.bmWidth, bmpInfo.bmHeight,
		//	m_hMemDC,
		//	0,0,
		//	SRCCOPY);

		BitBlt(hDC,
            m_vecScreen2Image[E_LEFT_BOTTOM_CORNER].first.x /*- bmpInfo.bmWidth + FALSH_SPOT_RADIUS*/, 
			m_vecScreen2Image[E_LEFT_BOTTOM_CORNER].first.y - bmpInfo.bmHeight/*- FALSH_SPOT_RADIUS*/,
			bmpInfo.bmWidth, bmpInfo.bmHeight,
			m_hMemDC,
			0,0,
			SRCCOPY);

	}
	else if(3 == m_nCurrentCornerNo)//右下角
	{
		//BitBlt(hDC,
		//	m_vecScreen2Image[E_RIGHT_BOTTOM_CORNER].first.x -  bmpInfo.bmWidth, 
		//	m_vecScreen2Image[E_RIGHT_BOTTOM_CORNER].first.y  - bmpInfo.bmHeight,
		//	bmpInfo.bmWidth, bmpInfo.bmHeight,
		//	m_hMemDC,
		//	0,0,
		//	SRCCOPY);

		BitBlt(hDC,
			m_vecScreen2Image[E_RIGHT_BOTTOM_CORNER].first.x - bmpInfo.bmWidth/*- FALSH_SPOT_RADIUS*/, 
			m_vecScreen2Image[E_RIGHT_BOTTOM_CORNER].first.y - bmpInfo.bmHeight/*- FALSH_SPOT_RADIUS*/,
			bmpInfo.bmWidth, bmpInfo.bmHeight,
			m_hMemDC,
			0,0,
			SRCCOPY);

	}

	SelectObject(m_hMemDC, hBmpOld);

}

void CManualCorrectWnd::DrawFlashCircle(HDC hDC, int nRadius)
{
	RECT rcCircle;

	if(m_nCurrentCornerNo >= E_CORNER_COUNT) return;

	if (0== m_nCurrentCornerNo )
	{
		rcCircle.left    = m_vecScreen2Image[m_nCurrentCornerNo].first.x ;
		rcCircle.top     = m_vecScreen2Image[m_nCurrentCornerNo].first.y ;
		rcCircle.right   = m_vecScreen2Image[m_nCurrentCornerNo].first.x + nRadius + nRadius ;
		rcCircle.bottom  = m_vecScreen2Image[m_nCurrentCornerNo].first.y + nRadius + nRadius;
	}

	else if (1== m_nCurrentCornerNo)
	{
		rcCircle.left    = m_vecScreen2Image[m_nCurrentCornerNo].first.x- nRadius- nRadius ;
		rcCircle.top     = m_vecScreen2Image[m_nCurrentCornerNo].first.y ;
		rcCircle.right   = m_vecScreen2Image[m_nCurrentCornerNo].first.x ;
		rcCircle.bottom  = m_vecScreen2Image[m_nCurrentCornerNo].first.y + nRadius + nRadius;
	}

	else if (2== m_nCurrentCornerNo)
	{
		rcCircle.left    = m_vecScreen2Image[m_nCurrentCornerNo].first.x;
		rcCircle.top     = m_vecScreen2Image[m_nCurrentCornerNo].first.y - nRadius- nRadius;
		rcCircle.right   = m_vecScreen2Image[m_nCurrentCornerNo].first.x + nRadius+nRadius;
		rcCircle.bottom  = m_vecScreen2Image[m_nCurrentCornerNo].first.y ;
	}

	else if (3== m_nCurrentCornerNo)
	{
		rcCircle.left    = m_vecScreen2Image[m_nCurrentCornerNo].first.x - nRadius-nRadius;
		rcCircle.top     = m_vecScreen2Image[m_nCurrentCornerNo].first.y - nRadius-nRadius;
		rcCircle.right   = m_vecScreen2Image[m_nCurrentCornerNo].first.x ;
		rcCircle.bottom  = m_vecScreen2Image[m_nCurrentCornerNo].first.y ;

	}

	HPEN hPen = ::CreatePen(PS_SOLID,2, RGB(255,255,255));		
	HBRUSH hFillBrush = ::CreateSolidBrush(RGB(255,255,0));
	HPEN hPenOld = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hBrushOld = (HBRUSH)::SelectObject(hDC, hFillBrush);

	Ellipse(hDC, rcCircle.left, rcCircle.top, rcCircle.right, rcCircle.bottom);

	::SelectObject(hDC, hBrushOld);
	::SelectObject(hDC, hPenOld);
	::DeleteObject(hPen);
	::DeleteObject(hFillBrush);

}


LRESULT CManualCorrectWnd::OnManualCalibrateData(WPARAM wParam,LPARAM lParam)
{

	POINT pt;

	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	switch(m_eCorrectStage)
	{
	case E_BEIGN_CORRECT:

		m_ptLastPos = pt;
		m_nInputWaitCount = 1;
		m_eCorrectStage = E_IS_CORRECTING;
		break;

	case E_IS_CORRECTING:
		{
			if(m_nCurrentCornerNo >= 1)
			{
				//判断距离前一个校正点的距离,如果距离太小, 说明仍然在上一个校正点附近。

				int nXOffsetFromLastCorner = pt.x - m_vecScreen2Image[m_nCurrentCornerNo - 1].second.x;
				int nYOffsetFromLastCorner = pt.y - m_vecScreen2Image[m_nCurrentCornerNo - 1].second.y;

				int distFromLastPoint = nXOffsetFromLastCorner*nXOffsetFromLastCorner + nYOffsetFromLastCorner*nYOffsetFromLastCorner;

				if(distFromLastPoint < MINIMUM_CORNER_DISTANCE*MINIMUM_CORNER_DISTANCE)
				{	
					m_nInputWaitCount = 0;
					return 0;
				}

			}

			//判断前后两次的输入数据的偏差
			int nXOffset = pt.x - m_ptLastPos.x;
			int nYOffset = pt.y - m_ptLastPos.y;

			if(m_nInputWaitCount > 1)
			{

				if(abs(nXOffset) > MAX_ALLOWED_X_OFFSET || abs(nYOffset) > MAX_ALLOWED_Y_OFFSET)
				{
					m_nInputWaitCount = 0;
					return 0;
				}

			}

			m_ptLastPos = pt;
			m_nInputWaitCount ++ ;


			//如果连续MAX_INPUT_WAIT_TIMES次收到数据, 则认为输入数据有效
			if(m_nInputWaitCount == MAX_INPUT_WAIT_TIMES)
			{
				m_vecScreen2Image[m_nCurrentCornerNo].second = pt;

				m_nCurrentCornerNo++;

				if(m_nCurrentCornerNo == E_CORNER_COUNT)//全部校正完毕
				{
					m_eCorrectStage  = E_CORRECT_DONE;
					EndCorrect(TRUE);

				}
				else
				{
					InvalidateRect(this->m_hWnd, NULL, FALSE);
					m_nInputWaitCount = 0;
				}
			}
		}//case



		break;


	case E_CORRECT_DONE:
		//do nothing
		break;


	}



	return 0L;
}

//====static variables and member functions======
BOOL CManualCorrectWnd::m_bInit = FALSE;
HBITMAP CManualCorrectWnd::m_aryBmpArrows[ARROW_COUNT];
BOOL CManualCorrectWnd::LoadArrowBitmaps()
{
    /*
	m_aryBmpArrows[0] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_TOP_LEFT_CORNER));
	m_aryBmpArrows[1] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_TOP_RIGHT_CORNER));
	m_aryBmpArrows[2] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_BOTTOM_LEFT_CORNER));
	m_aryBmpArrows[3] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_BOTTOM_RIGHT_CORNER));
    */
    m_aryBmpArrows[0] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_TOP_LEFT_CORNER));
	m_aryBmpArrows[1] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_TOP_RIGHT_CORNER));
	m_aryBmpArrows[2] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_BOTTOM_LEFT_CORNER));
	m_aryBmpArrows[3] = ::LoadBitmap(_AtlBaseModule.m_hInst,MAKEINTRESOURCE(IDB_BITMAP_ARROW_BOTTOM_RIGHT_CORNER));


	return 
		m_aryBmpArrows[0] != NULL
		&&
		m_aryBmpArrows[1] != NULL
		&&
		m_aryBmpArrows[2] != NULL
		&&
		m_aryBmpArrows[3] != NULL;
}