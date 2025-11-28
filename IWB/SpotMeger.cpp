#include "stdafx.h"
//#include "headers.h"
CSpotMerger::CSpotMerger()
{
#if 0
    int nCxScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
#else 
    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
#endif
    int nRange = nCxScreen > nCyScreen ? nCyScreen : nCxScreen;

    m_nMergeDistThreshold = nRange * 2 / 100;
}

CSpotMerger::~CSpotMerger()
{

}

////@功能:执行屏幕融合区光斑合并操作
////@参数:pLightSpots，输入/输出参数, 光斑数组
////      pLightSpotCount, 输入/输出参数，指向光斑个数的指针
//void CSpotMerger::DoMerge(TLightSpot* pLightSpots, int* pLightSpotCount)
//{
//    int nSpotCount = *pLightSpotCount;
//
//    if(nSpotCount < 2) return;
//
// 
//
//    int MERGE_THRESHOLD = m_nMergeDistThreshold * m_nMergeDistThreshold;//融合门限。
//
//    enum ESpotLocation
//    {   
//        E_NOT_IN_MERGE_AREA,
//        E_IN_RIGHT_MERGE_AREA,
//        E_IN_LEFT_MERGE_AREA
//    };
//
//    
//    //AtlTrace(_T("Before Merge %d"), nSpotCount);
//    BOOL bDoMerge = FALSE;
//
//    int i = 0;
//
//    while(i < nSpotCount)
//    {
//#ifdef _DEBUG
//        if(nSpotCount >=1)
//        {
//            //AtlTrace(_T("Before Merge nSpotCount = %d\n"), nSpotCount);
//            int nDebug = 0;
//        }
//#endif
//         TLightSpot& s1 = pLightSpots[i];
//
//         ESpotLocation eS1Location = E_NOT_IN_MERGE_AREA;//S1所在区域标志
//
//         //if(s1.ptPosInScreen.x < m_nSeperateX && (m_nSeperateX - s1.ptPosInScreen.x) < nHalfMergeAreaWidth)
//         if(m_nMergeAreaLeftBorder <= s1.ptPosInScreen.x  && s1.ptPosInScreen.x<= m_nSeperateX )
//         {
//            eS1Location = E_IN_LEFT_MERGE_AREA;
//         }
//         //else if(s1.ptPosInScreen.x > m_nSeperateX && (s1.ptPosInScreen.x - m_nSeperateX ) < nHalfMergeAreaWidth)
//         else if(m_nSeperateX < s1.ptPosInScreen.x && s1.ptPosInScreen.x <= m_nMergeAreaRightBorder)
//         {
//            eS1Location = E_IN_RIGHT_MERGE_AREA;
//         }
//
//
//
//        if( E_IN_LEFT_MERGE_AREA == eS1Location || E_IN_RIGHT_MERGE_AREA == eS1Location)
//        {
//            
//            for(int j = i+1; j < nSpotCount; j++)
//            {
//                
//                TLightSpot& s2 = pLightSpots[j];
//
//                ESpotLocation eS2Location = E_NOT_IN_MERGE_AREA;//S2所在区域标志
//
//
//                if(m_nMergeAreaLeftBorder <= s2.ptPosInScreen.x  && s2.ptPosInScreen.x < m_nSeperateX)
//                {
//                    eS2Location = E_IN_LEFT_MERGE_AREA;
//                }
//                else if( m_nSeperateX <= s2.ptPosInScreen.x  && s2.ptPosInScreen.x <= m_nMergeAreaRightBorder)
//                {
//                    eS2Location = E_IN_RIGHT_MERGE_AREA;
//                }
//
//
//                if(eS2Location != E_NOT_IN_MERGE_AREA) 
//                {
//                    int dx = s2.ptPosInScreen.x - s1.ptPosInScreen.x;
//                    int dy = s2.ptPosInScreen.y - s1.ptPosInScreen.y;
//
//                    int R2 = dx*dx + dy*dy;//两个光斑之间的距离
//
//                    if(R2 < MERGE_THRESHOLD)
//                    {
//                        //光斑距离够近, 需要被合并
//                        
//                        
//                        int total_mass = s1.mass + s2.mass;
//
//                        //按照光点的质量权重求取合并后的屏幕坐标
//                        s1.ptPosInScreen.x = (s1.mass * s1.ptPosInScreen.x)/total_mass + (s2.mass * s2.ptPosInScreen.x)/total_mass;
//                        s1.ptPosInScreen.y = (s1.mass * s1.ptPosInScreen.y)/total_mass + (s2.mass * s2.ptPosInScreen.y)/total_mass;
//
//                        s1.mass += s2.mass;
//
//
//                        ////取面积最大的光斑值, 
//                        ////注意:不能够简单地将光斑质量相加,否则光斑质量加倍，触发手势。
//                        //if(s1.mass < s2.mass)
//                        //{
//                        //    s1 = s2;
//                        //}
//                        s1.lStdSpotAreaInVideo += s2.lStdSpotAreaInVideo;
//
//                        //外接矩形面积相加，简化运算,
//                        //复杂的计算是:
//                        //bound.left   = min(s1.bound.left   , s2.bound.left  )
//                        //bound.right  = max(s1.bound.right  , s2.bound.right )
//                        //bound.top    = min(s1.bound.top    , s2.bound.top   )
//                        //bound.bottom = max(s1.bound.bottom , s2.bound.bottom)
//                        s1.lAreaInVideo += s2.lAreaInVideo;
//
//                        //后面的元素往前挪动一个位置
//                        for(int k =j+1; k < nSpotCount; k++)
//                        {
//                            pLightSpots[k-1] = pLightSpots[k];
//                        }
//                        //AtlTrace(_T("Merge a spot\n"));
//                        
//                        nSpotCount --;
//
//                        bDoMerge = TRUE;
//                    }//if
//
//                }//if
//
//            }//for
//        }//if
//        
//        i++;
//
//    }//while
//
//#ifdef _DEBUG
//    if(nSpotCount)
//    {
//        //AtlTrace(_T("After Merge nSpotCount = %d, bDoMerge=%d\n\n"), nSpotCount, bDoMerge);
//    }
//#endif
//
//    *pLightSpotCount = nSpotCount;
//
//}


//@功能:执行屏幕融合区光斑合并操作
//@参数:pLightSpots，输入/输出参数, 光斑数组
//      pLightSpotCount, 输入/输出参数，指向光斑个数的指针
void CSpotMerger::DoMerge(TLightSpot* pLightSpots, int* pLightSpotCount)
{
    int nSpotCount = *pLightSpotCount;

    if (nSpotCount < 2) return;

    //融合的距离门限的平方
    int MERGE_THRESHOLD = m_nMergeDistThreshold * m_nMergeDistThreshold;


    //融合发生标志
    BOOL bMergeHappened = FALSE;


    int nSpotIndex = 0;

    while (nSpotIndex < nSpotCount)
    {
        TLightSpot&  spot = pLightSpots[nSpotIndex];

        if (spot.aux.uMergeAreaIndex != UINT(-1))
        {
            for (int j = nSpotIndex + 1; j < nSpotCount; j++)
            {
                TLightSpot&  cmp_spot = pLightSpots[j];

                if (cmp_spot.aux.uMergeAreaIndex == spot.aux.uMergeAreaIndex)
                {//当前处理的光斑和与之比较的光斑处于同一融合区
                 //通过判断是否小于融合的距离门限, 来决定是否融合

                    int dx = cmp_spot.ptPosInScreen.x - spot.ptPosInScreen.x;
                    int dy = cmp_spot.ptPosInScreen.y - spot.ptPosInScreen.y;

                    int R2 = dx*dx + dy*dy;//两个光斑之间的距离

                    if (R2 < MERGE_THRESHOLD)
                    {
                        //光斑距离够近, 需要被合并
                        int total_mass = spot.mass + cmp_spot.mass;

                        //按照光点的质量权重求取合并后的屏幕坐标
                        spot.ptPosInScreen.x = (spot.mass * spot.ptPosInScreen.x) / total_mass + (cmp_spot.mass * cmp_spot.ptPosInScreen.x) / total_mass;
                        spot.ptPosInScreen.y = (spot.mass * spot.ptPosInScreen.y) / total_mass + (cmp_spot.mass * cmp_spot.ptPosInScreen.y) / total_mass;

                        spot.mass += cmp_spot.mass;


                        spot.lStdSpotAreaInVideo += cmp_spot.lStdSpotAreaInVideo;

                        spot.lAreaInVideo += cmp_spot.lAreaInVideo;

                        //后面的元素往前挪动一个位置
                        for (int k = j + 1; k < nSpotCount; k++)
                        {
                            pLightSpots[k - 1] = pLightSpots[k];
                        }//for(k)

                        nSpotCount--;
                        bMergeHappened = TRUE;


                        AtlTrace(
                            _T("spot(%d,%d) of camera_%d is merged\n"),
                            cmp_spot.ptPosInScreen.x, 
                            cmp_spot.ptPosInScreen.y,
                            cmp_spot.dwCameraId);
                    }//if(R2 < MERGE_THRESHOLD)

                }//if(cmp_spot.aux.mergeAreaIndex == spot.aux.mergeAreaIndex)

            }//for(j)

        }

        nSpotIndex++;
    }//while

#ifdef _DEBUG
    if (nSpotCount)
    {
        AtlTrace(_T("After Merge nSpotCount = %d, bDoMerge=%d\n\n"), nSpotCount, bMergeHappened);
    }
#endif

    *pLightSpotCount = nSpotCount;
}





//@功能:屏幕分辨率变化事件响应函数
//@参数:nScreenWidth, 新的屏幕宽度
//      nScreenHeight,新的屏幕高度
void CSpotMerger::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{
    this->m_nSeperateX =nScreenWidth   >> 1;//除以2
    int nMergeAreaWidth = nScreenWidth >> 6;//除以64
    this->m_nMergeAreaLeftBorder  = m_nSeperateX - nMergeAreaWidth/2;
    this->m_nMergeAreaRightBorder = m_nSeperateX + nMergeAreaWidth/2;

    int nRange = nScreenWidth > nScreenHeight ? nScreenHeight : nScreenWidth;
    //m_nMergeDistThreshold = nRange * 4/ 100;
    //m_nMergeDistThreshold = this->m_nMergeAreaRightBorder - this->m_nMergeAreaLeftBorder;
    m_nMergeDistThreshold = nMergeAreaWidth;
}


//@功能:设置融合区域
//@参数:mergeAreas, 融合区数组
//      nCount, 融合区数目
void CSpotMerger::SetMergeAreas(const RECT  mergeAreas[], int nAreaCount)
{
    m_vecMergeAreas.resize(nAreaCount);

    for (size_t i = 0; i < m_vecMergeAreas.size(); i++)
    {
        m_vecMergeAreas[i] = mergeAreas[i];
    }

}


//@功能:返回指定的融合区
const RECT* CSpotMerger::GetMergeArea(UINT nIndex) const
{
    if (nIndex >= m_vecMergeAreas.size()) return NULL;

    return &m_vecMergeAreas[nIndex];
}