#include "StdAfx.h"
#include "FAltLArrowStateMachine.h"
#include "FThree2FiveTouchPublic.h"
#include "..\..\GlobalLog.h"

//
#define FALTLARROW_MAX_MISSSHAPETOLLERANCE                   1
#define FALTLARROW_MAX_MISSTOUCHCOUNTTOLERANCE               5
#define FALTLARROW_MAX_NONMOVETOLERANCE                      30
#define FALTLARROW_MAX_MISSDIR                               2
//******

CFAltLArrowStateMachine::CFAltLArrowStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CFAltLArrowStateMachine::~CFAltLArrowStateMachine(void)
{
}

bool CFAltLArrowStateMachine::StartRun()
{
    m_curState = WGR_FALTLARROW_LISTEN_STATE;
    return true;
}

bool CFAltLArrowStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    return true;
}

IWGRStateMachine *CFAltLArrowStateMachine::GetSubMachineCurStateContained()
{
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFAltLArrowStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    //do 
    //{
    //    //if(pContext)
    //    //{//
    //    //    //pContext->TriggerMouseLeftButtonDown(&(CFThree2FiveTouchPublic::m_ptSartMid));
    //    //    pContext->TriggerHIDUp(&(CFThree2FiveTouchPublic::m_ptSartMid));
    //    //}

    //    g_oWGRTraceLog.AddSpotCount(nCount);

    //    if (g_oWGRTraceLog.IsLogHasStarted())
    //    {
    //        //
    //        POINT pt[16];
    //        for (int i = 0; i < nCount; i++)
    //        {
    //            pt[i] = aryLightSpots[i].ptPosInScreen;
    //        }

    //        g_oWGRTraceLog.AddSpotPos(pt, nCount);
    //    }

    //    if (gestureShape != E_WGR_GShape_Finger)
    //    {//非手指形状
    //        CFThree2FiveTouchPublic::m_curMissShapeTolerance++;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);

    //        if (CFThree2FiveTouchPublic::m_curMissShapeTolerance > FALTLARROW_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_FALTLARROW_CLOSE_STATE;                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFThree2FiveTouchPublic::m_curMissShapeTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);
    //    }

    //    if(nCount <= 2 || nCount > 5)
    //    {//光点数不等于2
    //        CFThree2FiveTouchPublic::m_curMissTouchCountTolerance++;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFThree2FiveTouchPublic::m_curMissTouchCountTolerance);

    //        if (CFThree2FiveTouchPublic::m_curMissTouchCountTolerance > FALTLARROW_MAX_MISSTOUCHCOUNTTOLERANCE)
    //        {
    //            m_curState = WGR_FALTLARROW_CLOSE_STATE;                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFThree2FiveTouchPublic::m_curMissTouchCountTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFThree2FiveTouchPublic::m_curMissTouchCountTolerance);
    //    }

    //    E_WGR_MOVEDIR dir = CFThree2FiveTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 4, 4);

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {//未移动
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;

    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance++;

    //            if (CFThree2FiveTouchPublic::m_curNonMoveTolerance > FALTLARROW_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_FALTLARROW_CLOSE_STATE;
    //            }
    //        }

    //        break;

    //    case E_WGR_MDIR_LHoriz:
    //        {
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            if (m_curState == WGR_FALTLARROW_LISTEN_STATE || m_curState == WGR_FALTLARROW_KEEP_STATE)
    //            {//在监听或者保持状态下，水平移动时，转化到执行状态
    //                m_curState = WGR_FALTLARROW_EXE_STATE;

    //                if(pContext)
    //                {
    //                    //触发ALT + Left Arrow事件
    //                    //pContext->TriggerAltLArrowEvent(CFThree2FiveTouchPublic::m_ptLastMid);
    //                }
    //            }

    //            CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
    //            CFThree2FiveTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    default:
    //        {
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance++;

    //            if(CFThree2FiveTouchPublic::m_curMissMDirTolerance > FALTLARROW_MAX_MISSDIR)
    //            {
    //                //CFTwoTouchPublic::ResetSomeMembers();
    //                m_curState = WGR_FALTLARROW_CLOSE_STATE;
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //} while (false);

    //if (m_curState == WGR_FALTLARROW_CLOSE_STATE)
    //{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CFThree2FiveTouchPublic::ResetSomeMembers();
    //}

    //if (m_curState == WGR_FALTLARROW_EXE_STATE)
    //{//WGR_FALTLARROW_EXE_STATE也是过渡状态，不持久性停留
    //    m_curState = WGR_FALTLARROW_KEEP_STATE;
    //}

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFThree2FiveTouchPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurMissDirTolerance(CFThree2FiveTouchPublic::m_curMissMDirTolerance);
    //g_oWGRTraceLog.AddSateToCovert(m_curState);



    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFAltLArrowStateMachine::TransitState(IWGRUtilContext *pContext)
{
	return WGR_IDLE_STATE;
}

