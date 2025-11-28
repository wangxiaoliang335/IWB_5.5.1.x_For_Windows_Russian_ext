#include "StdAfx.h"
#include "FSixMoreTouchStateMachine.h"
#include "FSixMoreTouchPublic.h"
#include "..\..\GlobalLog.h"

//
#define FSIXMORETOUCH_MAX_MISSSHAPETOLLERANCE                   1
#define FSIXMORETOUCH_MAX_MISSTOUCHCOUNTTOLERANCE               5
#define FSIXMORETOUCH_MAX_NONMOVETOLERANCE                      30
#define FSIXMORETOUCH_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define FSIXMORETOUCH_MAX_UNEXPECTEDKNOWNMOV                    7
//******

CFSixMoreTouchStateMachine::CFSixMoreTouchStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CFSixMoreTouchStateMachine::~CFSixMoreTouchStateMachine(void)
{
}

bool CFSixMoreTouchStateMachine::StartRun()
{
    m_curState = WGR_FSIXMORETOUCH_LISTEN_STATE;

    CWGRCommonUlity::ResetSomeMembers();
    return true;
}

bool CFSixMoreTouchStateMachine::StopRun()
{
    m_curState = WGR_FTHREE2FIVETOUCH_LISTEN_STATE;

    CWGRCommonUlity::ResetSomeMembers();
    return true;
}

IWGRStateMachine *CFSixMoreTouchStateMachine::GetSubMachineCurStateContained()
{
    if (m_curState > WGR_FMINIALLWND_STATE_BEGIN && m_curState < WGR_FMINIALLWND_STATE_END)
    {
        return (IWGRStateMachine *)&m_smMiniAllWnd;
    }

    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFSixMoreTouchStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    //do 
    //{       

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
    //        CFSixMoreTouchPublic::m_curMissShapeTolerance++;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFSixMoreTouchPublic::m_curMissShapeTolerance);

    //        if (CFSixMoreTouchPublic::m_curMissShapeTolerance > FSIXMORETOUCH_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFSixMoreTouchPublic::m_curMissShapeTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFSixMoreTouchPublic::m_curMissShapeTolerance);
    //    }

    //    if(nCount < 6)
    //    {//光点数不满足条件
    //        CFSixMoreTouchPublic::m_curMissTouchCountTolerance++;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);

    //        if (CFSixMoreTouchPublic::m_curMissTouchCountTolerance > FSIXMORETOUCH_MAX_MISSTOUCHCOUNTTOLERANCE)
    //        {
    //            m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFSixMoreTouchPublic::m_curMissTouchCountTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);
    //    }

    //    g_oWGRTraceLog.AddEnterLess(CFSixMoreTouchPublic::m_bEnterLessThanOne);
    //    if (CFSixMoreTouchPublic::m_bEnterLessThanOne)
    //    {            

    //        //重置参数
    //        CFSixMoreTouchPublic::ResetSomeMembers();           
    //        CFSixMoreTouchPublic::m_nXDisplacement = 0;
    //        CFSixMoreTouchPublic::m_nYDisplacement = 0;

    //        CFSixMoreTouchPublic::m_bEnterLessThanOne = false;

    //        //初始化m_ptLastMid
    //        if(!CFSixMoreTouchPublic::CalcMiddlePoint(aryLightSpots, nCount, CFSixMoreTouchPublic::m_ptLastMid))
    //        {
    //            CFSixMoreTouchPublic::m_bEnterLessThanOne = true;
    //            break;
    //        }            

    //        CFSixMoreTouchPublic::m_ptSartMid = CFSixMoreTouchPublic::m_ptLastMid;

    //        if(pContext)
    //        {//
    //            //pContext->TriggerMouseLeftButtonDown(&(CFThree2FiveTouchPublic::m_ptSartMid));
    //            pContext->TriggerHIDUp(&(CFSixMoreTouchPublic::m_ptSartMid));
    //        }
    //    }        

    //    E_WGR_MOVEDIR dir = CFSixMoreTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 6, 6);

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {//未移动
    //            CFSixMoreTouchPublic::m_curMissMDirTolerance = 0;

    //            CFSixMoreTouchPublic::m_curNonMoveTolerance++;

    //            if (CFSixMoreTouchPublic::m_curNonMoveTolerance > FSIXMORETOUCH_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;
    //            }
    //        }

    //        break;

    //    case E_WGR_MDIR_DVertic:
    //        {
    //            CFSixMoreTouchPublic::m_curMissMDirTolerance = 0;
    //            CFSixMoreTouchPublic::m_curNonMoveTolerance = 0;

    //            m_curState = WGR_FMINIALLWND_LISTEN_STATE;
    //            m_smMiniAllWnd.StartRun();
    //            //if(pContext)
    //            //{
    //            //    //触发显示桌面事件
    //            //    pContext->TriggerDisplayDesktop(CFSixMoreTouchPublic::m_ptSartMid);     
    //            //}


    //            CFSixMoreTouchPublic::m_ptSartMid = CFSixMoreTouchPublic::m_ptLastMid;
    //            CFSixMoreTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    default:
    //        {
    //            CFSixMoreTouchPublic::m_curNonMoveTolerance = 0;

    //            CFSixMoreTouchPublic::m_curMissMDirTolerance++;

    //            if(CFSixMoreTouchPublic::m_curMissMDirTolerance > FSIXMORETOUCH_MAX_MISSDIR)
    //            {
    //                //CFTwoTouchPublic::ResetSomeMembers();
    //                m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //} while (false);

    //if (m_curState == WGR_FSIXMORETOUCH_CLOSE_STATE)
    //{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CFSixMoreTouchPublic::ResetSomeMembers();
    //}    

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFSixMoreTouchStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
	{       

		//g_oWGRTraceLog.AddSpotCount(nCount);

		//if (g_oWGRTraceLog.IsLogHasStarted())
		//{
		//	//
		//	POINT pt[16];
		//	for (int i = 0; i < nCount; i++)
		//	{
		//		pt[i] = aryLightSpots[i].ptPosInScreen;
		//	}

		//	g_oWGRTraceLog.AddSpotPos(pt, nCount);
		//}

		if(pContext)
		{//
			if (WGR_FSIXMORETOUCH_LISTEN_STATE == m_curState)
			{
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEF_6MTOUCHS_PART))            
				{
					//此功能已经禁止，无需继续判断
					m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;

					break;
				}
			}            
		}
		else
		{//执行上下文为空，无需再继续判断
			m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;
			break;
		}

		if (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Finger)
		{//非手指形状
			CWGRCommonUlity::m_curUnexpectedShapeTimes++;
			//g_oWGRTraceLog.AddCurMissShapeTolerance(CFSixMoreTouchPublic::m_curMissShapeTolerance);

			if (CWGRCommonUlity::m_curUnexpectedShapeTimes > FSIXMORETOUCH_MAX_MISSSHAPETOLLERANCE)
			{
				m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;  
				CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			//g_oWGRTraceLog.AddCurMissShapeTolerance(CFSixMoreTouchPublic::m_curMissShapeTolerance);
		}

		if(!CWGRCommonUlity::IsInputTouchsBetween(6, 10))
		{//光点数不满足条件
			CWGRCommonUlity::m_curUnexpectedTouchsTimes++;
			//g_oWGRTraceLog.AddCurMissTouchCountTolerance(CWGRCommonUlity::m_curUnexpectedTouchsTimes);

			if (CWGRCommonUlity::m_curUnexpectedTouchsTimes > FSIXMORETOUCH_MAX_MISSTOUCHCOUNTTOLERANCE)
			{
				m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;                
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedTouchsTimes = 0;
			//g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);
		}

		//g_oWGRTraceLog.AddEnterLess(CFSixMoreTouchPublic::m_bEnterLessThanOne);
		if (!CWGRCommonUlity::m_havedEnteredTheSumSM)
		{            

			//重置参数
			CWGRCommonUlity::ResetSomeMembers();     
			CWGRCommonUlity::InitWhenFirstEnter(pContext);
		}        

		E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(6, 6, 6, 10);

		switch(dir)
		{
		case E_WGR_MDIR_None:
			{//未移动
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
				CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;

				if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FSIXMORETOUCH_MAX_NONMOVETOLERANCE)
				{
					m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;
				}
			}

			break;

		case E_WGR_MDIR_DVertic:
			{
				m_curState = WGR_FMINIALLWND_LISTEN_STATE;
				m_smMiniAllWnd.StartRun();
				
				CWGRCommonUlity::ResetDislaymentInLastTouchMap();
				CWGRCommonUlity::ResetSomeMembers();
			}

			break;

        case E_WGR_MDIR_Move:
        case E_WGR_MDIR_LHoriz:
        case E_WGR_MDIR_RHoriz:
        case E_WGR_MDIR_UVertic:
            {
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;                
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;                

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FSIXMORETOUCH_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();                
            }

            break;

		default:
			{
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

				if(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FSIXMORETOUCH_MAX_UNEXPECTEDUNKNOWNMOV)
				{
					//CFTwoTouchPublic::ResetSomeMembers();
					m_curState = WGR_FSIXMORETOUCH_CLOSE_STATE;
				}
			}
			break;
		}

		//g_oWGRTraceLog.AddMoveDirection(dir);
	} while (false);

	if (m_curState == WGR_FSIXMORETOUCH_CLOSE_STATE)
	{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
		m_curState = WGR_IDLE_STATE;
		CWGRCommonUlity::ResetSomeMembers();
	}    

	return m_curState;
}
