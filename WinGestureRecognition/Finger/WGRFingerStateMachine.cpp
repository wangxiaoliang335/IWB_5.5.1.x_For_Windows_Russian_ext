#include "StdAfx.h"
#include "WGRFingerStateMachine.h"
#include "..\WGRCommonUlity.h"

CWGRFingerStateMachine::CWGRFingerStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;    
}

CWGRFingerStateMachine::~CWGRFingerStateMachine(void)
{
}

bool CWGRFingerStateMachine::StartRun()
{
    m_curState = WGR_FINGER_STATE;

    return true;
}

bool CWGRFingerStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;

    return true;
}

IWGRStateMachine *CWGRFingerStateMachine::GetSubMachineCurStateContained()
{
    IWGRStateMachine *pSM = (IWGRStateMachine *)this;    

    if (m_curState > WGR_FTWOTOUCH_STATE_BEGIN && m_curState < WGR_FTWOTOUCH_STATE_END)
    {
        pSM = m_smTwoTouch.GetSubMachineCurStateContained();
    }
    
    if (m_curState > WGR_FTHREE2FIVETOUCH_STATE_BEGIN && m_curState < WGR_FTHREE2FIVETOUCH_STATE_END)
    {
        pSM = m_smThree2FiveTouch.GetSubMachineCurStateContained();
    }

    if (m_curState > WGR_FSIXMORETOUCH_STATE_BEGIN && m_curState < WGR_FSIXMORETOUCH_STATE_END)
    {
        pSM = m_smSixMoreTouch.GetSubMachineCurStateContained();
    }

    if (g_oLogHasStarted)
    {
        g_oCurCollects++;

        g_oWGRTraceLog.AddCollectTag(g_oCurCollects, m_curState);
    }

    return pSM;
}

WGR_STATE_TYPE CWGRFingerStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    //IWGRStateMachine *pSM = NULL;

    //do 
    //{
    //    if (pContext)
    //    {
    //        if (nCount > 0)
    //        {
    //            if((false == pContext->Get_TriggerEvent_Permission(E_TEF_PART, aryLightSpots[0].ptPosInScreen)))
    //            {
    //                //手掌下的功能全部禁止了
    //                m_curState = WGR_IDLE_STATE;

    //                break;
    //            }
    //        }
    //    }
    //    else
    //    {
    //        //无执行功能上下文，不需要继续计算判断
    //        m_curState = WGR_IDLE_STATE;

    //        break;
    //    }

    //    if(gestureShape != E_WGR_GShape_Finger)
    //    {//ֹͣ
    //        StopRun();
    //    }
    //    //else if (!g_oLogHasStarted)
    //    //{
    //    //    //if (nCount >= 2)
    //    //    {
    //    //        g_oWGRTraceLog.StartLog();
    //    //        g_oLogHasStarted = true;
    //    //        g_oCurCollects = 0;
    //    //    }
    //    //}    

    //    if (nCount == 2)
    //    {
    //        pSM = (IWGRStateMachine *)&m_smTwoTouch;
    //        m_curState = WGR_FTWOTOUCH_LISTEN_STATE;
    //    }

    //    if (nCount >= 3)
    //    {
    //        if (nCount <= 5)
    //        {
    //            pSM = (IWGRStateMachine *)&m_smThree2FiveTouch;
    //            m_curState = WGR_FTHREE2FIVETOUCH_LISTEN_STATE;
    //        }
    //        else
    //        {
    //            pSM = (IWGRStateMachine *)&m_smSixMoreTouch;
    //        }
    //    }

    //    if (g_oLogHasStarted)
    //    {
    //        g_oCurCollects++;

    //        g_oWGRTraceLog.AddCollectTag(g_oCurCollects, m_curState);
    //    }

    //    if (pSM)
    //    {
    //        pSM->StartRun();
    //        m_curState = pSM->TransitState(aryLightSpots, nCount, pContext, gestureShape);
    //    }    
    //} while (false);    

    //return m_curState;

    return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CWGRFingerStateMachine::TransitState(IWGRUtilContext *pContext)
{
	IWGRStateMachine *pSM = NULL;

	do 
	{
		if (pContext)
		{
			if((false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEF_PART)))
			{
				//手掌下的功能全部禁止了
				m_curState = WGR_IDLE_STATE;

				break;
			}
		}
		else
		{
			//无执行功能上下文，不需要继续计算判断
			m_curState = WGR_IDLE_STATE;

			break;
		}

		if(CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Finger)
		{//ֹͣ
			StopRun();

            break;
		}
		//else if (!g_oLogHasStarted)
		//{
		//    //if (nCount >= 2)
		//    {
		//        g_oWGRTraceLog.StartLog();
		//        g_oLogHasStarted = true;
		//        g_oCurCollects = 0;
		//    }
		//}    

		if (CWGRCommonUlity::IsInputTouchsEqual(2))
		{
			pSM = (IWGRStateMachine *)&m_smTwoTouch;
			m_curState = WGR_FTWOTOUCH_LISTEN_STATE;
		}

		if (CWGRCommonUlity::IsInputTouchsBetween(3, 10))
		{
			if (CWGRCommonUlity::IsInputTouchsBetween(3, 5))
			{
				pSM = (IWGRStateMachine *)&m_smThree2FiveTouch;
				m_curState = WGR_FTHREE2FIVETOUCH_LISTEN_STATE;
			}
			else
			{
				pSM = (IWGRStateMachine *)&m_smSixMoreTouch;
			}
		}

		if (g_oLogHasStarted)
		{
			g_oCurCollects++;

			g_oWGRTraceLog.AddCollectTag(g_oCurCollects, m_curState);
		}

		if (pSM)
		{
			pSM->StartRun();
			m_curState = pSM->TransitState(pContext);
		}    
	} while (false);    

	return m_curState;
}