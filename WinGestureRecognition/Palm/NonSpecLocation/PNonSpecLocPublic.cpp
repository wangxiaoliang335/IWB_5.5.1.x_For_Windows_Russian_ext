#include "StdAfx.h"
//#include "PNonSpecLocPublic.h"
//#include "..\..\..\inc\wgr\IWGRUtilContext.h"
//#include "..\..\GlobalLog.h"
//
////////////////////////////////////////////////////////////////////////////
////静态变量初始化
//int CPNonSpecLocPublic::m_curMissShapeTolerance = 0; 
//int CPNonSpecLocPublic::m_curMissMDirTolerance = 0; 
//int CPNonSpecLocPublic::m_curNonMoveTolerance = 0; 
//
//int CPNonSpecLocPublic::m_nXDisplacement = 0;
//int CPNonSpecLocPublic::m_nYDisplacement = 0;
//
//POINT CPNonSpecLocPublic::m_ptStart; 
//POINT CPNonSpecLocPublic::m_ptCur;
//POINT CPNonSpecLocPublic::m_ptLast;  
//int   CPNonSpecLocPublic::m_nLastMass = 0;
//int   CPNonSpecLocPublic::m_nLastStdMass = 0;
//
//DWORD CPNonSpecLocPublic::m_dwNonMoveStartTime = 0;
//
//bool  CPNonSpecLocPublic:: m_bEnterLessThanOne = true;
////
//
//CPNonSpecLocPublic::CPNonSpecLocPublic(void)
//{
//}
//
//CPNonSpecLocPublic::~CPNonSpecLocPublic(void)
//{
//}
//
//E_WGR_MOVEDIR CPNonSpecLocPublic::CalcMoveDirection(const TLightSpot * aryLightSpots,  int nCount,  const int nXMoveStandard, const int nYMoveStandard)
//{
//    E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;    
//
//    if (!MatchPalm(aryLightSpots, nCount, m_ptCur))
//    {//匹配失败
//        return dir;
//    }
//
//    g_oWGRTraceLog.AddStartPoint(&m_ptStart, 1);
//    
//    g_oWGRTraceLog.AddLastPoint(&m_ptCur, 1);
//
//    int xMove = m_ptCur.x - m_ptStart.x;
//    int yMove = m_ptCur.y - m_ptStart.y;
//
//    g_oWGRTraceLog.AddLastDisplacement(m_nXDisplacement, m_nYDisplacement);
//    g_oWGRTraceLog.AddCurrentDisplacement(xMove, yMove);    
//
//    m_ptLast = m_ptCur;
//
//    if (WGR_INT32_ABS(xMove) < nXMoveStandard && WGR_INT32_ABS(yMove) < nYMoveStandard)
//    {//认为没有移动
//        dir = E_WGR_MDIR_None;        
//    }
//    else
//    {
//        dir = E_WGR_MDIR_Move;         
//    } 
//
//    return dir;
//}
//
///*
//匹配准则：令massTi为araryLightSpots中元素i的质量，stdTi为aryLightSpots中元素i的标准质量，
//moves_i = 当前aryLightSpots元素i的位置与上次手掌位置的x方向与y方向距离之和
//则匹配需要满足如下条件
//a. massTi * m_lastStd >= LastMass * stdTi
//b.(massTi * m_lastStd - LastMass * stdTi) + moves_i最小
//c.|xMove| < c 且 |xMove| < c
//
//附加说明：
//a.主要判断光斑i是否符合手掌条件
//b.主要采用形状和距离最近准则进行匹配
//c.主要检测在a和b都满足情况下，光斑是否为干扰光斑
//*/
//bool CPNonSpecLocPublic::MatchPalm(const TLightSpot *aryLightSpots, int nCount, POINT &ptCurPalm)
//{
//    if (!aryLightSpots || nCount <= 0)
//    {
//        return false;
//    }
//
//    //int nPosMatch = 0;
//    //int xMove = WGR_INT32_ABS(m_ptLast.x - aryLightSpots[0].ptPosInScreen.x);
//    //int yMove = WGR_INT32_ABS(m_ptLast.y - aryLightSpots[0].ptPosInScreen.y);
//    //int minCondition_b = (xMove + yMove) << 1 + WGR_INT32_ABS(m_nLastMass - aryLightSpots[0].mass);
//
//    //for(int i = 1; i < nCount; i++)
//    //{
//    //    if (aryLightSpots[i].mass * m_nLastStdMass >= m_nLastMass * aryLightSpots[i].lStdSpotAreaInVideo)
//    //    {//光斑i符合手掌条件
//    //        int nXTmpMove = WGR_INT32_ABS(m_ptLast.x - aryLightSpots[i].ptPosInScreen.x);
//    //        int nYTmpMove = WGR_INT32_ABS(m_ptLast.y - aryLightSpots[0].ptPosInScreen.y);
//    //        int nTmpCondition_b = (nXTmpMove + nYTmpMove) << 1 + WGR_INT32_ABS(m_nLastMass - aryLightSpots[i].mass);
//
//    //        if (nTmpCondition_b < minCondition_b)
//    //        {
//    //            minCondition_b = nTmpCondition_b;
//    //            nPosMatch = i;
//    //            xMove = nXTmpMove;
//    //            yMove = nYTmpMove;
//    //        }
//    //    }
//    //    else
//    //    {//由于光斑数组是按照光斑大小排序的，如果光斑i不符合手掌条件，就认为光斑i+1及其后面的光斑都不符合手掌条件
//    //        break;
//    //    }
//    //}
//
//    //if (xMove > 12 || yMove > 12)
//    //{
//    //    //认为匹配到的手掌也是干扰手掌
//    //    return false;
//    //}
//
//    /*m_nLastMass = aryLightSpots[nPosMatch].mass;
//    m_nLastStdMass = aryLightSpots[nPosMatch].lStdSpotAreaInVideo;*/
//
//    ptCurPalm = aryLightSpots[0].ptPosInScreen;
//
//    return true;
//}
