#include "StdAfx.h"
#include "FThree2FiveTouchStateMachine.h"
#include "FThree2FiveTouchPublic.h"
#include "..\..\GlobalLog.h"

//
#define FTHREE2FIVETOUCH_MAX_MISSSHAPETOLLERANCE                   5
#define FTHREE2FIVETOUCH_MAX_MISSTOUCHCOUNTTOLERANCE               3
#define FTHREE2FIVEUCH_MAX_NONMOVETOLERANCE                        30
#define FTHREE2FIVETOUCH_MAX_UNEXPECTEDUNKNOWNMOV                  1
#define FTHREE2FIVETOUCH_MAX_UNEXPECTEDKNOWNMOV                    7
//******

CFThree2FiveTouchStateMachine::CFThree2FiveTouchStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CFThree2FiveTouchStateMachine::~CFThree2FiveTouchStateMachine(void)
{
}

bool CFThree2FiveTouchStateMachine::StartRun()
{
    m_curState = WGR_FTHREE2FIVETOUCH_LISTEN_STATE;
    
    CWGRCommonUlity::ResetSomeMembers();

    return true;
}

bool CFThree2FiveTouchStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;
    return true;
}

IWGRStateMachine *CFThree2FiveTouchStateMachine::GetSubMachineCurStateContained()
{
    if (m_curState > WGR_FMINICURWND_STATE_BEGIN && m_curState < WGR_FMINICURWND_STATE_END)
    {
        return (IWGRStateMachine *)&m_smMiniCurWnd;
    }
    else if (m_curState > WGR_FSWHWND_STATE_BEGIN && m_curState < WGR_FSWHWND_STATE_END)
    {
        return (IWGRStateMachine *)&m_smSWHWnd;
    }
    /*else if (m_curState > WGR_FALTRARROW_STATE_BEGIN && m_curState < WGR_FALTRARROW_STATE_END)
    {
        return (IWGRStateMachine *)&m_smAltRArrow;
    }*/
    /*else if (m_curState > WGR_FDSPDESKTOP_STATE_BEGIN && m_curState < WGR_FDSPDESKTOP_STATE_END)
    {
        return (IWGRStateMachine *)&m_smDspDesktop;
    }*/
    else if (m_curState > WGR_FMAXICURWND_STATE_BEGIN && m_curState < WGR_FMAXICURWND_STATE_END)
    {
        return (IWGRStateMachine *)&m_smMaxiCurWnd;
    }
    else
    {
        return (IWGRStateMachine *)this;
    }
    
}

WGR_STATE_TYPE CFThree2FiveTouchStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    //do 
    //{

    //    if(pContext)
    //    {//
    //        if (WGR_FTHREE2FIVETOUCH_LISTEN_STATE == m_curState)
    //        {
    //            if (nCount > 0)
    //            {
    //                if (false == pContext->Get_TriggerEvent_Permission(E_TEF_35TOUCHS_PART, aryLightSpots[0].ptPosInScreen))            
    //                {
    //                    //此功能已经禁止，无需继续判断
    //                    m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;

    //                    break;
    //                }
    //            }
    //        }            
    //    }
    //    else
    //    {//执行上下文为空，无需再继续判断
    //        m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
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

    //        if (CFThree2FiveTouchPublic::m_curMissShapeTolerance > FTHREE2FIVETOUCH_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;  
    //            CFThree2FiveTouchPublic::m_curMissShapeTolerance = 0;
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFThree2FiveTouchPublic::m_curMissShapeTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFThree2FiveTouchPublic::m_curMissShapeTolerance);
    //    }

    //    if(nCount <= 2 || nCount > 5)
    //    {//光点数不等于2
    //        CFThree2FiveTouchPublic::m_curMissTouchCountTolerance++;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFThree2FiveTouchPublic::m_curMissTouchCountTolerance);

    //        if (CFThree2FiveTouchPublic::m_curMissTouchCountTolerance > FTHREE2FIVETOUCH_MAX_MISSTOUCHCOUNTTOLERANCE)
    //        {
    //            m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE; 
    //            CFThree2FiveTouchPublic::m_curMissTouchCountTolerance = 0;
    //        }

    //        break;
    //    }
    //    else
    //    {
    //        CFThree2FiveTouchPublic::m_curMissTouchCountTolerance = 0;
    //        g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFThree2FiveTouchPublic::m_curMissTouchCountTolerance);
    //    }

    //    g_oWGRTraceLog.AddEnterLess(CFThree2FiveTouchPublic::m_bEnterLessThanOne);
    //    if (CFThree2FiveTouchPublic::m_bEnterLessThanOne)
    //    {            

    //        //重置参数
    //        CFThree2FiveTouchPublic::ResetSomeMembers();
    //        CFThree2FiveTouchPublic::m_nLastCentriMoments = 0;
    //        CFThree2FiveTouchPublic::m_curTimesCMDecreaced = 0;
    //        CFThree2FiveTouchPublic::m_curTimesCMIncreaced = 0;
    //        CFThree2FiveTouchPublic::m_nXDisplacement = 0;
    //        CFThree2FiveTouchPublic::m_nYDisplacement = 0;

    //        CFThree2FiveTouchPublic::m_bEnterLessThanOne = false;

    //        //初始化m_ptLastMid
    //        if(CFThree2FiveTouchPublic::CalcMiddlePoint(aryLightSpots, nCount, CFThree2FiveTouchPublic::m_ptLastMid))
    //        {
    //            ////初始化m_nLastCentriMoments
    //            //if(!CFThree2FiveTouchPublic::CalcCentriMoments(aryLightSpots, nCount, CFThree2FiveTouchPublic::m_ptLastMid, CFThree2FiveTouchPublic::m_nLastCentriMoments))
    //            //{
    //            //    CFThree2FiveTouchPublic::m_bEnterLessThanOne = true;
    //            //    break;
    //            //}
    //        }
    //        else
    //        {
    //            CFThree2FiveTouchPublic::m_bEnterLessThanOne = true;
    //            break;
    //        }

    //        CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;

    //        if(pContext)
    //        {//
    //            //pContext->TriggerMouseLeftButtonDown(&(CFThree2FiveTouchPublic::m_ptSartMid));
    //            pContext->TriggerHIDUp(&(CFThree2FiveTouchPublic::m_ptSartMid));
    //        }

    //    }
    //    
    //    E_WGR_MOVEDIR dir = CFThree2FiveTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 6, 6);

    //    switch(dir)
    //    {
    //    case E_WGR_MDIR_None:
    //        {//未移动
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;

    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance++;

    //            if (CFThree2FiveTouchPublic::m_curNonMoveTolerance > FTHREE2FIVEUCH_MAX_NONMOVETOLERANCE)
    //            {
    //                m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
    //            }
    //        }

    //        break;

    //    case E_WGR_MDIR_LHoriz:            
    //    case E_WGR_MDIR_RHoriz:
    //        {
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            m_curState = WGR_FSWHWND_LISTEN_STATE;
    //            m_smSWHWnd.StartRun();

    //            CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
    //            CFThree2FiveTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    case E_WGR_MDIR_DVertic:
    //        {
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            m_curState = WGR_FMINICURWND_LISTEN_STATE;
    //            m_smMiniCurWnd.StartRun();

    //            CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
    //            CFThree2FiveTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    /*case E_WGR_MDIR_Grab:
    //        {
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            m_curState = WGR_FDSPDESKTOP_LISTEN_STATE;
    //            m_smDspDesktop.StartRun();

    //            CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
    //            CFThree2FiveTouchPublic::ResetSomeMembers();
    //        }

    //        break;*/

    //    case E_WGR_MDIR_UVertic:
    //        {
    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            m_curState = WGR_FMAXICURWND_LISTEN_STATE;
    //            m_smMaxiCurWnd.StartRun();

    //            CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
    //            CFThree2FiveTouchPublic::ResetSomeMembers();
    //        }

    //        break;

    //    default:
    //        {
    //            CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

    //            CFThree2FiveTouchPublic::m_curMissMDirTolerance++;

    //            if(CFThree2FiveTouchPublic::m_curMissMDirTolerance > FTHREE2FIVETOUCH_MAX_MISSDIR)
    //            {
    //                //CFTwoTouchPublic::ResetSomeMembers();
    //                m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
    //            }
    //        }
    //        break;
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);

    //} while (false);

    //if (m_curState == WGR_FTHREE2FIVETOUCH_CLOSE_STATE)
    //{//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CFThree2FiveTouchPublic::ResetSomeMembers();
    //}

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFThree2FiveTouchPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFThree2FiveTouchPublic::m_curMissMDirTolerance);
    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFThree2FiveTouchStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do 
    {

        if(pContext)
        {//
            if (WGR_FTHREE2FIVETOUCH_LISTEN_STATE == m_curState)
            {
				if (false == CWGRCommonUlity::IsTheEventPermit(pContext, E_TEF_35TOUCHS_PART))            
				{
					//此功能已经禁止，无需继续判断
					m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;

					break;
				}
            }            
        }
        else
        {//执行上下文为空，无需再继续判断
            m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
            break;
        }        

        g_oWGRTraceLog.AddGestureShape(CWGRCommonUlity::m_curGestureShape);

        if (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Finger)
        {//非手指形状
            CWGRCommonUlity::m_curUnexpectedShapeTimes++;

            g_oWGRTraceLog.AddCurMissShapeTolerance(CWGRCommonUlity::m_curUnexpectedShapeTimes);

            if (CWGRCommonUlity::m_curUnexpectedShapeTimes > FTHREE2FIVETOUCH_MAX_MISSSHAPETOLLERANCE)
            {
                m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;  
                CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            }

            break;
        }
        else
        {
            CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            g_oWGRTraceLog.AddCurMissShapeTolerance(CWGRCommonUlity::m_curUnexpectedShapeTimes);
        }

        if(!CWGRCommonUlity::IsInputTouchsBetween(3, 5))
        {
            CWGRCommonUlity::m_curUnexpectedTouchsTimes++;
            g_oWGRTraceLog.AddCurMissTouchCountTolerance( CWGRCommonUlity::m_curUnexpectedTouchsTimes);

            if ( CWGRCommonUlity::m_curUnexpectedTouchsTimes > FTHREE2FIVETOUCH_MAX_MISSTOUCHCOUNTTOLERANCE)
            {
                m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE; 
                 CWGRCommonUlity::m_curUnexpectedTouchsTimes = 0;
            }

            break;
        }
        else
        {
             CWGRCommonUlity::m_curUnexpectedTouchsTimes = 0;
            g_oWGRTraceLog.AddCurMissTouchCountTolerance( CWGRCommonUlity::m_curUnexpectedTouchsTimes);
        }

        g_oWGRTraceLog.AddEnterLess(CWGRCommonUlity::m_havedEnteredTheSumSM);
        if (!CWGRCommonUlity::m_havedEnteredTheSumSM)
        {            

            //重置参数
			CWGRCommonUlity::ResetSomeMembers();
            CWGRCommonUlity::InitWhenFirstEnter(pContext);
        }
        
        E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(6, 6, 3, 5, 1, 2);

        switch(dir)
        {
        case E_WGR_MDIR_None:
            {//未移动
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

				CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;

                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FTHREE2FIVEUCH_MAX_NONMOVETOLERANCE)
                {
                    m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
                }
            }

            break;

        case E_WGR_MDIR_LHoriz:            
        case E_WGR_MDIR_RHoriz:
            {
                AtlTrace(_T("CWGRCommonUlity::m_nCurSameDirTouchs = %d \n"), CWGRCommonUlity::m_nCurSameDirTouchs);
                if(CWGRCommonUlity::m_nCurSameDirTouchs >= 3)
                {
                    m_curState = WGR_FSWHWND_LISTEN_STATE;
                    m_smSWHWnd.StartRun();

                    CWGRCommonUlity::ResetDislaymentInLastTouchMap();
                    CWGRCommonUlity::ResetSomeMembers();
                } 
                else
                {
                    CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                    CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                    CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                    if(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FTHREE2FIVETOUCH_MAX_UNEXPECTEDUNKNOWNMOV)
                    {
                        //CFTwoTouchPublic::ResetSomeMembers();
                        m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
                    }
                }
            }

            break;

        case E_WGR_MDIR_DVertic:
            {
                m_curState = WGR_FMINICURWND_LISTEN_STATE;
                m_smMiniCurWnd.StartRun();

				CWGRCommonUlity::ResetDislaymentInLastTouchMap();
				CWGRCommonUlity::ResetSomeMembers();
            }

            break;

        /*case E_WGR_MDIR_Grab:
            {
                CFThree2FiveTouchPublic::m_curMissMDirTolerance = 0;
                CFThree2FiveTouchPublic::m_curNonMoveTolerance = 0;

                m_curState = WGR_FDSPDESKTOP_LISTEN_STATE;
                m_smDspDesktop.StartRun();

                CFThree2FiveTouchPublic::m_ptSartMid = CFThree2FiveTouchPublic::m_ptLastMid;
                CFThree2FiveTouchPublic::ResetSomeMembers();
            }

            break;*/

        case E_WGR_MDIR_UVertic:
            {
                m_curState = WGR_FMAXICURWND_LISTEN_STATE;
                m_smMaxiCurWnd.StartRun();

				CWGRCommonUlity::ResetDislaymentInLastTouchMap();
				CWGRCommonUlity::ResetSomeMembers();
			}
            break;

        case E_WGR_MDIR_Move:
            {
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;                
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FTHREE2FIVETOUCH_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();               
            }
            break;

        default:
            {
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                if(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FTHREE2FIVETOUCH_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    //CFTwoTouchPublic::ResetSomeMembers();
                    m_curState = WGR_FTHREE2FIVETOUCH_CLOSE_STATE;
                }
            }
            break;
        }

        //g_oWGRTraceLog.AddMoveDirection(dir);

    } while (false);

    if (m_curState == WGR_FTHREE2FIVETOUCH_CLOSE_STATE)
    {//WGR_FTHREE2FIVETOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
        m_curState = WGR_IDLE_STATE;
        if (m_curState == E_WGR_GShape_Palm)
        {
            m_curState = E_WGR_GShape_Palm;
        }
        CWGRCommonUlity::ResetSomeMembers();
    }

    g_oWGRTraceLog.AddCurMissNonMoveTolerance(CWGRCommonUlity::m_curUnexpectedNonMoveTimes);
    g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes);
    g_oWGRTraceLog.AddSateToCovert(m_curState);

    return m_curState;
}
