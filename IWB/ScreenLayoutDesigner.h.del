#pragma once



#define BUTTON_ID_OK        0
#define BUTTON_ID_CANCEL    1
#define BUTTON_ID_RESET     2
#define BUTTON_ID_ROTATE_90 3
#define BUTTON_ID_CONFIG    4
//多屏拼接模式下的幕布局设计工具
class CScreenLayoutDesigner
{
public:
    CScreenLayoutDesigner ();
    ~CScreenLayoutDesigner();

    //@功能:布局初始化
   // void Init(int nScreenCount, int nDisplayWidth, int  nDisplayHeight);
    void Init(SplitMode splitMode , int nDisplayWidth, int  nDisplayHeight);

    //@功能:反初始化
    void Uninit();

    //@功  能:返回按照相对尺寸划分的屏幕区域数组。
    //@参  数:pCount, 存放区域个数的内存指针
    //@返回值:屏幕相对划分矩形区域数组的首地址。
    //const RectF* GetScreenRelativeLayouts(UINT* pScreenCount) const;

    //@功  能:返回按照像素尺寸划分的屏幕区域数组。
    //@参  数:pCount, 存放区域个数的内存指针
    //@返回值:按照像素尺寸划分的屏幕区域的首地址。
    const RECT* GetScreenAbsoluteLayouts(UINT* pAreaCount)const;


    //@功 能:设置屏幕的相对划分区域
    //@参 数:pRelativeLayouts, 屏幕划分矩形区域数组的首地址
    //       nAreaCount，屏幕划分矩形区域数组元素的个数
    //void SetScreenRelativeLayouts(const RectF* pRelativeLayouts, UINT uScreenCount);



    
    //@功  能:返回按照像素尺寸设置的触控融合区。
    //@参  数:pCount, 存放区域个数的内存指针
    //@返回值:触控融合区域数组首地址。
    const RECT* GetAbsoluteMergeAreas(UINT* pAreaCount)const;



    //@功  能:返回按照相对尺寸设置的触控融合区。
    //@参  数:pCount, 存放区域个数的内存指针
    //@返回值:触控融合区域数组首地址。
    //const RectF* GetRelativeMergeAreas(UINT* pAreaCount)const;


    //@功  能:相对尺寸设置的触控融合区。
    //@参  数:pRelativeMergeArea, 触控融合区数组
    //        nAreaCount， 触控融合区数目
    //@返回值:空
    //void  SetRelativeMergeAreas(const RectF* pRelativeMergeArea, UINT nAreaCount);


    //@功 能:响应屏幕分辨率发生变化的事件
    //
    //@参 数:
    //       nScreenWidth, 屏幕像素宽度
    //       nScreenHeight, 屏幕像素高度
    void OnDisplayChange(int nScreenWidth, int nScreenHeight);


    //@功能:执行设计
    void DoDesign(BOOL bYes);


    BOOL IsVisible()const;

    void Reset();

	//ESplitScreeMode GetSplitScreenMode()const;
    const SplitMode& GetSplitScreenMode()const;

	//@功能:设置屏幕划分布局
	//void SetScreenLayout(ESplitScreeMode eSplitMode,  const TScreenLayout* pScreenLayout);
    void SetScreenLayout(const TScreenLayout& pScreenLayout);




	//@功能:返回屏幕划分布局
	const TScreenLayout& GetScreenLayout()const;
protected:
    BOOL InitWindow();
    BOOL InitGDI();
    BOOL UninitGDI();

    LRESULT  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //void InitScreenArea(int nScreenCount);
    //void InitScreenArea();

	//@功能:沿垂直方向分割屏幕
	//void SliceScreenAreaVert(int nScreenCount);

	//@功能:沿水平方向分割屏幕
	//void SliceScreenAreaHorz(int nScreenCount);

    void InitActiveAreas();
    void Draw(HDC hDC);
    
    //@功能:绘制屏幕标号
    void DrawScreenLabel(HDC hDC);

    HINSTANCE m_hInst;
    HWND m_hWnd;
    HDC  m_hMemDC;
    HBRUSH m_hbrBackground;
    HBITMAP m_hBitmap;
    HBITMAP m_hBitmapOld;
    HFONT m_hFontButton;//按钮字体
    HFONT m_hFontOld   ;//旧字体

    HCURSOR m_hCursorhHand;//手形光标
    HCURSOR m_hCursorSplit_Horz;//水平切分光标
	HCURSOR m_hCursorSplit_Vert;//垂直切分光标
    HCURSOR m_hCursorArrow;
     
    //
    //屏幕相对布局, 整个屏幕坐标归一化到(0,0)到(1,1)范围。
   // std::vector<RectF> m_vecScreenRelativeLayouts;

    //屏幕绝对布局
    std::vector<RECT>  m_vecScreenAbsLayouts;


    //触控融合区所在的矩形区域的数组,归一化到(0,0)到(1,1)范围
    //std::vector<RectF> m_vecMergeAreasRelative;//

    //触控融合区所在的矩形区域的数组,屏幕像素尺寸
    std::vector<RECT> m_vecMergeAreasAbs;//

	//屏幕分割模式
	//ESplitScreeMode m_eSplitScreenModel;
    //SplitMode m_splitMode;

	//屏幕布局数据结构, 存储的坐标都是归一化到[0,1]的屏幕坐标
	TScreenLayout m_screenLayout;

	//屏幕尺寸
    SIZE m_DisplaySize;

    enum EAreaType
    {
        //E_AREA_TYPE_SPLITTER = 0,//屏幕分割条
        E_AREA_TYPE_VERT_SPLITTER = 0,//垂直分割条
        E_AREA_TYPE_HORZ_SPLITTER = 1,//水平分割条
        E_AREA_TYPE_BUTTON        = 2,//按钮
        E_AREA_TYPE_VERT_MERGE_BORDER = 3,//垂直融合区边界
        E_AREA_TYPE_HORZ_MERGE_BORDER = 4,//水平融合区边界

        
    };

   // enum EButtonId
   // {
   //     E_BUTTON_ID_OK = 0,
   //     E_BUTTON_ID_CANEL = 1
   //};
    static const UINT DEFAULT_ID = UINT(-1);

    //活动区结构，即可以拖拽和响应弹起按下事件的区域
    struct TActiveArea
    {
        UINT uID;//
        RECT rcBound;//矩形区域
        EAreaType eAreaType;//区域类型
        LPCTSTR szText;//文字信息
        ULONG  ulData;//关联的数据, 当活动区为分割条时, 用来保存分割条的索引号。

    };

    //屏幕分割条宽度
    static const int SPLITTER_WIDTH    = 8;
    
    //融合区拖拽宽度
    static const int BORDER_DRAG_WIDTH = 8;


	//融合区拖拽高度
	static const int BORDER_DRAG_HEIGHT = 8;

    std::vector < TActiveArea> m_vecActiveAreas;//活动区域



    void DrawButton(HDC  hDC, const TActiveArea& btn);
    //void DrawSplitter(HDC  hDC, const RECT& rcSpliiter);
    void DrawSplitter(HDC  hDC, const TActiveArea& splitter);
    void DrawAllMergeArea(HDC hDC);


    void OnLButtonDown(UINT uFlags, const POINT& ptCursor);
    void OnLButtonUp  (UINT uFlags, const POINT& ptCursor);
    void OnMouseMove  (UINT uFlags, const POINT& ptCursor);

    BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);

    //@功能:根据分割条的拖拽矩形的位置和分割条索引号, 来调整屏幕布局
    //@参数:dragRect, 拖拽的矩形区域
    //     splitterArea，活动区域对象
    void OnDragSplitterDone(const RECT& dragRect, TActiveArea& splitterArea);

    //@功能:融合区区边界拖拽完成事件响应函数
    //@参数:
    void OnDragMergeAreaBorderDone(const RECT& dragRect, TActiveArea& activeArea);

    //@功能:限制分割条的来限制鼠标移动范围
    //@参数:splitterArea, 分割条活动区域
    void ConfineSplitterCursor(const TActiveArea& splitterArea);


    //@功能:限制融合区边界移动的范围
    void ConfineMergeBorderCursor(const TActiveArea& activeArea);

    RECT m_rcClipCursorOld;
    

    TActiveArea* GetActiveArea(const POINT& ptCursor) ;

    BOOL m_bIsDragging        ;//正在拖拽标志
    RECT m_rcLastDragRect     ;//上一次拖拽的矩形区域
    TActiveArea * m_pDragArea;//当前正在拖拽的活动区域

    static const COLORREF TRANSPARENT_COLOR = RGB(0, 0, 0);
};


