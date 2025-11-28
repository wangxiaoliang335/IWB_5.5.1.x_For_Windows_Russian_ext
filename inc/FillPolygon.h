#pragma once

//分析:N个顶点组成的多边形,有N个线段。每条扫描线顶多和N个线段相交。
//     需要记录每行水平扫描线与各个线段的交点。
//
inline void DebugHeap2()
{
	std::vector<int> vecTemp;
	vecTemp.resize(10000);

	DWORD dwFlags = 0x0;

	BOOL bRet = HeapValidate(GetProcessHeap(), dwFlags, NULL);

	vecTemp[0] = 2;

	if (!bRet)
	{
		::DebugBreak();
	}

}
class CPolygonScanLineInfo
{
public:
    CPolygonScanLineInfo()
        :
    m_LineStatistic(NULL),
        m_pHorzPos(NULL),
        m_nMaxPtPerScanLine(0),
        m_nHeight(0)
    {

    }

    ~CPolygonScanLineInfo()
    {
        if(m_LineStatistic)
        {
            delete [] m_LineStatistic;
        }

        if(m_pHorzPos)
        {
            delete[] m_pHorzPos;
        }
    }


    //功能:设置画面尺寸
    void SetSize(int nImageHeight, int nPolygonVetexNum)
    {
        if(nImageHeight != m_nHeight)
        {
            if(m_LineStatistic)
            {
                delete[] m_LineStatistic;
                m_LineStatistic = NULL;
            }

            m_LineStatistic = new int[nImageHeight];

        }

        //初始化
        memset(m_LineStatistic, 0, sizeof(int)*nImageHeight);

        if(nImageHeight != m_nHeight || nPolygonVetexNum != m_nMaxPtPerScanLine)
        {
            if(m_pHorzPos)
            {   
                delete[] m_pHorzPos;
                m_pHorzPos = NULL;
            }
            m_pHorzPos = new int[nImageHeight*nPolygonVetexNum];
         
			m_nMaxPtPerScanLine = nPolygonVetexNum;
			m_nHeight           = nImageHeight;

        }
		
        memset(m_pHorzPos, 0, sizeof(int)*nImageHeight*nPolygonVetexNum);
    }


    //@功能:记录交点数据
    void AddPoint(const POINT& ptNew)
    {
        if(ptNew.y < 0 ) return;
        if(ptNew.y >=  m_nHeight) return;

        int& rPtNum = m_LineStatistic[ptNew.y];

        int* pHorzPos = &m_pHorzPos[ptNew.y * m_nMaxPtPerScanLine];

        //按照水平坐标的大小使用插入法排序
        int i=0;
        for(i= 0; i < rPtNum; i++)
        {
            if(pHorzPos[i]  < ptNew.x)
            {
                continue;
            }
            else
            {

                break;
            }

        }

        //i位置以后的元素向后移动一个单元,空出一个位置。
        for(int j= rPtNum; j>i; j--)
        {
            pHorzPos[j] = pHorzPos[j-1];
        }

        pHorzPos[i] = ptNew.x;

        //边界点数目增加1
        rPtNum ++;

    }



    //@功能:获取每个扫描行的边界点的数目
    int GetScanLinePtNumber(int nScanLineNo)const
    {
        if(nScanLineNo < 0) return 0;
        if(nScanLineNo > m_nHeight) return 0;
        return m_LineStatistic[nScanLineNo];
    }


    const INT* GetIntersectList(int nScanLineNo)const
    {
        if(nScanLineNo < 0) return 0;
        if(nScanLineNo > m_nHeight) return 0;
        return &m_pHorzPos[nScanLineNo * m_nMaxPtPerScanLine];
    }

protected:

    int*   m_LineStatistic   ;//每条水平扫描线与多边形的交点的统计计数。
    int*   m_pHorzPos        ;//边界点水平坐标数组
    int   m_nMaxPtPerScanLine;//每条水平扫描线与多边形交点的最大数目
    int   m_nHeight          ;//图片高度

};



//@功能:在灰度图片中填充多边形
//@参数:pCanvas, 指向画布的指针
//      nWidth, 画布宽度
//      nHeight, 画布高度
//      pVertices, 多边形顶点坐标数组
//      nVertexNum, 顶点个数
//      gray, 填充的灰度值
//      bDrawBorder, 绘制边界标志
inline void FillPolygon(BYTE* pCanvas, int nWidth, int nHeight, const POINT* pVertices, int nVertexNum, BYTE gray, BOOL bDrawBorder)
{
     //static CPolygonScanLineInfo edgePointInfo;//扫描线信息，静态变量避免重复初始化，提高速度。
	 CPolygonScanLineInfo edgePointInfo;//扫描线信息, 堆栈变量，便于多线程使用。
	 static HANDLE s_oldHeap = (HANDLE)-1;

	 if (s_oldHeap != GetProcessHeap())
	 {
		 if (s_oldHeap != (HANDLE)-1)
		 {
			 ::DebugBreak();
		 }
		 s_oldHeap = GetProcessHeap();
	 } 


    edgePointInfo.SetSize(nHeight, nVertexNum);

    if(nVertexNum < 3) return;//顶点数目<3, 立即返回


    POINT  nextSegmentEndVertex,curSegmentStartVertex, curSegmentEndVertex;

    curSegmentStartVertex = pVertices[0];
    curSegmentEndVertex   = pVertices[1];
    nextSegmentEndVertex  = pVertices[2];

    int nNextSegmentEndVerextIndex = 2;

    int nSegmentEndX = 0;
    BYTE borderColor = gray;
    for(int iLine = 0; iLine< nVertexNum; iLine ++)
    {
        POINT start = curSegmentStartVertex;
        POINT end   = curSegmentEndVertex;

        if(start.y == end.y)//水平直线
        {
            if(bDrawBorder)
            {
                
                int x0,x1;
                if(start.x > end.x)
                {
                    x0 = end.x;
                    x1 = start.x;
                }
                else
                {
                   x0 = start.x;
                   x1 = end.x;
                   
                }
                for(int x = x0; x <= x1; x++)
                {
                    if(0<= start.y &&  start.y < nHeight && 0 <= x && x < nWidth)
                    {
                        pCanvas[start.y * nWidth + x] = borderColor;
                    }
                }
            }
        }
        else//
        {

            edgePointInfo.AddPoint(start);

            LONG dx =  (end.x - start.x > 0)?end.x - start.x : start.x - end.x ;
            LONG dy  = (end.y - start.y > 0)?end.y - start.y :start.y  - end.y;
            LONG sx, sy, err, e2;

            sx = (start.x < end.x)?1:-1;
            sy = (start.y < end.y)?1:-1;
            err = dx -dy;

            do
            {
                if(bDrawBorder)
                {
                    if(0<= start.y &&  start.y < nHeight && 0 <= start.x && start.x < nWidth)
                    {
                        pCanvas[start.y * nWidth + start.x] = borderColor;
                    }
                }

                if(start.x == end.x && start.y == end.y) 
                {//遇到线段终点,判断相邻两个线段是否在同侧

                    //搜索下一条非水平的线段的终点
                    POINT nextNonHorizontalSegmentEndVertex = nextSegmentEndVertex;
                    int nSearchIndex = nNextSegmentEndVerextIndex;
                    while(nextNonHorizontalSegmentEndVertex.y == end.y)
                    {
                        nSearchIndex ++;
                        if(nSearchIndex == nVertexNum)
                        {
                            nSearchIndex = 0;
                        }

                        nextNonHorizontalSegmentEndVertex = pVertices[nSearchIndex];
                    }


                    //判断当前线段的起点与下一条非水平的线段的终点的Y的变化关系
                    //在同侧则线段终点作为边界点加入, 和下一条
                    //线段的起点为同一点。加入两次目的是确保正确填充。
                    if(nextNonHorizontalSegmentEndVertex.y > end.y  && curSegmentStartVertex.y > end.y
                        ||
                        nextNonHorizontalSegmentEndVertex.y < end.y  && curSegmentStartVertex.y < end.y)
                    {
                        start.x = nSegmentEndX;
                        edgePointInfo.AddPoint(start);
                    }

                    break;
                }


                e2 = err*2;
                if(e2 > -dy)
                {
                    err -= dy;
                    start.x += sx; 
                }
                if(e2 < dx)
                {
                    err += dx;
                    start.y  += sy;

                    if(start.y != end.y)//y变化了记入扫描信息
                    {
                        edgePointInfo.AddPoint(start);
                    }
                    else
                    {
                        nSegmentEndX = start.x;
                    }
               }
            }while(1);

        }//else

        curSegmentStartVertex = curSegmentEndVertex;
        curSegmentEndVertex   = nextSegmentEndVertex;

        nNextSegmentEndVerextIndex ++;

        if(nNextSegmentEndVerextIndex == nVertexNum)
        {
            nextSegmentEndVertex = pVertices[0];
            nNextSegmentEndVerextIndex = 0;
        }
        else
        {
            nextSegmentEndVertex = pVertices[nNextSegmentEndVerextIndex];
        }


    }//for each segments

    //根据记录的多变形边界信息, 两两一组配对填充
    for(int y=0; y <nHeight; y++)
    {
        int nPtNumber = edgePointInfo.GetScanLinePtNumber(y);

        //if(nPtNumber % 2 != 0)
        //{
        //    //assert(false);
        //}
        const INT* pIntersectList = edgePointInfo.GetIntersectList(y);

        for(int i = 0 ;i < nPtNumber; i+=2)
        {
            int x1 = pIntersectList[i];
            int x2 = pIntersectList[i+1];
            for(int x = x1;x <= x2;x++)
            {
                
                if(0<= y &&  y < nHeight && 0 <= x && x < nWidth)
                {
                    pCanvas[y * nWidth + x] = gray;
                }
            }
        }
    }//for



}