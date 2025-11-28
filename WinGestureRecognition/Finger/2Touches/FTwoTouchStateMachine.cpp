#include "StdAfx.h"
#include "FTwoTouchStateMachine.h"
#include "FTwoTouchPublic.h"
#include "..\..\GlobalLog.h"

//
#define FTWOTOUCH_MAX_MISSSHAPETOLLERANCE                   1
#define FTWOTOUCH_MAX_MISS2TOUCHCOUNTTOLERANCE              3
#define FTWOTOUCH_MAX_NONMOVETOLERANCE                      30
#define FTWOTOUCH_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define FTWOTOUCH_MAX_UNEXPECTEDKNOWNMOV                    7
//******

CFTwoTouchStateMachine::CFTwoTouchStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CFTwoTouchStateMachine::~CFTwoTouchStateMachine(void)
{
}

bool CFTwoTouchStateMachine::StartRun()
{
    m_curState = WGR_FTWOTOUCH_LISTEN_STATE;
    //CFTwoTouchPublic::m_bEnterLessThanOne = true;
    //m_bFirstEnter = true;
	CWGRCommonUlity::ResetSomeMembers();

    return true;
}

bool CFTwoTouchStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;

    return true;
}

IWGRStateMachine *CFTwoTouchStateMachine::GetSubMachineCurStateContained()
{
    if (m_curState > WGR_FHMOUSEWHEEL_STATE_BEGIN && m_curState < WGR_FHMOUSEWHEEL_STATE_END)
    {
        return (IWGRStateMachine *)&m_smHMouseWheel;
    }
    else if (m_curState > WGR_FVMOUSEWHEEL_STATE_BEGIN && m_curState < WGR_FVMOUSEWHEEL_STATE_END)
    {
        return (IWGRStateMachine *)&m_smVMouseWheel;
    }
    else
    {
        return (IWGRStateMachine *)this;
    }
}

WGR_STATE_TYPE CFTwoTouchStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    //do
    //{
    //    if(pContext)
    //    {
    //        if (WGR_FTWOTOUCH_LISTEN_STATE == m_curState)
    //        {
    //            if (nCount > 0)
    //            {
    //                if (false == pContext->Get_TriggerEvent_Permission(E_TEF_2TOUCHS_PART, aryLightSpots[0].ptPosInScreen))
    //                {//此功能未开启，不再继续进行判断计算
    //                    m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
    //                    break;
    //                }
    //            }
    //        }            
    //    }
    //    else
    //    {
    //        //功能执行上下文为空，不再继续进行判断计算
    //        m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
    //        break;
    //    }

    //    g_oWGRTraceLog.AddSpotCount(nCount);
    //    
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

    //    g_oWGRTraceLog.AddGestureShape(gestureShape);
    //    if (gestureShape != E_WGR_GShape_Finger)
    //    {//非手指形状
    //        CFTwoTouchPublic::m_curMissShapeTolerance++;

    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFTwoTouchPublic::m_curMissShapeTolerance);

    //        if (CFTwoTouchPublic::m_curMissShapeTolerance > FTWOTOUCH_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_FTWOTOUCH_CLOSE_STATE;                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFTwoTouchPublic::m_curMissShapeTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFTwoTouchPublic::m_curMissShapeTolerance);
    //    }        

    //    if(nCount != 2)
    //    {//光点数不等于2
    //        CFTwoTouchPublic::m_curMiss2TouchCountTolerance++;

    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFTwoTouchPublic::m_curMiss2TouchCountTolerance);

    //        if (CFTwoTouchPublic::m_curMiss2TouchCountTolerance > FTWOTOUCH_MAX_MISS2TOUCHCOUNTTOLERANCE)
    //        {
    //            m_curState = WGR_FTWOTOUCH_CLOSE_STATE;                                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFTwoTouchPublic::m_curMiss2TouchCountTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFTwoTouchPublic::m_curMiss2TouchCountTolerance);
    //    }

    //    g_oWGRTraceLog.AddEnterLess(CFTwoTouchPublic::m_bEnterLessThanOne);
    //    if (CFTwoTouchPublic::m_bEnterLessThanOne)
    //    {
    //        CFTwoTouchPublic::m_bEnterLessThanOne = false;

    //        CFTwoTouchPublic::m_ptCur[0] = CFTwoTouchPublic::m_ptLast[0] = aryLightSpots[0].ptPosInScreen;
    //        CFTwoTouchPublic::m_ptCur[1] = CFTwoTouchPublic::m_ptLast[1] = aryLightSpots[1].ptPosInScreen;

    //        if(pContext)
    //        {//           
    //            pContext->TriggerHIDUp(&(CFTwoTouchPublic::m_ptCur[0]));
    //        }
    //    }
    //    
    //    /*
    //    @计算移动方向的方法是：两点的平均位置ptCurMid与上一次平均位置ptLastMid比较
    //    */
    //    /*POINT ptCurMid;
    //    ptCurMid.x = (aryLightSpots[0].ptPosInScreen.x + aryLightSpots[1].ptPosInScreen.x + 1) >> 1;
    //    ptCurMid.y = (aryLightSpots[0].ptPosInScreen.y + aryLightSpots[1].ptPosInScreen.y + 1) >> 1;*/
    //    //

    //    E_WGR_MOVEDIR dir = CFTwoTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 4, 4);
    //    
    //    g_oWGRTraceLog.AddMoveDirection(dir);

    //    if (dir == E_WGR_MDIR_None)
    //    {//未移动
    //        CFTwoTouchPublic::m_curMissMDirTolerance = 0;

    //        CFTwoTouchPublic::m_curNonMoveTolerance++;            

    //        if (CFTwoTouchPublic::m_curNonMoveTolerance > FTWOTOUCH_MAX_NONMOVETOLERANCE)
    //        {
    //            m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
    //        }
    //    }
    //    else if (dir == E_WGR_MDIR_Horiz)
    //    {//水平移动
    //        CFTwoTouchPublic::m_curNonMoveTolerance = 0;
    //        CFTwoTouchPublic::m_curMissMDirTolerance = 0;
    //        m_curState = WGR_FHMOUSEWHEEL_LISTEN_STATE;
    //        m_smHMouseWheel.StartRun();

    //        //m_curState = WGR_FHMOUSEWHEEL_LISTEN_STATE;

    //        CFTwoTouchPublic::m_ptLast[0] = aryLightSpots[0].ptPosInScreen;
    //        CFTwoTouchPublic::m_ptLast[1] = aryLightSpots[0].ptPosInScreen;

    //        CFTwoTouchPublic::ResetSomeMembers();
    //    }
    //    else if (dir == E_WGR_MDIR_Vertic)
    //    {
    //        CFTwoTouchPublic::m_curNonMoveTolerance = 0;
    //        CFTwoTouchPublic::m_curMissMDirTolerance = 0;
    //        m_curState = WGR_FVMOUSEWHEEL_LISTEN_STATE;
    //        m_smVMouseWheel.StartRun();

    //        CFTwoTouchPublic::m_ptLast[0] = aryLightSpots[0].ptPosInScreen;
    //        CFTwoTouchPublic::m_ptLast[1] = aryLightSpots[0].ptPosInScreen;

    //        CFTwoTouchPublic::ResetSomeMembers();
    //    }
    //    else
    //    {
    //        CFTwoTouchPublic::m_curNonMoveTolerance = 0;

    //        CFTwoTouchPublic::m_curMissMDirTolerance++;

    //        if(CFTwoTouchPublic::m_curMissMDirTolerance > FTWOTOUCH_MAX_MISSDIR)
    //        {
    //            //CFTwoTouchPublic::ResetSomeMembers();
    //            m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
    //        }
    //    }

    //    g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFTwoTouchPublic::m_curNonMoveTolerance);
    //    g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFTwoTouchPublic::m_curMissMDirTolerance);
    //}while(false);

    //if (m_curState == WGR_FTWOTOUCH_CLOSE_STATE)
    //{//WGR_FTWOTOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CFTwoTouchPublic::ResetSomeMembers();
    //}

    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    ///*if(dir != )
    //{
    //    
    //}*/

    return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFTwoTouchStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
	{
		if(pContext)
		{
			if (WGR_FTWOTOUCH_LISTEN_STATE == m_curState)
			{
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEF_2TOUCHS_PART))
				{
					//此功能未开启，不再继续进行判断计算
					m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
					break;
				}				
			}            
		}
		else
		{
			//功能执行上下文为空，不再继续进行判断计算
			m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
			break;
		}

		if (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Finger)
		{//非手指形状
			CWGRCommonUlity::m_curUnexpectedShapeTimes++;

			g_oWGRTraceLog.AddCurMissShapeTolerance(CWGRCommonUlity::m_curUnexpectedShapeTimes);

			if (CWGRCommonUlity::m_curUnexpectedShapeTimes > FTWOTOUCH_MAX_MISSSHAPETOLLERANCE)
			{
				m_curState = WGR_FTWOTOUCH_CLOSE_STATE;                
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			g_oWGRTraceLog.AddCurMissShapeTolerance(CWGRCommonUlity::m_curUnexpectedShapeTimes);
		}    

		if(!CWGRCommonUlity::IsInputTouchsEqual(2))
		{//光点数不等于2
			CWGRCommonUlity::m_curUnexpectedTouchsTimes++;

			g_oWGRTraceLog.AddCurMissTouchCountTolerance(CWGRCommonUlity::m_curUnexpectedTouchsTimes);

			if (CWGRCommonUlity::m_curUnexpectedTouchsTimes > FTWOTOUCH_MAX_MISS2TOUCHCOUNTTOLERANCE)
			{
				m_curState = WGR_FTWOTOUCH_CLOSE_STATE;                                
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedTouchsTimes = 0;
			g_oWGRTraceLog.AddCurMissTouchCountTolerance(CWGRCommonUlity::m_curUnexpectedTouchsTimes);
		}

		g_oWGRTraceLog.AddEnterLess(!CWGRCommonUlity::m_havedEnteredTheSumSM);
		if (!CWGRCommonUlity::m_havedEnteredTheSumSM)
		{
			CWGRCommonUlity::ResetSomeMembers();
			CWGRCommonUlity::InitWhenFirstEnter(pContext);
		}

		E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(4, 4, 2, 2, 2, 2);

        switch(dir)
        {
        case E_WGR_MDIR_None:
            {//未移动
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

                CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;   

                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FTWOTOUCH_MAX_NONMOVETOLERANCE)
                {
                    m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
                }

            }
            break;

        case E_WGR_MDIR_LHoriz:          
        case E_WGR_MDIR_RHoriz:
            {
                m_curState = WGR_FHMOUSEWHEEL_LISTEN_STATE;
                m_smHMouseWheel.StartRun();

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();

                CWGRCommonUlity::ResetSomeMembers();
            }
            break;

        case E_WGR_MDIR_UVertic:
        case E_WGR_MDIR_DVertic:
            {
                m_curState = WGR_FVMOUSEWHEEL_LISTEN_STATE;
                m_smVMouseWheel.StartRun();

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();

                CWGRCommonUlity::ResetSomeMembers();
            }
            break;

        case E_WGR_MDIR_Move:        
            {                
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FTWOTOUCH_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();
            }
            break;

        default:
            {
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                if(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FTWOTOUCH_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    //CFTwoTouchPublic::ResetSomeMembers();
                    m_curState = WGR_FTWOTOUCH_CLOSE_STATE;
                }
            }
            break;

        }

		/*g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFTwoTouchPublic::m_curNonMoveTolerance);
		g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFTwoTouchPublic::m_curMissMDirTolerance);*/

	} while (false);

	if (m_curState == WGR_FTWOTOUCH_CLOSE_STATE)
	{//WGR_FTWOTOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
		m_curState = WGR_IDLE_STATE;
		CWGRCommonUlity::ResetSomeMembers();
	}

	g_oWGRTraceLog.AddSateToCovert(m_curState);

	return m_curState;
}