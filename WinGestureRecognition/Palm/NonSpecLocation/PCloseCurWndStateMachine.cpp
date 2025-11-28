#include "StdAfx.h"
#include "PCloseCurWndStateMachine.h"
#include "PNonSpecLocPublic.h"
#include "..\..\GlobalLog.h"

//
#define PCLSCWND_MAX_MISSDIR                               1
#define PCLSCWND_MIN_MISSSPALMTOLLERANCE                   5
#define PCLSCWND_MAX_MISSSPALMTOLLERANCE                   20
#define PCLSCWND_MAX_MISSSHAPETOLLERANCE                   3
#define PCLSCWND_MAX_NONMOVETOLERANCE                      15
//******

CPCloseCurWndStateMachine::CPCloseCurWndStateMachine(void)
{
    m_nConMissPalm = 0;
    m_curState = WGR_IDLE_STATE;
}

CPCloseCurWndStateMachine::~CPCloseCurWndStateMachine(void)
{
}

bool CPCloseCurWndStateMachine::StartRun()
{
    m_curState = WGR_PCLSCWND_LISTEN_STATE;
    m_nConMissPalm = 0;
    return true;
}

bool CPCloseCurWndStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    m_nConMissPalm = 0;
    return false;
}

IWGRStateMachine *CPCloseCurWndStateMachine::GetSubMachineCurStateContained()
{    
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CPCloseCurWndStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{   
    //do 
    //{
    //    if (pContext)
    //    {
    //        if (m_curState == WGR_PCLSCWND_LISTEN_STATE)
    //        {
    //            if (false == pContext->Get_TriggerEvent_Permission(E_TEP_NSPLOC_CLSCURWND, CPNonSpecLocPublic::m_ptStart))
    //            {
    //                //此功能已经禁止，无需继续判断
    //                m_curState = WGR_PCLSCWND_CLOSE_STATE;
    //                break;
    //            }
    //        }
    //    }
    //    else
    //    {//执行上下文为空，无需再继续判断
    //        m_curState = WGR_PCLSCWND_CLOSE_STATE;
    //        break;
    //    }

    //    bool bDClickComplete = false;
    //    g_oWGRTraceLog.AddGestureShape(gestureShape);
    //    
    //    if (gestureShape != E_WGR_GShape_Palm)
    //    {
    //        if (gestureShape == E_WGR_GShape_Finger)
    //        {
    //            CPNonSpecLocPublic::m_curMissShapeTolerance++;

    //            if (CPNonSpecLocPublic::m_curMissShapeTolerance > PCLSCWND_MAX_MISSSHAPETOLLERANCE)
    //            {               
    //                m_curState = WGR_PCLSCWND_CLOSE_STATE;               
    //                CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //            }
    //        }
    //        else
    //        {  
    //            m_nConMissPalm++;

    //            if (m_nConMissPalm > PCLSCWND_MAX_MISSSPALMTOLLERANCE)
    //            {
    //                m_curState = WGR_PCLSCWND_CLOSE_STATE;               
    //                m_nConMissPalm = 0;
    //            }
    //        }            

    //        break;
    //    }
    //    else
    //    {
    //        if (m_nConMissPalm > PCLSCWND_MIN_MISSSPALMTOLLERANCE & m_nConMissPalm < PCLSCWND_MAX_MISSSPALMTOLLERANCE && nCount == 1)
    //        {
    //            bDClickComplete = true;
    //        }
    //        else
    //        {
    //            m_curState = WGR_PCLSCWND_CLOSE_STATE; 
    //            break;
    //        }

    //        CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //        m_nConMissPalm = 0;
    //    }

    //    //if(pContext)
    //    //{//
    //    //    //pContext->TriggerMouseLeftButtonDown(&(CPNonSpecLocPublic::m_ptStart));
    //    //    pContext->TriggerHIDUp(&(CPNonSpecLocPublic::m_ptStart));
    //    //}

    //    //光斑的实际大小和标准大小
    //    g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
    //    g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);


    //    E_WGR_MOVEDIR dir = CPNonSpecLocPublic::CalcMoveDirection(aryLightSpots, nCount, 15, 15);

    //    g_oWGRTraceLog.AddMoveDirection(dir);

    //    bDClickComplete = bDClickComplete && (dir == E_WGR_MDIR_None);

    //    if(bDClickComplete)
    //    {
    //        //执行关闭窗口               
    //        if(bDClickComplete)
    //        {
    //            //
    //            if (pContext)
    //            {
    //                pContext->TriggerCloseCurrentWindowEvent(CPNonSpecLocPublic::m_ptStart);
    //            }
    //        }            
    //    }

    //    m_curState = WGR_PCLSCWND_CLOSE_STATE;

    //    //CPNonSpecLocPublic::ResetSomeMembers();
    //}while(false);

    //if (m_curState == WGR_PCLSCWND_CLOSE_STATE)
    //{
    //    m_curState = WGR_IDLE_STATE;

    //    CPNonSpecLocPublic::ResetSomeMembers();
    //}

    //return m_curState;
	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CPCloseCurWndStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
	{
		if (pContext)
		{
			if (m_curState == WGR_PCLSCWND_LISTEN_STATE)
			{
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEP_NSPLOC_CLSCURWND))
				{
					//此功能已经禁止，无需继续判断
					m_curState = WGR_PCLSCWND_CLOSE_STATE;
					break;
				}
			}
		}
		else
		{//执行上下文为空，无需再继续判断
			m_curState = WGR_PCLSCWND_CLOSE_STATE;
			break;
		}

		bool bDClickComplete = false;
		//g_oWGRTraceLog.AddGestureShape(gestureShape);

		if (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Palm)
		{
			if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Finger)
			{
				CWGRCommonUlity::m_curUnexpectedShapeTimes++;

				if (CWGRCommonUlity::m_curUnexpectedShapeTimes > PCLSCWND_MAX_MISSSHAPETOLLERANCE)
				{               
					m_curState = WGR_PCLSCWND_CLOSE_STATE;               
					CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
				}
			}
			else
			{  
				m_nConMissPalm++;

				if (m_nConMissPalm > PCLSCWND_MAX_MISSSPALMTOLLERANCE)
				{
					m_curState = WGR_PCLSCWND_CLOSE_STATE;               
					m_nConMissPalm = 0;
				}
			}            

			break;
		}
		else
		{
			if (m_nConMissPalm > PCLSCWND_MIN_MISSSPALMTOLLERANCE && m_nConMissPalm < PCLSCWND_MAX_MISSSPALMTOLLERANCE && CWGRCommonUlity::IsInputTouchsEqual(1))
			{
				bDClickComplete = true;
			}
			else
			{
				m_curState = WGR_PCLSCWND_CLOSE_STATE; 
				break;
			}

			CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			m_nConMissPalm = 0;
		}		

		////光斑的实际大小和标准大小
		//g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
		//g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);


		E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(15, 15, 1, 10);

		//g_oWGRTraceLog.AddMoveDirection(dir);

		bDClickComplete = bDClickComplete && (dir == E_WGR_MDIR_None);

		if(bDClickComplete)
		{
			//执行关闭窗口               
			if(bDClickComplete)
			{
				//
				if (pContext)
				{
					pContext->TriggerCloseCurrentWindowEvent(CWGRCommonUlity::m_curPtMiddle);
				}
			}            
		}

		m_curState = WGR_PCLSCWND_CLOSE_STATE;

		//CPNonSpecLocPublic::ResetSomeMembers();
	}while(false);

	if (m_curState == WGR_PCLSCWND_CLOSE_STATE)
	{
		m_curState = WGR_IDLE_STATE;

		CWGRCommonUlity::ResetSomeMembers();
	}

	return m_curState;
}
