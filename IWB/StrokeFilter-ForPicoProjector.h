#pragma once
//笔迹滤波器
//X(smooth) = (1 - λ) * X(last) + λ*X(new)
//
//随着笔迹前后两帧移动距离矢量的长度, 动态更新滤波系数λ。
//λ = K*|ΔD|^2 + b
//设|ΔD^2| <= D1时, λ = 0,即运动矢量距离<D1，则笔迹位置保持不变
//设|ΔD^2| >= D2时,,λ = 1,即运动矢量距离>D2，不进行滤波 笔迹自动跟随新的输入点。
//
//为了规避浮点运算, 将系数k,b,和滤波系数λ都扩大1024倍，这样滤波后的坐标值需要右移10位。
//
//
template <int MAX_STROKE_NUM>
class CStrokeFilter
{
public:
    CStrokeFilter()
    {
        //////////////////////////////////////////////////////////////////////////
        //根据物理尺寸和屏幕分辩率，计算相应的逻辑距离D1和D2
        int nScreenDiagonalPhysicalLength = int(DEFAULT_SCREEN_DIAGONAL_LENGTH);//缺省物理尺寸80inches
        int nCx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        SIZE screenResolution;
        screenResolution.cx = nCx;
        screenResolution.cy = nCy;

        OnSetTouchScreenDimension(nScreenDiagonalPhysicalLength, screenResolution);
        //////////////////////////////////////////////////////////////////////////



        ////根据方程组计算方程
        ////λ = K*|Δd^2| + b
        ////的K系数和b系数
        ////
        //// 0 = K*D1 + b
        //// 1 = K*D2 + b
        ////注意:
        //m_Coef_K = MAGINIFY/(D2 - D1);
        //m_Coef_B = -m_Coef_K*D1;

        ////初始化滤波器状态
        //for(UINT32 i = 0; i < _countof(m_aryFilters); i++)
        //{
        //   // for(UINT32 j = 0; j < DEBOUCNE_DELAY; j++)
        //   //{
        //       //m_aryFilters[i].m_LastPoints[j].x  = 0;
        //       // m_aryFilters[i].m_LastPoints[j].y  = 0;
        //        m_aryFilters[i].m_LastPoint.x = 0;
        //        m_aryFilters[i].m_LastPoint.y = 0;
        //    //}
        //    m_aryFilters[i].m_eFilterState   = E_FILTER_STATE_UP;
        //    //m_aryFilters[i].m_uFrameCount    = 0;

        //};

    }

    ~CStrokeFilter()
    {

    }

    //@功能:设置触屏的尺寸事件响应函数
    //@参数:nPhysicalDiagonalLength, 屏幕尺寸,对角线长度, 单位:毫米
    //      szLogicalDimension, 屏幕逻辑尺寸，单位:像素
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
    {
        int nCx = szLogicalDimension.cx;
        int nCy = szLogicalDimension.cy;

        int nScreenDialgonalLogicalLength = (int)sqrt((float)(nCx*nCx + nCy*nCy));


        m_nD1 = (KD1PhysicalDistance * nScreenDialgonalLogicalLength)/nPhysicalDiagonalLength;
        m_nD1 *= m_nD1;

        //modified by xuke,2016/09/24
        //m_nD1 = 1;


        m_nD2 = (KD2PhysicalDistance * nScreenDialgonalLogicalLength)/nPhysicalDiagonalLength;
        m_nD2 *= m_nD2;

        //m_nD2 = 3;

        InitFilter();

        //int nDefaultScreenDiagonalPhysicalLength = int(DEFAULT_SCREEN_DIAGONAL_LENGTH);//缺省物理尺寸80inches
        //int nDefaultScreenDiagonalLogicalLength = int(DEFAULT_SCREEN_DIAGONAL_LOGICAL_LENGTH);//缺省逻辑尺寸

        //int nCx = szLogicalDimension.cx;
        //int nCy = szLogicalDimension.cy;
        //int nScreenDialgonalLogicalLength = (int)sqrt((float)(nCx*nCx + nCy*nCy));

        //int nRatio = (int)(((float)(nDefaultScreenDiagonalPhysicalLength * nScreenDialgonalLogicalLength) / (nPhysicalDiagonalLength * nDefaultScreenDiagonalLogicalLength)) + 0.5);

        //if (nRatio > 1)
        //{            
        //    m_nD1 = 4 + nRatio;
        //    m_nD2 = 10 + nRatio;
        //}
        //else if (nRatio < 1)
        //{
        //    m_nD1 = 4 - nRatio;
        //    m_nD2 = 10 - nRatio;
        //}
        //else
        //{
        //    m_nD1 = 4;
        //    m_nD2 = 10;
        //}

        //m_nD1 *= m_nD1;
        //m_nD2 *= m_nD2;

        //InitFilter();

    }


    //@功能:对输入的笔迹进行滤波处理
    //@参数:pContactInfo, 输入/输出参数, 触控点信息
    //      nCount, 输入的触控点个数
    void DoFilter(TContactInfo* pContactInfo, int nCount)
    {
        for(int i=0; i < nCount; i++)
        {
            TContactInfo& contactInfo = pContactInfo[i];
            UINT32 uId = contactInfo.uId;

            if(uId >= MAX_STROKE_NUM)
            {//数组下标越界。
                continue;
            }

            TFilterState& filter = m_aryFilters[uId];

            switch(filter.m_eFilterState)
            {
            case E_FILTER_STATE_UP://弹起状态

                if(pContactInfo->ePenState == E_PEN_STATE_DOWN)
                {
                    //filter.m_uFrameCount = 0;

                    //contactInfo.ePenState  = E_PEN_STATE_UP;//让触控点暂时保持弹起状态，实现延迟按下效果

                    filter.m_eFilterState = E_FILTER_STATE_DN;
                    filter.m_LastPoint    = contactInfo.pt;
                }
                 
                break;


            //case E_FILTER_STATE_DN_1://"按下"暂态
            //    if(contactInfo.ePenState ==  E_PEN_STATE_UP)
            //    {
            //        filter.m_uFrameCount  = 0;
            //        filter.m_eFilterState = E_FILTER_STATE_UP;
            //    }
            //    else if(contactInfo.ePenState == E_PEN_STATE_DOWN)
            //    {
            //        filter.m_LastPoints[filter.m_uFrameCount] = contactInfo.pt;

            //        filter.m_uFrameCount ++;
            //        if(DEBOUCNE_DELAY == filter.m_uFrameCount)
            //        {
            //           filter.m_eFilterState = E_FILTER_STATE_DN_2;//
            //        }
            //        else
            //        {
            //            contactInfo.ePenState  = E_PEN_STATE_UP;//更新触控点的状态，暂时保持弹起状态，实现延迟按下效果
            //        }
            //    }
            //    break;


            case E_FILTER_STATE_DN://"按下"稳态
                if(contactInfo.ePenState ==  E_PEN_STATE_UP)
                {
                    //filter.m_uFrameCount  = 0;
                    filter.m_eFilterState = E_FILTER_STATE_UP;
                }
                else if(contactInfo.ePenState == E_PEN_STATE_DOWN)
                {
                    const POINT& ptLast  = filter.m_LastPoint;
                    const POINT& ptNew   = contactInfo.pt;

                    //int abs_dx = ptNew.x > ptLast.x ? (ptNew.x - ptLast.x):(ptLast.x - ptNew.x);
                    //int abs_dy = ptNew.y > ptLast.y ? (ptNew.y - ptLast.y):(ptLast.y - ptNew.y);

                    int dx = ptNew.x - ptLast.x;
                    int dy = ptNew.y - ptLast.y;

                    int dx2 = dx*dx;
                    int dy2 = dy*dy;

                    int d2   = dx2 + dy2;
                   

                    POINT ptSmooth;

                    //计算x平滑位置
                    if(d2 < m_nD1)
                    {
                        ptSmooth = ptLast;
                        
                    }
                    else if( m_nD1 <= d2 && d2 <= m_nD2)
                    {
                        //计算平滑系数                        
                        int Lambda = m_Coef_K*d2 + m_Coef_B;
                        ptSmooth.x = ((MAGINIFY - Lambda) * ptLast.x + Lambda * ptNew.x) >> 10;
                        ptSmooth.y = ((MAGINIFY - Lambda) * ptLast.y + Lambda * ptNew.y) >> 10;
                    }
                    else
                    {
                        ptSmooth = ptNew;
                    }

                    contactInfo.pt = ptSmooth;

                    //for(int j=0; j< E_FILTER_STATE_DN_2 - 1; j++)
                    //{
                    //    filter.m_LastPoints[j] = filter.m_LastPoints[j +1];
                    //}

                   filter.m_LastPoint = ptSmooth;

                }

                break;

            }//switch


        }
        
        
    }

private:
    //<added by Jiqw 201505191136
    void InitFilter()
    {
        //根据方程组计算方程
        //λ = K*|Δd^2| + b
        //的K系数和b系数
        //
        // 0 = K*D1 + b
        // 1 = K*D2 + b
        //注意:
        m_Coef_K = MAGINIFY/(m_nD2 - m_nD1);
        m_Coef_B = -m_Coef_K*m_nD1;

        //初始化滤波器状态
        for(UINT32 i = 0; i < _countof(m_aryFilters); i++)
        {
            // for(UINT32 j = 0; j < DEBOUCNE_DELAY; j++)
            //{
            //m_aryFilters[i].m_LastPoints[j].x  = 0;
            // m_aryFilters[i].m_LastPoints[j].y  = 0;
            m_aryFilters[i].m_LastPoint.x = 0;
            m_aryFilters[i].m_LastPoint.y = 0;
            //}
            m_aryFilters[i].m_eFilterState   = E_FILTER_STATE_UP;
            //m_aryFilters[i].m_uFrameCount    = 0;

        };

    }
    //>

protected:
    enum E_FilterState
    {
        E_FILTER_STATE_UP   ,//光笔弹起状态
        E_FILTER_STATE_DN   ,//光笔按下状态
        //E_FILTER_STATE_DN_1 ,//光斑第一次按下状态
        //E_FILTER_STATE_DN_2 ,//光斑持续按下状态
    };

    static const int DEBOUCNE_DELAY = 2;//消抖延迟
    static const int MAGINIFY = 1024;

    //滤波状态
    struct TFilterState{
    //POINT         m_LastPoints[DEBOUCNE_DELAY] ;//笔迹上一个点的位置。
    POINT          m_LastPoint;//笔迹上一个点的位置。
    E_FilterState m_eFilterState ;//滤波状态机状态
    //UINT          m_uFrameCount  ;//帧计数
    };//

    TFilterState m_aryFilters[MAX_STROKE_NUM];//记录过滤器状态的数组

    int m_nD1;//逻辑距离，单位像素，其值与物理尺寸和屏幕分辨率相关，移动距离<D1, 则认为不动 
    int m_nD2;//逻辑距离，单位像素，其值与物理尺寸和屏幕分辨率相关，移动距离>D2, 则认为不需要作平滑操作

    //////////////////////////////////////////////////////////////////////////
    //<added by jiqw 201505191105
    //static const int KD1PhysicalDistance = 5; //D1对应的物理距离，其只和物理尺寸相关,单位为mm，即物理在任何投影区域，只要物理移动距离<m_nD1PhysicalDistance，则认为其未移动
#ifdef PICO_PROJECTOR

    static const int KD1PhysicalDistance = 2; //D1对应的物理距离，其只和物理尺寸相关,单位为mm，即物理在任何投影区域，只要物理移动距离<m_nD1PhysicalDistance，则认为其未移动

    //<<added by xuke 2016/09/024
    static const int KD2PhysicalDistance = 5; //D2对应的物理距离，其只和物理尺寸相关,单位为mm，即物理在任何投影区域，只要物理移动距离>m_nD2PhysicalDistance，则认为不需要作平滑操作
    //>>

#else
    static const int KD1PhysicalDistance = 2; //D1对应的物理距离，其只和物理尺寸相关,单位为mm，即物理在任何投影区域，只要物理移动距离<m_nD1PhysicalDistance，则认为其未移动
    static const int KD2PhysicalDistance = 14; //D2对应的物理距离，其只和物理尺寸相关,单位为mm，即物理在任何投影区域，只要物理移动距离>m_nD2PhysicalDistance，则认为不需要作平滑操作
#endif
    

    static const int KSTANDARDPDIALEN    = int(DEFAULT_SCREEN_DIAGONAL_LENGTH); //物理尺寸以80inches为标准
    static const int KSTANDARDLDIALEN    = 2032;                                //逻辑尺寸以1024*7658为标准

    //static const int K
    //>
    //////////////////////////////////////////////////////////////////////////

    //λ = K*|Δx| + b
    int m_Coef_K;//K系数, 扩大1024倍
    int m_Coef_B;//B系数, 扩大1024倍

};
