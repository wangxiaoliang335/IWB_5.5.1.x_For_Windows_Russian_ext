#pragma once
static const int            GLBOARDMAXPENNUM    = 30;
static const int            GLBOARDMAXERASERNUM = 30;

/*
 @功能：多笔模式下的手势识别，多笔模式下有两种手势，多笔和多橡皮擦，当传入的光斑大于一定阈值时，视为橡皮擦，否则为笔；
 @多笔的实现方式，分为两种：
        1.当前模式为触屏模式（E_DEV_MODE_TOUCHSCREEN）时，直接输入的笔光点集到设备；
        2.当前模式为鼠标模式（E_DEV_MODE_MOUSE时）时，其中的一个光点送入鼠标设备，其余的通过消息传入白板；
 @多橡皮擦的实现方式，只有一种：
        1.通过消息传入白板。
*/
class CSmartMultiPen
{
public:
    CSmartMultiPen();
    ~CSmartMultiPen();

public:
    bool DoRecognition(POINT *points, int nPointCount);

public:
    bool IsTouchPadMode() {return m_bIsTouchPadMode;}
    void SetTouchPadMode(bool isTouchPadMode) {m_bIsTouchPadMode = isTouchPadMode;}

    void SetGLBoadHWnd(HWND hWnd) {m_hWndGLBoad = hWnd;}

    bool IsIdle() {return m_bIsIdle;}

    void ResetSmartMatch();

    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

private:
    bool                        m_bIsTouchPadMode;      //当前工作模式是否为触屏模式
    bool                        m_bIsIdle;

    HWND                        m_hWndGLBoad;
    
    CSmartMatch<GLBOARDMAXPENNUM>  m_oSmartPenMatch;
    CStrokeFilter<GLBOARDMAXPENNUM>  m_oStrokFilter;//笔?
    bool                        m_isSmartMatchReset;
};

class CSmartMultiEraser
{
public:
    CSmartMultiEraser();
    ~CSmartMultiEraser();

public:
    bool DoRecognition(POINT *points, int nPointCount);

public:    
    void SetGLBoadHWnd(HWND hWnd) {m_hWndGLBoad = hWnd;}

    bool IsIdle() {return m_bIsIdle;}

    void ResetSmartMatch();

private:  
    bool                        m_bIsIdle;

    HWND                        m_hWndGLBoad;

    CSmartMatch<GLBOARDMAXPENNUM>  m_oSmartEraserMatch;
    CStrokeFilter<GLBOARDMAXPENNUM>  m_oStrokFilter;//笔?
    bool                        m_isSmartMatchReset;
};

class CMultiPenGestureRecognition
{
public:
    CMultiPenGestureRecognition(void);
    ~CMultiPenGestureRecognition(void);

public:
    void SetGLBoadHWnd(HWND hWnd)
    {
        m_multiPen.SetGLBoadHWnd(hWnd);
        m_multiEraser.SetGLBoadHWnd(hWnd);
    }

    void SetTouchPadMode(bool isTouchPadMode)
    {
        m_multiPen.SetTouchPadMode(isTouchPadMode);
    }  

    void SetEableGR(bool isEnable) {m_isEnableGR = isEnable;}

    bool IsMultiPenGRIdle() {return m_multiPen.IsIdle() && m_multiEraser.IsIdle();}

    void ResetSmartMatch();

    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

public:
    bool DoGestureRecognition(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser);

private:
    /*
    @功能：根据光斑质量把光点分为两类——笔和橡皮擦
    @参数：[in]aryLightSpots，光斑数组；
           [in]nCount，光斑个数；
    @拆分方式：根据一定的规则把光斑分为两类——笔和橡皮擦， 
               即，如果aryLightSpots[i].mass > nMultiEraser * aryLightSpots[i].lStdSpotAreaInVideo，则aryLightSpots[i]为橡皮擦，
               并把他周围一定范围内的光斑合并，按照这种方式，找出光斑中的所有橡皮擦，其他的作为笔
    */
    bool SplitLightSpotByMass(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser);

private:    
    CSmartMultiPen          m_multiPen;
    CSmartMultiEraser       m_multiEraser;

    POINT                   m_ptSplitPens[GLBOARDMAXPENNUM];
    int                     m_nActualSplitPenCount;

    POINT                   m_ptSplitErasers[GLBOARDMAXPENNUM];
    int                     m_nActualSplitEraserCount;

    bool                    m_isEnableGR;

    POINT                   m_ptHistoryEraser[GLBOARDMAXERASERNUM];
    int                     m_nHistoryErasers;    

    //std::map<UINT>          m_lastEraserMap;

    /*static  int             m_*/
};
