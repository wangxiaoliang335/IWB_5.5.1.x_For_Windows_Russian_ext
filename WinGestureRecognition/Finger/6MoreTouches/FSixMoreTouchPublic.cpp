#include "StdAfx.h"
//#include "FSixMoreTouchPublic.h"
//#include "..\..\..\inc\wgr\IWGRUtilContext.h"
//#include "..\..\GlobalLog.h"
//
////////////////////////////////////////////////////////////////////////////
////静态成员初始化
//int CFSixMoreTouchPublic::m_curMissShapeTolerance = 0;
//int CFSixMoreTouchPublic::m_curMissTouchCountTolerance = 0;
//int CFSixMoreTouchPublic::m_curMissDisLimitTolerance = 0;
//int CFSixMoreTouchPublic::m_curMissMDirTolerance = 0;
//int CFSixMoreTouchPublic::m_curNonMoveTolerance = 0;
//
//int CFSixMoreTouchPublic::m_nXDisplacement = 0;
//int CFSixMoreTouchPublic::m_nYDisplacement = 0;
//
//bool CFSixMoreTouchPublic::m_bEnterLessThanOne = true;
//
//POINT CFSixMoreTouchPublic::m_ptLastMid;
//POINT CFSixMoreTouchPublic::m_ptSartMid;
//
////////////////////////////////////////////////////////////////////////////
//
//CFSixMoreTouchPublic::CFSixMoreTouchPublic(void)
//{
//}
//
//CFSixMoreTouchPublic::~CFSixMoreTouchPublic(void)
//{
//}
//
//E_WGR_MOVEDIR CFSixMoreTouchPublic::CalcMoveDirection(const TLightSpot * aryLightSpots,  int nCount, const int nXMoveStandard, const int nYMoveStandard)
//{
//    E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;
//    
//    if(!MatchTouches(aryLightSpots, nCount, 100, 100))
//    {
//        return dir;
//    }
//
//    POINT ptCurMid;
//    ptCurMid.x = 0;
//    ptCurMid.y = 0;
//
//    g_oWGRTraceLog.AddLastMiddlePoint(m_ptLastMid);
//    g_oWGRTraceLog.AddStartMiddlePoint(m_ptSartMid);
//
//    if(!CalcMiddlePoint(aryLightSpots,nCount, ptCurMid))
//    {
//        /*if (!CalcCentriMoments(aryLightSpots, nCount, ptCurMid, nCurCentriMoments))
//        {
//        return dir;
//        }*/
//
//         return dir;
//    }    
//
//    g_oWGRTraceLog.AddCurrentMiddlePoint(ptCurMid);    
//
//    //更新m_ptLastMid
//    m_ptLastMid = ptCurMid;
//    
//    int xShift = ptCurMid.x - m_ptSartMid.x;
//    int yShift = ptCurMid.y - m_ptSartMid.y;
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
//            if (WGR_INT32_ABS(xShift - m_nXDisplacement) > 9)
//            {//突然大位移，一般认为是有干扰光点变多造成的
//                m_ptSartMid.x = ptCurMid.x;
//                xShift = m_nXDisplacement;
//            }
//
//            //if (WGR_INT32_ABS(yShift - m_nYDisplacement) > 9)
//            //{//突然大位移，一般认为是有干扰光点变多造成的
//            //    m_ptSartMid.y = ptCurMid.y;
//            //    break;
//            //}
//
//            ////水平运动的标准是|dx| > 3 * |dy|
//            //if (WGR_INT32_ABS(xShift) > 3 * WGR_INT32_ABS(yShift))
//            //{                
//
//            //    //接下来判断是否是同向运动
//            //    if (xShift * m_nXDisplacement > 0)
//            //    {               
//            //        if (xShift < 0)
//            //        {//水平向左
//            //            dir = E_WGR_MDIR_LHoriz;                      
//            //        }
//            //        else
//            //        {
//            //            dir = E_WGR_MDIR_RHoriz;                       
//            //        }
//            //    }            
//            //}
//
//            //垂直运动的标准是|dy| > 3 * |dx|
//            if (WGR_INT32_ABS(yShift) > 3 * WGR_INT32_ABS(xShift))
//            {
//
//
//                //接下来判断是否是同向运动
//                if (yShift * m_nYDisplacement > 0)
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
//bool CFSixMoreTouchPublic::MatchTouches(const TLightSpot * aryLightSpots,  int nCount, int xMaxDisLimit, int yMaxDisLimit)
//{
//    if ( nCount < 6)
//    {//光点数不满足
//        return false;
//    }    
//
//    return true;
//}
//
//bool CFSixMoreTouchPublic::CalcMiddlePoint(const TLightSpot * aryLightSpots,  int nCount, POINT &ptMid)
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
