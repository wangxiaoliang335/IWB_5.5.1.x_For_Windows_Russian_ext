#include "StdAfx.h"
//#include "FTwoTouchPublic.h"
//#include "..\..\..\inc\wgr\IWGRUtilContext.h"
//#include "..\..\GlobalLog.h"
////////////////////////////////////////////////////////////////////////////
////
//
//CFTwoTouchPublic::CFTwoTouchPublic(void)
//{
//}
//
//CFTwoTouchPublic::~CFTwoTouchPublic(void)
//{
//}
//
////静态成员初始化
//int CFTwoTouchPublic::m_curMiss2TouchCountTolerance = 0;
//int CFTwoTouchPublic::m_curMiss2TouchDisLimitTolerance = 0;
//int CFTwoTouchPublic::m_curMissMDirTolerance = 0;
//int CFTwoTouchPublic::m_curMissShapeTolerance = 0;
//int CFTwoTouchPublic::m_curNonMoveTolerance = 0;
//int CFTwoTouchPublic::m_nDisplacement = 0;
//
//bool CFTwoTouchPublic::m_bEnterLessThanOne = true;
//
//POINT CFTwoTouchPublic::m_ptCur[2];
//POINT CFTwoTouchPublic::m_ptLast[2];
//
//E_WGR_MOVEDIR CFTwoTouchPublic::CalcMoveDirection(const TLightSpot * aryLightSpots,  int nCount, const int nXMoveStandard, const int nYMoveStandard)
//{
//    E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;
//
//    if (!MatchTouches(aryLightSpots, nCount, 100, 100))
//    {//匹配失败
//        return dir;
//    }
//
//    g_oWGRTraceLog.AddStartPoint(m_ptLast, 2);
//    g_oWGRTraceLog.AddLastPoint(m_ptCur, 2);
//
//    POINT ptLastMid, ptCurMid;
//    ptLastMid.x = (m_ptLast[0].x + m_ptLast[1].x + 1) >> 1;
//    ptLastMid.y = (m_ptLast[0].y + m_ptLast[1].y + 1) >> 1;
//
//    ptCurMid.x = (m_ptCur[0].x + m_ptCur[1].x + 1) >> 1;
//    ptCurMid.y = (m_ptCur[0].y + m_ptCur[1].y + 1) >> 1;
//
//    int xShift = ptCurMid.x - ptLastMid.x;
//    int yShift = ptCurMid.y - ptLastMid.y;
//
//    g_oWGRTraceLog.AddXShift(xShift);
//    g_oWGRTraceLog.AddYShift(yShift);
//    
//    if (WGR_INT32_ABS(xShift) < nXMoveStandard &&  WGR_INT32_ABS(yShift) < nYMoveStandard)
//    {
//        m_nDisplacement = 0;
//        dir = E_WGR_MDIR_None;
//    }
//    else/* if (WGR_INT32_ABS(xShift) >= nXMoveStandard)*/
//    {
//        //水平运动的标准是|dx| > 3 * |dy|
//        if (WGR_INT32_ABS(xShift) > 3 * WGR_INT32_ABS(yShift))
//        {
//            //接下来判断是否是同向运动
//            if ((m_ptCur[0].x - m_ptLast[0].x) * (m_ptCur[1].x - m_ptLast[1].x) > 0)
//            {
//                m_nDisplacement = xShift;
//
//                dir = E_WGR_MDIR_Horiz;
//            }            
//        }
//
//        //垂直运动的标准是|dy| > 3 * |dx|
//        if (WGR_INT32_ABS(yShift) > 3 * WGR_INT32_ABS(xShift))
//        {
//            //接下来判断是否是同向运动
//            if ((m_ptCur[0].y - m_ptLast[0].y) * (m_ptCur[1].y - m_ptLast[1].y) > 0)
//            {
//                m_nDisplacement = yShift;
//
//                dir = E_WGR_MDIR_Vertic;
//            }                        
//        }
//    }
//
//    g_oWGRTraceLog.AddCurrentDisplacement(xShift, yShift);
//
//    return dir;
//}
//
//bool  CFTwoTouchPublic::MatchTouches(const TLightSpot * aryLightSpots,  int nCount, int xMaxDisLimit, int yMaxDisLimit)
//{
//    if (nCount != 2 || !aryLightSpots)
//    {//光点数不等于2或者光点数组为空
//        return false;
//    }
//
//    int nXDis = WGR_INT32_ABS(aryLightSpots[0].ptPosInScreen.x - aryLightSpots[1].ptPosInScreen.x);
//    int nYDis = WGR_INT32_ABS(aryLightSpots[0].ptPosInScreen.y - aryLightSpots[1].ptPosInScreen.y);
//
//    if (nXDis > xMaxDisLimit || nYDis > yMaxDisLimit)
//    {//当前两个光点间距离超出限制
//        return false;
//    }
//
//    //开始匹配
//    //匹配准则是：移动距离的平方和最小
//
//    //假设0，光点数组中的0点与上次采样光斑数组的0点相对应，光点数组中的1点与上次采样光斑数组的1点相对应，
//    LONG moveSquare0 = (aryLightSpots[0].ptPosInScreen.x - m_ptLast[0].x) * (aryLightSpots[0].ptPosInScreen.x - m_ptLast[0].x)
//        + (aryLightSpots[0].ptPosInScreen.y - m_ptLast[0].y) * (aryLightSpots[0].ptPosInScreen.y - m_ptLast[0].y)
//        + (aryLightSpots[1].ptPosInScreen.x - m_ptLast[1].x) * (aryLightSpots[1].ptPosInScreen.x - m_ptLast[1].x)
//        + (aryLightSpots[1].ptPosInScreen.y - m_ptLast[1].y) * (aryLightSpots[1].ptPosInScreen.y - m_ptLast[1].y);
//
//    //假设1，光点数组中的0点与上次采样光斑数组的1点相对应，光点数组中的1点与上次采样光斑数组的0点相对应，
//    LONG moveSquare1 = (aryLightSpots[0].ptPosInScreen.x - m_ptLast[1].x) * (aryLightSpots[0].ptPosInScreen.x - m_ptLast[1].x)
//        + (aryLightSpots[0].ptPosInScreen.y - m_ptLast[1].y) * (aryLightSpots[0].ptPosInScreen.y - m_ptLast[1].y)
//        + (aryLightSpots[1].ptPosInScreen.x - m_ptLast[0].x) * (aryLightSpots[1].ptPosInScreen.x - m_ptLast[0].x)
//        + (aryLightSpots[1].ptPosInScreen.y - m_ptLast[0].y) * (aryLightSpots[1].ptPosInScreen.y - m_ptLast[0].y);
//
//    if(moveSquare0 < moveSquare1)
//    {//0假设成立
//        m_ptCur[0] = aryLightSpots[0].ptPosInScreen;
//        m_ptCur[1] = aryLightSpots[1].ptPosInScreen;
//    }
//    else
//    {//1假设成立
//        m_ptCur[0] = aryLightSpots[1].ptPosInScreen;
//        m_ptCur[1] = aryLightSpots[0].ptPosInScreen;
//    }
//
//    return true;
//}