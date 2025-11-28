#include "StdAfx.h"
#include "WGRStateMachine.h"
#include "WGRCommonUlity.h"

CWGRStateMachine::CWGRStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
    m_pIWGRSM = NULL;   
}

CWGRStateMachine::~CWGRStateMachine(void)
{
    //StopRun();
}

bool CWGRStateMachine::StartRun()
{
    m_curState = WGR_IDLE_STATE;
    return true;
}

bool CWGRStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    return true;
}

IWGRStateMachine *CWGRStateMachine::GetSubMachineCurStateContained()
{
    m_pIWGRSM = (IWGRStateMachine *)this;

    if(m_curState == WGR_IDLE_STATE) return m_pIWGRSM;

    if (m_curState > WGR_FINGER_STATE_BEGIN && m_curState < WGR_FINGER_STATE_END)
    {//当前状态为手指触点中的状态
        m_pIWGRSM = m_smFinger.GetSubMachineCurStateContained();
    }
    else if (m_curState > WGR_PALM_STATE_BEGIN && m_curState < WGR_PALM_STATE_END)
    {//当前状态为手掌触点中的状态
        m_pIWGRSM = m_smPalm.GetSubMachineCurStateContained();
    }

    return m_pIWGRSM;
}

WGR_STATE_TYPE CWGRStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{ 
    do
    {
        if (pContext)
        {
            if (nCount > 0)
            {
                if (false == pContext->Get_TriggerEvent_Permission(E_TE_PART, aryLightSpots[0].ptPosInScreen))
                {//所有手势功能被禁止了
                    m_curState = WGR_IDLE_STATE;

                    break;
                }
            }
        }
        else
        {
            m_curState = WGR_IDLE_STATE;
            break;
        }

        if (m_curState == WGR_IDLE_STATE)
        {//空闲状态，需要根据手势的形状来决定是进入手指状态还是手掌状态子自动机
            if(gestureShape == E_WGR_GShape_Finger)
            {//手指形状
                m_pIWGRSM = (IWGRStateMachine *)&m_smFinger;             
            }
            else 
                if (gestureShape == E_WGR_GShape_Palm)
                {//手掌形状
                    m_pIWGRSM = (IWGRStateMachine *)&m_smPalm;
                }
                else
                {//其他            

                    return WGR_IDLE_STATE;
                }

                //开启子自动机
                m_pIWGRSM->StartRun();
        }
        else
        {//正处在手指或手掌子自动机的某个状态
            GetSubMachineCurStateContained();
        }    

        //执行自自动机的状态转换
        m_curState = m_pIWGRSM->TransitState(aryLightSpots, nCount, pContext, gestureShape);
    }while(false);

    return m_curState;
}

WGR_STATE_TYPE CWGRStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do
	{
		if (pContext)
		{
			if ((m_curState == WGR_IDLE_STATE) && CWGRCommonUlity::IsInputTouchsBetween(1, 10))
			{                
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TE_PART))
				{//所有手势功能被禁止了
					m_curState = WGR_IDLE_STATE;

					break;
				}
			}
		}
		else
		{
			m_curState = WGR_IDLE_STATE;
			break;
		}

		if (m_curState == WGR_IDLE_STATE)
		{//空闲状态，需要根据手势的形状来决定是进入手指状态还是手掌状态子自动机
			if(CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Finger)
			{//手指形状
				m_pIWGRSM = (IWGRStateMachine *)&m_smFinger;             
			}
			else 
			if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Palm)
			{//手掌形状
				m_pIWGRSM = (IWGRStateMachine *)&m_smPalm;
			}
			else
			{//其他            

				return WGR_IDLE_STATE;
			}

			//开启子自动机
			m_pIWGRSM->StartRun();
		}
		else
		{//正处在手指或手掌子自动机的某个状态
			GetSubMachineCurStateContained();
		}    

		//执行自自动机的状态转换
		m_curState = m_pIWGRSM->TransitState(pContext);
	}while(false);

	return m_curState;
}
