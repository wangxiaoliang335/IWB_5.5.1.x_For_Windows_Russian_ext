#include "StdAfx.h"
//#include "FThree2FiveTouchPublic.h"
//#include "..\..\..\inc\wgr\IWGRUtilContext.h"
//#include "..\..\GlobalLog.h"

////////////////////////////////////////////////////////////////////////////
////静态成员初始化
//int CFThree2FiveTouchPublic::m_curMissShapeTolerance = 0;
//int CFThree2FiveTouchPublic::m_curMissTouchCountTolerance = 0;
//int CFThree2FiveTouchPublic::m_curMissDisLimitTolerance = 0;
//int CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
//int CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;
//
//int CFThree2FiveTouchPublic::m_nXDisplacement = 0;
//int CFThree2FiveTouchPublic::m_nYDisplacement = 0;
//
//int CFThree2FiveTouchPublic::m_nLastCentriMoments = 0;
//int CFThree2FiveTouchPublic::m_curTimesCMIncreaced = 0;
//int CFThree2FiveTouchPublic::m_curTimesCMDecreaced = 0;
//int CFThree2FiveTouchPublic::m_nTotalCollects = 0;
//
//bool CFThree2FiveTouchPublic::m_bEnterLessThanOne = true;
//
//POINT CFThree2FiveTouchPublic::m_ptLastMid;
//POINT CFThree2FiveTouchPublic::m_ptSartMid;
//POINT CFThree2FiveTouchPublic::m_ptCur[5];
//POINT CFThree2FiveTouchPublic::m_ptLast[5];
//int CFThree2FiveTouchPublic::m_nLastPts = 0;
//
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
////宏定义
//#define MINGRABCMLIMIT          150           //抓取动作中心距减小量的阈值，即减少的数量大于该值，才被认为是真正的减少了
//#define MINOPENCMLIMIT          150           //张开动作中心距增大量的阈值，即增大的数量大于该值，才被认为是真正的增大了
//
//#define MAXCOLLETS_BEFORE_CMOTIONANALSIS     20  //复杂运动分析前最大采样次数（由于抓取和张开动作移动距离小，所以需要设定一定的采集次数间隔来分析）
////////////////////////////////////////////////////////////////////////////
//
//CFThree2FiveTouchPublic::CFThree2FiveTouchPublic(void)
//{
//}
//
//CFThree2FiveTouchPublic::~CFThree2FiveTouchPublic(void)
//{
//}
//
//E_WGR_MOVEDIR CFThree2FiveTouchPublic::CalcMoveDirection(const TLightSpot * aryLightSpots,  int nCount, const int nXMoveStandard, const int nYMoveStandard)
//{
//    E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;
//
//    m_nTotalCollects++;
//
//    g_oWGRTraceLog.AddTotCollects(m_nTotalCollects);
//
//    if(!MatchTouches(aryLightSpots, nCount, 100, 100))
//    {
//        return dir;
//    }
//
//    POINT ptCurMid;
//    ptCurMid.x = 0;
//    ptCurMid.y = 0;
//    int nCurCentriMoments = 0;
//
//    g_oWGRTraceLog.AddLastMiddlePoint(m_ptLastMid);
//    g_oWGRTraceLog.AddStartMiddlePoint(m_ptSartMid);
//
//    if(CalcMiddlePoint(aryLightSpots,nCount, ptCurMid))
//    {
//        /*if (!CalcCentriMoments(aryLightSpots, nCount, ptCurMid, nCurCentriMoments))
//        {
//            return dir;
//        }*/
//    }
//    else
//    {
//        return dir;
//    }   
//
//    g_oWGRTraceLog.AddCurrentMiddlePoint(ptCurMid);
//
//    /**/
//
//    int xShift = ptCurMid.x - m_ptLastMid.x;
//    int yShift = ptCurMid.y - m_ptLastMid.y;
//
//    //if (nCurCentriMoments + MINGRABCMLIMIT < m_nLastCentriMoments)
//    //{//为去除抖动干扰，当前的CentriMoments只有小于上一次CentriMoments到一定程度，才被认为是变小趋势
//    //    m_curTimesCMDecreaced++;
//    //}
//
//    ////if (nCurCentriMoments - MINOPENCMLIMIT > m_nLastCentriMoments)
//    ////{//为去除抖动干扰，当前的CentriMoments只有大于上一次CentriMoments到一定程度，才被认为是变大趋势
//    ////    m_curTimesCMIncreaced++;
//    ////}
//
//    g_oWGRTraceLog.AddTimesCMDecreased(m_curTimesCMDecreaced);
//    g_oWGRTraceLog.AddTimesCMIncreased(m_curTimesCMIncreaced);
//
//    //更新m_ptLastMid
//    m_ptLastMid = ptCurMid;
//
//    //m_nLastCentriMoments = nCurCentriMoments;
//
//    //
//    /*if(m_nTotalCollects >= MAXCOLLETS_BEFORE_CMOTIONANALSIS)
//    {
//        m_nTotalCollects = 0;
//        m_curTimesCMIncreaced = 0;
//        m_curTimesCMDecreaced = 0;
//
//        if ((m_curTimesCMIncreaced + 5) > m_nTotalCollects + 1)
//        {
//            return E_WGR_MDIR_Open;
//        }
//
//        if ((m_curTimesCMDecreaced + 5) > m_nTotalCollects + 1)
//        {
//            return E_WGR_MDIR_Grab;
//        }       
//    }*/
//
//    /*
//     这里说明一下m_ptLastMid和m_ptSartMid的区别：前者每一次采集后都变化，后者只有判断有移动后，才发生变化
//    */
//    xShift = ptCurMid.x - m_ptSartMid.x;
//    yShift = ptCurMid.y - m_ptSartMid.y;
//
//    g_oWGRTraceLog.AddLastDisplacement(m_nXDisplacement, m_nYDisplacement);
//    g_oWGRTraceLog.AddXShift(xShift);
//    g_oWGRTraceLog.AddYShift(yShift);
//
//    if (WGR_INT32_ABS(xShift) < nXMoveStandard && WGR_INT32_ABS(yShift) < nYMoveStandard)
//    {
//        m_nXDisplacement = xShift;
//        m_nYDisplacement = yShift;
//
//        dir = E_WGR_MDIR_None;
//    }
//    else
//    {
//        do 
//        {
//            ////if (m_nTotalCollects >= MAXCOLLETS_BEFORE_CMOTIONANALSIS)
//            //{
//            //    /*if (((m_curTimesCMIncreaced + 5) > m_nTotalCollects + 1))
//            //    {
//            //        dir = E_WGR_MDIR_Open;
//
//            //        break;
//            //    }*/
//
//            //    if ((m_curTimesCMDecreaced << 1) > m_nTotalCollects + 1)
//            //    {
//            //        dir =  E_WGR_MDIR_Grab;
//
//            //        break;
//            //    }
//            //}
//            if (WGR_INT32_ABS(xShift - m_nXDisplacement) > 100)
//            {//突然大位移，一般认为是有干扰光点变多造成的
//                m_ptSartMid.x = ptCurMid.x;
//                m_ptSartMid.y = ptCurMid.y;
//                m_nXDisplacement = xShift;
//                m_nYDisplacement = yShift;   
//                break;
//            }
//
//            if (WGR_INT32_ABS(yShift - m_nYDisplacement) > 100)
//            {//突然大位移，一般认为是有干扰光点变多造成的
//                m_ptSartMid.y = ptCurMid.y;
//                m_ptSartMid.x = ptCurMid.x;
//                m_nXDisplacement = xShift;
//                m_nYDisplacement = yShift;   
//                break;
//            }
//
//            //水平运动的标准是|dx| > 3 * |dy|
//            if (WGR_INT32_ABS(xShift) > 2 * WGR_INT32_ABS(yShift))
//            {                
//
//                //接下来判断是否是同向运动
//                if (xShift * m_nXDisplacement > 0 || m_nXDisplacement == 0)
//                {               
//                    if (xShift < 0)
//                    {//水平向左
//                        dir = E_WGR_MDIR_LHoriz;                      
//                    }
//                    else
//                    {
//                        dir = E_WGR_MDIR_RHoriz;                       
//                    }
//                }            
//            }
//
//            //垂直运动的标准是|dy| > 3 * |dx|
//            if (WGR_INT32_ABS(yShift) > 2 * WGR_INT32_ABS(xShift))
//            {
//                
//
//                //接下来判断是否是同向运动
//                if (yShift * m_nYDisplacement > 0 || m_nYDisplacement == 0)
//                {                
//                    if (yShift < 0)
//                    {//竖直向上
//                        dir = E_WGR_MDIR_UVertic;
//                    }
//                    else
//                    {//竖直想下
//                        dir = E_WGR_MDIR_DVertic;
//                    }
//                }                            
//            }   
//        } while (false);
//
//        m_nXDisplacement = xShift;
//        m_nYDisplacement = yShift;             
//
//    }
//
//    return dir;
//}
//
//bool CFThree2FiveTouchPublic::MatchTouches(const TLightSpot * aryLightSpots,  int nCount, int xMaxDisLimit, int yMaxDisLimit)
//{
//    if (nCount > 5 || nCount < 3)
//    {//光点数不满足
//        return false;
//    }
//
//    //POINT ptTmp[5];
//    //for(int i = 0; i < nCount; i++)
//    //{
//    //    ptTmp[i] = aryLightSpots[i].ptPosInScreen;
//    //}
//
//    //if (nCount == m_nLastPts)
//    //{//光点数一样，不再进行匹配，直接进行赋值
//    //    memcpy_s(m_ptCur, sizeof(m_ptCur), ptTmp, nCount * sizeof(POINT));
//    //}
//    //else
//    //{
//    //    //当前光点数，与先前光点数不同，需要进行匹配，否则
//    //    //如果根据比较curMid和lastMid，会出问题，比如当前光点多了一个，可能造成水平移动的错误判断
//
//    //    /*
//    //    此处匹配采用取巧方式：找出元素比较少的集合中的最小的x坐标和最大的x坐标，在元素比较多的集合中，
//    //                        剔除离这个区间最远的|m_nLastPts - nCount|个元素
//    //    */     
//    //    if (nCount > m_nLastPts)
//    //    {//此次采集多了一个光点，需要匹配出m_nLasPts个点，其他点不参与计算curMid
//    //        int nMinX = m_ptLast[0].x;
//    //        int nMaxX = m_ptLast[0].x;
//    //        for (int i = 0; i < m_nLastPts; i++)
//    //        {
//
//    //        }
//    //    }
//    //}
//
//    return true;
//}
//
//bool CFThree2FiveTouchPublic::CalcMiddlePoint(const TLightSpot * aryLightSpots,  int nCount, POINT &ptMid)
//{
//    if (aryLightSpots == NULL) return false;
//
//    ptMid.x = 0;
//    ptMid.y = 0;
//
//    for (int i = 0; i < nCount; i++)
//    {
//        ptMid.x += aryLightSpots[i].ptPosInScreen.x;
//        ptMid.y += aryLightSpots[i].ptPosInScreen.y;
//    }
//
//    ptMid.x = (ptMid.x + nCount - 1) / nCount;
//    ptMid.y = (ptMid.y + nCount - 1) / nCount;
//
//    return true;
//}
//
//bool CFThree2FiveTouchPublic::CalcCentriMoments(const TLightSpot * aryLightSpots,  int nCount, const POINT &ptMid, int &nCentriMoments)
//{
//    if (aryLightSpots == NULL) return false;
//
//    nCentriMoments = 0;
//
//    for(int i = 0; i < nCount; i++)
//    {
//        nCentriMoments += (aryLightSpots[i].ptPosInScreen.x - ptMid.x) * (aryLightSpots[i].ptPosInScreen.x - ptMid.x)
//                        + (aryLightSpots[i].ptPosInScreen.y - ptMid.y) * (aryLightSpots[i].ptPosInScreen.y - ptMid.y);
//    }
//
//    //计算平均的中心距
//    nCentriMoments = (nCentriMoments + nCount - 1) / nCount;
//
//    return true;
//}