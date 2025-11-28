#include "StdAfx.h"
#include "FSwitchWndStateMachine.h"
#include "FThree2FiveTouchPublic.h"
#include "..\..\GlobalLog.h"

//
#define FSWHWND_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define FSWHWND_MAX_UNEXPECTEDKNOWNMOV                    15
#define FSWHWND_MAX_MISSTOUCHCOUNTTOLERANCE               5
#define FSWHWND_MAX_MISSSHAPETOLLERANCE                   7
#define FSWHWND_MAX_NONMOVETOLERANCE                      150
#define FSWHWND_MINCONMOVES_NEED                          8        //至少连续移动多少次，才进行一次窗口切换
//******

CFSwitchWndStateMachine::CFSwitchWndStateMachine(void)
{
    m_nConLMoves = m_nConRMoves = 0;
    m_curState = WGR_IDLE_STATE;
}

CFSwitchWndStateMachine::~CFSwitchWndStateMachine(void)
{
}

bool CFSwitchWndStateMachine::StartRun()
{
    m_curState = WGR_FSWHWND_LISTEN_STATE;
    m_nConLMoves = m_nConRMoves = 0;
    return true;
}

bool CFSwitchWndStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    m_nConLMoves = m_nConRMoves = 0;
    return false;
}

IWGRStateMachine *CFSwitchWndStateMachine::GetSubMachineCurStateContained()
{
    /*if(m_curState > WGR_PSVDOC_STATE_BEGIN && m_curState < WGR_PSVDOC_STATE_END)
    {
        return (IWGRStateMachine *)&m_smSVDoc;
    }*/

    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFSwitchWndStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{     
    //do 
    //{
    //    if(pContext)
    //    {//
    //        if (WGR_FSWHWND_LISTEN_STATE == m_curState)
    //        {
    //            if (false == pContext->Get_TriggerEvent_Permission(E_TEF_35TOUCHS_SWHWND, CFThree2FiveTouchPublic::m_ptSartMid))
    //            {
    //                //此功能已经禁止，无需继续判断
    //                m_curState = WGR_FSWHWND_CLOSE_STATE;

    //                break;
    //            }
    //        }
    //        
    //        //pContext->TriggerHIDUp(&(CFThree2FiveTouchPublic::m_ptSartMid));            
    //    }
    //    else
    //    {//执行上下文为空，无需再继续判断
    //        m_curState = WGR_FSWHWND_CLOSE_STATE;
    //        break;
    //    }

    //    g_oWGRTraceLog.AddSpotCount(nCount);

    //    if (gestureShape == E_WGR_GShape_Unknown)
    //    {
    //        CFThree2FiveTouchPublic::m_curMissShapeTolerance++;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);

    //        if (CFThree2FiveTouchPublic::m_curMissShapeTolerance > FSWHWND_MAX_MISSSHAPETOLLERANCE)
    //        {                    
    //            if (m_curState == WGR_FSWHWND_EXE_STATE)
    //            {
    //                if(pContext)
    //                {
    //                    pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_ENDMODE);
    //                }
    //            }

    //            if (m_curState == WGR_FSWHWND_KEEP_STATE)
    //            {
    //                if(pContext)
    //                {
    //                    pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_CANCLEMODE);
    //                }
    //            }

    //            m_curState = WGR_FSWHWND_CLOSE_STATE; 
    //            CFThree2FiveTouchPublic::m_curMissShapeTolerance = 0;
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFThree2FiveTouchPublic::m_curMissShapeTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);
    //    }

    //    E_WGR_MOVEDIR dir;

    //    //////////////////////////////////////////////////////////////////////////
    //    //
    //    if(m_curState == WGR_FSWHWND_EXE_STATE)
    //    {
    //        POINT curMid;

    //        if (CFThree2FiveTouchPublic::CalcMiddlePoint(aryLightSpots, nCount, curMid))
    //        {
    //            int nXShift = curMid.x - CFThree2FiveTouchPublic::m_ptLastMid.x;
    //            g_oWGRTraceLog.AddCurrentMiddlePoint(curMid);
    //            g_oWGRTraceLog.AddLastMiddlePoint(CFThree2FiveTouchPublic::m_ptLastMid);
    //            g_oWGRTraceLog.AddTimesCMIncreased(m_nConLMoves);
    //            g_oWGRTraceLog.AddTimesCMDecreased(m_nConRMoves);

    //            if (WGR_INT32_ABS(nXShift) > 2)
    //            {
    //                if (nXShift < 0)
    //                {
    //                    dir = E_WGR_MDIR_LHoriz;
    //                }

    //                if (nXShift > 0)
    //                {
    //                    dir = E_WGR_MDIR_RHoriz;
    //                }

    //                CFThree2FiveTouchPublic::m_ptLastMid = curMid;
    //            }
    //            else
    //            {
    //                dir = E_WGR_MDIR_None;
    //            }                

    //        }                        
    //    }
    //    else
    //    {
    //        dir = CFThree2FiveTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 3, 3);
    //    }

    //    //////////////////////////////////////////////////////////////////////////

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {//未移动
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;

    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance++;

    //            if (CFThree2FiveTouchPublic::m_curNonMoveTolerance > FSWHWND_MAX_NONMOVETOLERANCE)
    //            {
    //                if (m_nConLMoves < 2 || m_nConRMoves < 2)
    //                {
    //                    //
    //                    if (m_curState == WGR_FSWHWND_EXE_STATE)
    //                    {
    //                        if (pContext)
    //                        {
    //                            pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_ENDMODE);
    //                        }
    //                    }

    //                    if (m_curState == WGR_FSWHWND_KEEP_STATE)
    //                    {
    //                        if(pContext)
    //                        {
    //                            pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_CANCLEMODE);
    //                        }
    //                    }

    //                    m_curState = WGR_FSWHWND_CLOSE_STATE;                        
    //                }                    

    //                m_nConLMoves--;
    //                m_nConRMoves--;

    //                if (m_nConLMoves < 0)
    //                {
    //                    m_nConLMoves = 0;
    //                }

    //                if (m_nConRMoves < 0)
    //                {
    //                    m_nConRMoves = 0;
    //                }
    //            }                
    //        }            

    //        break;

    //    case E_WGR_MDIR_LHoriz:
    //        {                
    //            m_nConRMoves = 0;
    //            m_nConLMoves++;

    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            if (m_curState == WGR_FSWHWND_LISTEN_STATE)
    //            {
    //                if(m_nConLMoves >= 2)                    
    //                {
    //                    m_curState = WGR_FSWHWND_EXE_STATE;

    //                    if(pContext)
    //                    {
    //                        pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_STARTMODE);
    //                    }

    //                    m_nConLMoves = 0; 
    //                }
    //            }
    //            else if (m_curState == WGR_FSWHWND_KEEP_STATE || m_curState == WGR_FSWHWND_EXE_STATE)
    //            {
    //                m_curState = WGR_FSWHWND_EXE_STATE;

    //                if (m_nConLMoves >= FSWHWND_MINCONMOVES_NEED)
    //                {
    //                    if (pContext)
    //                    {
    //                        pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_EXELMODE);                       
    //                    }

    //                    m_nConLMoves = 0;  
    //                }                                        
    //            }

    //            CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
    //            CFThree2FiveTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    case E_WGR_MDIR_RHoriz:
    //        {               
    //            m_nConLMoves = 0;
    //            m_nConRMoves++;

    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;


    //            if (m_curState == WGR_FSWHWND_LISTEN_STATE)
    //            {
    //                if(m_nConRMoves > 2)                    
    //                {
    //                    m_curState = WGR_FSWHWND_KEEP_STATE;

    //                    if(pContext)
    //                    {
    //                        pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_STARTMODE);
    //                    }

    //                    m_nConRMoves = 0; 
    //                }                  
    //            }
    //            else if (m_curState == WGR_FSWHWND_KEEP_STATE || m_curState == WGR_FSWHWND_EXE_STATE)
    //            {
    //                m_curState = WGR_FSWHWND_EXE_STATE;

    //                if (m_nConRMoves >= FSWHWND_MINCONMOVES_NEED)
    //                {
    //                    if (pContext)
    //                    {
    //                        pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_EXERMODE);                       
    //                    }

    //                    m_nConRMoves = 0;    
    //                }                                      
    //            }

    //            CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
    //            CFThree2FiveTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    default:
    //        {
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance++;

    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            if (CFThree2FiveTouchPublic::m_curMissMDirTolerance > FSWHWND_MAX_MISSDIR)
    //            {
    //                if (m_nConLMoves < 2 || m_nConRMoves < 2)
    //                {
    //                    if (m_curState == WGR_FSWHWND_KEEP_STATE)
    //                    {
    //                        if(pContext)
    //                        {
    //                            pContext->TriggerSwitchWindowEvent(CFThree2FiveTouchPublic::m_ptSartMid, E_SWHWND_CANCLEMODE);
    //                        }

    //                        m_curState = WGR_FSWHWND_CLOSE_STATE;      
    //                    }                                         
    //                }                                       
    //            }

    //            m_nConLMoves--;
    //            m_nConRMoves--;

    //            if (m_nConLMoves < 0)
    //            {
    //                m_nConLMoves = 0;
    //            }

    //            if (m_nConRMoves < 0)
    //            {
    //                m_nConRMoves = 0;
    //            }

    //        }            
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);


    //} while (false);

    //if (m_curState == WGR_FSWHWND_CLOSE_STATE)
    //{//WGR_FSWHWND_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    m_nConLMoves = 0;
    //    m_nConRMoves = 0;
    //    CFThree2FiveTouchPublic::ResetSomeMembers();
    //}    

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFThree2FiveTouchPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFThree2FiveTouchPublic::m_curMissMDirTolerance);
    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE  CFSwitchWndStateMachine::TransitState(IWGRUtilContext *pContext)
{
    /*CPerfDetector perf(_T("CFSwitchWndStateMachine::TransitState:"));*/
	do 
	{
		if(pContext)
		{//
			if (WGR_FSWHWND_LISTEN_STATE == m_curState)
			{
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEF_35TOUCHS_SWHWND))
				{
					//此功能已经禁止，无需继续判断
					m_curState = WGR_FSWHWND_CLOSE_STATE;

					break;
				}
			}

			//pContext->TriggerHIDUp(&(CFThree2FiveTouchPublic::m_ptSartMid));            
		}
		else
		{//执行上下文为空，无需再继续判断
			m_curState = WGR_FSWHWND_CLOSE_STATE;
			break;
		}

		//g_oWGRTraceLog.AddSpotCount(nCount);

		if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Unknown)
		{
			CWGRCommonUlity::m_curUnexpectedShapeTimes++;
			g_oWGRTraceLog.AddCurMissShapeTolerance(CWGRCommonUlity::m_curUnexpectedShapeTimes);

			if (CWGRCommonUlity::m_curUnexpectedShapeTimes > FSWHWND_MAX_MISSSHAPETOLLERANCE)
			{                    
				if (m_curState == WGR_FSWHWND_EXE_STATE)
				{
					if(pContext)
					{
						pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_ENDMODE);
					}
				}

				if (m_curState == WGR_FSWHWND_KEEP_STATE)
				{
					if(pContext)
					{
						pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_CANCLEMODE);
					}
				}

				m_curState = WGR_FSWHWND_CLOSE_STATE; 
				CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			}

			break;
		}
		else
		{
			CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
			//g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);
		}

		E_WGR_MOVEDIR dir;

		//////////////////////////////////////////////////////////////////////////
		//
		if(m_curState == WGR_FSWHWND_EXE_STATE)
		{			
			dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(3, 4, 1, 10, 1);			            			                   
		}
		else
		{
			dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(3, 4, 3, 5, 1);
		}

		//////////////////////////////////////////////////////////////////////////

		switch(dir)
		{
		case E_WGR_MDIR_None:
			{//未移动
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

				CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;  

				if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FSWHWND_MAX_NONMOVETOLERANCE)
				{
					if (m_nConLMoves < 2 || m_nConRMoves < 2)
					{
						//
						if (m_curState == WGR_FSWHWND_EXE_STATE)
						{
							if (pContext)
							{
								pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_ENDMODE);
							}
						}

						if (m_curState == WGR_FSWHWND_KEEP_STATE)
						{
							if(pContext)
							{
								pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_CANCLEMODE);
							}
						}

						m_curState = WGR_FSWHWND_CLOSE_STATE;                        
					}                    

					m_nConLMoves--;
					m_nConRMoves--;

					if (m_nConLMoves < 0)
					{
						m_nConLMoves = 0;
					}

					if (m_nConRMoves < 0)
					{
						m_nConRMoves = 0;
					}
				}                
			}            

			break;

		case E_WGR_MDIR_LHoriz:
			{                
				m_nConRMoves = 0;
				m_nConLMoves++;				

				if (m_curState == WGR_FSWHWND_LISTEN_STATE)
				{
					if(m_nConLMoves >= 2)                    
					{
						m_curState = WGR_FSWHWND_EXE_STATE;

						if(pContext)
						{
							pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_STARTMODE);
						}

						m_nConLMoves = 0; 
					}
				}
				else if (m_curState == WGR_FSWHWND_KEEP_STATE || m_curState == WGR_FSWHWND_EXE_STATE)
				{
					m_curState = WGR_FSWHWND_EXE_STATE;

					if (m_nConLMoves >= FSWHWND_MINCONMOVES_NEED)
					{
						if (pContext)
						{
							pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_EXELMODE);                       
						}

						m_nConLMoves = 0;  
					}                                        
				}

				CWGRCommonUlity::ResetDislaymentInLastTouchMap();
				CWGRCommonUlity::ResetSomeMembers();
			}

			break;

		case E_WGR_MDIR_RHoriz:
			{               
				m_nConLMoves = 0;
				m_nConRMoves++;				

				if (m_curState == WGR_FSWHWND_LISTEN_STATE)
				{
					if(m_nConRMoves > 2)                    
					{
						m_curState = WGR_FSWHWND_KEEP_STATE;

						if(pContext)
						{
							pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_STARTMODE);
						}

						m_nConRMoves = 0; 
					}                  
				}
				else if (m_curState == WGR_FSWHWND_KEEP_STATE || m_curState == WGR_FSWHWND_EXE_STATE)
				{
					m_curState = WGR_FSWHWND_EXE_STATE;

					if (m_nConRMoves >= FSWHWND_MINCONMOVES_NEED)
					{
						if (pContext)
						{
							pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_EXERMODE);                       
						}

						m_nConRMoves = 0;    
					}                                      
				}

				CWGRCommonUlity::ResetDislaymentInLastTouchMap();
				CWGRCommonUlity::ResetSomeMembers();
			}

			break;

        case E_WGR_MDIR_Move:
        case E_WGR_MDIR_UVertic:
        case E_WGR_MDIR_DVertic:
            {
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;                
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;                

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FSWHWND_MAX_UNEXPECTEDKNOWNMOV)
                {
                    /*if (m_nConLMoves < 2 || m_nConRMoves < 2)
                    {
                        if (m_curState == WGR_FSWHWND_KEEP_STATE)
                        {
                            if(pContext)
                            {
                                pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_CANCLEMODE);
                            }

                            m_curState = WGR_FSWHWND_CLOSE_STATE;      
                        }   

                        if (m_curState == WGR_FSWHWND_EXE_STATE)
                        {
                            if (pContext)
                            {
                                pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_ENDMODE);
                            }
                        }
                    } 

                    m_nConLMoves--;
                    m_nConRMoves--;

                    if (m_nConLMoves < 0)
                    {
                        m_nConLMoves = 0;
                    }

                    if (m_nConRMoves < 0)
                    {
                        m_nConRMoves = 0;
                    }*/

                    /*if (m_curState == WGR_FSWHWND_KEEP_STATE)
                    {
                        if(pContext)
                        {
                            pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_CANCLEMODE);
                        }

                        m_curState = WGR_FSWHWND_CLOSE_STATE;      
                    }   

                    if (m_curState == WGR_FSWHWND_EXE_STATE)
                    {
                        if (pContext)
                        {
                            pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_ENDMODE);
                        }
                    }

                    m_nConLMoves = 0;
                    m_nConRMoves = 0;*/
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();                
               
            }

            break;

		default:
			{
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

				if (CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FSWHWND_MAX_UNEXPECTEDUNKNOWNMOV)
				{
					if (m_nConLMoves < 2 || m_nConRMoves < 2)
					{
						if (m_curState == WGR_FSWHWND_KEEP_STATE)
						{
							if(pContext)
							{
								pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_CANCLEMODE);
							}

							m_curState = WGR_FSWHWND_CLOSE_STATE;      
						} 

                        if (m_curState == WGR_FSWHWND_EXE_STATE)
                        {
                            if (pContext)
                            {
                                pContext->TriggerSwitchWindowEvent(CWGRCommonUlity::m_curPtMiddle, E_SWHWND_ENDMODE);
                            }

                            m_curState = WGR_FSWHWND_CLOSE_STATE;  
                        }                        
					} 

					m_nConLMoves--;
					m_nConRMoves--;

					if (m_nConLMoves < 0)
					{
						m_nConLMoves = 0;
					}

					if (m_nConRMoves < 0)
					{
						m_nConRMoves = 0;
					}
				}				

			}            
			break;
		}

		//g_oWGRTraceLog.AddMoveDirection(dir);


	} while (false);

	if (m_curState == WGR_FSWHWND_CLOSE_STATE)
	{//WGR_FSWHWND_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
		m_curState = WGR_IDLE_STATE;
		m_nConLMoves = 0;
		m_nConRMoves = 0;
		CWGRCommonUlity::ResetSomeMembers();
	}    

	/*g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFThree2FiveTouchPublic::m_curNonMoveTolerance);
	g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFThree2FiveTouchPublic::m_curMissMDirTolerance);*/
	g_oWGRTraceLog.AddSateToCovert(m_curState);

	return m_curState;
}