#include "StdAfx.h"
#include "FMiniCurWndStateMachine.h"
#include "FThree2FiveTouchPublic.h"
#include "..\..\GlobalLog.h"

#include <atlbase.h>

//
#define FMINICURWND_MAX_MISSSHAPETOLLERANCE                   1
#define FMINICURWND_MAX_MISSTOUCHCOUNTTOLERANCE               15
#define FMINICURWND_MAX_NONMOVETOLERANCE                      40
#define FMINICURWND_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define FMINICURWND_MAX_UNEXPECTEDKNOWNMOV                    7
#define FMINICURWND_MINCONMOVES_NEED                          2

#define FMINICURWND_CLEAR_MCMN_NOMOV                          5
#define FMINICURWND_CLEAR_MCMN_UNEXPKNMOV                     10
#define FMINICURWND_CLEAR_MCMN_UNEXPNKNMOV                    5
//******

CFMiniCurWndStateMachine::CFMiniCurWndStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
    m_nConMoves = 0;
}

CFMiniCurWndStateMachine::~CFMiniCurWndStateMachine(void)
{
}

bool CFMiniCurWndStateMachine::StartRun()
{
    m_curState = WGR_FMINICURWND_LISTEN_STATE;
    m_nConMoves = 0;
    return true;
}

bool CFMiniCurWndStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    m_nConMoves = 0;
    return true;
}

IWGRStateMachine *CFMiniCurWndStateMachine::GetSubMachineCurStateContained()
{
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFMiniCurWndStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    //do 
    //{
    //    if(pContext)
    //    {//
    //        if (WGR_FMINICURWND_LISTEN_STATE == m_curState)
    //        {
    //            if (false == pContext->Get_TriggerEvent_Permission(E_TEF_35TOUCHS_MINICURWND, CFThree2FiveTouchPublic::m_ptSartMid))
    //            {
    //                //此功能已经禁止，无需继续判断
    //                m_curState = WGR_FMINICURWND_CLOSE_STATE;

    //                break;
    //            }
    //        }
    //                    
    //        //pContext->TriggerHIDUp(&(CFThree2FiveTouchPublic::m_ptSartMid));          
    //    }
    //    else
    //    {//执行上下文为空，无需再继续判断
    //        m_curState = WGR_FMINICURWND_CLOSE_STATE;
    //        break;
    //    }


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

    //    g_oWGRTraceLog.AddGestureShape(gestureShape);

    //    if (gestureShape != E_WGR_GShape_Finger)
    //    {//非手指形状
    //        CFThree2FiveTouchPublic::m_curMissShapeTolerance++;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);

    //        if (CFThree2FiveTouchPublic::m_curMissShapeTolerance > FMINICURWND_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_FMINICURWND_CLOSE_STATE;                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFThree2FiveTouchPublic::m_curMissShapeTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);
    //    }

    //    if(nCount < 3 || nCount > 5)
    //    {//光点数不等于2
    //        CFThree2FiveTouchPublic::m_curMissTouchCountTolerance++;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFThree2FiveTouchPublic::m_curMissTouchCountTolerance);

    //        if (CFThree2FiveTouchPublic::m_curMissTouchCountTolerance > FMINICURWND_MAX_MISSTOUCHCOUNTTOLERANCE)
    //        {
    //            m_curState = WGR_FMINICURWND_CLOSE_STATE;                
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

    //            m_nConMoves = 0;

    //            if (CFThree2FiveTouchPublic::m_curNonMoveTolerance > FMINICURWND_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_FMINICURWND_CLOSE_STATE;
    //            }
    //        }

    //        break;

    //    case E_WGR_MDIR_DVertic:
    //        {
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            m_nConMoves++;

    //            if (m_curState == WGR_FMINICURWND_LISTEN_STATE && m_nConMoves >= FMINICURWND_MINCONMOVES_NEED)
    //            {//在监听或者保持状态下，竖直移动时，转化到执行状态
    //                m_curState = WGR_FMINICURWND_KEEP_STATE;
    //                m_nConMoves = 0;

    //                if(pContext)
    //                {
    //                    //触发ALT + Right Arrow事件
    //                    ////AtlTrace("\r\n Trigger MiniCurWindow Event!");
    //                    pContext->TriggerMiniCurWindow(CFThree2FiveTouchPublic::m_ptSartMid);  
    //                    //AtlTrace("\r\n Trigger MiniCurWindow Event!");
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

    //            if(CFThree2FiveTouchPublic::m_curMissMDirTolerance > FMINICURWND_MAX_MISSDIR)
    //            {
    //                //CFTwoTouchPublic::ResetSomeMembers();
    //                m_curState = WGR_FMINICURWND_CLOSE_STATE;
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //} while (false);

    //if (m_curState == WGR_FMINICURWND_CLOSE_STATE)
    //{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CFThree2FiveTouchPublic::ResetSomeMembers();
    //}

    //if (m_curState == WGR_FMINICURWND_EXE_STATE)
    //{//WGR_FMINICURWND_EXE_STATE也是过渡状态，不持久性停留，执行之后就转化为WGR_FMINICURWND_KEEP_STATE
    // //WGR_FMINICURWND_KEEP_STATE保持到直到结束
    // //最小化窗口只执行一次，执行后，之所以不直接结束该手势识别，是因为用户不能即时停止向下的手势，如果窗口A最小化后，
    // //由于用户向下的手势动作未停止，可能继续最小化窗口A的下层窗口B
    //    m_curState = WGR_FMINICURWND_KEEP_STATE;      
    //}

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFThree2FiveTouchPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFThree2FiveTouchPublic::m_curMissMDirTolerance);
    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFMiniCurWndStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
	{
		if(pContext)
		{//
			if (WGR_FMINICURWND_LISTEN_STATE == m_curState)
			{			
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEF_35TOUCHS_MINICURWND))
				{
					//此功能已经禁止，无需继续判断
					m_curState = WGR_FMINICURWND_CLOSE_STATE;

					break;
				}
			}


			// pContext->TriggerHIDUp(&(CFThree2FiveTouchPublic::m_ptSartMid));
		}
		else
		{//执行上下文为空，无需再继续判断
			m_curState = WGR_FMINICURWND_CLOSE_STATE;
			break;
		}

        g_oWGRTraceLog.AddGestureShape(CWGRCommonUlity::m_curGestureShape);
		
		if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Unknown)
		{//非手指形状
			CWGRCommonUlity::m_curUnexpectedShapeTimes++;
			//g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);

			if (CWGRCommonUlity::m_curUnexpectedShapeTimes > FMINICURWND_MAX_MISSSHAPETOLLERANCE)
			{
				m_curState = WGR_FMINICURWND_CLOSE_STATE;                
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			//g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);
		}
        
		if(!CWGRCommonUlity::IsInputTouchsBetween(3, 5) && m_curState == WGR_FMINICURWND_LISTEN_STATE)
		{//光点数不等于2
			CWGRCommonUlity::m_curUnexpectedShapeTimes++;
			//g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFThree2FiveTouchPublic::m_curMissTouchCountTolerance);

			if (CWGRCommonUlity::m_curUnexpectedShapeTimes > FMINICURWND_MAX_MISSTOUCHCOUNTTOLERANCE)
			{
				m_curState = WGR_FMINICURWND_CLOSE_STATE;                
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			//g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFThree2FiveTouchPublic::m_curMissTouchCountTolerance);
		}

		E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;
        if (m_curState == WGR_FMINICURWND_KEEP_STATE)
        {
            dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(4, 4, 1, 5, 3, 2);
        }
        else
        {
            dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(4, 4, 3, 5, 3, 2);
        }

		switch(dir)
		{
		case E_WGR_MDIR_None:
			{//未移动
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

				CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;       				

                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FMINICURWND_CLEAR_MCMN_NOMOV)
                {
                    m_nConMoves = 0;
                }
                else if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FMINICURWND_MAX_NONMOVETOLERANCE)
				{
					m_curState = WGR_FMINICURWND_CLOSE_STATE;
				}
			}

			break;

		case E_WGR_MDIR_DVertic:
			{				
				m_nConMoves++;

                if (m_curState == WGR_FMINICURWND_LISTEN_STATE)
                {
                    m_curState = WGR_FMINICURWND_KEEP_STATE;
                }

				if (m_curState == WGR_FMINICURWND_KEEP_STATE && m_nConMoves >= FMINICURWND_MINCONMOVES_NEED)
				{//在监听或者保持状态下，水平移动时，转化到执行状态
					m_curState = WGR_FMINICURWND_EXE_STATE;

					m_nConMoves = 0;

					pContext->TriggerMiniCurWindow(CWGRCommonUlity::m_curPtMiddle);

                    AtlTrace(_T("Mini Current Window Event had triggered!!\r\n"));
				}

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

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FMINICURWND_CLEAR_MCMN_UNEXPKNMOV)
                {
                    m_nConMoves = 0;
                } 
                else if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FMINICURWND_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_FMINICURWND_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();                

                break;
            }

		default:
			{
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                m_nConMoves = 0;

                if(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FMINICURWND_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    //CFTwoTouchPublic::ResetSomeMembers();
                    m_curState = WGR_FMINICURWND_CLOSE_STATE;
                }
			}
			break;
		}

		//g_oWGRTraceLog.AddMoveDirection(dir);
	} while (false);

	if (m_curState == WGR_FMINICURWND_CLOSE_STATE)
	{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
		m_curState = WGR_IDLE_STATE;
		m_nConMoves = 0;
		CWGRCommonUlity::ResetSomeMembers();
	}

	//if (m_curState == WGR_FMINICURWND_EXE_STATE)
	//{//WGR_FMAXICURWND_EXE_STATE也是过渡状态，不持久性停留，执行之后就转化为WGR_FMAXICURWND_KEEP_STATE
	//	//WGR_FMAXICURWND_KEEP_STATE保持到直到结束
	//	//最大化窗口只执行一次，执行后，之所以不直接结束该手势识别，是因为用户不能即时停止向下的手势，如果窗口A最小化后，
	//	//由于用户向下的手势动作未停止，可能继续最小化窗口A的下层窗口B
	//	m_curState = WGR_FMINICURWND_KEEP_STATE;
	//}

	/*g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFThree2FiveTouchPublic::m_curNonMoveTolerance);
	g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFThree2FiveTouchPublic::m_curMissMDirTolerance);
	g_oWGRTraceLog.AddSateToCovert(m_curState);*/

	return m_curState;
}

