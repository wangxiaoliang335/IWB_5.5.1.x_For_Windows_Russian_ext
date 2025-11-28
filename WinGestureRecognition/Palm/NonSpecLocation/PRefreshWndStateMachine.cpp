#include "StdAfx.h"
#include "PRefreshWndStateMachine.h"
#include "PNonSpecLocPublic.h"
#include "..\..\GlobalLog.h"

//
#define PREFWND_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define PREFWND_MAX_UNEXPECTEDKNOWNMOV                    7
#define PREFWND_MAX_MISSSHAPETOLLERANCE                   15
#define PREFWND_MAX_MISSSHAPE_UNKNOWN_TOLLERANCE          1
#define PREFWND_MAX_NONMOVETOLERANCE                      20

#define PREFWND_CLEAR_MCMN_NOMOV                          5
#define PREFWND_CLEAR_MCMN_UNEXPKNMOV                     5
#define PREFWND_CLEAR_MCMN_UNEXPNKNMOV                    5
//******

CPRefreshWndStateMachine::CPRefreshWndStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CPRefreshWndStateMachine::~CPRefreshWndStateMachine(void)
{
}

bool CPRefreshWndStateMachine::StartRun()
{
    m_curState = WGR_PREFWND_LISTEN_STATE;
	m_lastMovDir = E_WGR_MDIR_Unknown;
    return true;
}

bool CPRefreshWndStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    return false;
}

IWGRStateMachine *CPRefreshWndStateMachine::GetSubMachineCurStateContained()
{    

    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CPRefreshWndStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{    
    //do 
    //{
    //    if(pContext)
    //    {
    //        if (WGR_PREFWND_LISTEN_STATE == m_curState)
    //        {
    //            if (false == pContext->Get_TriggerEvent_Permission(E_TEP_NSPLOC_REFRESHWND, CPNonSpecLocPublic::m_ptStart))
    //            {//此功能禁止，无需继续进行判断
    //                m_curState = WGR_PREFWND_CLOSE_STATE;
    //                break;
    //            }
    //        }            
    //        

    //        //{
    //        //    //弹起鼠标左键            
    //        //    pContext->TriggerHIDUp(&(CPNonSpecLocPublic::m_ptStart));
    //        //}
    //    }
    //    else
    //    {
    //        //此功能禁止，无需继续进行判断
    //        m_curState = WGR_PREFWND_CLOSE_STATE;
    //        break;
    //    }

    //    g_oWGRTraceLog.AddGestureShape(gestureShape);
    //   
    //    if (gestureShape != E_WGR_GShape_Palm)
    //    {
    //        CPNonSpecLocPublic::m_curMissShapeTolerance++;

    //        if (CPNonSpecLocPublic::m_curMissShapeTolerance > PREFWND_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_PREFWND_CLOSE_STATE;
    //            CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //    }

    //    //光斑的实际大小和标准大小
    //    g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
    //    g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);


    //    E_WGR_MOVEDIR dir = CPNonSpecLocPublic::CalcMoveDirection(aryLightSpots, nCount, 4, 4);

    //    /*if (CPNonSpecLocPublic::m_ptCur.x - CPNonSpecLocPublic::m_ptStart.x > 6)
    //    {
    //        int b = 12;
    //        b = 13;
    //    }*/

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {
    //            CPNonSpecLocPublic::m_curMissMDirTolerance = 0;
    //            
    //            CPNonSpecLocPublic::m_curNonMoveTolerance++;
    //            if (CPNonSpecLocPublic::m_curNonMoveTolerance > PREFWND_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_PREFWND_CLOSE_STATE;
    //            }
    //        }

    //        break;

    //    case E_WGR_MDIR_Move:
    //        {
    //            int curXMove = CPNonSpecLocPublic::m_ptCur.x - CPNonSpecLocPublic::m_ptStart.x;
    //            int curYMove = CPNonSpecLocPublic::m_ptCur.y - CPNonSpecLocPublic::m_ptStart.y;
    //            if(WGR_INT32_OPPSIGN(curXMove, CPNonSpecLocPublic::m_nXDisplacement)/* || WGR_INT32_OPPSIGN(curYMove, CPNonSpecLocPublic::m_nYDisplacement)*/)
    //            {
    //                //无论x方向还是y方向，只要有异向运动，就认为在进行来回移动
    //                if (m_curState == WGR_PREFWND_LISTEN_STATE)
    //                {
    //                    m_curState = WGR_PREFWND_SECMOVE_STATE;
    //                }else if (m_curState == WGR_PREFWND_SECMOVE_STATE)
    //                {
    //                    m_curState = WGR_PREFWND_THDMOVE_STATE;

    //                    if (pContext)
    //                    {//执行刷新窗口
    //                        pContext->TriggerRefreshWnd(CPNonSpecLocPublic::m_ptStart);
    //                    }
    //                }                     


    //            }

    //            CPNonSpecLocPublic::m_ptStart = CPNonSpecLocPublic::m_ptCur;
    //            CPNonSpecLocPublic::m_nXDisplacement = curXMove;
    //            CPNonSpecLocPublic::m_nYDisplacement = curYMove;               
    //        }
    //        break;

    //    default:
    //        {
    //            CPNonSpecLocPublic::m_curNonMoveTolerance = 0;

    //            CPNonSpecLocPublic::m_curMissMDirTolerance++;
    //            if (CPNonSpecLocPublic::m_curMissMDirTolerance > PREFWND_MAX_MISSDIR)
    //            {
    //                m_curState = WGR_IDLE_STATE;
    //                CPNonSpecLocPublic::ResetSomeMembers();
    //            }
    //        }

    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //} while (false);

    //if (m_curState == WGR_PREFWND_CLOSE_STATE)
    //{
    //    m_curState = WGR_IDLE_STATE;
    //    CPNonSpecLocPublic::ResetSomeMembers();
    //}

    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;
	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CPRefreshWndStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
    {
        if(pContext)
        {
            if (WGR_PREFWND_LISTEN_STATE == m_curState)
            {
                if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEP_NSPLOC_REFRESHWND))
                {//此功能禁止，无需继续进行判断
                    m_curState = WGR_PREFWND_CLOSE_STATE;
                    break;
                }
            }                                    
        }
        else
        {
            //此功能禁止，无需继续进行判断
            m_curState = WGR_PREFWND_CLOSE_STATE;
            break;
        }

        //g_oWGRTraceLog.AddGestureShape(gestureShape);
       
        if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Unknown)
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;

            CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes++;            

            if (CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes > PREFWND_MAX_MISSSHAPE_UNKNOWN_TOLLERANCE)
            {
                m_curState = WGR_PREFWND_CLOSE_STATE;
                CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes = 0;
            }

            break;
        }
        else if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Finger)
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes++;

            if (CWGRCommonUlity::m_curUnexpectedShapeTimes > PREFWND_MAX_MISSSHAPETOLLERANCE)
            {
                 m_curState = WGR_PREFWND_CLOSE_STATE;

                 break;
            }
        }
        else
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes = 0;
        }        


        E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(3, 3, 1, 10, 1, 5);        

        switch(dir)
        {
        case E_WGR_MDIR_None:
            {
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;
                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > PREFWND_MAX_NONMOVETOLERANCE)
                {
                    m_curState = WGR_PREFWND_CLOSE_STATE;
                }
            }

            break;

        case E_WGR_MDIR_LHoriz:
		case E_WGR_MDIR_RHoriz:
            {
                if (m_lastMovDir == E_WGR_MDIR_Unknown) m_lastMovDir = dir;

                if(m_lastMovDir != dir)
                {
                    //无论x方向还是y方向，只要有异向运动，就认为在进行来回移动
                    if (m_curState == WGR_PREFWND_LISTEN_STATE)
                    {
                        m_curState = WGR_PREFWND_SECMOVE_STATE;
                    }else if (m_curState == WGR_PREFWND_SECMOVE_STATE)
                    {
                        m_curState = WGR_PREFWND_THDMOVE_STATE;

                        if (pContext)
                        {//执行刷新窗口
                            pContext->TriggerRefreshWnd(CWGRCommonUlity::m_curPtMiddle);
                        }
                    }                     


                }

               m_lastMovDir = dir;

			   CWGRCommonUlity::ResetDislaymentInLastTouchMap();
			   CWGRCommonUlity::ResetSomeMembers();
            }
            break;

        case E_WGR_MDIR_Move:    
        case E_WGR_MDIR_DVertic:
        case E_WGR_MDIR_UVertic:
            {
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;                
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;                

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > PREFWND_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_IDLE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();   
            }
            break;

        default:
            {				
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                if (CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > PREFWND_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    m_curState = WGR_IDLE_STATE;
                    CWGRCommonUlity::ResetSomeMembers();
                }
            }

            break;
        }

        //g_oWGRTraceLog.AddMoveDirection(dir);
    } while (false);

    if (m_curState == WGR_PREFWND_CLOSE_STATE)
    {
        m_curState = WGR_IDLE_STATE;
        CWGRCommonUlity::ResetSomeMembers();
    }

    g_oWGRTraceLog.AddSateToCovert(m_curState);

    return m_curState;
}
