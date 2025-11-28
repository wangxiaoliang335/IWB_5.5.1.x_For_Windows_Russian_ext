#include "StdAfx.h"
#include "PNonSpeccLocStateMachine.h"
#include "PNonSpecLocPublic.h"
#include "..\..\GlobalLog.h"

//
#define PNONSPECLOC_MAX_UNEXPECTEDUNKNOWNMOV                  2
#define PNONSPECLOC_MAX_UNEXPECTEDKNOWNMOV                    10
#define PNONSPECLOC_MAX_MISSSHAPETOLLERANCE                   5
#define PNONSPECLOC_MAX_MISSSHAPE_UNKWONSHAPE_TOLLERANCE      1
#define PNONSPECLOC_MAX_NONMOVETOLERANCE                      12

#define FNONSPECLOC_CLEAR_MCMN_NOMOV                          5
#define FNONSPECLOC_CLEAR_MCMN_UNEXPKNMOV                     5
#define FNONSPECLOC_CLEAR_MCMN_UNEXPNKNMOV                    5
//******

#define PMaxWInterMoveWnd                                  200 //窗口移动最大等待间隔

CPNonSpecLocStateMachine::CPNonSpecLocStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CPNonSpecLocStateMachine::~CPNonSpecLocStateMachine(void)
{
}

bool CPNonSpecLocStateMachine::StartRun()
{
    m_curState = WGR_PNONSPECLOC_LISTEN_STATE;
    m_nCurConMoves = 0;
    CWGRCommonUlity::ResetSomeMembers();

	m_dwNonMoveStartTime = GetTickCount();

    return true;
}

bool CPNonSpecLocStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    m_nCurConMoves = 0;
    return false;
}

IWGRStateMachine *CPNonSpecLocStateMachine::GetSubMachineCurStateContained()
{
    if(m_curState > WGR_PREFWND_STATE_BEGIN && m_curState < WGR_PREFWND_STATE_END)
    {
        return (IWGRStateMachine *)&m_smRefreshWnd;
    }

    if (m_curState > WGR_PMOVWND_STATE_BEGIN && m_curState < WGR_PMOVWND_STATE_END)
    {
        return (IWGRStateMachine *)&m_smMOVWnd;
    }

    if (m_curState > WGR_PCLSCWND_STATE_BEGIN && m_curState < WGR_PCLSCWND_STATE_END)
    {
        return (IWGRStateMachine *)&m_smCloseCurWnd;
    }

    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CPNonSpecLocStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{   
    //do 
    //{
    //    if (pContext)
    //    {
    //        if (m_curState == WGR_PNONSPECLOC_LISTEN_STATE)
    //        {
    //            if (nCount > 0)
    //            {
    //                if (false == pContext->Get_TriggerEvent_Permission(E_TEP_NSPLOC_PART, aryLightSpots[0].ptPosInScreen))
    //                {
    //                    //此功能已经禁止，无需继续判断
    //                    m_curState = WGR_IDLE_STATE;

    //                    break;
    //                }
    //            }
    //            /*else
    //            {
    //                m_curState = WGR_IDLE_STATE;
    //                break;
    //            }*/
    //        }            
    //    }
    //    else
    //    {
    //        //执行上下文为空，无需再继续判断
    //        m_curState = WGR_IDLE_STATE;
    //        break;
    //    }
    //    g_oWGRTraceLog.AddGestureShape(gestureShape);        

    //    if (gestureShape != E_WGR_GShape_Palm)
    //    {
    //        CPNonSpecLocPublic::m_curMissShapeTolerance++;

    //        if (CPNonSpecLocPublic::m_curMissShapeTolerance > PNONSPECLOC_MAX_MISSSHAPETOLLERANCE)
    //        {                
    //            if(gestureShape != E_WGR_GShape_Finger)
    //            {//过渡到双击关闭窗口状态
    //                m_curState = WGR_PCLSCWND_LISTEN_STATE;
    //                m_smCloseCurWnd.StartRun();
    //                CPNonSpecLocPublic::ResetSomeMembers();
    //                CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //                
    //                CPNonSpecLocPublic::m_nXDisplacement = CPNonSpecLocPublic::m_nYDisplacement = 0;                    
    //                CPNonSpecLocPublic::m_ptStart = CPNonSpecLocPublic::m_ptLast;
    //            }
    //            else
    //            {
    //                m_curState = WGR_IDLE_STATE;
    //                CPNonSpecLocPublic::ResetSomeMembers();
    //            }
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //    }

    //    if (CPNonSpecLocPublic::m_bEnterLessThanOne)
    //    {
    //        CPNonSpecLocPublic::ResetSomeMembers();
    //        CPNonSpecLocPublic::UpdateStartTime();

    //        CPNonSpecLocPublic::m_nXDisplacement = CPNonSpecLocPublic::m_nYDisplacement = 0;
    //        CPNonSpecLocPublic::m_nLastMass = aryLightSpots[0].mass;
    //        CPNonSpecLocPublic::m_nLastStdMass = aryLightSpots[0].lStdSpotAreaInVideo;
    //        CPNonSpecLocPublic::m_ptLast = CPNonSpecLocPublic::m_ptStart = aryLightSpots[0].ptPosInScreen;
    //        CPNonSpecLocPublic::m_bEnterLessThanOne = false;

    //        if(pContext)
    //        {//                                
    //            //pContext->TriggerMouseLeftButtonDown(&(CPNonSpecLocPublic::m_ptStart));
    //            pContext->TriggerHIDUp(&(CPNonSpecLocPublic::m_ptStart));                
    //        }
    //    }                

    //    //光斑的实际大小和标准大小
    //    g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
    //    g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);

    //    E_WGR_MOVEDIR dir = CPNonSpecLocPublic::CalcMoveDirection(aryLightSpots, nCount, 7, 7);

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {
    //            DWORD dwCurTime = GetTickCount();

    //            CPNonSpecLocPublic::m_curMissMDirTolerance = 0;
    //            m_nCurConMoves = 0;

    //            if (dwCurTime - CPNonSpecLocPublic::m_dwNonMoveStartTime >= PMaxWInterMoveWnd)
    //            {//转为窗口移动状态
    //                m_curState = WGR_PMOVWND_LISTEN_STATE;
    //                m_smMOVWnd.StartRun();
    //                CPNonSpecLocPublic::ResetSomeMembers();       

    //              /*  CPNonSpecLocPublic::ResetSomeMembers();
    //                m_curState = WGR_IDLE_STATE;*/
    //            }
    //        }
    //        break;

    //    case E_WGR_MDIR_Move:
    //        {  
    //            m_nCurConMoves++;

    //            if (m_nCurConMoves > 2)
    //            {
    //                m_nCurConMoves = 0;
    //                m_curState = WGR_PREFWND_LISTEN_STATE;
    //                m_smRefreshWnd.StartRun();
    //                CPNonSpecLocPublic::ResetSomeMembers();
    //            }

    //            CPNonSpecLocPublic::m_nXDisplacement = CPNonSpecLocPublic::m_ptCur.x - CPNonSpecLocPublic::m_ptStart.x;
    //            CPNonSpecLocPublic::m_nYDisplacement = CPNonSpecLocPublic::m_ptCur.y - CPNonSpecLocPublic::m_ptStart.y;
    //            CPNonSpecLocPublic::m_ptStart = CPNonSpecLocPublic::m_ptCur;
    //        }
    //        break;

    //    default:
    //        {
    //            CPNonSpecLocPublic::m_curMissMDirTolerance++;
    //            m_nCurConMoves = 0;
    //            if (CPNonSpecLocPublic::m_curMissMDirTolerance > PNONSPECLOC_MAX_MISSDIR)
    //            {
    //                m_curState = WGR_IDLE_STATE;
    //                CPNonSpecLocPublic::ResetSomeMembers();
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //    //switch()
    //} while (false);

    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;
	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CPNonSpecLocStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
    {
        if (pContext)
        {
            if (m_curState == WGR_PNONSPECLOC_LISTEN_STATE)
            {
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEP_NSPLOC_PART))
				{
					//此功能已经禁止，无需继续判断
					m_curState = WGR_IDLE_STATE;

					break;
				}
            }            
        }
        else
        {
            //执行上下文为空，无需再继续判断
            m_curState = WGR_IDLE_STATE;
            break;
        }
        
		g_oWGRTraceLog.AddGestureShape(CWGRCommonUlity::m_curGestureShape);        

        if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Unknown)
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes++;

            if (CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes > PNONSPECLOC_MAX_MISSSHAPE_UNKWONSHAPE_TOLLERANCE)
            {
                m_curState = WGR_IDLE_STATE;
                CWGRCommonUlity::ResetSomeMembers();
            }

            break;
        }
        else if(CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Finger)
        {
            CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes = 0;

            CWGRCommonUlity::m_curUnexpectedShapeTimes++;
            if (CWGRCommonUlity::m_curUnexpectedShapeTimes > PNONSPECLOC_MAX_MISSSHAPETOLLERANCE)
            {
                if (m_nCurConMoves < 2)
                {
                    m_curState = WGR_IDLE_STATE;
                    CWGRCommonUlity::ResetSomeMembers();

                    break;
                }
                else
                {
                    m_nCurConMoves--;
                }
            }            
            
        }
        else
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes = 0;
        }

        if (!CWGRCommonUlity::m_havedEnteredTheSumSM)
        {
            CWGRCommonUlity::ResetSomeMembers();  
			CWGRCommonUlity::InitWhenFirstEnter(pContext);
			m_dwNonMoveStartTime = GetTickCount();
        }                

        ////光斑的实际大小和标准大小
        //g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
        //g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);

        E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(8, 8, 1, 10, 1, 4);

        switch(dir)
        {
        case E_WGR_MDIR_None:
            {
                DWORD dwCurTime = GetTickCount();

                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes ++;
                
                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FNONSPECLOC_CLEAR_MCMN_NOMOV)
                {
                    m_nCurConMoves = 0;
                }

                if (dwCurTime - m_dwNonMoveStartTime >= PMaxWInterMoveWnd)
                {//转为窗口移动状态
                    m_curState = WGR_PMOVWND_LISTEN_STATE;
                    m_smMOVWnd.StartRun();
                    CWGRCommonUlity::ResetSomeMembers();       

                  /*  CPNonSpecLocPublic::ResetSomeMembers();
                    m_curState = WGR_IDLE_STATE;*/
                }
            }
            break;

		//case E_WGR_MDIR_Move:
		case E_WGR_MDIR_LHoriz:
		case E_WGR_MDIR_RHoriz:		
            /*{  
                m_nCurConMoves++;

                if (m_nCurConMoves > 2)
                {
                    m_nCurConMoves = 0;
                    m_curState = WGR_PREFWND_LISTEN_STATE;
                    m_smRefreshWnd.StartRun();                   

                    CWGRCommonUlity::ResetSomeMembers();
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();				

				m_dwNonMoveStartTime = GetTickCount();
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;
            }
            break;*/

        case E_WGR_MDIR_Move:    
        case E_WGR_MDIR_DVertic:
        case E_WGR_MDIR_UVertic:
            {
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;                
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;                

                /*if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FNONSPECLOC_CLEAR_MCMN_UNEXPKNMOV)
                {
                    m_nCurConMoves = 0;
                }*/

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > PNONSPECLOC_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_IDLE_STATE;
                    CWGRCommonUlity::ResetSomeMembers();
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();                                                                    
                m_dwNonMoveStartTime = GetTickCount();
            }
            break;

        default:
            {
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                m_nCurConMoves = 0;
                if (CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > PNONSPECLOC_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    m_curState = WGR_IDLE_STATE;
                    CWGRCommonUlity::ResetSomeMembers();
                }

				m_dwNonMoveStartTime = GetTickCount();
            }
            break;
        }

        //g_oWGRTraceLog.AddMoveDirection(dir);
        //switch()
    } while (false);

    g_oWGRTraceLog.AddSateToCovert(m_curState);

    return m_curState;
}