#include "StdAfx.h"
#include "PLowLimStateMachine.h"
#include "PLowLimPublic.h"
#include "..\..\..\GlobalLog.h"
//
#define PLOWLIM_MAX_MISSSHAPETOLLERANCE                   1
#define PLOWLIM_MAX_NONMOVETOLERANCE                      30
#define PLOWLIM_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define PLOWLIM_MAX_UNEXPECTEDKNOWNMOV                    7
//******

CPLowLimStateMachine::CPLowLimStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CPLowLimStateMachine::~CPLowLimStateMachine(void)
{
}

bool CPLowLimStateMachine::StartRun()
{
    m_curState = WGR_PLOWLIM_LISTEN_STATE;
    return true;
}

bool CPLowLimStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    return false;
}

IWGRStateMachine *CPLowLimStateMachine::GetSubMachineCurStateContained()
{
    if (m_curState > WGR_SHOWWNDSBS_STATE_BEGIN && m_curState < WGR_SHOWWNDSBS_STATE_END)
    {
        return (IWGRStateMachine *)&m_smShowWndSBS;
    }

    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CPLowLimStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{    
    //do 
    //{   
    //    if(pContext)
    //    {
    //        if (WGR_PLOWLIM_LISTEN_STATE == m_curState)
    //        {
    //            if (nCount > 0)
    //            {
    //                if (false == pContext->Get_TriggerEvent_Permission(E_TEP_LOWLIM_PART, aryLightSpots[0].ptPosInScreen))
    //                {//此功能未开启，不再继续进行判断计算
    //                    m_curState = WGR_PLOWLIM_CLOSE_STATE;
    //                    break;
    //                }
    //            }
    //        }            
    //    }
    //    else
    //    {
    //        //功能执行上下文为空，不再继续进行判断计算
    //        m_curState = WGR_PLOWLIM_CLOSE_STATE;
    //        break;
    //    }

    //    g_oWGRTraceLog.AddGestureShape(gestureShape);
    //    
    //    if (gestureShape != E_WGR_GShape_Palm)
    //    {
    //        CPLowLimPublic::m_curMissShapeTolerance++;

    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CPLowLimPublic::m_curMissShapeTolerance);

    //        if(CPLowLimPublic::m_curMissShapeTolerance > PLOWLIM_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_PLOWLIM_CLOSE_STATE;
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
    //        m_curState = WGR_PLOWLIM_CLOSE_STATE;
    //        break;
    //    }

    //    //光斑的实际大小和标准大小
    //    g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
    //    g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);


    //    g_oWGRTraceLog.AddSpotPos(&(aryLightSpots[0].ptPosInScreen), 1);

    //    g_oWGRTraceLog.AddEnterLess(CPLowLimPublic::m_bEnterLessThanOne);
    //    if (CPLowLimPublic::m_bEnterLessThanOne)
    //    {
    //        CPLowLimPublic::ResetSomeMembers();
    //        CPLowLimPublic::m_nXDisplacement = CPLowLimPublic::m_nYDisplacement = 0;
    //        CPLowLimPublic::m_nLastMass = aryLightSpots[0].mass;
    //        CPLowLimPublic::m_nLastStdMass = aryLightSpots[0].lStdSpotAreaInVideo;
    //        CPLowLimPublic::m_ptLast = CPLowLimPublic::m_ptStart = aryLightSpots[0].ptPosInScreen;
    //        CPLowLimPublic::m_bEnterLessThanOne = false;

    //        if(pContext)
    //        {//弹起鼠标左键
    //            ////pContext->TriggerMouseLeftButtonDown(&(CPLowLimPublic::m_ptStart));
    //            pContext->TriggerHIDUp(&(CPLowLimPublic::m_ptStart));
    //        }
    //    }
    //    
    //    E_WGR_MOVEDIR dir = CPLowLimPublic::CalcMoveDirection(aryLightSpots, nCount, 6, 6);

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {
    //            CPLowLimPublic::m_curMissMDirTolerance = 0;

    //            CPLowLimPublic::m_curNonMoveTolerance++;

    //            if (CPLowLimPublic::m_curNonMoveTolerance > PLOWLIM_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_PLOWLIM_CLOSE_STATE;
    //            }
    //        }
    //        break;

    //    case E_WGR_MDIR_UVertic:
    //        {
    //            CPLowLimPublic::m_curMissMDirTolerance = 0;
    //            CPLowLimPublic::m_curNonMoveTolerance = 0;

    //            m_curState = WGR_SHOWWNDSBS_LISTEN_STATE;
    //            m_smShowWndSBS.StartRun();

    //            CPLowLimPublic::ResetSomeMembers();
    //        }
    //        break;

    //    default:
    //        {
    //            CPLowLimPublic::m_curMissMDirTolerance++;

    //            CPLowLimPublic::m_curNonMoveTolerance = 0;

    //            if (CPLowLimPublic::m_curMissMDirTolerance > PLOWLIM_MAX_MISSDIR)
    //            {
    //                m_curState = WGR_PLOWLIM_CLOSE_STATE;
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //} while (false);

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CPLowLimPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CPLowLimPublic::m_curMissMDirTolerance);

    //if (m_curState == WGR_PLOWLIM_CLOSE_STATE)
    //{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CPLowLimPublic::ResetSomeMembers();
    //}

    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CPLowLimStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
	{   
		if(pContext)
		{
			if (WGR_PLOWLIM_LISTEN_STATE == m_curState)
			{
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEP_LOWLIM_PART))
				{//此功能未开启，不再继续进行判断计算
					m_curState = WGR_PLOWLIM_CLOSE_STATE;
					break;
				}				
			}            
		}
		else
		{
			//功能执行上下文为空，不再继续进行判断计算
			m_curState = WGR_PLOWLIM_CLOSE_STATE;
			break;
		}

		//g_oWGRTraceLog.AddGestureShape(gestureShape);

		if (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Palm)
		{
			CWGRCommonUlity::m_curUnexpectedShapeTimes++;

			//g_oWGRTraceLog.AddCurMissShapeTolerance(CPLowLimPublic::m_curMissShapeTolerance);

			if(CWGRCommonUlity::m_curUnexpectedShapeTimes > PLOWLIM_MAX_MISSSHAPETOLLERANCE)
			{
				m_curState = WGR_PLOWLIM_CLOSE_STATE;
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
			m_curState = WGR_PLOWLIM_CLOSE_STATE;
			break;
		}

		////光斑的实际大小和标准大小
		//g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
		//g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);


		//g_oWGRTraceLog.AddSpotPos(&(aryLightSpots[0].ptPosInScreen), 1);

		//g_oWGRTraceLog.AddEnterLess(CPLowLimPublic::m_bEnterLessThanOne);
		if (!CWGRCommonUlity::m_havedEnteredTheSumSM)
		{			
			CWGRCommonUlity::ResetSomeMembers();			
			CWGRCommonUlity::InitWhenFirstEnter(pContext);
		}

		E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(6, 6, 1, 10);

		switch(dir)
		{
		case E_WGR_MDIR_None:
			{
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

				CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;

				if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > PLOWLIM_MAX_NONMOVETOLERANCE)
				{
					m_curState = WGR_PLOWLIM_CLOSE_STATE;
				}
			}
			break;

		case E_WGR_MDIR_UVertic:
			{
				m_curState = WGR_SHOWWNDSBS_LISTEN_STATE;
				m_smShowWndSBS.StartRun();

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

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > PLOWLIM_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_PLOWLIM_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();                
            }

            break;

		default:
			{				
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;


				if (CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > PLOWLIM_MAX_UNEXPECTEDUNKNOWNMOV)
				{
					m_curState = WGR_PLOWLIM_CLOSE_STATE;
				}
			}
			break;
		}

		//g_oWGRTraceLog.AddMoveDirection(dir);
	} while (false);

	//g_oWGRTraceLog.AddCurMissNonMoveTolerance(CPLowLimPublic::m_curNonMoveTolerance);
	//g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CPLowLimPublic::m_curMissMDirTolerance);

	if (m_curState == WGR_PLOWLIM_CLOSE_STATE)
	{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
		m_curState = WGR_IDLE_STATE;
		CWGRCommonUlity::ResetSomeMembers();
	}

	//g_oWGRTraceLog.AddSateToCovert(m_curState);

	return m_curState;
}
