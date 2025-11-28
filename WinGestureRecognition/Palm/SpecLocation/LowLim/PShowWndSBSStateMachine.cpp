#include "StdAfx.h"
#include "PShowWndSBSStateMachine.h"
#include "PLowLimPublic.h"
#include "..\..\..\GlobalLog.h"

//
#define PSHOWWNDSBS_MAX_MISSSHAPETOLLERANCE                   2
#define PSHOWWNDSBS_MAX_NONMOVETOLERANCE                      30
#define PSHOWWNDSBS_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define PSHOWWNDSBS_MAX_UNEXPECTEDKNOWNMOV                    7
//******

CPShowWndSBSStateMachine::CPShowWndSBSStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CPShowWndSBSStateMachine::~CPShowWndSBSStateMachine(void)
{
}

bool CPShowWndSBSStateMachine::StartRun()
{
    m_curState = WGR_SHOWWNDSBS_LISTEN_STATE;
    return true;
}

bool CPShowWndSBSStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    return false;
}

IWGRStateMachine *CPShowWndSBSStateMachine::GetSubMachineCurStateContained()
{
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CPShowWndSBSStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    //do 
    //{
    //    if(pContext)
    //    {//
    //        ////pContext->TriggerMouseLeftButtonDown(&(CPLowLimPublic::m_ptStart));
    //        if (m_curState == WGR_SHOWWNDSBS_LISTEN_STATE)
    //        {
    //            if (false == pContext->Get_TriggerEvent_Permission(E_TEP_LOWLIM_SHOWWNDSBS, CPLowLimPublic::m_ptStart))
    //            {
    //                m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
    //                break;
    //            }
    //        }

    //        //pContext->TriggerHIDUp(&(CPLowLimPublic::m_ptStart));
    //    }
    //    else
    //    {
    //        m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
    //        break;
    //    }

    //    g_oWGRTraceLog.AddGestureShape(gestureShape);
    //    
    //    if (gestureShape != E_WGR_GShape_Palm)
    //    {
    //        CPLowLimPublic::m_curMissShapeTolerance++;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CPLowLimPublic::m_curMissShapeTolerance);

    //        if(CPLowLimPublic::m_curMissShapeTolerance > PSHOWWNDSBS_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
    //            CPLowLimPublic::m_curMissShapeTolerance = 0;
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CPLowLimPublic::m_curMissShapeTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CPLowLimPublic::m_curMissShapeTolerance);
    //    }

    //    g_oWGRTraceLog.AddSpotCount(nCount);

    //    if (!aryLightSpots || nCount < 0)
    //    {
    //        m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
    //        break;
    //    }

    //    //光斑的实际大小和标准大小
    //    g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
    //    g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);


    //    g_oWGRTraceLog.AddSpotPos(&(aryLightSpots[0].ptPosInScreen), 1);
    //    /*if (CPLowLimPublic::m_bEnterLessThanOne)
    //    {
    //        CPLowLimPublic::ResetSomeMembers();
    //        CPLowLimPublic::m_nXDisplacement = CPLowLimPublic::m_nXDisplacement = 0;
    //        CPLowLimPublic::m_ptLast = CPLowLimPublic::m_ptStart = aryLightSpots[0].ptPosInScreen;
    //        CPLowLimPublic::m_bEnterLessThanOne = false;
    //    }*/

    //    E_WGR_MOVEDIR dir = CPLowLimPublic::CalcMoveDirection(aryLightSpots, nCount, 6, 6);

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {
    //            CPLowLimPublic::m_curMissMDirTolerance = 0;

    //            CPLowLimPublic::m_curNonMoveTolerance++;

    //            if (CPLowLimPublic::m_curNonMoveTolerance > PSHOWWNDSBS_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
    //            }
    //        }
    //        break;

    //    case E_WGR_MDIR_UVertic:
    //        {
    //            CPLowLimPublic::m_curMissMDirTolerance = 0;
    //            CPLowLimPublic::m_curNonMoveTolerance = 0;

    //            if (m_curState == WGR_SHOWWNDSBS_LISTEN_STATE)
    //            {
    //                m_curState = WGR_SHOWWNDSBS_EXE_STATE;
    //            }

    //            if(m_curState == WGR_SHOWWNDSBS_EXE_STATE)
    //            {
    //                if(pContext)
    //                {
    //                    pContext->TriggerShowWndSBS(CPLowLimPublic::m_ptStart);
    //                }

    //                m_curState = WGR_SHOWWNDSBS_KEEP_STATE;
    //            }

    //            CPLowLimPublic::ResetSomeMembers();
    //        }
    //        break;

    //    default:
    //        {
    //            CPLowLimPublic::m_curMissMDirTolerance++;

    //            CPLowLimPublic::m_curNonMoveTolerance = 0;

    //            if (CPLowLimPublic::m_curMissMDirTolerance > PSHOWWNDSBS_MAX_MISSDIR)
    //            {
    //                m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //} while (false);


    //if (m_curState == WGR_SHOWWNDSBS_CLOSE_STATE)
    //{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CPLowLimPublic::ResetSomeMembers();
    //}

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CPLowLimPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CPLowLimPublic::m_curMissMDirTolerance);
    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;
	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CPShowWndSBSStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
    {
        if(pContext)
        {//
            ////pContext->TriggerMouseLeftButtonDown(&(CPLowLimPublic::m_ptStart));
            if (m_curState == WGR_SHOWWNDSBS_LISTEN_STATE)
            {
                if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEP_LOWLIM_SHOWWNDSBS))
                {
                    m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
                    break;
                }
            }

            //pContext->TriggerHIDUp(&(CPLowLimPublic::m_ptStart));
        }
        else
        {
            m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
            break;
        }

        //g_oWGRTraceLog.AddGestureShape(gestureShape);
        
        if (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Palm)
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes++;
            //g_oWGRTraceLog.AddCurMissShapeTolerance(CPLowLimPublic::m_curMissShapeTolerance);

            if(CWGRCommonUlity::m_curUnexpectedShapeTimes > PSHOWWNDSBS_MAX_MISSSHAPETOLLERANCE)
            {
                m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
                CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            }

            break;
        }
        else
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            //g_oWGRTraceLog.AddCurMissShapeTolerance(CPLowLimPublic::m_curMissShapeTolerance);
        }

        //g_oWGRTraceLog.AddSpotCount(nCount);

        if (!CWGRCommonUlity::IsInputTouchsBetween(1, 10))
        {
            m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
            break;
        }

        ////光斑的实际大小和标准大小
        //g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
        //g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);


        //g_oWGRTraceLog.AddSpotPos(&(aryLightSpots[0].ptPosInScreen), 1);        

        E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(6, 6, 1, 10);

        switch(dir)
        {
        case E_WGR_MDIR_None:
            {
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

				CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;

                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > PSHOWWNDSBS_MAX_NONMOVETOLERANCE)
                {
                    m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
                }
            }
            break;

        case E_WGR_MDIR_UVertic:
            {                
                if (m_curState == WGR_SHOWWNDSBS_LISTEN_STATE)
                {
                    m_curState = WGR_SHOWWNDSBS_EXE_STATE;
                }

                if(m_curState == WGR_SHOWWNDSBS_EXE_STATE)
                {
                    if(pContext)
                    {
                        pContext->TriggerShowWndSBS(CWGRCommonUlity::m_curPtMiddle);
                    }

                    m_curState = WGR_SHOWWNDSBS_KEEP_STATE;
                }

				CWGRCommonUlity::ResetDislaymentInLastTouchMap();
				CWGRCommonUlity::ResetSomeMembers();
            }
            break;

        case E_WGR_MDIR_Move:
        case E_WGR_MDIR_LHoriz:
        case E_WGR_MDIR_RHoriz:
        case E_WGR_MDIR_DVertic:
            {
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;                
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;                

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > PSHOWWNDSBS_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();                
            }

            break;


        default:
            {
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;


                if (CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > PSHOWWNDSBS_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    m_curState = WGR_SHOWWNDSBS_CLOSE_STATE;
                }
            }
            break;
        }

        //g_oWGRTraceLog.AddMoveDirection(dir);
    } while (false);


    if (m_curState == WGR_SHOWWNDSBS_CLOSE_STATE)
    {//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
        m_curState = WGR_IDLE_STATE;
        CWGRCommonUlity::ResetSomeMembers();
    }

	/*g_oWGRTraceLog.AddCurMissNonMoveTolerance(CPLowLimPublic::m_curNonMoveTolerance);
	g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CPLowLimPublic::m_curMissMDirTolerance);
	g_oWGRTraceLog.AddSateToCovert(m_curState);*/

    return m_curState;
}
