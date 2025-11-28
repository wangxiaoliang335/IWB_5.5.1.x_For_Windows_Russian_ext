#include "stdafx.h"

CScreenLayoutDesigner::CScreenLayoutDesigner()
    :
    m_hInst(NULL),
    m_hWnd(NULL),
    m_hBitmap(NULL),
    m_hBitmapOld(NULL),
    m_hbrBackground(NULL),
    m_hMemDC(NULL),
    m_hFontButton(NULL),
    m_hFontOld(NULL),
    m_bIsDragging(FALSE),
    m_pDragArea(NULL)//,
	//m_eSplitScreenModel(E_SPLIT_SCREEN_VERT)
{
    m_ScreenSize.cx = ::GetSystemMetrics(SM_CXSCREEN);

    m_ScreenSize.cy = ::GetSystemMetrics(SM_CYSCREEN);

    //手形光标
    m_hCursorhHand = theApp.LoadCursor(IDC_CURSOR_HAND);

    //水平切分光标
    m_hCursorSplit_Horz = theApp.LoadCursor(IDC_CURSOR_SPLIT_HORZ);

	//垂直切分光标
	m_hCursorSplit_Vert = theApp.LoadCursor(IDC_CURSOR_SPLIT_VERT);

    //鼠标光标
    m_hCursorArrow = LoadCursor(NULL, IDC_ARROW);
}


CScreenLayoutDesigner::~CScreenLayoutDesigner()
{

    Uninit();
}

BOOL CScreenLayoutDesigner::IsVisible()const
{
    if (NULL == m_hWnd) return FALSE;

    return IsWindowVisible(m_hWnd);
}

//void CScreenLayoutDesigner::Init(int nScreenCount, int nDisplayWidth, int  nDisplayHeight)
//void CScreenLayoutDesigner::Init(SplitMode splitMode, int nLeft, int nTop, int nDisplayWidth, int  nDisplayHeight)
void CScreenLayoutDesigner::Init(SplitMode splitMode, EScreenTargetType eScreenTargetType)
{
    m_eScreenTargetType = eScreenTargetType;

    RefreshScreenSize();

    this->m_screenLayout.Init(splitMode);
    this->SetScreenLayout(m_screenLayout);

    InitWindow();

    InitActiveAreas();
}


BOOL CScreenLayoutDesigner::InitWindow()
{
    static int s_InstanceCount = 1;
    CAtlString strClassName;
    strClassName.Format(_T("TouchScreen Designer %d"), s_InstanceCount++);

    m_hbrBackground = ::CreateSolidBrush(TRANSPARENT_COLOR);

    WNDCLASSEX wndclass;
    wndclass.cbSize = sizeof wndclass;
    wndclass.style = CS_SAVEBITS;
    wndclass.lpfnWndProc = StaticWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    m_hInst = wndclass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);;
    wndclass.hIcon = 0;
    wndclass.hCursor = 0;
    wndclass.hbrBackground = m_hbrBackground;
    wndclass.lpszMenuName = 0;
    wndclass.lpszClassName = strClassName;
    wndclass.hIconSm = 0;
    ATOM  atom = RegisterClassEx(&wndclass);

    m_hWnd = ::CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,//分层窗体。
        //WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        strClassName,
        strClassName,//windows name
        WS_POPUP,
        0,
        0,
        m_ScreenSize.cx,
        m_ScreenSize.cy,
        NULL,//owner window
        0,//Menu ID
        m_hInst,
        (LPVOID)this); //传入CREATESTRUCT结构的参数


    SetLayeredWindowAttributes(
        this->m_hWnd, //A handle to the layered window.
        TRANSPARENT_COLOR, //specifies the transparency color key
        255,//the opacity of the layered window. 
        LWA_ALPHA |   //Use bAlpha to determine the opacity of the layered window.
        LWA_COLORKEY  //se crKey as the transparency color
    );


    HDC hDCScreen = ::GetDC(GetDesktopWindow());
    m_hMemDC      = ::CreateCompatibleDC(hDCScreen);

    m_hBitmap     = ::CreateCompatibleBitmap(hDCScreen, m_ScreenSize.cx, m_ScreenSize.cy);

    m_hBitmapOld = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);
    SetBkMode(m_hMemDC, TRANSPARENT);

    LONG height = m_ScreenSize.cy > m_ScreenSize.cx ? m_ScreenSize.cx : m_ScreenSize.cy;
    //int nButtonHeight = m_ScreenSize.cy / 16;
    int nButtonHeight = height / 24;

    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = nButtonHeight;
    lf.lfWidth = 0;
    lf.lfWeight = FW_BOLD;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

    m_hFontButton = CreateFontIndirect(&lf);

    m_hFontOld = (HFONT)SelectObject(m_hMemDC, m_hFontButton);

    ::ReleaseDC(GetDesktopWindow(), hDCScreen);

    return TRUE;
}


void CScreenLayoutDesigner::Uninit()
{
    ::DestroyWindow(m_hWnd);//销毁
    FreeThunk(&CScreenLayoutDesigner::WndProc, this);

    UninitGDI();

    //m_vecScreenRelativeLayouts.clear();
    m_vecScreenAbsLayouts.clear();
    //m_vecMergeAreasRelative.clear();
    m_vecMergeAreasAbs.clear();
    m_vecActiveAreas.clear();
}

BOOL CScreenLayoutDesigner::InitGDI()
{
    UninitGDI();

    HDC hDCScreen = ::GetDC(GetDesktopWindow());
    m_hMemDC = ::CreateCompatibleDC(hDCScreen);

    m_hBitmap = ::CreateCompatibleBitmap(hDCScreen, m_ScreenSize.cx, m_ScreenSize.cy);

    m_hBitmapOld = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);
    SetBkMode(m_hMemDC, TRANSPARENT);

    LONG height = m_ScreenSize.cy > m_ScreenSize.cx ? m_ScreenSize.cx : m_ScreenSize.cy;
    //int nButtonHeight = m_ScreenSize.cy / 16;
    int nButtonHeight = height / 24;

    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = nButtonHeight;
    lf.lfWidth = 0;
    lf.lfWeight = FW_BOLD;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

    m_hFontButton = CreateFontIndirect(&lf);

    m_hFontOld = (HFONT)SelectObject(m_hMemDC, m_hFontButton);

    ::ReleaseDC(GetDesktopWindow(), hDCScreen);

    return TRUE;
}


BOOL CScreenLayoutDesigner::UninitGDI()
{
    if (m_hBitmapOld)
    {
        SelectObject(m_hMemDC, m_hBitmapOld);
        DeleteObject(m_hBitmap);
        m_hBitmap = NULL;
    }

    if (m_hMemDC)
    {
        ::DeleteDC(m_hMemDC);
        m_hMemDC = NULL;
    }

    if (m_hbrBackground)
    {
        ::DeleteObject(m_hbrBackground);
        m_hbrBackground = NULL;
    }

    if (m_hFontOld)
    {
        ::SelectObject(m_hMemDC, m_hFontOld);
        m_hFontOld = NULL;
    }

    if (m_hFontButton)
    {
        ::DeleteObject(m_hFontButton);
        m_hFontButton = NULL;
    }

    return TRUE;

}

//@功能:初始化活动区数组
void CScreenLayoutDesigner::InitActiveAreas()
{
    m_vecActiveAreas.clear();

    //1.屏幕分割条
    int rows = this->m_screenLayout.GetSplitMode().rows;
    int cols = this->m_screenLayout.GetSplitMode().cols;


    const std::vector<SplitEdge>& splitEdge = m_screenLayout.GetSplitEdges();

    const SplitEdge* pHorzEdge = &splitEdge[0];

    ULONG splitterIndex = 0;
    //水平分割条[1..rows-1]
    for (int r = 1; r < rows; r++)
    {
        TActiveArea activeArea;
        activeArea.uID = DEFAULT_ID;

        //long y = (long)(this->m_rcScreenArea.top + pHorzEdge[r].pos * this->m_ScreenSize.cy);
		long y = (long)(0 + pHorzEdge[r].pos * this->m_ScreenSize.cy);

        activeArea.rcBound.left   = this->m_rcScreenArea.left;
        activeArea.rcBound.right  = this->m_rcScreenArea.right;
        activeArea.rcBound.top    = y - (SPLITTER_WIDTH >> 1);
        activeArea.rcBound.bottom = y + (SPLITTER_WIDTH >> 1);

        activeArea.eAreaType = E_AREA_TYPE_HORZ_SPLITTER;
        activeArea.ulData = splitterIndex;//分割条的索引号
        
        m_vecActiveAreas.push_back(activeArea);

        splitterIndex++;
    }


    //垂直分割条
    splitterIndex = 0;

    const SplitEdge* pVertEdge = &splitEdge[rows+1];

    for (int c = 1; c < cols ; c++)
    {//跳过第一根垂直分割线和最有一根垂直分割线, 因为者两根线用作左右边界。
        TActiveArea activeArea;
        activeArea.uID = DEFAULT_ID;

        //long x = (long)(this->m_rcScreenArea.left + pVertEdge[c].pos * this->m_ScreenSize.cx);
		long x = (long)(0 + pVertEdge[c].pos * this->m_ScreenSize.cx);

        activeArea.rcBound.left   = x - (SPLITTER_WIDTH >> 1);
        activeArea.rcBound.right  = x + (SPLITTER_WIDTH >> 1);
        activeArea.rcBound.top    = m_rcScreenArea.top;
        activeArea.rcBound.bottom = m_rcScreenArea.bottom;

        activeArea.eAreaType = E_AREA_TYPE_VERT_SPLITTER;
        activeArea.ulData = splitterIndex;//分割条的索引号

        m_vecActiveAreas.push_back(activeArea);
        splitterIndex++;
    }


    do
    {
        if (m_vecMergeAreasAbs.size() == 0) break;

        const RECT* pHorzMergeArea = &m_vecMergeAreasAbs[0];

        ULONG mergeAreaIndex = 0;

        //水平融合区上下边界
        for (int r = 0; r < rows - 1; r++)
        {
            const RECT& mergeArea = pHorzMergeArea[r];

            TActiveArea activeArea;
            activeArea.uID = DEFAULT_ID;


            //上边界活动区
            activeArea.rcBound.left = mergeArea.left;
            activeArea.rcBound.right = mergeArea.right;
            activeArea.rcBound.top = mergeArea.top - (BORDER_DRAG_WIDTH >> 1);
            activeArea.rcBound.bottom = mergeArea.top + (BORDER_DRAG_WIDTH >> 1);

            activeArea.eAreaType = E_AREA_TYPE_HORZ_MERGE_BORDER;//水平融合区边界
            activeArea.ulData = 2 * mergeAreaIndex + 0;//上融合区左边界编号

            m_vecActiveAreas.push_back(activeArea);

            //下边界活动区
            activeArea.rcBound.top = mergeArea.bottom - (BORDER_DRAG_WIDTH >> 1);
            activeArea.rcBound.bottom = mergeArea.bottom + (BORDER_DRAG_WIDTH >> 1);
            activeArea.ulData = 2 * mergeAreaIndex + 1;//下融合区右边界编号

            m_vecActiveAreas.push_back(activeArea);

            mergeAreaIndex++;
        }


        mergeAreaIndex = 0;

        if (m_vecMergeAreasAbs.size() == rows - 1) break;
        //垂直融合区
        const RECT* pVertMergeArea = &m_vecMergeAreasAbs[rows - 1];
        for (int c = 0; c < cols - 1; c++)
        {
            const RECT& mergeArea = pVertMergeArea[c];

            TActiveArea activeArea;
            activeArea.uID = DEFAULT_ID;

            //左边界活动区
            activeArea.rcBound.left = mergeArea.left - (BORDER_DRAG_WIDTH >> 1);
            activeArea.rcBound.right = mergeArea.left + (BORDER_DRAG_WIDTH >> 1);
            activeArea.rcBound.top = mergeArea.top;
            activeArea.rcBound.bottom = mergeArea.bottom;

            activeArea.eAreaType = E_AREA_TYPE_VERT_MERGE_BORDER;//水平融合区边界
            activeArea.ulData = 2 * mergeAreaIndex + 0;//左融合区边界编号

            m_vecActiveAreas.push_back(activeArea);

            //右边界活动区
            activeArea.rcBound.left = mergeArea.right - (BORDER_DRAG_WIDTH >> 1);
            activeArea.rcBound.right = mergeArea.right + (BORDER_DRAG_WIDTH >> 1);
            activeArea.ulData = 2 * mergeAreaIndex + 1;//右融合区边界编号

            m_vecActiveAreas.push_back(activeArea);

            mergeAreaIndex++;
        }


    } while (0);
    /*
    int nScreenCount = m_vecScreenAbsLayouts.size();



    for (int i = 0; i < nScreenCount - 1; i++)
    {
        const RECT& rcScreen = m_vecScreenAbsLayouts[i];
        TActiveArea activeArea;
        activeArea.uID = DEFAULT_ID;

		switch (m_eSplitScreenModel)
		{
			case E_SPLIT_SCREEN_VERT:
				activeArea.rcBound.left   = rcScreen.right - (SPLITTER_WIDTH >> 1);
				activeArea.rcBound.right  = rcScreen.right + (SPLITTER_WIDTH >> 1);
				activeArea.rcBound.top    = rcScreen.top;
				activeArea.rcBound.bottom = rcScreen.bottom;
				break;

			case E_SPLIT_SCREEN_HORZ:
				activeArea.rcBound.left   = rcScreen.left;
				activeArea.rcBound.right  = rcScreen.right;
				activeArea.rcBound.top    = rcScreen.bottom - (SPLITTER_WIDTH >> 1);
				activeArea.rcBound.bottom = rcScreen.bottom + (SPLITTER_WIDTH >> 1);
				break;
		}


        activeArea.eAreaType = E_AREA_TYPE_SPLITTER;
        activeArea.ulData = i;//分割条的索引号

        m_vecActiveAreas.push_back(activeArea);

    }//for
    */


    /*
    //2.融合区边界
    int nMergeAreaCount = m_vecMergeAreasAbs.size();
    for (int i = 0; i < nMergeAreaCount; i++)
    {
        const RECT& mergeArea = m_vecMergeAreasAbs[i];

        //左边界
        TActiveArea activeArea;
        activeArea.uID = DEFAULT_ID;
        activeArea.eAreaType      = E_AREA_TYPE_MERGE_BORDER;
        activeArea.ulData = 2 * i;//融合区边界编号

		switch (m_eSplitScreenModel)
		{
			case E_SPLIT_SCREEN_VERT:
				activeArea.rcBound.left   = mergeArea.left - (BORDER_DRAG_WIDTH >> 1);
				activeArea.rcBound.right  = mergeArea.left + (BORDER_DRAG_WIDTH >> 1);
				activeArea.rcBound.top    = mergeArea.top;
				activeArea.rcBound.bottom = mergeArea.bottom;
				break;

			case E_SPLIT_SCREEN_HORZ:
				activeArea.rcBound.left  = mergeArea.left ;
				activeArea.rcBound.right = mergeArea.right;
				activeArea.rcBound.top    = mergeArea.top - (BORDER_DRAG_WIDTH >> 1);
				activeArea.rcBound.bottom = mergeArea.top + (BORDER_DRAG_WIDTH >> 1);
				break;
		};

        m_vecActiveAreas.push_back(activeArea);


        //右边界
        activeArea.ulData = 2 * i + 1;//融合区边界编号


		switch (m_eSplitScreenModel)
		{
		case E_SPLIT_SCREEN_VERT:
			activeArea.rcBound.left = mergeArea.right - (BORDER_DRAG_WIDTH >> 1);
			activeArea.rcBound.right = mergeArea.right + (BORDER_DRAG_WIDTH >> 1);
			break;

		case E_SPLIT_SCREEN_HORZ:
			activeArea.rcBound.top    = mergeArea.bottom - (BORDER_DRAG_WIDTH >> 1);
			activeArea.rcBound.bottom = mergeArea.bottom + (BORDER_DRAG_WIDTH >> 1);
			
			break;
		}

        m_vecActiveAreas.push_back(activeArea);

    }//for
    
    */

    TActiveArea btns[] = 
    {
		//{ BUTTON_ID_ROTATE_90, RECT{ 0,0,0,0 }, E_AREA_TYPE_BUTTON, _T("Rotate 90°"), 0 },
        { BUTTON_ID_CONFIG, RECT{ 0,0,0,0 }, E_AREA_TYPE_BUTTON, _T("CONFIG"), 0 },
        { BUTTON_ID_RESET, RECT{ 0,0,0,0 }, E_AREA_TYPE_BUTTON, _T("RESET"), 0 },
        { BUTTON_ID_OK, RECT{0,0,0,0}, E_AREA_TYPE_BUTTON, _T("OK"), 0},
        { BUTTON_ID_CANCEL, RECT{ 0,0,0,0 }, E_AREA_TYPE_BUTTON, _T("CANCEL"), 0 },
        
       
    };


    UINT btnCount = _countof(btns);

    //计算按钮的最大宽度
    LONG nMaxButtonWidth  = 0;
    LONG nMaxButtonHeight = 0;
    for (UINT uBtnIdx = 0; uBtnIdx < btnCount; uBtnIdx++)
    {
        TActiveArea& btn = btns[uBtnIdx];

        ::DrawText(
            m_hMemDC,
            btn.szText,
            _tcsclen(btn.szText),
            &btn.rcBound,
            DT_CALCRECT | DT_SINGLELINE);

        LONG lBtnWidth  = btn.rcBound.right - btn.rcBound.left;
        LONG lBtnHeight = btn.rcBound.bottom - btn.rcBound.top;

        if (nMaxButtonWidth < lBtnWidth) nMaxButtonWidth = lBtnWidth;
        if (nMaxButtonHeight < lBtnHeight) nMaxButtonHeight = lBtnHeight;
    }

    LONG nButtonHorzInterval = 10;
    LONG nButtonVerzInterval = 10;

    int nTotoalWidth = btnCount*nMaxButtonWidth + (btnCount - 1)*nButtonHorzInterval;

    LONG left = ((m_ScreenSize.cx - nTotoalWidth) >> 1) + 18*nButtonHorzInterval;
    LONG top  = (m_ScreenSize.cy >> 3)*7 - (nMaxButtonHeight >> 1);

    for (UINT uBtnIdx = 0; uBtnIdx < btnCount; uBtnIdx++)
    {
        TActiveArea& btn = btns[uBtnIdx];

        if (uBtnIdx == 2)
        {
            left = ((m_ScreenSize.cx - nTotoalWidth) >> 1) + 18*nButtonHorzInterval;
            top += nMaxButtonHeight + nButtonVerzInterval;
        }

        if (uBtnIdx > 2)
        {
            left += nMaxButtonWidth + nButtonHorzInterval;
        }

        btn.rcBound.left    = left;
        btn.rcBound.top     = top;
        btn.rcBound.right   = left + nMaxButtonWidth;
        btn.rcBound.bottom  = top  + nMaxButtonHeight;

        if (uBtnIdx <= 1)
        {
            left += nMaxButtonWidth + nButtonHorzInterval;
        }

        m_vecActiveAreas.push_back(btn);
    }
	
}

void CScreenLayoutDesigner::SetScreenTargetType(EScreenTargetType eScreenTargetType)
{
    this->m_eScreenTargetType = eScreenTargetType;
    RefreshScreenSize();
}

EScreenTargetType CScreenLayoutDesigner::GetScreenTargetType()const
{
    return m_eScreenTargetType;
}

void CScreenLayoutDesigner::SetScreenLayout(const TScreenLayout& screenLayout)
{

    m_screenLayout = screenLayout;

    const SplitMode& splitMode = m_screenLayout.GetSplitMode();

    int rows = splitMode.rows;
    int cols = splitMode.cols;


    const std::vector<RectF>& vecNormScreenAreas = m_screenLayout.GetScreenAreas();

    m_vecScreenAbsLayouts.resize(rows * cols);

    for (size_t i = 0; i < m_vecScreenAbsLayouts.size(); i++)
    {
        const RectF& normArea = vecNormScreenAreas[i];
        RECT& area = m_vecScreenAbsLayouts[i];

        area.left   = LONG(normArea.left  * m_ScreenSize.cx  + m_rcScreenArea.left);
        area.right  = LONG(normArea.right * m_ScreenSize.cx  + m_rcScreenArea.left);

        area.top    = LONG(normArea.top    * m_ScreenSize.cy + m_rcScreenArea.top);
        area.bottom = LONG(normArea.bottom * m_ScreenSize.cy + m_rcScreenArea.top);
    }


    const std::vector<RectF>& vecNormMergeAreas = m_screenLayout.GetMergeAreas();

    m_vecMergeAreasAbs.resize(rows - 1 + cols - 1);

    for (size_t i = 0; i < vecNormMergeAreas.size(); i++)
    {
        const RectF& normArea = vecNormMergeAreas[i];
        RECT& area = m_vecMergeAreasAbs[i];

        area.left   = LONG(normArea.left   * m_ScreenSize.cx);
        area.right  = LONG(normArea.right  * m_ScreenSize.cx);

        area.top    = LONG(normArea.top    * m_ScreenSize.cy);
        area.bottom = LONG(normArea.bottom * m_ScreenSize.cy);

    }

	InitActiveAreas();


	//绘制界面
	Draw(m_hMemDC);

	//作废窗体区域，以便在UpdateWindows时触发WM_PAINT消息。
	RECT rcWnd = RECT{ 0, 0, m_ScreenSize.cx, m_ScreenSize.cy };
	::InvalidateRect(m_hWnd, &rcWnd, TRUE);
	UpdateWindow(this->m_hWnd);

}


//@功  能:返回按照相对尺寸划分的屏幕区域数组。
//@参  数:pCount, 存放区域个数的内存指针
//@返回值:屏幕相对划分矩形区域数组的首地址。
//const RectF* CScreenLayoutDesigner::GetScreenRelativeLayouts(UINT* pScreenCount) const
//{ 
//    const RectF* pRelativeScreens = NULL;
//
//    if (pScreenCount) *pScreenCount = m_vecScreenRelativeLayouts.size();
//
//    if (m_vecScreenRelativeLayouts.size() > 0)
//    {
//        pRelativeScreens = &m_vecScreenRelativeLayouts[0];
//    }
//
//    return pRelativeScreens;
//}

//@功  能:返回按照像素尺寸划分的屏幕区域数组。
//@参  数:pCount, 存放区域个数的内存指针
//@返回值:按照像素尺寸划分的屏幕区域的首地址。
const RECT* CScreenLayoutDesigner::GetScreenAbsoluteLayouts(UINT* pScreenCount)const
{
    const RECT* pAbsScreen = NULL;
    
    if(pScreenCount) *pScreenCount = m_vecScreenAbsLayouts.size();

    if (m_vecScreenAbsLayouts.size() >= 0)
    {
        pAbsScreen = &m_vecScreenAbsLayouts[0];
    }


    return pAbsScreen;

}


//@功 能:设置屏幕的相对划分区域
//@参 数:pRelativeLayouts, 屏幕划分矩形区域数组的首地址
//       nAreaCount，屏幕划分矩形区域数组元素的个数
//void CScreenLayoutDesigner::SetScreenRelativeLayouts(const RectF* pRelativeLayouts, UINT uScreenCount)
//{
//    m_vecScreenRelativeLayouts.resize(uScreenCount);
//
//    for(UINT screenIndex = 0; screenIndex < uScreenCount; screenIndex++)
//    {
//        m_vecScreenRelativeLayouts[screenIndex] = pRelativeLayouts[screenIndex];
//    }
//
//    //更新绝对像素尺寸的屏幕区域数组
//    m_vecScreenAbsLayouts.resize(uScreenCount);
//
//    for (UINT screenIndex = 0; screenIndex < uScreenCount; screenIndex++)
//    {
//        const RectF& rectRel = m_vecScreenRelativeLayouts[screenIndex];
//        RECT& rectAbs        = m_vecScreenAbsLayouts[screenIndex];
//
//        rectAbs.left  = LONG(rectRel.left  * m_ScreenSize.cx);
//        rectAbs.right = LONG(rectRel.right * m_ScreenSize.cx);
//
//        rectAbs.top    = LONG(rectRel.top    * m_ScreenSize.cy);
//        rectAbs.bottom = LONG(rectRel.bottom * m_ScreenSize.cy);
//    }
//
//    InitActiveAreas();
//}



//@功  能:相对尺寸设置的触控融合区。
//@参  数:pRelativeMergeArea, 触控融合区数组
//        nAreaCount， 触控融合区数目
//@返回值:空
/*
void  CScreenLayoutDesigner::SetRelativeMergeAreas(const RectF* pRelativeMergeArea, UINT nAreaCount)
{
    m_vecMergeAreasRelative.resize(nAreaCount);

    for (UINT areaIndex = 0; areaIndex < nAreaCount; areaIndex++)
    {
        m_vecMergeAreasRelative[areaIndex] = pRelativeMergeArea[areaIndex];

    }//for

    //更新绝对像素尺寸的融合区数组
    m_vecMergeAreasAbs.resize(nAreaCount);

    for (UINT areaIndex = 0; areaIndex < nAreaCount; areaIndex++)
    {
        const RectF& rectRel = m_vecMergeAreasRelative[areaIndex];
        RECT&  rectAbs       = m_vecMergeAreasAbs[areaIndex];

        rectAbs.left   = LONG(rectRel.left   * m_ScreenSize.cx);
        rectAbs.right  = LONG(rectRel.right  * m_ScreenSize.cx);
        rectAbs.top    = LONG(rectRel.top    * m_ScreenSize.cy);
        rectAbs.bottom = LONG(rectRel.bottom * m_ScreenSize.cy);

    }//for

    InitActiveAreas();

}
*/

//@功  能:返回按照相对尺寸设置的触控融合区。
//@参  数:pCount, 存放区域个数的内存指针
//@返回值:触控融合区域数组首地址。
/*
const RectF* CScreenLayoutDesigner::GetRelativeMergeAreas(UINT* pAreaCount)const
{
    const RectF* pRelMergeArea = NULL;

    if (pAreaCount) *pAreaCount = m_vecMergeAreasRelative.size();

    if (m_vecMergeAreasRelative.size() > 0)
    {
        pRelMergeArea = &m_vecMergeAreasRelative[0];
    }
    
    return pRelMergeArea;
}
*/

//@功 能:响应屏幕分辨率发生变化的事件
//@参 数:
//       nScreenWidth, 屏幕像素宽度
//       nScreenHeight, 屏幕像素高度
//
//@说明:屏幕区域和融合区的相对尺寸保持不变, 绝对尺寸按照原先的相对尺寸进行调整。
void CScreenLayoutDesigner::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{

    RefreshScreenSize();

 
}

void CScreenLayoutDesigner::RefreshScreenSize()
{
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    int nLeft = 0;
    int nTop = 0;
    switch (m_eScreenTargetType)
    {
    case EScreenTargetType::E_SCREEN_TARTGE_PRIMARY:
        nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
        nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
        nLeft = 0;
        nTop   = 0;
        break;
        
    case EScreenTargetType::E_SCREEN_TARGET_VIRTUAL_DESKTOP:
        nScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        nScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        nLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
        nTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
        break;
    }//switch

    m_ScreenSize.cx = nScreenWidth;
    m_ScreenSize.cy = nScreenHeight;

    m_rcScreenArea.left = nLeft;
    m_rcScreenArea.top = nTop;
    m_rcScreenArea.right = nLeft + nScreenWidth;
    m_rcScreenArea.bottom = nTop + nScreenHeight;

    this->SetScreenLayout(this->m_screenLayout);


    if (this->m_hWnd)
    {
        //调整窗体大小
        MoveWindow(m_hWnd, m_rcScreenArea.left, m_rcScreenArea.top, m_ScreenSize.cx, m_ScreenSize.cy, FALSE);

        m_rcClipCursorOld = RECT{ m_rcScreenArea.left, m_rcScreenArea.top, m_ScreenSize.cx, m_ScreenSize.cy };

        //初始化活动区域
        InitActiveAreas();

        //初始化GDI
        InitGDI();

        //重绘
        Draw(this->m_hMemDC);

        //作废窗体区域，以便在UpdateWindows时触发WM_PAINT消息。
        RECT rcWnd = RECT{ m_rcScreenArea.left, m_rcScreenArea.top, m_ScreenSize.cx, m_ScreenSize.cy };
        ::InvalidateRect(m_hWnd, &rcWnd, TRUE);

        UpdateWindow(m_hWnd);
    }

}

//@功  能:返回按照像素尺寸设置的触控融合区。
//@参  数:pCount, 存放区域个数的内存指针
//@返回值:触控融合区域数组首地址。
const RECT* CScreenLayoutDesigner::GetAbsoluteMergeAreas(UINT* pAreaCount)const
{

    if (m_vecMergeAreasAbs.size() == 0) return NULL;

    if (*pAreaCount) *pAreaCount = m_vecMergeAreasAbs.size();

    return &m_vecMergeAreasAbs[0];
}

LRESULT  CScreenLayoutDesigner::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //AtlTrace(_T("msg=0x%x\n"), uMsg);

    switch (uMsg)
    {

    case WM_NCDESTROY:
    {


        break;
    }

    case WM_CREATE:
    {

        break;
    }

    case WM_SHOWWINDOW:

        break;

    case WM_ERASEBKGND:

        return 1;//erases the background
        break;

    case WM_NCPAINT:

        return 0;
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        BitBlt(
            hdc,
            0,
            0,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            m_hMemDC,
            0,
            0,
            SRCCOPY);

        
        EndPaint(hWnd, &ps);

        AtlTrace(_T("WM_PAINT\n"));
        return 0;

        break;
    }

    case WM_TIMER:

        break;

    case WM_KEYDOWN:
        //ShowWindow(this->m_hWnd, SW_HIDE);
        break;

    case WM_LBUTTONDOWN:
    {
        POINT cursorPos;
        cursorPos.x = LOWORD(lParam);
        cursorPos.y = HIWORD(lParam);
        OnLButtonDown(wParam, cursorPos);
    }
    break;

    case WM_LBUTTONUP:
    {
        POINT cursorPos;
        cursorPos.x = LOWORD(lParam);
        cursorPos.y = HIWORD(lParam);
        OnLButtonUp(wParam, cursorPos);
    }
    break;


    case WM_MOUSEMOVE:
    {
        POINT cursorPos;
        cursorPos.x = LOWORD(lParam);
        cursorPos.y = HIWORD(lParam);
        OnMouseMove(wParam, cursorPos);
    }
    break;

    case WM_SETCURSOR:
    {
        HWND hWnd = (HWND)wParam;
        UINT nHitTest = LOWORD(lParam);
        UINT uMessage = HIWORD(lParam);
        return OnSetCursor(hWnd, nHitTest, uMessage);
    }
        break;



    case WM_DISPLAYCHANGE:

        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CScreenLayoutDesigner::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    if (uMsg == WM_CREATE)
    {
        CREATESTRUCT* lpcs = reinterpret_cast< CREATESTRUCT*>(lParam);

        SetWindowLong(
            hWnd,
            GWL_WNDPROC,
            (LONG)MF2GF<WNDPROC>(&CScreenLayoutDesigner::WndProc, (CScreenLayoutDesigner*)lpcs->lpCreateParams));
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



//@功能:执行设计
void CScreenLayoutDesigner::DoDesign(BOOL bYes)
{
    ShowWindow(this->m_hWnd, bYes?SW_SHOW:SW_HIDE);


    Draw(m_hMemDC);

    UpdateWindow(this->m_hWnd);
}

#include "Windows_CreateSolidBrushEx.hpp"
void CScreenLayoutDesigner::Draw(HDC hDC)
{
    //透明色画刷

   // HBRUSH  transBrush  = Windows::CreateSolidBrushEx(TRANSPARENT_COLOR, 128);
    HBRUSH transBrush = ::CreateSolidBrush(TRANSPARENT_COLOR);
    HBRUSH hBrushOld = (HBRUSH)SelectObject(hDC, transBrush);
    
    //用透明画面清空背景
    Rectangle(hDC, 0, 0, m_ScreenSize.cx, m_ScreenSize.cy);
   
    SelectObject(hDC, hBrushOld);
    DeleteObject(transBrush);

    DrawScreenLabel(hDC);

    UINT nActiveAreaCount = m_vecActiveAreas.size();

    for (UINT areaIndex = 0; areaIndex < nActiveAreaCount; areaIndex++)
    {
        const TActiveArea&  activeArea = m_vecActiveAreas[areaIndex];

        switch (activeArea.eAreaType)
        {
        case E_AREA_TYPE_HORZ_SPLITTER://绘制分割条
        case E_AREA_TYPE_VERT_SPLITTER:
            //DrawSplitter(hDC, activeArea.rcBound);
            DrawSplitter(hDC, activeArea);
            break;


        case E_AREA_TYPE_BUTTON://绘制按钮
            DrawButton(hDC, activeArea);
            break;
        }
    }


    //绘制融合区
    DrawAllMergeArea(hDC);

}

//@功能:绘制屏幕标号
void CScreenLayoutDesigner::DrawScreenLabel(HDC hDC)
{
    UINT uScreenCount = m_vecScreenAbsLayouts.size();
    for (UINT uScreenIndex = 0; uScreenIndex < uScreenCount; uScreenIndex++)
    {
        const RECT& rcAbsArea = m_vecScreenAbsLayouts[uScreenIndex];

		RECT rcArea;
		rcArea.left   = rcAbsArea.left - m_rcScreenArea.left;
		rcArea.right  = rcArea.left + rcAbsArea.right - rcAbsArea.left;

		rcArea.top    = rcAbsArea.top - m_rcScreenArea.top;
		rcArea.bottom = rcArea.top + rcAbsArea.bottom - rcAbsArea.top;

		
        LONG areaWidth  = rcArea.right  - rcArea.left;
        LONG areaHeight = rcArea.bottom - rcArea.top;

        LONG lFontHeight = areaWidth > areaHeight ? areaHeight : areaWidth;

        LOGFONT lf;
        memset(&lf, 0, sizeof(LOGFONT));
        lf.lfHeight       = lFontHeight;
        lf.lfWidth        = 0;
        lf.lfWeight       = FW_BOLD;
        lf.lfCharSet      = ANSI_CHARSET;
        lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

        HFONT hFont  = CreateFontIndirect(&lf);

        HFONT hFontOld = (HFONT)::SelectObject(hDC, hFont);

        TCHAR szLabel[32];
        _stprintf_s(szLabel, _countof(szLabel), _T("%d"), uScreenIndex + 1);

        RECT rcText = RECT{ 0, 0, areaWidth, areaHeight};

        DrawText(hDC, szLabel, _tcsclen(szLabel), &rcText,  DT_CALCRECT);

        int nTextWidth  = rcText.right - rcText.left;
        int nTextHeight = rcText.bottom - rcText.top;

        SelectObject(hDC, hFontOld);

        {//Memory DC
            HDC hMemDC = ::CreateCompatibleDC(hDC);
            HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, nTextWidth, nTextHeight);
            HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hMemDC, hBitmap);

            HDC hMaskDC = ::CreateCompatibleDC(hDC);
            HBITMAP hMaskBitmap = ::CreateCompatibleBitmap(hDC, nTextWidth, nTextHeight);
            HBITMAP hMaksBitmapOld = (HBITMAP)::SelectObject(hMaskDC, hMaskBitmap);

            COLORREF BLACK_COLOR = RGB(0, 0, 0);
            COLORREF WHITE_COLOR = RGB(255, 255, 255);

            //黑色画刷
            HBRUSH hBrushBlack = ::CreateSolidBrush(BLACK_COLOR);
            HBRUSH hBrushOld = (HBRUSH)SelectObject(hMaskDC, hBrushBlack);

            //用黑色画刷清空背景
            Rectangle(hMaskDC, 0, 0, nTextWidth, nTextHeight);

            SelectObject(hMaskDC, hBrushOld);
 

            hFontOld = SelectFont(hMaskDC, hFont);

            //生成屏蔽图
            ::SetTextColor(hMaskDC, WHITE_COLOR);
            ::SetBkMode(hMaskDC, TRANSPARENT);
            DrawText(hMaskDC, szLabel, _tcsclen(szLabel), &rcText, DT_CENTER | DT_VCENTER);
            SelectObject(hMaskDC, hFontOld);


            //#define MERGECOPY           (DWORD)0x00C000CA /* dest = (source AND pattern)     */
            //
            //A brush created by using a monochrome (1 bit per pixel) bitmap has the text and background colors of the device context to which it is drawn. 
            //Pixels represented by a 0 bit are drawn with the current text color;
            //pixels represented by a 1 bit are drawn with the current background color.
            HBRUSH hBrushHalfTone = GetHalfToneBrush();
            COLORREF labelColor = RGB(0, 255, 255);

            hBrushOld = (HBRUSH)SelectObject(hMemDC, hBrushHalfTone);

            /*
            //0-->TextColor -> Text color
            SetTextColor(hMemDC, TextColor);
            //1->Backgroudn Color->Transparent Color
            SetBkColor(hMemDC, TRANSPARENT_COLOR);
            */
            //0-->TextColor -> Transparent Color
            SetTextColor(hMemDC, TRANSPARENT_COLOR);
            //1->Backgroudn Color->Label Color
            SetBkColor(hMemDC, labelColor);

            //memDC = maskDC & HalfToneBrush
            //文字区域用Halftone画刷填充
            BitBlt(
                hMemDC,
                0,
                0,
                nTextWidth,
                nTextHeight,
                hMaskDC,
                0,
                0,
                MERGECOPY);//dest = (source AND pattern))

            SelectObject(hMemDC, hBrushOld);


            //反转屏蔽图
            PatBlt(hMaskDC, 0, 0, nTextWidth, nTextHeight, DSTINVERT);

            //destDC = destDC AND maskDC
            BitBlt(
                hDC,
                rcArea.left + ((areaWidth - nTextWidth) >> 1),
                rcArea.top  + ((areaHeight - nTextHeight) >> 1),
                nTextWidth,
                nTextHeight,
                hMaskDC,
                0,
                0,
                SRCAND);

            //destDC = destDC OR memDC
            BitBlt(
                hDC,
                rcArea.left + ((areaWidth - nTextWidth) >> 1),
                rcArea.top + ((areaHeight - nTextHeight) >> 1),
                nTextWidth,
                nTextHeight,
                hMemDC,
                0,
                0,
                SRCPAINT);



            DeleteObject(hBrushBlack);
            
            DeleteObject(hFont);

            ::SelectObject(hMaskDC, hMaksBitmapOld);
            ::DeleteObject(hMaskBitmap);
            ::DeleteDC(hMaskDC);


             SelectObject(hMemDC, hBitmapOld);
            ::DeleteObject(hBitmap);
            ::DeleteDC(hMemDC);
        }


    }//for

}


void CScreenLayoutDesigner::DrawButton(HDC hDC, const TActiveArea& btn)
{
    RECT rcText = btn.rcBound;
    ///HBRUSH brush = ::CreateSolidBrush(RGB(255, 0, 0));

    HBRUSH brush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    HBRUSH brushOld = (HBRUSH)::SelectObject(hDC, brush);

    //FrameRect(hDC, &btn.rcBound, brush);
    Rectangle(
        hDC, 
        btn.rcBound.left,
        btn.rcBound.top, 
        btn.rcBound.right,
        btn.rcBound.bottom);
    
    SetTextColor(hDC, RGB(0, 0, 0));

    DrawText(hDC, btn.szText, _tcsclen(btn.szText), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


    SelectObject(hDC, brushOld);
    //DeleteObject(brush);

}

//void CScreenLayoutDesigner::DrawSplitter(HDC hDC, const RECT& rcSplitter)
void CScreenLayoutDesigner::DrawSplitter(HDC  hDC, const TActiveArea& splitter)
{
    LOGPEN logPen;
    //logPen.lopnStyle = PS_DASHDOTDOT;
	logPen.lopnStyle = PS_DASH;
    logPen.lopnWidth = { 2, 0 };
    logPen.lopnColor = RGB(255, 0, 0);
    HPEN hRedPen = ::CreatePenIndirect(&logPen);


    logPen.lopnStyle = PS_SOLID;
    logPen.lopnWidth = { 2, 0 };
    logPen.lopnColor = GetSysColor(COLOR_BTNFACE);
    HPEN hGraypen = ::CreatePenIndirect(&logPen); 
    
    HBRUSH hLightGrayBrush = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
    HBRUSH hBrushOld = (HBRUSH)::SelectObject(hDC, hLightGrayBrush);

    HPEN hPenOld = (HPEN)::SelectObject(hDC, hGraypen);
    


    const RECT& rcSplitter = splitter.rcBound;

    Rectangle(
        hDC,
        rcSplitter.left,
        rcSplitter.top,
        rcSplitter.right,
        rcSplitter.bottom);


    //选择红色笔
    ::SelectObject(hDC, hRedPen);



	POINT pt0, pt1;

    switch (splitter.eAreaType)
    {
    case E_AREA_TYPE_VERT_SPLITTER:
        pt0.x = (rcSplitter.left + rcSplitter.right) >> 1;
        pt0.y = rcSplitter.top;

        pt1.x = pt0.x;
        pt1.y = rcSplitter.bottom;
        break;


    case E_AREA_TYPE_HORZ_SPLITTER:
        pt0.x = rcSplitter.left;
        pt0.y = (rcSplitter.top + rcSplitter.bottom) >> 1;

        pt1.x = rcSplitter.right;
        pt1.y = pt0.y;
        break;


    }


    //绘制中分线
    MoveToEx(hDC, pt0.x, pt0.y, NULL);
    LineTo(hDC, pt1.x, pt1.y);

    SelectObject(hDC, hBrushOld);
    SelectObject(hDC, hPenOld);

    ::DeleteObject(hGraypen);
    ::DeleteObject(hRedPen);


}


void CScreenLayoutDesigner::DrawAllMergeArea(HDC hDC)
{
    int nMergeAreaCount = m_vecMergeAreasAbs.size();
    HRGN allRgn = ::CreateRectRgn(0, 0, 0, 0);

    for (int i = 0; i < nMergeAreaCount; i++)
    {
        RECT&  rcMergeArea = m_vecMergeAreasAbs[i];

        RECT  rcShrink = rcMergeArea;

        //InflateRect(&rcShrink, -2, -2);
        InflateRect(&rcShrink, -8, -8);

        HRGN rgn       = ::CreateRectRgnIndirect(&rcMergeArea);
        HRGN rgnShrink = ::CreateRectRgnIndirect(&rcShrink);
		//rgn = rgn XOR rgnShrink
        ::CombineRgn(rgn, rgnShrink, rgn, RGN_XOR);
        
		//allRgn = allRgn Or rgn
        ::CombineRgn(allRgn, rgn, allRgn, RGN_OR);


        ::DeleteRgn(rgnShrink);
        ::DeleteRgn(rgn);

        
    }//for
    

    HBRUSH hBrushHalfTone = GetHalfToneBrush();
    HBRUSH hBrushOld = (HBRUSH)::SelectObject(hDC, hBrushHalfTone);
    //https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createpatternbrush
    //A brush created by using a monochrome (1 bit per pixel) bitmap has the text and background colors of the device context to which it is drawn. 
    //Pixels represented by a 0 bit are drawn with the current text color; pixels represented by a 1 bit are drawn with the current background color.
    SetTextColor(hDC, RGB (0,     0,   0));
    SetBkColor(hDC,   RGB (0,   255,   0));


    SelectClipRgn(hDC, allRgn);

    RECT rcClipBox;
    GetClipBox(hDC, &rcClipBox);

    PatBlt(
        hDC,
        rcClipBox.left,
        rcClipBox.top,
        rcClipBox.right  - rcClipBox.left,
        rcClipBox.bottom - rcClipBox.top,
        PATINVERT);


     SelectClipRgn(hDC, NULL);
    ::SelectObject(hDC, hBrushOld);


    ::DeleteRgn(allRgn);
}


//@功能:用分割条的活动区域来限制鼠标移动范围。
//@参数:splitterArea, 分割条活动区域
void CScreenLayoutDesigner::ConfineSplitterCursor(const TActiveArea& activeArea)
{
    if (activeArea.eAreaType != E_AREA_TYPE_VERT_SPLITTER 
        &&
        activeArea.eAreaType != E_AREA_TYPE_HORZ_SPLITTER)
        return;

    UINT uSplitterIndex = activeArea.ulData;

    GetClipCursor(&m_rcClipCursorOld);
    RECT rcClipCursorNew;


    int rows = this->m_screenLayout.GetSplitMode().rows;
    int cols = this->m_screenLayout.GetSplitMode().cols;

    const std::vector<SplitEdge>& splitEdges = this->m_screenLayout.GetSplitEdges();

    const SplitEdge* pHorzSplitEdges = &splitEdges[0];//有rows+1条水平分割线
    const SplitEdge* pVertSplitEdges = &splitEdges[rows + 1];//有cols+1条水平分割线


    switch (activeArea.eAreaType)
    {
    case E_AREA_TYPE_HORZ_SPLITTER:
        {
            UINT upSplitEdgeIndex = uSplitterIndex;
            UINT downSplitEdgeIndex = upSplitEdgeIndex + 2;
            const SplitEdge& upSplitEdge = pHorzSplitEdges[upSplitEdgeIndex];
            const SplitEdge& downSplitEdge = pHorzSplitEdges[downSplitEdgeIndex];


            long top = (upSplitEdge.pos + upSplitEdge.limit[1]) * m_ScreenSize.cy;
            long bottom = (downSplitEdge.pos - downSplitEdge.limit[0]) * m_ScreenSize.cy;;

            rcClipCursorNew.left = 0;
            rcClipCursorNew.right = m_ScreenSize.cx;
            rcClipCursorNew.top = top;
            rcClipCursorNew.bottom = bottom;
        }
        break;

    case E_AREA_TYPE_VERT_SPLITTER:
        {
            UINT uLHSplitEdgeIndex = uSplitterIndex;
            UINT uRHSplitEdgeIndex = uSplitterIndex + 2;
            const SplitEdge& LHSplitEdge = pVertSplitEdges[uLHSplitEdgeIndex];
            const SplitEdge& RHSplitEdge = pVertSplitEdges[uRHSplitEdgeIndex];


            long left = (LHSplitEdge.pos + LHSplitEdge.limit[1])  * m_ScreenSize.cx;
            long right = (RHSplitEdge.pos - RHSplitEdge.limit[0]) * m_ScreenSize.cx;;

            rcClipCursorNew.left = left;
            rcClipCursorNew.right = right;
            rcClipCursorNew.top = 0;
            rcClipCursorNew.bottom = m_ScreenSize.cy;

        }
        break;
    }
	//2024/05/05
	//ClipCusor输入参数需要屏幕坐标
	rcClipCursorNew.left   += this->m_rcScreenArea.left;
	rcClipCursorNew.right  += this->m_rcScreenArea.left;
	rcClipCursorNew.top    += this->m_rcScreenArea.top;
	rcClipCursorNew.bottom += this->m_rcScreenArea.top;

	ClipCursor(&rcClipCursorNew);
}


//@功能:限制融合区边界移动的范围
void CScreenLayoutDesigner::ConfineMergeBorderCursor(const TActiveArea& activeArea)
{
    if (activeArea.eAreaType != E_AREA_TYPE_HORZ_MERGE_BORDER
         &&
        activeArea.eAreaType != E_AREA_TYPE_VERT_MERGE_BORDER
        ) return;

    GetClipCursor(&m_rcClipCursorOld);
    RECT rcClipCursorNew;

    rcClipCursorNew = activeArea.rcBound;


    int rows = this->m_screenLayout.GetSplitMode().rows;
    int cols = this->m_screenLayout.GetSplitMode().cols;

    const std::vector<SplitEdge>& splitEdges = this->m_screenLayout.GetSplitEdges();

    const SplitEdge* pHorzSplitEdges = &splitEdges[0];//有rows+1条水平分割线
    const SplitEdge* pVertSplitEdges = &splitEdges[rows + 1];//有cols+1条水平分割线


    ULONG mergeAreaIndex = activeArea.ulData >> 1;
    

    switch (activeArea.eAreaType)
    {
    case E_AREA_TYPE_HORZ_MERGE_BORDER://水平融合区
        {
            bool bLowerBorder = activeArea.ulData & 0x01;

            LONG ScreenHeight = m_ScreenSize.cy;

            //最大融合区宽度为屏幕宽度的1/10
            LONG MAX_MERGE_AREA_HEIGHT = ScreenHeight * 10 / 100;

            
             UINT upSplitEdgeIndex  = mergeAreaIndex;
             UINT curSplitEdgeIndex = upSplitEdgeIndex + 1;
             UINT downSplitEdgeIndex = upSplitEdgeIndex + 2;

             const SplitEdge& upSplitEdge   = pHorzSplitEdges[upSplitEdgeIndex];
             const SplitEdge& curSplitEdge  = pHorzSplitEdges[curSplitEdgeIndex];
             const SplitEdge& downSplitEdge = pHorzSplitEdges[downSplitEdgeIndex];

             if (bLowerBorder)
             {//下边界
                 long top = curSplitEdge.pos * m_ScreenSize.cy;
                 long bottom = top + (MAX_MERGE_AREA_HEIGHT >> 1);
                 long bottomMost = (downSplitEdge.pos - downSplitEdge.limit[0]) * m_ScreenSize.cy - (BORDER_DRAG_WIDTH >> 1);
                 if (bottom > bottomMost)
                 {
                     bottom = bottomMost;
                 }
                 rcClipCursorNew.left   = 0;
                 rcClipCursorNew.right  = m_ScreenSize.cx;
                 rcClipCursorNew.top    = top;
                 rcClipCursorNew.bottom = bottom;


             }

             else
             {//上边界
                 long bottom = curSplitEdge.pos * m_ScreenSize.cy;
                 long top = bottom - (MAX_MERGE_AREA_HEIGHT >> 1);
                 long topMost = (upSplitEdge.pos + upSplitEdge.limit[1]) * m_ScreenSize.cy + (BORDER_DRAG_WIDTH >> 1);

                 if (top < topMost)
                 {
                     top = topMost;
                 }
                 
                 rcClipCursorNew.left   = 0;
                 rcClipCursorNew.right  = m_ScreenSize.cx;
                 rcClipCursorNew.top    = top;
                 rcClipCursorNew.bottom = bottom;

             }
  
             
        }

        break;



    case E_AREA_TYPE_VERT_MERGE_BORDER://垂直融合区
        {
            bool bRightBorder = activeArea.ulData & 0x01;

            LONG ScreenWidth = m_ScreenSize.cx;

            //最大融合区宽度为屏幕宽度的1/10
            LONG MAX_MERGE_AREA_WIDTH = ScreenWidth * 10 / 100;


            UINT LHSplitEdgeIndex  = mergeAreaIndex;
            UINT curSplitEdgeIndex = LHSplitEdgeIndex  + 1;
            UINT RHSplitEdgeIndex  = curSplitEdgeIndex + 1;

            const SplitEdge& LHSplitEdge  = pVertSplitEdges[LHSplitEdgeIndex];
            const SplitEdge& curSplitEdge = pVertSplitEdges[curSplitEdgeIndex];
            const SplitEdge& RHSplitEdge  = pVertSplitEdges[RHSplitEdgeIndex];

            if (bRightBorder)
            {
                long left  = curSplitEdge.pos * m_ScreenSize.cx;
                long right = left + (MAX_MERGE_AREA_WIDTH >> 1);

                long rightMost = (RHSplitEdge.pos - RHSplitEdge.limit[0]) * m_ScreenSize.cx - (BORDER_DRAG_WIDTH >> 1);

                if (right > rightMost)
                {
                    right = rightMost;
                }
                
                rcClipCursorNew.left  = left;
                rcClipCursorNew.right = right;

                rcClipCursorNew.top    = 0;
                rcClipCursorNew.bottom = m_ScreenSize.cy;

            }
            else
            {
                long right = curSplitEdge.pos * m_ScreenSize.cx;
                long left  = right - (MAX_MERGE_AREA_WIDTH >> 1);

                long leftMost = (LHSplitEdge.pos + LHSplitEdge.limit[1]) *m_ScreenSize.cx + (BORDER_DRAG_WIDTH >> 1);
                if (left > leftMost)
                {
                    left = leftMost;
                }
                rcClipCursorNew.left = left;
                rcClipCursorNew.right = right;

                rcClipCursorNew.top = 0;
                rcClipCursorNew.bottom = m_ScreenSize.cy;
            }
        }

        break;
    }

	//2024/05/05
	//ClipCusor输入参数需要屏幕坐标
	rcClipCursorNew.left += this->m_rcScreenArea.left;
	rcClipCursorNew.right += this->m_rcScreenArea.left;
	rcClipCursorNew.top += this->m_rcScreenArea.top;
	rcClipCursorNew.bottom += this->m_rcScreenArea.top;

    ClipCursor(&rcClipCursorNew);
}

void CScreenLayoutDesigner::OnLButtonDown(UINT uFlags, const POINT& ptCursor)
{
    TActiveArea* pActiveArea = GetActiveArea(ptCursor);

    if (pActiveArea == NULL) return;

    if (pActiveArea->eAreaType == E_AREA_TYPE_HORZ_SPLITTER
        || 
        pActiveArea->eAreaType == E_AREA_TYPE_VERT_SPLITTER
        ||
        pActiveArea->eAreaType == E_AREA_TYPE_HORZ_MERGE_BORDER
        ||
        pActiveArea->eAreaType == E_AREA_TYPE_VERT_MERGE_BORDER)
    {
        SetCapture(m_hWnd);
        m_bIsDragging = TRUE;
        m_pDragArea   = pActiveArea;

        RECT dragRect = pActiveArea->rcBound;

        CWindowDC dc(CWnd::GetDesktopWindow());

        dc.SetTextColor(RGB(0, 0, 0));
        dc.SetBkColor(RGB(255, 255, 255));

        //绘制拖拽框
        CSize size(1, 1);
        dc.DrawDragRect(&dragRect, size, NULL, size, NULL, NULL);

        m_rcLastDragRect = dragRect;

        switch (pActiveArea->eAreaType)
        {
            case E_AREA_TYPE_HORZ_SPLITTER:
            case E_AREA_TYPE_VERT_SPLITTER:
                ConfineSplitterCursor(*pActiveArea);
                break;


            case E_AREA_TYPE_VERT_MERGE_BORDER:
            case E_AREA_TYPE_HORZ_MERGE_BORDER:
                ConfineMergeBorderCursor(*pActiveArea);
                break;
        }      
    }   
}

void CScreenLayoutDesigner::OnMouseMove(UINT uFlags, const POINT& ptCursor)
{
    if (m_bIsDragging)
    {
        //拖拽矩形的绘制区域
        RECT dragRect;
		int splitterWidth = 0;

        switch (m_pDragArea->eAreaType)
        {
            case E_AREA_TYPE_VERT_SPLITTER:
            case E_AREA_TYPE_VERT_MERGE_BORDER:
                dragRect.top = m_pDragArea->rcBound.top;
                dragRect.bottom = m_pDragArea->rcBound.bottom;
                splitterWidth = m_pDragArea->rcBound.right - m_pDragArea->rcBound.left;
                dragRect.left = ptCursor.x - (splitterWidth >> 1);
                dragRect.right = ptCursor.x + (splitterWidth >> 1);
                break;

            case E_AREA_TYPE_HORZ_SPLITTER:
            case E_AREA_TYPE_HORZ_MERGE_BORDER:
                dragRect.left = m_pDragArea->rcBound.left;
                dragRect.right = m_pDragArea->rcBound.right;
                splitterWidth = m_pDragArea->rcBound.bottom - m_pDragArea->rcBound.top;
                dragRect.top = ptCursor.y - (splitterWidth >> 1);
                dragRect.bottom = ptCursor.y + (splitterWidth >> 1);
                break;
        }

        /*
		switch (this->m_eSplitScreenModel)
		{
			case E_SPLIT_SCREEN_VERT://垂直切分

				break;

			case E_SPLIT_SCREEN_HORZ://水平切分
				dragRect.left  = m_pDragArea->rcBound.left;
				dragRect.right = m_pDragArea->rcBound.right;
				splitterWidth = m_pDragArea->rcBound.bottom - m_pDragArea->rcBound.top;
				dragRect.top    = ptCursor.y - (splitterWidth >> 1);
				dragRect.bottom = ptCursor.y + (splitterWidth >> 1);
				break;

		}
        */


        CWindowDC dc(CWnd::GetDesktopWindow());

        dc.SetTextColor(RGB(0, 0, 0));
        dc.SetBkColor  (RGB(255, 255, 255));

        //绘制拖拽框
        CSize size(1, 1);
        dc.DrawDragRect(&dragRect, size, &m_rcLastDragRect, size, NULL, NULL);

        m_rcLastDragRect = dragRect;

    }

    //AtlTrace(_T("OnMouseMove \n"));
    //AtlTrace(_T("\n MouseMove pt.x=%d, pt.y=%d\n"), ptCursor.x, ptCursor.y);

}


void CScreenLayoutDesigner::OnLButtonUp(UINT uFlags, const POINT& ptCursor)
{
   if (m_bIsDragging)
    {
        ReleaseCapture();
        m_bIsDragging = FALSE;

        switch(m_pDragArea->eAreaType)
        {
            case E_AREA_TYPE_VERT_SPLITTER:
            case E_AREA_TYPE_HORZ_SPLITTER:
                ClipCursor(&m_rcClipCursorOld);
                OnDragSplitterDone(m_rcLastDragRect, *m_pDragArea);
                break;

            case E_AREA_TYPE_VERT_MERGE_BORDER:
            case E_AREA_TYPE_HORZ_MERGE_BORDER:
                ClipCursor(&m_rcClipCursorOld);
                OnDragMergeAreaBorderDone(m_rcLastDragRect, *m_pDragArea);
                break;
        }

    }
    else
    {

        const TActiveArea* pActiveArea = GetActiveArea(ptCursor);

        if (pActiveArea && pActiveArea->eAreaType == E_AREA_TYPE_BUTTON)
        {
            ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CREEN_LAYOUT_DESIGN_BUTTON_CLICK, (WPARAM)0, (LPARAM)pActiveArea->uID);
        }

    }

    AtlTrace(_T("\nOnLButtonUp pt.x=%d, pt.y=%d\n"), ptCursor.x, ptCursor.y);
}



//@功能:根据分割条的拖拽矩形的位置和分割条索引号, 来调整屏幕布局
//@参数:lpDragRect, 拖拽的矩形区域
//     nSplitterIndex，分割条索引号
void CScreenLayoutDesigner::OnDragSplitterDone(const RECT& dragRect, TActiveArea& splitterArea)
{
 
    //
    //if (uSplitterIndex >= m_vecMergeAreasAbs.size()) return;



    int rows = this->m_screenLayout.GetSplitMode().rows;
    int cols = this->m_screenLayout.GetSplitMode().cols;
        
    std::vector<SplitEdge> splitEdges = this->m_screenLayout.GetSplitEdges();

    SplitEdge* pHorzSplitEdges = &splitEdges[0];//有rows+1条水平分割线
    SplitEdge* pVertSplitEdges = &splitEdges[rows + 1];//有cols+1条水平分割线


    UINT uSplitterIndex = splitterArea.ulData;

    UINT uSplitEdgeIndex = uSplitterIndex + 1;


    switch (splitterArea.eAreaType)
    {
        case E_AREA_TYPE_HORZ_SPLITTER:
            {//分割条的新位置
                LONG nNewSplitterPos = (dragRect.top + dragRect.bottom) >> 1;

                pHorzSplitEdges[uSplitEdgeIndex].pos = (float)nNewSplitterPos / (float)m_ScreenSize.cy;
            }

            break;


        case E_AREA_TYPE_VERT_SPLITTER:
            {//分割条的新位置
                LONG nNewSplitterPos = (dragRect.left + dragRect.right) >> 1;

                pVertSplitEdges[uSplitEdgeIndex].pos = (float)nNewSplitterPos / (float)m_ScreenSize.cx;
            }
        break;

    }

    
    this->m_screenLayout.UpdateLayout(m_screenLayout.GetSplitMode(), splitEdges);

    this->SetScreenLayout(m_screenLayout);

   
    //重新绘制
    Draw(m_hMemDC);

    //作废窗体区域，以便在UpdateWindows时触发WM_PAINT消息。
    RECT rcWnd = RECT{ 0, 0, m_ScreenSize.cx, m_ScreenSize.cy };
    ::InvalidateRect(m_hWnd, &rcWnd, TRUE);

    UpdateWindow(m_hWnd);
}




//@功能:融合区区边界拖拽完成事件响应函数
//@参数:
void CScreenLayoutDesigner::OnDragMergeAreaBorderDone(const RECT& dragRect, TActiveArea& activeArea)
{
    UINT uBorderIndex = activeArea.ulData;
    UINT uSplitterIndex = (uBorderIndex >> 1);

    int rows = this->m_screenLayout.GetSplitMode().rows;
    int cols = this->m_screenLayout.GetSplitMode().cols;

    std::vector<SplitEdge> splitEdges = this->m_screenLayout.GetSplitEdges();

    SplitEdge* pHorzSplitEdges = &splitEdges[0];//有rows+1条水平分割线
    SplitEdge* pVertSplitEdges = &splitEdges[rows + 1];//有cols+1条水平分割线


    UINT uSplitEdgeIndex = uSplitterIndex + 1;

    switch (activeArea.eAreaType)
    {
        case E_AREA_TYPE_HORZ_MERGE_BORDER:
        {

            //分割条的新位置
            LONG borderPos = (dragRect.top + dragRect.bottom) >> 1;

            bool bLowerBorder = uBorderIndex & 0x01;
            if (bLowerBorder)
            {

                pHorzSplitEdges[uSplitEdgeIndex].limit[1] = (float)borderPos / (float)m_ScreenSize.cy - pHorzSplitEdges[uSplitEdgeIndex].pos;

            }
            else
            {

                pHorzSplitEdges[uSplitEdgeIndex].limit[0] = pHorzSplitEdges[uSplitEdgeIndex].pos - (float)borderPos / (float)m_ScreenSize.cy ;
            }

        }
        break;


        case E_AREA_TYPE_VERT_MERGE_BORDER:
        {

            //分割条的新位置
            LONG borderPos = (dragRect.left + dragRect.right) >> 1;

            bool bRightBorder = uBorderIndex & 0x01;

            if (bRightBorder)
            {
                pVertSplitEdges[uSplitEdgeIndex].limit[1] = (float)borderPos / (float)m_ScreenSize.cx - pVertSplitEdges[uSplitEdgeIndex].pos;

            }
            else
            {
                pVertSplitEdges[uSplitEdgeIndex].limit[0] = pVertSplitEdges[uSplitEdgeIndex].pos - (float)borderPos / (float)m_ScreenSize.cx;
            }
        }
         break;

    }

    this->m_screenLayout.UpdateLayout(m_screenLayout.GetSplitMode(), splitEdges);

    this->SetScreenLayout(m_screenLayout);

    //重新绘制
    Draw(m_hMemDC);

    //作废窗体区域，以便在UpdateWindows时触发WM_PAINT消息。
    RECT rcWnd = RECT{ 0, 0, m_ScreenSize.cx, m_ScreenSize.cy };
    ::InvalidateRect(m_hWnd, &rcWnd, TRUE);

    UpdateWindow(m_hWnd);
}


BOOL CScreenLayoutDesigner::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
    POINT cursorPos;
    GetCursorPos(&cursorPos);

	::ScreenToClient(hWnd, &cursorPos);

    const TActiveArea* pActiveArea = GetActiveArea(cursorPos);

    if (pActiveArea)
    {
        switch (pActiveArea->eAreaType)
        {
        case E_AREA_TYPE_BUTTON:
            SetCursor(m_hCursorhHand);
            return TRUE;
            break;

        case E_AREA_TYPE_VERT_SPLITTER:
            SetCursor(m_hCursorSplit_Vert);
            return TRUE;
            break;

        case E_AREA_TYPE_HORZ_SPLITTER:
            SetCursor(m_hCursorSplit_Horz);
            return TRUE;
            break;

        case E_AREA_TYPE_VERT_MERGE_BORDER:

            SetCursor(m_hCursorSplit_Vert);
            return true;
            break;

        case E_AREA_TYPE_HORZ_MERGE_BORDER:
            SetCursor(m_hCursorSplit_Horz);
            return true;
            break;

        }

    }
    else
    {
        SetCursor(m_hCursorArrow);

    }

   //     if (pActiveArea->eAreaType == E_AREA_TYPE_BUTTON)
   //     {
   //         SetCursor(m_hCursorhHand);
   //         return TRUE;
   //     }
   //     else if (pActiveArea->eAreaType == E_AREA_TYPE_SPLITTER )
   //     {
			//switch (this->m_eSplitScreenModel)
			//{
			//case E_SPLIT_SCREEN_VERT:
			//	SetCursor(m_hCursorSplit_Vert);
			//	break;

			//case E_SPLIT_SCREEN_HORZ:
			//	SetCursor(m_hCursorSplit_Horz);
			//	break;
			//}
   //         

   //         return TRUE;
   //     }
   //     else if (pActiveArea->eAreaType == E_AREA_TYPE_MERGE_BORDER)
   //     {
			//switch (this->m_eSplitScreenModel)
			//{
			//	case E_SPLIT_SCREEN_VERT:
			//		SetCursor(m_hCursorSplit_Vert);
			//		break;

			//	case E_SPLIT_SCREEN_HORZ:
			//		SetCursor(m_hCursorSplit_Horz);
			//		break;
			//}
   //         return TRUE;
   //     }
   // }
   // else
   // {
   //     SetCursor(m_hCursorArrow);
   // }

    //AtlTrace(_T("OnSetCursor \n"));

    return FALSE;
}


 CScreenLayoutDesigner::TActiveArea* CScreenLayoutDesigner::GetActiveArea(const POINT& ptCursor)
{
    int nActiveAreaCount = m_vecActiveAreas.size();
    for (int i = 0; i < nActiveAreaCount; i++)
    {
         TActiveArea& activeArea = m_vecActiveAreas[i];

        if (PtInRect(&activeArea.rcBound, ptCursor))
        {
            return &activeArea;
        }

    }//for

    return NULL;

}

 void CScreenLayoutDesigner::Reset()
 {
     int nScreenCount = m_vecScreenAbsLayouts.size();

    // InitScreenArea(nScreenCount);

     this->m_screenLayout.Init(m_screenLayout.GetSplitMode());
     this->SetScreenLayout(m_screenLayout);


     InitActiveAreas();

     Draw(m_hMemDC);

     //作废窗体区域，以便在UpdateWindows时触发WM_PAINT消息。
     RECT rcWnd = RECT{ 0, 0, m_ScreenSize.cx, m_ScreenSize.cy };
     ::InvalidateRect(m_hWnd, &rcWnd, TRUE);
     UpdateWindow(this->m_hWnd);
 }


 const SplitMode& CScreenLayoutDesigner::GetSplitScreenMode()const
 {
     return this->m_screenLayout.GetSplitMode();
 }

 const TScreenLayout& CScreenLayoutDesigner::GetScreenLayout()const
 {

	 return m_screenLayout;
 }