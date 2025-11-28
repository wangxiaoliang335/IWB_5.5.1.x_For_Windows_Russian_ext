#include "StdAfx.h"
#include "PMoveWndStateMachine.h"
#include "PNonSpecLocPublic.h"
#include "..\..\GlobalLog.h"

//
#define PMOVWND_MAX_UNEXPECTEDUNKNOWNMOV                  5
#define PMOVWND_MAX_UNEXPECTEDKNOWNMOV                    7
#define PMOVWND_MAX_MISSSHAPETOLLERANCE                   2
#define PMOVWND_STARTHINT_MAX_NONMOVETOLERANCE            10        //开启切换窗口提示
#define PMOVWND_STOPHINT_MAX_NONMOVETOLERANCE             120        //结束切换窗口提示，又成为进入文档保存状态的阈值
#define PMOVWND_STOPSW_MAX_NONMOVETOLERANCE               60        //结束切换窗口,它与PMOVWND_STOPHINT_MAX_NONMOVETOLERANCE区别是，只有进入窗口执行状态后，它才起作用

#define PMOVWND_MINCONMOVES_NEED                          12         //至少连续移动多少次，才进行一次窗口切换
//******

CPMoveWndStateMachine::CPMoveWndStateMachine(void)
{
    m_nConMoves = 0;
    m_curState = WGR_IDLE_STATE;
}

CPMoveWndStateMachine::~CPMoveWndStateMachine(void)
{
}

bool CPMoveWndStateMachine::StartRun()
{
    m_curState = WGR_PMOVWND_LISTEN_STATE;
    m_nConMoves = 0;
    return true;
}

bool CPMoveWndStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    m_nConMoves = 0;
    return false;
}

IWGRStateMachine *CPMoveWndStateMachine::GetSubMachineCurStateContained()
{
    /*if(m_curState > WGR_PSVDOC_STATE_BEGIN && m_curState < WGR_PSVDOC_STATE_END)
    {
        return (IWGRStateMachine *)&m_smSVDoc;
    }*/

    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CPMoveWndStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{   
    //do 
    //{
    //    if (pContext)
    //    {
    //        if (WGR_PMOVWND_LISTEN_STATE == m_curState)
    //        {
    //            if (false == pContext->Get_TriggerEvent_Permission(E_TEP_NSPLOC_MOVWND, CPNonSpecLocPublic::m_ptStart))
    //            {
    //                //此功能已经禁止，无需继续判断
    //                m_curState = WGR_PMOVWND_CLOSE_STATE;
    //                break;
    //            }
    //        }            
    //    }
    //    else
    //    {//执行上下文为空，无需再继续判断
    //        m_curState = WGR_PMOVWND_CLOSE_STATE;
    //        break;
    //    }

    //    g_oWGRTraceLog.AddGestureShape(gestureShape);
    //    
    //    if (gestureShape == E_WGR_GShape_Unknown || (gestureShape == E_WGR_GShape_Finger && m_curState != WGR_PMOVWND_KEEP_STATE && m_curState != WGR_PMOVWND_EXE_STATE))
    //    {
    //        CPNonSpecLocPublic::m_curMissShapeTolerance++;

    //        if (CPNonSpecLocPublic::m_curMissShapeTolerance > PMOVWND_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            //
    //            if (m_curState == WGR_PMOVWND_EXE_STATE || m_curState == WGR_PMOVWND_KEEP_STATE)
    //            {
    //                pContext->TriggerMoveWindowEvent(CPNonSpecLocPublic::m_ptCur, CPNonSpecLocPublic::m_ptLast, E_MOVWND_ENDMODE);
    //            }
    //            m_curState = WGR_PMOVWND_CLOSE_STATE; 
    //            CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
    //    }  

    //    if(!aryLightSpots || nCount <= 0) break;

    //    //光斑的实际大小和标准大小
    //    g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
    //    g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);

    //    E_WGR_MOVEDIR dir = CPNonSpecLocPublic::CalcMoveDirection(aryLightSpots, nCount, 4, 4);

    //    if(m_curState == WGR_PMOVWND_EXE_STATE)
    //    {
    //        pContext->TriggerMoveWindowEvent(CPNonSpecLocPublic::m_ptCur, CPNonSpecLocPublic::m_ptStart, E_MOVWND_EXECMODE);

    //        CPNonSpecLocPublic::m_ptStart = CPNonSpecLocPublic::m_ptCur; 

    //        break;
    //    }

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {
    //            CPNonSpecLocPublic::m_curMissMDirTolerance = 0;
    //            CPNonSpecLocPublic::m_curNonMoveTolerance++;

    //            if (m_curState == WGR_PMOVWND_LISTEN_STATE)
    //            {
    //                m_curState = WGR_PMOVWND_KEEP_STATE;
    //                pContext->TriggerMoveWindowHintEvent(CPNonSpecLocPublic::m_ptCur, true);
    //            }

    //            if(CPNonSpecLocPublic::m_curNonMoveTolerance > PMOVWND_STOPHINT_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_PMOVWND_CLOSE_STATE;
    //                pContext->TriggerMoveWindowHintEvent(CPNonSpecLocPublic::m_ptCur, false);
    //            }
    //        }

    //        break;

    //    case E_WGR_MDIR_Move:
    //        {
    //            CPNonSpecLocPublic::m_curMissMDirTolerance = 0;
    //            CPNonSpecLocPublic::m_curNonMoveTolerance = 0;

    //            if (m_curState == WGR_PMOVWND_KEEP_STATE)
    //            {
    //                m_curState = WGR_PMOVWND_EXE_STATE;
    //                pContext->TriggerMoveWindowEvent(CPNonSpecLocPublic::m_ptCur, CPNonSpecLocPublic::m_ptStart, E_MOVWND_STARTMODE);
    //            }
    //            

    //            CPNonSpecLocPublic::m_ptStart = CPNonSpecLocPublic::m_ptCur;                
    //        }

    //        break;

    //    case E_WGR_MDIR_Unknown:
    //        {
    //            CPNonSpecLocPublic::m_curNonMoveTolerance = 0;
    //            CPNonSpecLocPublic::m_curMissMDirTolerance++;                

    //            if (CPNonSpecLocPublic::m_curMissMDirTolerance > PMOVWND_MAX_MISSDIR)
    //            {
    //                m_curState = WGR_PMOVWND_CLOSE_STATE;
    //                pContext->TriggerMoveWindowEvent(CPNonSpecLocPublic::m_ptCur, CPNonSpecLocPublic::m_ptStart, E_MOVWND_ENDMODE);
    //            }
    //        }

    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //    //switch()
    //} while (false);

    //if (m_curState == WGR_PMOVWND_CLOSE_STATE)
    //{
    //    m_curState = WGR_IDLE_STATE;

    //    /*if (pContext)
    //    {
    //        pContext->TriggerSwitchWindowHintEvent(CPNonSpecLocPublic::m_ptStart, false);
    //    }*/

    //    CPNonSpecLocPublic::ResetSomeMembers();
    //}

    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CPMoveWndStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
    {
        if (pContext)
        {
            if (WGR_PMOVWND_LISTEN_STATE == m_curState)
            {
                if (false ==CWGRCommonUlity::IsTheEventPermit(pContext, E_TEP_NSPLOC_MOVWND))
                {
                    //此功能已经禁止，无需继续判断
                    m_curState = WGR_PMOVWND_CLOSE_STATE;
                    break;
                }
            }            
        }
        else
        {//执行上下文为空，无需再继续判断
            m_curState = WGR_PMOVWND_CLOSE_STATE;
            break;
        }

        g_oWGRTraceLog.AddGestureShape(CWGRCommonUlity::m_curGestureShape);
        
        if (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Unknown || (CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Finger && m_curState != WGR_PMOVWND_KEEP_STATE && m_curState != WGR_PMOVWND_EXE_STATE))
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes++;
            g_oWGRTraceLog.AddCurMissShapeTolerance(CWGRCommonUlity::m_curUnexpectedShapeTimes);

            if (CWGRCommonUlity::m_curUnexpectedShapeTimes > PMOVWND_MAX_MISSSHAPETOLLERANCE)
            {
                //
                if (m_curState == WGR_PMOVWND_EXE_STATE || m_curState == WGR_PMOVWND_KEEP_STATE)
                {
                    pContext->TriggerMoveWindowEvent(CWGRCommonUlity::m_curPtMiddle, m_ptLast, E_MOVWND_ENDMODE);
                }
                m_curState = WGR_PMOVWND_CLOSE_STATE; 
                CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            }

            break;
        }
        else
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            g_oWGRTraceLog.AddCurMissShapeTolerance(CWGRCommonUlity::m_curUnexpectedShapeTimes);
        }          

        ////光斑的实际大小和标准大小
        //g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
        //g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);  

        E_WGR_MOVEDIR dir;
        if (m_curState == WGR_PMOVWND_EXE_STATE)
        {
            dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(3, 3, 1, 10);
        }
        /*else if (m_curState == WGR_PMOVWND_KEEP_STATE)
        {
            dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(5, 5, 1, 10);
        }*/
        else
        {
            dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(8, 8, 1, 10);
        }

        if(m_curState == WGR_PMOVWND_EXE_STATE)
        {
            pContext->TriggerMoveWindowEvent(CWGRCommonUlity::m_curPtMiddle, m_ptLast, E_MOVWND_EXECMODE);

            m_ptLast = CWGRCommonUlity::m_curPtMiddle;

			CWGRCommonUlity::ResetDislaymentInLastTouchMap();

            break;
        }        

        switch(dir)
        {
        case E_WGR_MDIR_None:
            {
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
				CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;

                g_oWGRTraceLog.AddCurMissNonMoveTolerance(CWGRCommonUlity::m_curUnexpectedNonMoveTimes);

                if (m_curState == WGR_PMOVWND_LISTEN_STATE)
                {
                    m_curState = WGR_PMOVWND_KEEP_STATE;
                    pContext->TriggerMoveWindowHintEvent(CWGRCommonUlity::m_curPtMiddle, true);
                }

                if(CWGRCommonUlity::m_curUnexpectedNonMoveTimes > PMOVWND_STOPHINT_MAX_NONMOVETOLERANCE)
                {
                    m_curState = WGR_PMOVWND_CLOSE_STATE;
                    pContext->TriggerMoveWindowHintEvent(CWGRCommonUlity::m_curPtMiddle, false);
                }
            }

            break;

        case E_WGR_MDIR_Move:
		case E_WGR_MDIR_LHoriz:
		case E_WGR_MDIR_RHoriz:
		case E_WGR_MDIR_DVertic:
		case E_WGR_MDIR_UVertic:
            {                
                if (m_curState == WGR_PMOVWND_KEEP_STATE)
                {
                    m_curState = WGR_PMOVWND_EXE_STATE;
                    pContext->TriggerMoveWindowEvent(CWGRCommonUlity::m_curPtMiddle, m_ptLast, E_MOVWND_STARTMODE);
                }
                

                m_ptLast = CWGRCommonUlity::m_curPtMiddle; 
				CWGRCommonUlity::ResetDislaymentInLastTouchMap();
            }

            break;

        case E_WGR_MDIR_Unknown:
            {				
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes  = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance( CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes);

                if (CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > PMOVWND_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    m_curState = WGR_PMOVWND_CLOSE_STATE;
                    pContext->TriggerMoveWindowEvent(CWGRCommonUlity::m_curPtMiddle, m_ptLast, E_MOVWND_ENDMODE);
                }
            }

            break;
        }

        //g_oWGRTraceLog.AddMoveDirection(dir);
        //switch()
    } while (false);

    if (m_curState == WGR_PMOVWND_CLOSE_STATE)
    {
        m_curState = WGR_IDLE_STATE;

        /*if (pContext)
        {
            pContext->TriggerSwitchWindowHintEvent(CPNonSpecLocPublic::m_ptStart, false);
        }*/

        CWGRCommonUlity::ResetSomeMembers();
    }

    g_oWGRTraceLog.AddSateToCovert(m_curState);

    return m_curState;
}