#include "AlgoLib.h"
#include <vector>
//added by toxuke@gmail.com, 2013/01/07
//校正图案类
//矩形层圈结构
struct TRectLayer
{
    int m_nHorzMarkNumber ;//水平方向校正点数目
    int m_nVertMarkNumber ;//垂直方向校正点数目
    std::vector<int> m_vecScreenMarkNo;//圈层上屏幕校正点的索引号组成的数组。
};



//@功能:
//
inline int  Pattern2ColNum(E_AutoCalibratePattern ePattern)
{
    int nColNum = 11;
    switch(ePattern)
    {
    case E_CALIBRATE_PATTERN_9_COLS:
        nColNum = 9;
        break;

    case E_CALIBRATE_PATTERN_10_COLS:
        nColNum = 10;
        break;

    case E_CALIBRATE_PATTERN_11_COLS:
        nColNum = 11;
        break;

    case E_CALIBRATE_PATTERN_12_COLS:
        nColNum = 12;
        break;

    case E_CALIBRATE_PATTERN_13_COLS:
        nColNum = 13;
        break;

    case E_CALIBRATE_PATTERN_14_COLS:
        nColNum = 14;
        break;

    case E_CALIBRATE_PATTERN_15_COLS:
        nColNum = 15;
        break;

    case E_CALIBRATE_PATTERN_16_COLS:
        nColNum = 16;
        break;

    case E_CALIBRATE_PATTERN_17_COLS:
        nColNum = 17;
        break;
    }//switch

    return nColNum;
}


//负责生成并绘制校正图案, 
class CCalibratePatternBase
{
public:
    CCalibratePatternBase()
    {

    }

    virtual ~CCalibratePatternBase()
    {

    }

    //@功能:初始化校正图案
    //@参数:szPattern, 校正图案的尺寸
    //      eGridNum, 校正图案中矩形块的列数
    //virtual void InitPattern(E_AutoCalibratePattern ePattern, SIZE szPattern) = 0;


    //virtual void DrawPattern(HWND hWnd) = 0;

};


//校正图案中各行矩形块的高度大小满足等差数列关系
class CArithSeriesRowPattern: public CCalibratePatternBase
{
public:
    CArithSeriesRowPattern()
        :
    m_nRowCount(0),
        m_nColCount(0),
        m_nCalibrateMarkCount(0),
        m_nUpperPartMarkCount(0),
        m_nUpperGridCount(0),
        m_nUpperGridRow(0),
        m_nLowerPartMarkCount(0),
        m_nLowerGridCount(0),
        m_nLowerGridRow(0)
    {

        memset(&m_UpperArea, 0, sizeof(m_UpperArea));
        memset(&m_LowerArea, 0, sizeof(m_LowerArea));

    }

    virtual ~CArithSeriesRowPattern()
    {

        DeleteAllGrayBrush();
    }

    //@功能:初始化校正图案
    //@参数:
    //      ePattern, 校正图案列数枚举量
    //      rcMonitor, 显示器的尺寸
    //      dbRowHeigthRatio, 第一行行高和最后一行行高之比
    //@说明:应该根据校正图案上半部的面积和下半部的面积之比确定dbRowHeigthRatio参数
    //      第一行到最后一行的各行行高H1, H2,...Hn组成一个等差数列。
    //
    //测试数据:
    //         dbRowHeightRation = 0.31293
    //         row = 17;
    //         col = 12
    virtual void InitPattern(E_AutoCalibratePattern ePattern, const RECT& rcMonitor, double dbRowHeigthRatio = 1.0)
    {
        m_patternSize.cx = rcMonitor.right  - rcMonitor.left;
        m_patternSize.cy = rcMonitor.bottom - rcMonitor.top;




        //限定长宽比范围
        if(dbRowHeigthRatio > 3.0) dbRowHeigthRatio = 3.0;

        if(dbRowHeigthRatio < 1.0/3.0) dbRowHeigthRatio = 1.0/3.0;


        //========计算校正矩形块的行数和列数
        //指定列数
        m_nColCount = Pattern2ColNum(ePattern);

        //行数由计算得出

        //根据列数计算列宽
        int nColWidth = m_patternSize.cx / m_nColCount;


        double dbFirstRowH   = 0.0; // 第一行行高
        double dbLastRowH    = 0.0; // 第二行行高
        double dbDiff        = 0.0; //等差数列的公差       


        //第一行行高最大
        if(dbRowHeigthRatio > 1.0)
        {
            //认为最大的行高等于列宽的1.3倍
            dbFirstRowH = (double)nColWidth*1.5;

            //计算行数
            m_nRowCount = int(2.0*(double)m_patternSize.cy/((1.0 + 1.0/dbRowHeigthRatio)*dbFirstRowH) + .5);

            //最后一行行高
            dbLastRowH  = dbFirstRowH/dbRowHeigthRatio;
        }
        else//最后一行行高最大
        {
            //认为最大的行高等于列宽1.3倍
            dbLastRowH = (double)nColWidth*2;

            //计算行数
            m_nRowCount = int(2.0*(double)m_patternSize.cy/((1.0 + dbRowHeigthRatio)*dbLastRowH) + .5);

            //第一行行高
            dbFirstRowH = dbLastRowH*dbRowHeigthRatio;

        }

        dbDiff = (dbLastRowH - dbFirstRowH)/double(m_nRowCount -1);



        //校正点的总数
        m_nCalibrateMarkCount = (m_nRowCount - 1) * (m_nColCount - 1);


        //计算每个校正矩形块的位置
        CalcGridCoord(
            rcMonitor,
            m_nRowCount, 
            m_nColCount,
            nColWidth,
            nColWidth,
            dbFirstRowH,
            dbDiff);


        //划分上下部分区域
        SplitArea(
            rcMonitor, 
            m_nRowCount, 
            m_nColCount,
            dbFirstRowH, 
            dbDiff);


        //更新校正点坐标
        UpdateMarkCoord();



        //收集圈层信息
        CollectLayerMarkInfo();



        //InitGrayBrushes(0xA9, 0xFF);
        //InitGrayBrushes(0x80, 0xFF);
        InitGrayBrushes(0xFF, 0xFF);
    }



    //@功能:绘制完整的校正图案
    virtual void DrawPattern(HWND hWnd)
    {
        //do nothing
        if(hWnd == NULL) return;


        HDC hdc = ::GetDC(hWnd);

        //HBRUSH hRedBrush = ::CreateSolidBrush(RGB(255,0,0));//红色画刷
        //modify by zwn 是把255的纯白色修改为灰色
        //HBRUSH hWhiteBrush = ::CreateSolidBrush(RGB(255, 255, 255));//白色画刷
        HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0, 0 ));//黑色画刷
        //      HBRUSH hWhite1Brush = ::CreateSolidBrush(RGB(211, 211, 211)); //淡灰
        //      HBRUSH hWhite2Brush = ::CreateSolidBrush(RGB(192, 192, 192)); //浅灰
        //      HBRUSH hWhite3Brush = ::CreateSolidBrush(RGB(169, 169, 169)); //深灰


        //HBRUSH brushes[2];
        //brushes[0] = hRedBrush;
        //brushes[0] = hWhiteBrush;
        // brushes[1] = hBlackBrush;


        int nRowStartBrushIndex = 0;
        int nColBrushIndex = 0;
        int iGridIndex = 0;

        for(int row=0; row < this->m_nRowCount; row++)
        {
            int row_center = row - m_nRowCount/2;

            for(int col=0; col < this->m_nColCount; col++)
            {

                int col_center = col - m_nColCount/2;

                if(nColBrushIndex == 0)//是白色画刷,按照距离中心的距离准备灰度画刷
                {
                    double R2 = sqrt((double)(row_center*row_center  + col_center*col_center));
                    UINT  nGrayBrushIndex = int(R2);
                    if(nGrayBrushIndex >= m_vecGrayBrushes.size())
                    {
                        nGrayBrushIndex = m_vecGrayBrushes.size() - 1;
                    }


                    FillRect(hdc, &this->m_vecGrids[iGridIndex], m_vecGrayBrushes[nGrayBrushIndex]);
                }
                else
                {


                    FillRect(hdc, &this->m_vecGrids[iGridIndex], hBlackBrush);
                }

                nColBrushIndex = (nColBrushIndex + 1) & 0x01;

                iGridIndex ++;

            }//for-each col

            nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;
            nColBrushIndex = nRowStartBrushIndex;

        }//for-each row



        //::DeleteObject(hRedBrush);
        //::DeleteObject(hWhiteBrush);
        ::DeleteObject(hBlackBrush);

        ReleaseDC(hWnd, hdc);

    }



    /*
    //@功能:绘制上半部校正图案
    void DrawUpperPartPattern(HWnd hWnd)
    {
    //do nothing if no window specified
    if(hWnd == NULL) return;

    HDC hDC = ::GetDC(hWnd);


    HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,  0 ));//黑色画刷



    //下半部黑色填充
    RECT rc = m_LowerArea;
    ScreenToClient(hWnd, &rc.left);
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);

    //最后一行的行编号=this->m_nRowCount - 1, 依据行号的奇偶选择画刷类型, 偶编号选择灰度画刷, 奇编号选择黑色画刷
    int nRowStartBrushIndex = (this->m_nRowCount - 1) % 2;


    int nColBrushIndex = nRowStartBrushIndex;
    int iGridIndex = 0;
    for(int row=0; row < this->m_nUpperGridRow; row++)
    {
    int offset_row = row - m_nRowCount/2;

    for(int col=0; col < this->m_nColCount; col++)
    {
    int offset_col = col - m_nColCount/2;

    if(0 == nColBrushIndex)
    {
    double R2 = sqrt((double)(offset_row*offset_row  + offset_col*offset_col));

    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, m_vecGrayBrushes[int(R2)]);

    }
    else
    {
    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);
    }
    nColBrushIndex = (nColBrushIndex + 1) &0x01 ;//%2

    iGridIndex ++;

    }//for-each col

    nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
    nColBrushIndex = nRowStartBrushIndex;

    }//for-each row

    ::DeleteObject(hBlackBrush);
    ::ReleaseDC(hWnd, hDC);
    }





    //@功能:绘制下半部图案
    void DrawLowerPartPattern(HWND hWnd)
    {
    //do nothing if no specified 
    if(hWnd == NULL) return;

    HDC hDC = ::GetDC(hWnd);

    HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,  0 ));//黑色画刷


    //上半部黑色填充
    RECT rc = m_UpperArea;
    ScreenToClient(hWnd, &rc.left);
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);

    int nRowStartBrushIndex = 0;
    int nColBrushIndex = 0;
    int iGridIndex = m_nRowCount*m_nColCount - 1;


    for(int row=m_nRowCount - 1; row > (this->m_nRowCount - 1 - m_nLowerGridRow); row --)
    {
    int offset_row = row - m_nRowCount/2;

    for(int col=0; col < this->m_nColCount; col++)
    {
    int offset_col = col - m_nColCount/2;

    if(0 == nColBrushIndex)
    {
    double R2 = sqrt((double)(offset_row*offset_row  + offset_col*offset_col));

    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right); 
    FillRect(hDC, &rc, m_vecGrayBrushes[int(R2)]);

    }
    else
    {
    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);
    }

    nColBrushIndex = (nColBrushIndex + 1) & 0x01;//%2

    iGridIndex --;

    }//for-each col

    nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
    nColBrushIndex = nRowStartBrushIndex;

    }//for-each row

    ::DeleteObject(hBlackBrush);
    ::ReleaseDC(hWnd, hDC);
    }*/


    //@功能:绘制上半部校正图案
    //@参数:hWnd, 显示图案的窗体
    //      clrHilight, 高亮的颜色
    void DrawUpperPartPattern(HWND hWnd, COLORREF clrHilight)
    {

        if(hWnd == NULL) return;

        HDC hDC = GetDC(hWnd);

        HBRUSH hBlackBrush   = ::CreateSolidBrush(RGB(  0,   0,  0 ));//黑色画刷

        HBRUSH hHilightBrush = ::CreateSolidBrush(clrHilight);//高亮画刷


        //下半部黑色填充
        RECT rc = m_LowerArea;

       //映射屏幕坐标到校正窗体的坐标
        MapWindowPoints(
            NULL,                     //handle to the source window, desktop
            hWnd,       //handle to destination window
            (LPPOINT)&rc,             //array of points to map 
            sizeof(RECT)/sizeof(POINT)//number of points in array
            );


        FillRect(hDC, &rc, hBlackBrush);

        //最后一行的行编号=this->m_nRowCount - 1, 依据行号的奇偶选择画刷类型, 偶编号选择灰度画刷, 奇编号选择黑色画刷
        int nRowStartBrushIndex = (this->m_nRowCount - 1) % 2;

        int nColBrushIndex = nRowStartBrushIndex;
        int iGridIndex = 0;
        for(int row=0; row < this->m_nUpperGridRow; row++)
        {


            for(int col=0; col < this->m_nColCount; col++)
            {
                rc = this->m_vecGrids[iGridIndex];
               //映射屏幕坐标到校正窗体的坐标
                MapWindowPoints(
                    NULL,                     //handle to the source window, desktop
                    hWnd,       //handle to destination window
                    (LPPOINT)&rc,             //array of points to map 
                    sizeof(RECT)/sizeof(POINT)//number of points in array
                    );

                if(0 == nColBrushIndex)
                {
                    FillRect(hDC, &rc, hHilightBrush);

                }
                else
                {
                    FillRect(hDC, &rc, hBlackBrush);
                }
                nColBrushIndex = (nColBrushIndex + 1) &0x01 ;//%2

                iGridIndex ++;

            }//for-each col

            nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
            nColBrushIndex = nRowStartBrushIndex;

        }//for-each row

        ::DeleteObject(hBlackBrush);
        ::DeleteObject(hHilightBrush);

    }





    //@功能:绘制下半部图案
    //@参数:hWnd, 显示图案的窗体
    //      clrHilight, 高亮的颜色
    void DrawLowerPartPattern(HWND hWnd, COLORREF clrHilight)
    {
        //do nothing if no window specified
        if(hWnd == NULL) return;

        HDC hDC = ::GetDC(hWnd);

        HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,  0 ));//黑色画刷
        HBRUSH hHilightBrush = ::CreateSolidBrush(clrHilight);//高亮画刷


        //上半部黑色填充
        RECT rc = m_UpperArea;
       //映射屏幕坐标到校正窗体的坐标
        MapWindowPoints(
            NULL,                     //handle to the source window, desktop
            hWnd,       //handle to destination window
            (LPPOINT)&rc,             //array of points to map 
            sizeof(RECT)/sizeof(POINT)//number of points in array
            );
        FillRect(hDC, &rc, hBlackBrush);
        FillRect(hDC, &rc, hBlackBrush);

        int nRowStartBrushIndex = 0;
        int nColBrushIndex = 0;
        int iGridIndex = m_nRowCount*m_nColCount - 1;


        for(int row=m_nRowCount - 1; row > (this->m_nRowCount - 1 - m_nLowerGridRow); row --)
        {
            int offset_row = row - m_nRowCount/2;

            for(int col=0; col < this->m_nColCount; col++)
            {
                int offset_col = col - m_nColCount/2;

                rc = this->m_vecGrids[iGridIndex];
               //映射屏幕坐标到校正窗体的坐标
                MapWindowPoints(
                    NULL,                     //handle to the source window, desktop
                    hWnd,       //handle to destination window
                    (LPPOINT)&rc,             //array of points to map 
                    sizeof(RECT)/sizeof(POINT)//number of points in array
                    );

                if(0 == nColBrushIndex)
                {
                    FillRect(hDC, &rc, hHilightBrush);

                }
                else
                {
                    FillRect(hDC, &rc, hBlackBrush);
                }

                nColBrushIndex = (nColBrushIndex + 1) & 0x01;//%2

                iGridIndex --;

            }//for-each col

            nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
            nColBrushIndex = nRowStartBrushIndex;

        }//for-each row

        ::DeleteObject(hHilightBrush);
        ::DeleteObject(hBlackBrush);
        //::ReleaseDC(hWnd, hdc);
    }




    //@功能:获取校正图案中矩形块的行数
    int GetRowCount()const
    {
        return m_nRowCount;
    }

    //@功能:获取校正图案中矩形块的列数
    int GetColCount()const
    {
        return m_nColCount;
    }

    //@功能:返回校正点的总数
    int GetCalibrateMarkCount()const
    {
        return m_nCalibrateMarkCount;
    }

    //@功能:获取上半部校正点的数目
    int GetUpperPartMarkCount()const
    {
        return m_nUpperPartMarkCount;
    }

    //@功能:获取上半部矩形块的数目
    int GetUpperGridCount() const
    {
        return m_nUpperPartMarkCount;
    }

    //@功能:获取下半部校正点的数目
    int GetLowerPartMarkCount()const
    {
        return m_nLowerPartMarkCount;
    }

    //@功能:获取下半部矩形块的数目
    int GetLowerGridCount() const
    {
        return m_nLowerPartMarkCount;
    }

    const RECT& GetUpperArea()const
    {
        return m_UpperArea;
    }


    const RECT& GetLowerArea()const
    {
        return m_LowerArea;
    }

    const POINT& GetMarkCoord(int i)const
    {
        assert( i>= 0 && i< this->m_nCalibrateMarkCount);

        return this->m_vecMarkerCoord[i];
    }



    //@功能:判断是否是最后一行的校正点
    const BOOL IsMarkerOfLastRow(int nIndex)
    {
        if( nIndex>= 0 && nIndex< this->m_nCalibrateMarkCount)
        {

            int nFirstIndex =  (m_nRowCount - 2) * (m_nColCount -1);
            int nLastIndex  =  (m_nRowCount - 1) * (m_nColCount -1) - 1;

            if(nFirstIndex <= nIndex && nIndex <= nLastIndex)
            {
                return TRUE;
            }

        }
        return FALSE;
    }




    //@功能:返回圈层的数目
    const int GetLayerCount()const
    {
        return m_vecLayers.size();
    }


    //@功能:返回指定的圈层信息
    const TRectLayer&  GetLayer(int nLayerIndex)const
    {
        assert(nLayerIndex >=0 && nLayerIndex < (int)m_vecLayers.size());

        return m_vecLayers[nLayerIndex];
    }


    //@功能:显示最外圈校正点的屏蔽区
    //@参数:hWnd, 显示屏蔽区的窗体句柄
    //      nOutermostMarkerIndex, 最外圈校正点在最外圈中的序号
    void PrintOutermostMask(HWND hWnd, int nOutermostLayerMarkerIndex)
    {
        if(nOutermostLayerMarkerIndex >= GetOutermostMarkerCount()) return;

        int nMarkGlobalIndex = GetOutmostLayerMarkGlobalIndex(nOutermostLayerMarkerIndex);


        //查找校正点关联的四个Grid的索引号
        int nMarkerEachRow = this->m_nColCount - 1;

        int nLeftTopGridIndex     = nMarkGlobalIndex/nMarkerEachRow * this->m_nColCount + (nMarkGlobalIndex % nMarkerEachRow);
        int nRightBottomGridIndex = nLeftTopGridIndex  + this->m_nColCount + 1;

        HDC hDC = ::GetDC(hWnd);


        //将屏幕刷成全黑
        RECT rcScreen;
        rcScreen.left   = 0;
        rcScreen.top    = 0;
        rcScreen.right  = m_patternSize.cx;
        rcScreen.bottom = m_patternSize.cy;
        FillRect(hDC, &rcScreen, m_hBlackBrush);


        //将屏蔽区刷成白色
        RECT rcGrid;

        rcGrid.left   = m_vecGrids[nLeftTopGridIndex].left;
        rcGrid.top    = m_vecGrids[nLeftTopGridIndex].top;

        rcGrid.right  = m_vecGrids[nRightBottomGridIndex].right;
        rcGrid.bottom = m_vecGrids[nRightBottomGridIndex].bottom;


        FillRect(hDC, &rcGrid, m_hWhiteBrush);

        ReleaseDC(hWnd, hDC);

    }



    //@功能:绘制外圈第一个校正图形
    //@参数:hWnd, 显示校正图案的窗体句柄
    //      nMarkerIndex, 校正点索引号
    void PintOutermostMarker(HWND hWnd, int nOutermostLayerMarkerIndex)
    {
        if(nOutermostLayerMarkerIndex >= GetOutermostMarkerCount()) return;

        int nMarkGlobalIndex = GetOutmostLayerMarkGlobalIndex(nOutermostLayerMarkerIndex);


        //查找校正点关联的四个Grid的索引号
        int nMarkerEachRow = this->m_nColCount - 1;

        int nLeftTopGridIndex     = nMarkGlobalIndex/nMarkerEachRow * this->m_nColCount + (nMarkGlobalIndex % nMarkerEachRow);
        int nRightTopGridIndex    = nLeftTopGridIndex  + 1;
        int nRightBottomGridIndex = nLeftTopGridIndex  + this->m_nColCount + 1;
        int nLeftBottomGridIndex  = nLeftTopGridIndex  + this->m_nColCount;






        HDC hDC = ::GetDC(hWnd);

        RECT rcGrid;

        //A B
        //□■
        //■□
        //D  C
        rcGrid = m_vecGrids[nLeftTopGridIndex];

        //绘制白块A
        FillRect(hDC, &rcGrid, m_hWhiteBrush);


        //绘制黑块B
        rcGrid = m_vecGrids[nRightTopGridIndex];
        FillRect(hDC, &rcGrid, m_hBlackBrush);

        //绘制白块C
        rcGrid = m_vecGrids[nRightBottomGridIndex];
        FillRect(hDC, &rcGrid, m_hWhiteBrush);

        //绘制黑块D
        rcGrid = m_vecGrids[nLeftBottomGridIndex];
        FillRect(hDC, &rcGrid, m_hBlackBrush);


        ReleaseDC(hWnd, hDC);

    }



    //@功能:获取最外圈校正点的数目
    int GetOutermostMarkerCount()
    {
        int nOutermostMarkerCount = 0;

        if(this->m_nRowCount == 2)
        {
            nOutermostMarkerCount = this->m_nColCount - 1;

        }
        else
        {
            nOutermostMarkerCount = (this->m_nColCount - 1) * 2 + (this->m_nRowCount - 3) * 2;
        }

        return nOutermostMarkerCount;
    }


    //@功能:将最外层校正校正点在层中的编号转化为全局编号
    //     ↓[0]                      ↓[m_nColCount - 2]
    //     ¤  ¤  ¤  ¤  ¤  ¤     ¤
    //     ¤                         ¤
    //     ¤                         ¤
    //     .                           .
    //     .                          .
    //     ¤  ¤  ¤  ¤  ¤  ¤     ¤←[m_nColCount - 21] + [m_nRowCount - 2]
    //     ↑
    //     [m_nColCount - 2] + [m_nRowCount - 2] + [m_nColCount - 2] + 
    int GetOutmostLayerMarkGlobalIndex(int nOutermostLayerMarkerIndex)
    {

        if(nOutermostLayerMarkerIndex >= GetOutermostMarkerCount()) return - 1;

        int nMarkGlobalIndex  = nOutermostLayerMarkerIndex;
        int nMarkCountEachRow = m_nColCount - 1;
        int nMarkCountEachCol = m_nRowCount - 1;

        int nOutermostLayerTopRightOutMarkIndex  = nMarkCountEachRow     - 1;
        int nOutermostLayerBottomRightMarkIndex  = nOutermostLayerTopRightOutMarkIndex + nMarkCountEachCol - 1;
        int  nOutermostLayerBottomLeftMarkIndex   = nOutermostLayerBottomRightMarkIndex + nMarkCountEachRow - 1;

        //将"最外圈校正点序号"转化为为全局校正点编号
        if(nOutermostLayerMarkerIndex <= nOutermostLayerTopRightOutMarkIndex)
        {
            nMarkGlobalIndex = nOutermostLayerMarkerIndex;
        }
        else if(nOutermostLayerMarkerIndex <=  nOutermostLayerBottomRightMarkIndex)
        {
            nMarkGlobalIndex = nOutermostLayerTopRightOutMarkIndex + (nOutermostLayerMarkerIndex - nOutermostLayerTopRightOutMarkIndex)*nMarkCountEachRow;
        }
        else if(nOutermostLayerMarkerIndex <= nOutermostLayerBottomLeftMarkIndex)
        {
            nMarkGlobalIndex = nMarkCountEachRow*(m_nRowCount - 2);
            nMarkGlobalIndex += nOutermostLayerBottomLeftMarkIndex  -  nOutermostLayerMarkerIndex;
        }
        else
        {
            nMarkGlobalIndex = (nMarkCountEachCol - 1 - (nOutermostLayerMarkerIndex - nOutermostLayerBottomLeftMarkIndex))*nMarkCountEachRow;
        }

        return nMarkGlobalIndex;

    }






protected:
    //member function
    //@功能:计算校正图案中每个小的矩形块的位置坐标
    void CalcGridCoord(
        const RECT& rcPatternArea      ,//校正图案的区域尺寸
        int         nRows              ,//校正图案中矩形块的行数
        int         nCols              ,//校正图案中矩形块的列数
        int         nLeftMostGridWidth ,//处于图案最左边的小矩形块的宽度
        int         nRightMostGridWidth,//处于图案最右边的小矩形块的宽度
        double      dbFirstRowHeight   ,//第一行行高
        double      dbDiff              //等差数列公差
        )
    {

        //
        int nAreaWidth  = rcPatternArea.right - rcPatternArea.left;
        int nAreaHeight = rcPatternArea.bottom - rcPatternArea.top;


        //除去左右两边矩形块后, 计算中间列的矩形块的宽度
        int nMiddelGridWidth = (nAreaWidth - nLeftMostGridWidth - nRightMostGridWidth)/(nCols - 2);
        int nMiddleGridWidthFractionInc = (nAreaWidth - nLeftMostGridWidth - nRightMostGridWidth )%(nCols - 2);

        m_vecGrids.resize(nRows * nCols);

        RECT    rcGrid;
        double dbRowHeight = 0.0;

        int nGridIndex = 0;

        for(int r = 0; r < nRows; r++)
        {
            //计算垂直坐标
            //第一行
            if(r == 0)
            {
                dbRowHeight   = dbFirstRowHeight;
                rcGrid.top    = rcPatternArea.top;
            }
            else
            {
                dbRowHeight += dbDiff; //行高等差变化

                rcGrid.top    = rcGrid.bottom;
            }
            rcGrid.bottom =  rcGrid.top + (int)(dbRowHeight + .5);

            int nMiddleGridWidthFractionCounter = 0;//矩形块宽度的小数部分累加器

            //计算水平坐标
            for(int c = 0 ; c < nCols; c++)
            {
                //第一列
                if(0== c)//第一列
                {
                    rcGrid.left = rcPatternArea.left;
                    rcGrid.right = rcPatternArea.left + nLeftMostGridWidth;
                }
                else if(nCols - 1 == c)//最后一列
                {  
                    rcGrid.left  = rcGrid.right;
                    rcGrid.right = rcGrid.left + nRightMostGridWidth;
                }
                else //中间列
                {
                    rcGrid.left  = rcGrid.right;
                    rcGrid.right = rcGrid.left + nMiddelGridWidth;

                    nMiddleGridWidthFractionCounter += nMiddleGridWidthFractionInc;
                    if(nMiddleGridWidthFractionCounter >= nCols - 2)
                    {
                        nMiddleGridWidthFractionCounter -= (nCols - 2);
                        rcGrid.right ++;
                    }

                }

                m_vecGrids[nGridIndex++] = rcGrid;
            }//for each column
        }//for each row




    }


    //@功能:确定上下两个区域的面积。
    //@参数:
    //@说明:将区域划分为上下两个大约相等的面积
    void SplitArea(
        const RECT& rcPatternArea      ,//校正图案的区域尺寸)
        int         nRows              ,//校正图案中矩形块的行数
        int         nCols              ,//校正图案中矩形块的列数
        double      dbFirstRowHeight   ,//第一行行高
        double      dbDiff              //等差数列公差
        )

    {
        int nAreaWidth  = rcPatternArea.right  - rcPatternArea.left;
        int nAreaHeight = rcPatternArea.bottom - rcPatternArea.top;


        RECT    rcGrid;
        double dbRowHeight = 0.0;

        for(int r = 0; r < nRows; r++)
        {
            //计算垂直坐标
            //第一行
            if(r == 0)
            {
                dbRowHeight   = dbFirstRowHeight;
                rcGrid.top    = rcPatternArea.top;
            }
            else
            {
                dbRowHeight += dbDiff; //行高等差变化

                rcGrid.top    = rcGrid.bottom;
            }
            rcGrid.bottom =  rcGrid.top + (int)(dbRowHeight + .5);

            //从上往下，底边大于等于一半校正图案高度的棋盘格即为上部分区域的最后一行。
            if(rcGrid.bottom >= nAreaHeight/2)
            {
                m_UpperArea.left   = rcPatternArea.left;
                m_UpperArea.right  = rcPatternArea.right;
                m_UpperArea.top    = rcPatternArea.top;
                m_UpperArea.bottom = rcGrid.bottom;

                //上部分校正点个数
                m_nUpperPartMarkCount = r * (nCols - 1);

                //上部分矩形块个数
                m_nUpperGridCount = (r+ 1) * nCols;

                //上部分矩形块行数
                m_nUpperGridRow = r+1;


                m_LowerArea.left   = rcPatternArea.left;
                m_LowerArea.right  = rcPatternArea.right;
                m_LowerArea.top    = rcGrid.top;
                m_LowerArea.bottom = rcPatternArea.bottom;

                //下部分校正点个数
                m_nLowerPartMarkCount = ((nRows -1) - r ) * (nCols - 1);


                //下部分矩形块个数
                m_nLowerGridCount = (nRows - r ) * nCols;


                //下部分矩形块行数
                m_nLowerGridRow = nRows - r;


                break;
            }

        }//


    }



    //@功能:计算校正图案中每个校正点的坐标
    void UpdateMarkCoord()
    {
        m_vecMarkerCoord.resize(m_nCalibrateMarkCount);

        int nMarkIndex = 0;
        for(int r = 0; r < m_nRowCount - 1; r++)
        {
            for(int c = 0; c < m_nColCount - 1; c++)
            {
                const RECT& rcGrid = m_vecGrids[r*m_nColCount + c];

                m_vecMarkerCoord[nMarkIndex].x = rcGrid.right;
                m_vecMarkerCoord[nMarkIndex].y = rcGrid.bottom;
                nMarkIndex ++;
            }
        }
    }




    //@功能:收集每个圈层上的校正点的编号信息
    void CollectLayerMarkInfo()
    {

        //计算圈层数目
        //公式LayerNum = (min(row, col) + 1)/2
        int nLayerNum = 0;
        if(m_nRowCount > m_nColCount)
        {
            nLayerNum = m_nColCount/2;
        }
        else 
        {
            nLayerNum = m_nRowCount /2;
        }

        m_vecLayers.resize(nLayerNum);


        //一行中校正点的个数
        int nMarkNumInRow = m_nColCount - 1;

        //每处理完一圈校正点后, 剔除一圈校正点后剩下的校正点的列数、行数
        int nMarkCols = m_nColCount - 1;
        int nMarkRows = m_nRowCount - 1;

        //
        for(int L=0; L < nLayerNum; L++)
        {

            TRectLayer& rectLayer = m_vecLayers[L];

            ///第L层中的第一个校正点的编号对应定L列的第L个元素
            int nMarkNo = 0 + L*nMarkNumInRow + L; 
            if(1 == nMarkCols)//当前层只有一列
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkRows);
                rectLayer.m_nHorzMarkNumber = 1;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                for(int i = 0; i < nMarkRows; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }

            }//if
            else if(1 == nMarkRows)//当前层只有一行
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkCols);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = 1;
                for(int i = 0; i< nMarkCols; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo ++;
                }
            }//else if
            else
            {
                int nLayerMarkCount = (nMarkCols - 1) + (nMarkRows - 1) + (nMarkCols - 1) + (nMarkRows - 1);
                rectLayer.m_vecScreenMarkNo.resize(nLayerMarkCount);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                int nLayerMarkIndex = 0;
                for(int i=0; i < nMarkCols -1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo++;
                }

                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }

                for(int i=0; i < nMarkCols-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo--;
                }

                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo -= nMarkNumInRow;
                }

                if(nMarkCols >= 2)
                {
                    nMarkCols -= 2;
                }

                if(nMarkRows >= 2)
                {
                    nMarkRows -= 2;
                }

            }//else

        }//for-each layer

    }


    //@功能:初始化画刷数组
    //@参数:YMax, 最亮值
    //      YMin, 最暗值
    //@说明:数组中每个画刷的亮度与距离屏幕中心的距离成递增关系。
    void InitGrayBrushes(BYTE YMin,BYTE YMax)
    {

        //删除清空已创建的灰度画刷
        DeleteAllGrayBrush();

        //投影机投影时，若打出的图案高亮, 则在广角摄像头中可以看到投影机灯泡造成的亮斑，
        //造成校正图案难以分辨，导致自动校正失败。
        //我们将白块的亮度从中间向外逐渐增强。
        //设白块的亮度与屏幕中心的关系满足
        //Y = A*R^2 + B
        //
        const double dbYmin = (double)YMin;
        const double dbYmax = (double)YMax;

        //R=0, Y = Ymin = B
        double B = YMin;

        //R = Rmax时, Y = Ymax = A*Rmax^(1/2) + B, 
        double Rmax2 = (double)(m_nRowCount*m_nRowCount)/4 + (double)(m_nColCount*m_nColCount)/4;
        double A = (dbYmax - dbYmin)/Rmax2;

        int nBrushCount = int(sqrt(Rmax2) + 0.5);
        m_vecGrayBrushes.resize(nBrushCount);

        for(int R=0; R < nBrushCount; R++)
        {
            double R2 = R*R;
            int Y = int(A*R2 + B);
            m_vecGrayBrushes[R] = ::CreateSolidBrush(RGB(Y , Y , Y ));
        }



        m_hWhiteBrush = ::CreateSolidBrush(RGB(255, 255, 255));
        m_hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,   0));
    }

    void DeleteAllGrayBrush()
    {
        int nSize = m_vecGrayBrushes.size();

        for(int i=0; i < nSize; i ++)
        {

        }

        m_vecGrayBrushes.clear();

        DeleteObject(m_hWhiteBrush);
        DeleteObject(m_hBlackBrush);

    }

    //member variables
    SIZE   m_patternSize        ;//校正图案的尺寸
    int    m_nRowCount          ;//校正图案的矩形块行数
    int    m_nColCount          ;//校正图案的矩形块列数
    int    m_nCalibrateMarkCount;//

    int    m_nUpperPartMarkCount;//上半部校正点个数
    int    m_nUpperGridCount    ;//上半部矩形块总数
    int    m_nUpperGridRow      ;//上半部矩形块行数


    int    m_nLowerPartMarkCount;//下半部校正点个数
    int    m_nLowerGridCount    ;//下半部矩形块总数
    int    m_nLowerGridRow      ;//下半部矩形块行数


    RECT   m_UpperArea          ;//上半部区域
    RECT   m_LowerArea          ;//下半部区域

    std::vector<RECT>  m_vecGrids      ;//校正图案中的每个小的矩形块的位置坐标。
    std::vector<POINT> m_vecMarkerCoord;//校正图案中的每个校正点的坐标
    std::vector<TRectLayer> m_vecLayers;//圈层信息

    std::vector<HBRUSH> m_vecGrayBrushes;//灰度画刷。
    HBRUSH m_hWhiteBrush;//白色画刷
    HBRUSH m_hBlackBrush;//黑色画刷

};//CArithSeriesRowPattern







//圆形校正图案
class CCirclePattern
{
public:

    CCirclePattern()
    {
        m_nRadius   = 40;
        m_nColCount = 9;
        m_nRowCount = 5;
        m_nCalibrateMarkCount = m_nColCount*m_nRowCount;

    }

    int CalculateCalibPatternRadius(E_AutoCalibratePattern ePattern, int nScreenWidth)
    {
        int nCols = 5;
        switch(ePattern)
        {
        case E_CALIBRATE_PATTERN_5_COLS:
            nCols = 5;
            break;

		case E_CALIBRATE_PATTERN_6_COLS:
			nCols = 6;
			break;
		case E_CALIBRATE_PATTERN_7_COLS:
			nCols = 7;
			break;

		case E_CALIBRATE_PATTERN_8_COLS:
			nCols = 8;
			break;

        case E_CALIBRATE_PATTERN_9_COLS:
            nCols = 9;
            break;

        case E_CALIBRATE_PATTERN_10_COLS:
            nCols =10;
            break;

        case E_CALIBRATE_PATTERN_11_COLS:
            nCols = 11;
            break;

        case E_CALIBRATE_PATTERN_12_COLS:
            nCols = 12;
            break;

        case E_CALIBRATE_PATTERN_13_COLS:
            nCols = 13;
            break;

        case E_CALIBRATE_PATTERN_14_COLS:
            nCols = 14;

        case E_CALIBRATE_PATTERN_15_COLS:
            nCols = 15;

        case E_CALIBRATE_PATTERN_16_COLS:
            nCols = 16;
            break;

        case E_CALIBRATE_PATTERN_17_COLS:
            nCols = 17;
            break;

        default:
            nCols = 5;
        }

		int leftMargin  = 10;// (nRadius >> 2) > 10 ? nRadius >> 2): 10;
		int rightMargin = 10;// (nRadius >> 2) > 10 ? nRadius >> 2): 10;

        //W - leftMargin - rightMargin - 2*R = (nCols - 1)*4R

        int R = (nScreenWidth - leftMargin - rightMargin)/(4*(nCols-1) + 2);

        return R;
    }


    //功能:初始化图案
    virtual void InitPattern(int nRadius, const RECT& rcMonitor)
    {

        if(nRadius <= 0) nRadius = 40;
        m_nRadius = nRadius;

        int nScreenWidth  = rcMonitor.right - rcMonitor.left;
        int nScreenHeight = rcMonitor.bottom - rcMonitor.top;

        int leftMargin   = 10;//nRadius/4;
        int rightMargin  = 10;//nRadius/4;
        int topMargin    = 10;//nRadius/4;
        int bottomMargin = 10;//nRadius/4;

        int fourRadius = 4*nRadius;


         m_nColCount  = (nScreenWidth  - leftMargin - rightMargin - nRadius - nRadius) / fourRadius + 1;
         m_nRowCount  = (nScreenHeight - leftMargin - rightMargin - nRadius - nRadius) / fourRadius + 1;
        
         //如果行*列乘积大于MAX_OBJ_NUMBER,减小行数和列数中的大者。
         if (m_nColCount * m_nRowCount > MAX_OBJ_NUMBER)
         {
             if (m_nColCount > m_nRowCount)
             {
                 m_nColCount = MAX_OBJ_NUMBER / m_nRowCount;
             }

             if (m_nRowCount > m_nColCount)
             {
                 m_nRowCount = MAX_OBJ_NUMBER / m_nColCount;
             }
         }

         if (m_nColCount < 2)
         {
             m_nColCount = 2;
         }

         if (m_nRowCount < 2)
         {
             m_nRowCount = 2;
         }
        
        int nHorzInterval_integer     = (nScreenWidth - leftMargin - rightMargin - nRadius - nRadius) / (m_nColCount - 1);
        int nHorzInterval_remainder   = (nScreenWidth - leftMargin - rightMargin - nRadius - nRadius) % (m_nColCount - 1);

        int nVertInterval_integer     = (nScreenHeight - topMargin - bottomMargin - nRadius - nRadius) / (m_nRowCount - 1);
        int nVertInterval_remainder   = (nScreenHeight - topMargin - bottomMargin - nRadius - nRadius) % (m_nRowCount - 1);


        //计算校正点个数
        m_nCalibrateMarkCount= m_nColCount * m_nRowCount;


        m_vecMarkerCoord.resize(m_nCalibrateMarkCount);

        int nCircleIndex = 0;

		int x = rcMonitor.left;
		int y = rcMonitor.top ;
        int x_fraction_count = 0;
        int y_fraction_count = 0;

        y = rcMonitor.top + nRadius;
        for(int r = 0; r < m_nRowCount; r++)
        {
            x = rcMonitor.left + leftMargin + nRadius;
            for(int c = 0; c < m_nColCount; c++)
            {
                
                m_vecMarkerCoord[nCircleIndex].x = x;
                m_vecMarkerCoord[nCircleIndex].y = y;

                nCircleIndex ++;

                x += nHorzInterval_integer;

                x_fraction_count += nHorzInterval_remainder;
                if(x_fraction_count > (m_nColCount - 1))
                {
                    x_fraction_count -= (m_nColCount - 1);
                    x ++;
                }

            }//for(c)

            y += nVertInterval_integer;
            y_fraction_count += nVertInterval_remainder;
            if(y_fraction_count > (m_nRowCount - 1))
            {
                y_fraction_count -= (m_nRowCount - 1);
                y ++;
            }

        }//for(r)
        
        
        //收集圈层信息
        CollectLayerMarkInfo();
    }


	//@功能:返回校正圆的半径。
	int GetCircleRaidus()const
	{
		return m_nRadius;
	}

    //@功能:返回圈层的数目
    int GetLayerCount()const
    {
        return m_vecLayers.size();
    }

    //@功能:返回校正点的总数
    int GetCalibrateMarkCount()const
    {
        return m_nCalibrateMarkCount;
    }


    const POINT& GetMarkCoord(int i)const
    {
        assert( i>= 0 && i< this->m_nCalibrateMarkCount);

        return this->m_vecMarkerCoord[i];
    }



    //@功能:返回指定的圈层信息
    const TRectLayer&  GetLayer(int nLayerIndex)const
    {
        assert(nLayerIndex >=0 && nLayerIndex < (int)m_vecLayers.size());

        return m_vecLayers[nLayerIndex];
    }


    //@功能:绘制完整的校正图案
    virtual void DrawPattern(HWND hWnd, COLORREF clrForeground = RGB(255,255,255), COLORREF clrBackground = RGB(0,0,0))
    {
        //do nothing
        if(hWnd == NULL) return;

        HDC hdc = ::GetDC(hWnd);
        HBRUSH hBlackBrush = ::CreateSolidBrush(clrBackground);//黑色画刷
        HBRUSH hWhiteBrush = ::CreateSolidBrush(clrForeground);//白色画刷

		/*
		COLORREF clrGray = RGB(\
			max(0, GetRValue(clrForeground) - 50),\
			max(0, GetGValue(clrForeground) - 50),\
			max(0, GetBValue(clrForeground) - 50));
		
		HBRUSH hGrayBrush = ::CreateSolidBrush(clrGray);
		*/


        HBRUSH hOldBrush = (HBRUSH)::SelectObject(hdc, hWhiteBrush);

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        FillRect(hdc, &rcClient, hBlackBrush);

        int nMarkCount = m_vecMarkerCoord.size();

        for(int i = 0; i < nMarkCount; i++)
        {
            
            RECT rcCircle;
            rcCircle.left   = m_vecMarkerCoord[i].x - m_nRadius;
            rcCircle.top    = m_vecMarkerCoord[i].y - m_nRadius;
            rcCircle.right  = m_vecMarkerCoord[i].x + m_nRadius;
            rcCircle.bottom = m_vecMarkerCoord[i].y + m_nRadius;
            
			//映射屏幕坐标到校正窗体的坐标
			MapWindowPoints(
				NULL,       //handle to the source window, desktop
				hWnd,       //handle to destination window
				(LPPOINT)&rcCircle,             //array of points to map 
				sizeof(RECT) / sizeof(POINT)//number of points in array
			);

            Ellipse(hdc, 
                rcCircle.left,
                rcCircle.top,
                rcCircle.right,
                rcCircle.bottom);

			/*
			SelectObject(hdc, hGrayBrush);

			int nXRadial1 = rcCircle.right;
			int nYRaidal1 = (rcCircle.top + rcCircle.bottom) >> 1;
			int nXRadial2 = (rcCircle.left + rcCircle.right) >> 1;
			int nYRaidal2 = rcCircle.top;

			Pie(hdc,
				rcCircle.left,
				rcCircle.top,
				rcCircle.right,
				rcCircle.bottom,
				nXRadial1,
				nYRaidal1,
				nXRadial2,
				nYRaidal2);


			 nXRadial1 = rcCircle.left;
			 nYRaidal1 = (rcCircle.top + rcCircle.bottom) >> 1;
			 nXRadial2 = (rcCircle.left + rcCircle.right) >> 1;
			 nYRaidal2 = rcCircle.bottom;

			Pie(hdc,
				rcCircle.left,
				rcCircle.top,
				rcCircle.right,
				rcCircle.bottom,
				nXRadial1,
				nYRaidal1,
				nXRadial2,
				nYRaidal2);
			
			SelectObject(hdc, hWhiteBrush);
			*/
			
       
        }//for
        

          SelectObject(hdc, hOldBrush);
        ::DeleteObject(hWhiteBrush);
        ::DeleteObject(hBlackBrush);
		//::DeleteObject(hGrayBrush);

        ReleaseDC(hWnd, hdc);

    }


protected:
      //@功能:收集每个圈层上的校正点的编号信息
    void CollectLayerMarkInfo()
    {

        //计算圈层数目
        //公式LayerNum = (min(row, col) + 1)/2
        int nLayerNum = 0;
        if(m_nRowCount > m_nColCount)
        {
            nLayerNum = (m_nColCount + 1)/2;
        }
        else 
        {
            nLayerNum = (m_nRowCount  + 1)/2;
        }

        m_vecLayers.resize(nLayerNum);


        //一行中校正点的个数
        int nMarkNumInRow = m_nColCount;

        //每处理完一圈校正点后, 剔除一圈校正点后剩下的校正点的列数、行数
        int nMarkCols = m_nColCount;
        int nMarkRows = m_nRowCount;

        //
        for(int L=0; L < nLayerNum; L++)
        {

            TRectLayer& rectLayer = m_vecLayers[L];

            ///第L层中的第一个校正点的编号对应定L列的第L个元素
            int nMarkNo = 0 + L*nMarkNumInRow + L; 
            if(1 == nMarkCols)//当前层只有一列
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkRows);
                rectLayer.m_nHorzMarkNumber = 1;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                for(int i = 0; i < nMarkRows; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }

            }//if
            else if(1 == nMarkRows)//当前层只有一行
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkCols);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = 1;
                for(int i = 0; i< nMarkCols; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo ++;
                }
            }//else if
            else
            {
                int nLayerMarkCount = (nMarkCols - 1) + (nMarkRows - 1) + (nMarkCols - 1) + (nMarkRows - 1);
                rectLayer.m_vecScreenMarkNo.resize(nLayerMarkCount);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                int nLayerMarkIndex = 0;

                //圈层顶边
                for(int i = 0; i < nMarkCols -1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo++;
                }

                //圈层右边
                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }
                
                //圈层底边
                for(int i=0; i < nMarkCols-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo--;
                }
                
                //圈层左边
                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo -= nMarkNumInRow;
                }

                if(nMarkCols >= 2)//除去本圈层后的一列校正标记的个数。
                {
                    nMarkCols -= 2;
                }

                if(nMarkRows >= 2)//除去本圈层后的一行校正标记的个数。
                {
                    nMarkRows -= 2;
                }

            }//else

        }//for-each layer

    }



protected:
    int                     m_nRadius       ;//圆圈图案的半径
    int                     m_nCalibrateMarkCount;//圆圈图案的个数
    int                     m_nColCount     ;//校正圆圈的列数
    int                     m_nRowCount     ;//校正圆圈的行数
    std::vector<POINT>      m_vecMarkerCoord;//校正图案中的每个校正点的坐标
    std::vector<TRectLayer> m_vecLayers     ;//圈层信息
    std::vector<HBRUSH>     m_vecGrayBrushes;//灰度画刷。
    //HBRUSH                  m_hWhiteBrush   ;//白色画刷
    //HBRUSH                  m_hBlackBrush   ;//黑色画刷
    

};
