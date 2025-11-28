#include "StdAfx.h"
//#include "FDspDesktopStateMachine.h"
//#include "FSixMoreTouchPublic.h"
//#include "..\..\GlobalLog.h"
//
////
//#define FDSPDESKTOP_MAX_MISSSHAPETOLLERANCE                   1
//#define FDSPDESKTOP_MAX_MISSTOUCHCOUNTTOLERANCE               3
//#define FDSPDESKTOP_MAX_NONMOVETOLERANCE                      30
//#define FDSPDESKTOP_MAX_MISSDIR                               2
////******
//
//CFDspDesktopStateMachine::CFDspDesktopStateMachine(void)
//{
//    m_curState = WGR_IDLE_STATE;
//}
//
//CFDspDesktopStateMachine::~CFDspDesktopStateMachine(void)
//{
//}
//
//bool CFDspDesktopStateMachine::StartRun()
//{
//    m_curState = WGR_FDSPDESKTOP_LISTEN_STATE;
//    CFSixMoreTouchPublic::ResetSomeMembers();
//    return true;
//}
//
//bool CFDspDesktopStateMachine::StopRun()
//{
//    m_curState = WGR_IDLE_STATE;
//    return true;
//}
//
//IWGRStateMachine *CFDspDesktopStateMachine::GetSubMachineCurStateContained()
//{
//    return (IWGRStateMachine *)this;
//}
//
//WGR_STATE_TYPE CFDspDesktopStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
//{
//    do 
//    {
//        g_oWGRTraceLog.AddSpotCount(nCount);
//
//        if (g_oWGRTraceLog.IsLogHasStarted())
//        {
//            //
//            POINT pt[16];
//            for (int i = 0; i < nCount; i++)
//            {
//                pt[i] = aryLightSpots[i].ptPosInScreen;
//            }
//
//            g_oWGRTraceLog.AddSpotPos(pt, nCount);
//        }
//
//
//        if (gestureShape != E_WGR_GShape_Finger)
//        {//非手指形状
//            CFSixMoreTouchPublic::m_curMissShapeTolerance++;
//            g_oWGRTraceLog.AddCurMissShapeTolerance(CFSixMoreTouchPublic::m_curMissShapeTolerance);
//
//            if (CFSixMoreTouchPublic::m_curMissShapeTolerance > FDSPDESKTOP_MAX_MISSSHAPETOLLERANCE)
//            {
//                m_curState = WGR_FDSPDESKTOP_CLOSE_STATE;
//            }
//
//            break;
//        }
//        else
//        {
//            CFSixMoreTouchPublic::m_curMissShapeTolerance = 0;
//            g_oWGRTraceLog.AddCurMissShapeTolerance(CFSixMoreTouchPublic::m_curMissShapeTolerance);
//        }
//
//        if(nCount < 6)
//        {//光点数不满足条件
//            CFSixMoreTouchPublic::m_curMissTouchCountTolerance++;
//            g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);
//
//            if (CFSixMoreTouchPublic::m_curMissTouchCountTolerance > FDSPDESKTOP_MAX_MISSTOUCHCOUNTTOLERANCE)
//            {
//                m_curState = WGR_FDSPDESKTOP_CLOSE_STATE;                
//            }
//
//            break;
//        }
//        else
//        {
//            CFSixMoreTouchPublic::m_curMissTouchCountTolerance = 0;
//            g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);
//        }
//
//        E_WGR_MOVEDIR dir = CFSixMoreTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 6, 6);
//
//        switch(dir)
//        {
//        case E_WGR_MDIR_None:
//            {//未移动
//                CFSixMoreTouchPublic::m_curMissMDirTolerance = 0;
//
//                CFSixMoreTouchPublic::m_curNonMoveTolerance++;
//
//                if (CFSixMoreTouchPublic::m_curNonMoveTolerance > FDSPDESKTOP_MAX_NONMOVETOLERANCE)
//                {
//                    m_curState = WGR_FDSPDESKTOP_CLOSE_STATE;
//                }
//            }
//
//            break;
//
//        case E_WGR_MDIR_DVertic:
//            {
//                CFSixMoreTouchPublic::m_curMissMDirTolerance = 0;
//                CFSixMoreTouchPublic::m_curNonMoveTolerance = 0;
//
//                if (m_curState == WGR_FDSPDESKTOP_LISTEN_STATE)
//                {//在监听或者保持状态下，水平移动时，转化到执行状态
//                    m_curState = WGR_FDSPDESKTOP_KEEP_STATE;                    
//
//                    CFSixMoreTouchPublic::m_ptSartMid = CFSixMoreTouchPublic::m_ptLastMid;
//                    CFSixMoreTouchPublic::ResetSomeMembers();
//                }                
//
//                if (m_curState == WGR_FDSPDESKTOP_KEEP_STATE)
//                {
//                    m_curState = WGR_FDSPDESKTOP_EXE_STATE;
//                    if(pContext)
//                    {
//                        //触发显示桌面事件
//                        pContext->TriggerDisplayDesktop(CFSixMoreTouchPublic::m_ptSartMid);
//
//                        CFSixMoreTouchPublic::m_ptSartMid = CFSixMoreTouchPublic::m_ptLastMid;
//                        CFSixMoreTouchPublic::ResetSomeMembers();
//                    }
//
//                }
//            }
//
//            break;
//
//        default:
//            {
//                CFSixMoreTouchPublic::m_curNonMoveTolerance = 0;
//
//                CFSixMoreTouchPublic::m_curMissMDirTolerance++;
//
//                if(CFSixMoreTouchPublic::m_curMissMDirTolerance > FDSPDESKTOP_MAX_MISSDIR)
//                {
//                    //CFTwoTouchPublic::ResetSomeMembers();
//                    m_curState = WGR_FDSPDESKTOP_CLOSE_STATE;
//                }
//            }
//            break;
//        }
//
//        g_oWGRTraceLog.AddMoveDirection(dir);
//    } while (false);
//
//    if (m_curState == WGR_FDSPDESKTOP_CLOSE_STATE)
//    {//WGR_FDSPDESKTOP_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
//        m_curState = WGR_IDLE_STATE;
//        CFSixMoreTouchPublic::ResetSomeMembers();
//    }
//
//    //if (m_curState == WGR_FDSPDESKTOP_EXE_STATE)
//    //{//WGR_FDSPDESKTOP_EXE_STATE也是过渡状态，不持久性停留，执行之后就转化为WGR_FDSPDESKTOP_KEEP_STATE
//    //    //WGR_FDSPDESKTOP_KEEP_STATE保持到直到结束        
//    //    m_curState = WGR_FDSPDESKTOP_KEEP_STATE;
//    //}
//
//    g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFSixMoreTouchPublic::m_curNonMoveTolerance);
//    g_oWGRTraceLog.AddCurMissDirTolerance(CFSixMoreTouchPublic::m_curMissMDirTolerance);
//    g_oWGRTraceLog.AddSateToCovert(m_curState);    
//
//    return m_curState;
//}
