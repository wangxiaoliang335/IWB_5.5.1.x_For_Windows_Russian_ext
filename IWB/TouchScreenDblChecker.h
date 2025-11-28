#pragma once



struct TSingleClickEvent
{
    UINT   uId      ;//笔的编号
    POINT  ptClkPos ;//单击位置
    DWORD  dwClkTime;//单击时间

};

struct TDownEvent
{
    POINT ptDownPos   ;//第一次按下位置
    DWORD dwDownTime  ;//第一次按下时间
    BOOL  bAlreadyDown;//已经按下标志
};

//template<int MaxNum>
//class CTouchScreenSingleClickChecker
//{
//
//public:
//    CTouchScreenSingleClickChecker()
//        :
//    m_nTimeThreshold(300)//300毫秒
//    {
//        int nInitPhysicalDimension = int(80.0 * 25.4);//单位:mm
//        int nCx = GetSystemMetrics(SM_CXSCREEN);
//        int nCy = GetSystemMetrics(SM_CYSCREEN);
//
//        SIZE screenSize;
//        screenSize.cx = nCx;
//        screenSize.cy = nCy;
//
//        OnSetTouchScreenDimension(nInitPhysicalDimension, screenSize);
//
//        Reset();
//    }
//
//    ~CTouchScreenSingleClickChecker()
//    {
//
//    }
//
//    void Reset()
//    {
//        memset(&m_aryDownEvent[0], 0, sizeof(m_aryDownEvent));
//        m_nDownEventCount = 0;
//
//        memset(&m_arySingleClickEvent[0], 0, sizeof(m_arySingleClickEvent));
//        m_nSingleClickeEventCount = 0;
//    }
//
//
//    void DoChecker(const TContactInfo* pContactInfos, int nContactCount)
//    {
//        m_nSingleClickeEventCount = 0;
//
//        for(int i=0; i < nContactCount; i++)
//        {
//            const TContactInfo* pPen = pContactInfos + i;
//
//            if(pPen->uId >= MaxNum)
//            {
//                continue;
//            }
//
//            UINT uId = pPen->uId;
//            switch(pPen->ePenState)
//            {
//            case E_PEN_STATE_UP:
//                //检测单击时间
//                {
//                    DWORD dwNow = GetTickCount();
//
//                    const POINT& ptContact = pPen->pt;
//
//                    if(!m_aryDownEvent[uId].bAlreadyDown)
//                    {//出错了, 发现次id对应的笔尽然还未按下
//                        break;
//                    }
//
//                    const POINT& ptOldDownPos = m_aryDownEvent[uId].ptDownPos;
//
//                    int dx   = ptContact.x - ptOldDownPos.x;
//                    int dy   = ptContact.y - ptOldDownPos.y;
//                    int dist2 = dx*dx + dy*dy;
//
//                    int dT   = dwNow - m_aryDownEvent[uId].dwDownTime;
//
//                    if(dT  < m_nTimeThreshold && dist2  < m_nDistance2Threshold)
//                    {//时间和空间门限都满足,则认为单击事件触发。
//                        m_arySingleClickEvent[m_nSingleClickeEventCount].uId       = uId;
//                        m_arySingleClickEvent[m_nSingleClickeEventCount].ptClkPos  = ptOldDownPos;
//                        m_arySingleClickEvent[m_nSingleClickeEventCount].dwClkTime = dwNow;
//                        m_nSingleClickeEventCount ++;
//                    }
//
//                    m_aryDownEvent[uId].bAlreadyDown = FALSE;//复位按下标志, 清除按下事件
//
//
//                }
//
//
//                break;
//
//
//            case E_PEN_STATE_DOWN:
//                if(!m_aryDownEvent[uId].bAlreadyDown)
//                {
//                    m_aryDownEvent[uId].dwDownTime   = GetTickCount();
//                    m_aryDownEvent[uId].ptDownPos    =pPen->pt;
//                    m_aryDownEvent[uId].bAlreadyDown = TRUE;
//                    m_nDownEventCount ++;
//                }
//
//                break;
//            }//switch
//
//        }
//    }
//
//    //@功能:设置触屏的尺寸事件响应函数
//    //@参数:nPhysicalDiagonalLength, 屏幕尺寸,对角线长度, 单位:毫米
//    //      szLogicalDimension, 屏幕逻辑尺寸，单位:像素
//    //
//    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
//    {
//        int  nCxScreen = szLogicalDimension.cx;
//        int  nCyScreen = szLogicalDimension.cy;
//
//        int  nLogicalDiagonalLength = (int)sqrt(double(nCxScreen *  nCxScreen + nCyScreen * nCyScreen));
//
//        int  nDistanceThreshold = nLogicalDiagonalLength * MAX_SINGLE_CLICK_OFFSET_PHY_DISTANCE /nPhysicalDiagonalLength;
//
//         m_nDistance2Threshold = nDistanceThreshold * nDistanceThreshold;
//    }
//
//    const TSingleClickEvent* GetSingleClickEvent(int* pEventCount)
//    {
//        if(pEventCount) 
//        {
//            *pEventCount = m_nSingleClickeEventCount;
//        }
//        return &m_arySingleClickEvent[0];
//    }
//protected:
//    struct TDownEvent
//    {
//        POINT ptDownPos   ;//第一次按下位置
//        DWORD dwDownTime  ;//第一次按下时间
//        BOOL  bAlreadyDown;//已经按下标志
//    };
//
//
//    TDownEvent          m_aryDownEvent[MaxNum];//触点按下事件数组
//    int                 m_nDownEventCount     ;//触点按下事件个数
//
//    TSingleClickEvent   m_arySingleClickEvent[MaxNum];//单击事件数组
//    int                 m_nSingleClickeEventCount     ;//单击事件个数
//
//
//    int  m_nDistance2Threshold;//距离平方门限,单位:像素
//    int  m_nTimeThreshold     ;//时间门限,单位:毫秒
//
//    static const int MAX_SINGLE_CLICK_OFFSET_PHY_DISTANCE = 7;//单位:毫米,单击时允许的空间容错距离
//
//};


//触屏双击事件检测器。
//如果第二次按下位置与第一次按下位置小于一定范围,且时间间隔在设定门限内,则将第二次按下的位置坐标在送入虚拟触屏驱动前修改为第一次按下时的位置坐标.
//
template<int MaxNum>
class CTouchScreenDblClickChecker
{
public:
    CTouchScreenDblClickChecker()
        :
    m_nDlbClkTimeThreshold(500)//双击时间门限, 单位:毫秒
    {
        int nInitPhysicalDimension = int(80.0 * 25.4);//80inches, 单位:mm
        int nCx = GetSystemMetrics(SM_CXSCREEN);
        int nCy = GetSystemMetrics(SM_CYSCREEN);

        SIZE screenSize;
        screenSize.cx = nCx;
        screenSize.cy = nCy;

        OnSetTouchScreenDimension(nInitPhysicalDimension, screenSize);

        Reset();
    }

    ~CTouchScreenDblClickChecker()
    {


    }

    void Reset()
    {
        memset(&m_aryDownEvents[0], 0, sizeof(m_aryDownEvents));
        m_nDownEventCount = 0;

        m_nSingleClickeEventCount = 0;
        memset(&m_arySingleClickEvents[0], 0, sizeof(m_arySingleClickEvents));
    }


    //@功能:设置触屏的尺寸事件响应函数
    //@参数:nPhysicalDiagonalLength, 屏幕尺寸,对角线长度, 单位:毫米
    //      szLogicalDimension, 屏幕逻辑尺寸，单位:像素
    //
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
    {
        int  nCxScreen = szLogicalDimension.cx;
        int  nCyScreen = szLogicalDimension.cy;

        int  nLogicalDiagonalLength = (int)sqrt(double(nCxScreen *  nCxScreen + nCyScreen * nCyScreen));
        int  nDistanceThreshold = nLogicalDiagonalLength * MAX_DBL_CLICK_OFFSET_PHY_DISTANCE /nPhysicalDiagonalLength;

        m_nDblClkSpan2Threshold = nDistanceThreshold * nDistanceThreshold;

    }


    //@参数:pContactInfos, 输入/输出参数，触点信息
    //      nContactCount, 触点个数
    //
    //@功能:如果检测到满足双击触发条件，则让第二次按下的点的坐标等于第一次按下的坐标
    //@说明:算法说明
    //     1)开始， 单击事件集合SinglClickEvents为空
    //     2)读入一个触控点c的信息，信息包括触控点编号，触控点坐标
    //       触控点c的编号为id,
    //       i)如果触控点c的状态为按下, 
    //       将触点c的与SinglClickEvents中的每一个元素进行距离比较和时间比较
    //       如果时间和距离差值均满足双击触发门限，则强制将触控点c的位置修改为与之相匹配的单击事件的起始位置，以便系统触发双击事件。
    //       如果触控点c不与任何单击事件匹配，且"按下事件集合"DownEvents中未记录编号为id的触点的信息，则在集合DownEvents中新增一个编号为id的记录。
    //
    //       ii)如果触控点c的状态为弹起
    //       则根据"按下事件集合"DownEvents中查找编号为id的信息内容,判断时间和距离之差是否满足判断门限, 
    //      如果满足则在"单击事件集合"SinglClickEvents中新生成一个单击事件, 同时从DownEvents集合众移除记录。
    //
    //     3)移除"单击事件集合"SinglClickEvents和"按下事件集合"DownEvents中所有超过时间门限的记录。
    //P.S:
    //一次单击事件的判断标准
    //1.弹起按下时距离在范围R内。
    //2.按下弹起时间间隔在T内。
    //3.按下弹起为同一支笔
    //
    //

    void DoChecker(TContactInfo* pContactInfos, int nContactCount)
    {
        int i,j;
        //1.首先查找按下的触点与记录的"单击事件"数组进行空间和时间相关
        //   若相关，则修改触点坐标为第一次按下时的坐标
        //  若不相关，则作为新的"按下事件"记录
        DWORD dwNow = GetTickCount();

        //2.让弹起的点与"按下事件"进行空间和时间相关, 以便生成"单击事件"
       for(i=0; i < nContactCount; i++)
       {
            TContactInfo* pPenInfo = pContactInfos + i;

            UINT uId = pPenInfo->uId;

            if (uId >= MaxNum)
            {
                continue;//避免越界访问
            }
            
            switch(pPenInfo->ePenState)
            {
                  case E_PEN_STATE_DOWN://笔按下
                      {
                          BOOL bDblClkTriggered = FALSE;//双击触发标志

                          for(j = 0; j < m_nSingleClickeEventCount; j++)
                          {
                                const TSingleClickEvent& oEvent = m_arySingleClickEvents[j];
                                
                                int dT    = dwNow - oEvent.dwClkTime;
                                
                                int dx     = pPenInfo->pt.x  - oEvent.ptClkPos.x;
                                int dy     = pPenInfo->pt.y  - oEvent.ptClkPos.y;
                                int dSpan2 = dx*dx + dy*dy;

                                //AtlTrace(_T("dT=%d, dSpan2 =%d\n"), dT, dSpan2);
                                if(dT < m_nDlbClkTimeThreshold && dSpan2 < m_nDblClkSpan2Threshold)
                                {
                                    
                                    //！！！核心！！！
                                    //触发了双击事件
                                    //修改笔的位置，使之等于第一次按下时刻的位置，利于系统生成双击事件。
                                    pPenInfo->pt = oEvent.ptClkPos;

                                    bDblClkTriggered = TRUE;

                                    //从"单击事件"集合中移除此单击事件
                                    int k = 0;
                                    for(k = j; k < (m_nSingleClickeEventCount - 1); k++)
                                    {
                                        m_arySingleClickEvents[k] = m_arySingleClickEvents[k+1];
                                    }

                                    m_nSingleClickeEventCount -- ;

                                    //<<debug
                                    //static int s_count = 0;
                                    //AtlTrace(_T("[Touch Double Click Checker][%d]double click triggered.\n"), s_count++);
                                    //debug>>
                                    break;
                                }
                          }//for-each(i)


                        if(!bDblClkTriggered)
                        {//未触发双击事件,则在"按下事件"集合中添加一条记录
                            if(!m_aryDownEvents[uId].bAlreadyDown)
                            {
                                m_aryDownEvents[uId].dwDownTime   = GetTickCount();
                                m_aryDownEvents[uId].ptDownPos    = pPenInfo->pt;
                                m_aryDownEvents[uId].bAlreadyDown = TRUE;
                                m_nDownEventCount ++;
                            }
                        }

                      }//case
                        break;

                  case E_PEN_STATE_UP://笔弹起
                        //检测单击事件
                        {
                            DWORD dwNow = GetTickCount();

                            const POINT& ptContact = pPenInfo->pt;

                            if(!m_aryDownEvents[uId].bAlreadyDown)
                            {//出错了, 发现次id对应的笔尽然还未按下
                                break;
                            }

                            const POINT& ptOldDownPos = m_aryDownEvents[uId].ptDownPos;

                            int dx   = ptContact.x - ptOldDownPos.x;
                            int dy   = ptContact.y - ptOldDownPos.y;
                            int dist2 = dx*dx + dy*dy;

                            int dT   = dwNow - m_aryDownEvents[uId].dwDownTime;

                            if(dT  < m_nDlbClkTimeThreshold && dist2  < m_nDblClkSpan2Threshold)
                            {//时间和空间门限都满足,则认为单击事件触发。

                                if(m_nSingleClickeEventCount < _countof(m_arySingleClickEvents))
                                {
                                    m_arySingleClickEvents[m_nSingleClickeEventCount].uId       = uId;
                                    m_arySingleClickEvents[m_nSingleClickeEventCount].ptClkPos  = ptOldDownPos;
                                    //m_arySingleClickEvents[m_nSingleClickeEventCount].ptClkPos  = ptContact;
                                    m_arySingleClickEvents[m_nSingleClickeEventCount].dwClkTime = dwNow;
                                    m_nSingleClickeEventCount ++;
                                }
                            }

                            m_aryDownEvents[uId].bAlreadyDown = FALSE;//既然已经抬起，从"按下事件"集合众清除记录
                            m_nDownEventCount -- ;
                            
                        }//case E_PEN_STATE_UP

                        break;

            }//switch

       }//for(i)

        //3.移除"单击事件"集合中的符合"双击超时"判据的记录。
        i = 0;
        while(i < m_nSingleClickeEventCount)
        {
            DWORD dwElapse = dwNow - m_arySingleClickEvents[i].dwClkTime;

            if((int)dwElapse > m_nDlbClkTimeThreshold)
            {
                //从后往前挪动一个元素
                for(j = i; j < m_nSingleClickeEventCount - 1; j++)
                {
                    m_arySingleClickEvents[j] = m_arySingleClickEvents[j+1];
                    
                }//for(j)

                m_nSingleClickeEventCount --;
            }
            else
            {
                i ++;
            }
        }//while(i)


        //在长按情形下, 发现移除"按下事件"集合中的超时记录,按下又事件会被立即重新记录。
        //
        //移除"按下事件"集合中的符合"超时"判据的记录
        //for( i = 0; i < _countof(m_aryDownEvents); i++)
        //{
        //    if(!m_aryDownEvents[i].bAlreadyDown)
        //    {
        //        continue;
        //    }

        //    DWORD dwElapse = dwNow - m_aryDownEvents[i].dwDownTime;
        //    if(dwElapse > m_nDlbClkTimeThreshold)
        //    {
        //        m_aryDownEvents[i].bAlreadyDown = FALSE;
        //        m_nDownEventCount --;
        //    }
        //}//for



        
    }

protected:
    //CTouchScreenSingleClickChecker<MaxNum> m_oSingleClickChecker;

    TSingleClickEvent   m_arySingleClickEvents[MaxNum] ;//单击事件数组,
    int                 m_nSingleClickeEventCount      ;//单击事件个数数

    TDownEvent          m_aryDownEvents[MaxNum]        ;//触点按下事件数组
    int                 m_nDownEventCount              ;//触点按下事件个数



    int  m_nDblClkSpan2Threshold;//双击距离平方门限,单位:像素
    int  m_nDlbClkTimeThreshold ;//双击时间门限,单位:毫秒

    static const int MAX_DBL_CLICK_OFFSET_PHY_DISTANCE = 8;//单位:毫米,单击时允许的空间容错距离


};