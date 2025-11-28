#include "StdAfx.h"
#include "FMiniAllWndStateMachine.h"
#include "FSixMoreTouchPublic.h"
#include "..\..\GlobalLog.h"

//
#define FMINIALLWND_MAX_MISSSHAPETOLLERANCE                   1
#define FMINIALLWND_MAX_MISSTOUCHCOUNTTOLERANCE               10
#define FMINIALLWND_MAX_NONMOVETOLERANCE                      30
#define FMINIALLWND_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define FMINIALLWND_MAX_UNEXPECTEDKNOWNMOV                    7
#define FMINIALLWND_MAX_CONMOVES_NEED                         2

#define FMINIALLWND_CLEAR_MCMN_NOMOV                          5
#define FMINIALLWND_CLEAR_MCMN_UNEXPKNMOV                     5
#define FMINIALLWND_CLEAR_MCMN_UNEXPNKNMOV                    5
//******

CFMiniAllWndStateMachine::CFMiniAllWndStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
    m_nConMoves = 0;
}

CFMiniAllWndStateMachine::~CFMiniAllWndStateMachine(void)
{
}

bool CFMiniAllWndStateMachine::StartRun()
{
    m_curState = WGR_FMINIALLWND_LISTEN_STATE;
    CWGRCommonUlity::ResetSomeMembers();
    m_nConMoves = 0;
    return true;
}

bool CFMiniAllWndStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    m_nConMoves = 0;
    return true;
}

IWGRStateMachine *CFMiniAllWndStateMachine::GetSubMachineCurStateContained()
{
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFMiniAllWndStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
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

    //        if (CFSixMoreTouchPublic::m_curMissShapeTolerance > FMINIALLWND_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_FMINIALLWND_CLOSE_STATE;
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

    //        if (CFSixMoreTouchPublic::m_curMissTouchCountTolerance > FMINIALLWND_MAX_MISSTOUCHCOUNTTOLERANCE)
    //        {
    //            m_curState = WGR_FMINIALLWND_CLOSE_STATE;                
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFSixMoreTouchPublic::m_curMissTouchCountTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);
    //    }

    //    //if(pContext)
    //    //{//
    //    //    //pContext->TriggerMouseLeftButtonDown(&(CFThree2FiveTouchPublic::m_ptSartMid));
    //    //    pContext->TriggerHIDUp(&(CFSixMoreTouchPublic::m_ptSartMid));
    //    //}

    //    E_WGR_MOVEDIR dir = CFSixMoreTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 6, 6);

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {//未移动
    //            CFSixMoreTouchPublic::m_curMissMDirTolerance = 0;

    //            CFSixMoreTouchPublic::m_curNonMoveTolerance++;

    //            m_nConMoves = 0;

    //            if (CFSixMoreTouchPublic::m_curNonMoveTolerance > FMINIALLWND_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_FMINIALLWND_CLOSE_STATE;
    //            }
    //        }

    //        break;

    //    case E_WGR_MDIR_DVertic:
    //        {
    //            CFSixMoreTouchPublic::m_curMissMDirTolerance = 0;
    //            CFSixMoreTouchPublic::m_curNonMoveTolerance = 0;

    //            m_nConMoves++;

    //            if (m_curState == WGR_FMINIALLWND_LISTEN_STATE)
    //            {//在监听或者保持状态下，水平移动时，转化到执行状态
    //                if(m_nConMoves >= FMINIALLWND_MAX_CONMOVES_NEED)
    //                {
    //                    m_curState = WGR_FMINIALLWND_EXE_STATE;  

    //                    if(pContext)
    //                    {
    //                        //触发显示桌面事件
    //                        pContext->TriggerDisplayDesktop(CFSixMoreTouchPublic::m_ptSartMid);      
    //                    }

    //                    m_nConMoves = 0;

    //                }                    
    //                
    //            }  

    //            CFSixMoreTouchPublic::m_ptSartMid = CFSixMoreTouchPublic::m_ptLastMid;
    //            CFSixMoreTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    default:
    //        {
    //            CFSixMoreTouchPublic::m_curNonMoveTolerance = 0;

    //            m_nConMoves = 0;

    //            CFSixMoreTouchPublic::m_curMissMDirTolerance++;

    //            if(CFSixMoreTouchPublic::m_curMissMDirTolerance > FMINIALLWND_MAX_MISSDIR)
    //            {
    //                //CFTwoTouchPublic::ResetSomeMembers();
    //                m_curState = WGR_FMINIALLWND_CLOSE_STATE;
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //} while (false);

    //if (m_curState == WGR_FMINIALLWND_CLOSE_STATE)
    //{//WGR_FMINIALLWND_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    m_nConMoves = 0;
    //    CFSixMoreTouchPublic::ResetSomeMembers();
    //}

    //if (m_curState == WGR_FMINIALLWND_EXE_STATE)
    //{//WGR_FMINIALLWND_EXE_STATE也是过渡状态，不持久性停留，执行之后就转化为WGR_FMINIALLWND_KEEP_STATE
    //    //WGR_FMINIALLWND_KEEP_STATE保持到直到结束        
    //    m_curState = WGR_FMINIALLWND_KEEP_STATE;
    //}

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFSixMoreTouchPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFSixMoreTouchPublic::m_curMissMDirTolerance);
    //g_oWGRTraceLog.AddSateToCovert(m_curState);    

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFMiniAllWndStateMachine::TransitState(IWGRUtilContext *pContext)
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


		if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Unknown)
		{//非手指形状
			CWGRCommonUlity::m_curUnexpectedShapeTimes++;
			//g_oWGRTraceLog.AddCurMissShapeTolerance(CFSixMoreTouchPublic::m_curMissShapeTolerance);            

			if (CWGRCommonUlity::m_curUnexpectedShapeTimes > FMINIALLWND_MAX_MISSSHAPETOLLERANCE)
			{
				m_curState = WGR_FMINIALLWND_CLOSE_STATE;
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
			//g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);

			if (CWGRCommonUlity::m_curUnexpectedTouchsTimes > FMINIALLWND_MAX_MISSTOUCHCOUNTTOLERANCE)
			{
				m_curState = WGR_FMINIALLWND_CLOSE_STATE;                
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedTouchsTimes = 0;
			//g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFSixMoreTouchPublic::m_curMissTouchCountTolerance);
		}

		//if(pContext)
		//{//
		//    //pContext->TriggerMouseLeftButtonDown(&(CFThree2FiveTouchPublic::m_ptSartMid));
		//    pContext->TriggerHIDUp(&(CFSixMoreTouchPublic::m_ptSartMid));
		//}

		E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;
        if (m_curState == WGR_FMINIALLWND_KEEP_STATE)
        {
            dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(4, 4, 6, 10, 3, 2);
        } 
        else
        {
            dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(6, 6, 6, 10, 3, 2);
        }

		switch(dir)
		{
		case E_WGR_MDIR_None:
			{//未移动
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

                CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;       

                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FMINIALLWND_CLEAR_MCMN_NOMOV)
                {
                    m_nConMoves = 0;
                }				

				if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FMINIALLWND_MAX_NONMOVETOLERANCE)
				{
					m_curState = WGR_FMINIALLWND_CLOSE_STATE;
				}
			}

			break;

		case E_WGR_MDIR_DVertic:
			{				
				m_nConMoves++;

                if (m_curState == WGR_FMINIALLWND_LISTEN_STATE)
                {
                    m_curState = WGR_FMINIALLWND_KEEP_STATE;  
                }

				if (m_curState == WGR_FMINIALLWND_KEEP_STATE)
				{//在监听或者保持状态下，水平移动时，转化到执行状态
					if(m_nConMoves >= FMINIALLWND_MAX_CONMOVES_NEED)
					{
						m_curState = WGR_FMINIALLWND_EXE_STATE;  

						if(pContext)
						{
							//触发显示桌面事件
							pContext->TriggerDisplayDesktop(CWGRCommonUlity::m_curPtMiddle);      
						}

						m_nConMoves = 0;

					}                    

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

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FMINIALLWND_CLEAR_MCMN_UNEXPKNMOV)
                {
                    m_nConMoves = 0;
                }				


                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FMINIALLWND_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_FMINIALLWND_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();                
            }

            break;

		default:
			{
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

				m_nConMoves = 0;

				if(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FMINIALLWND_MAX_UNEXPECTEDUNKNOWNMOV)
				{
					//CFTwoTouchPublic::ResetSomeMembers();
					m_curState = WGR_FMINIALLWND_CLOSE_STATE;
				}
			}
			break;
		}

		//g_oWGRTraceLog.AddMoveDirection(dir);
	} while (false);

	if (m_curState == WGR_FMINIALLWND_CLOSE_STATE)
	{//WGR_FMINIALLWND_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
		m_curState = WGR_IDLE_STATE;
		m_nConMoves = 0;
		CWGRCommonUlity::ResetSomeMembers();
	}

	//if (m_curState == WGR_FMINIALLWND_EXE_STATE)
	//{//WGR_FMINIALLWND_EXE_STATE也是过渡状态，不持久性停留，执行之后就转化为WGR_FMINIALLWND_KEEP_STATE
	//	//WGR_FMINIALLWND_KEEP_STATE保持到直到结束        
	//	m_curState = WGR_FMINIALLWND_KEEP_STATE;
	//}

	/*g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFSixMoreTouchPublic::m_curNonMoveTolerance);
	g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFSixMoreTouchPublic::m_curMissMDirTolerance);
	g_oWGRTraceLog.AddSateToCovert(m_curState); */   

	return m_curState;
}
