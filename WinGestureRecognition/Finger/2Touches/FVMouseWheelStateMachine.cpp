#include "StdAfx.h"
#include "FVMouseWheelStateMachine.h"
#include "FTwoTouchPublic.h"
#include "..\..\GlobalLog.h"

//////////////////////////////////////////////////////////////////////////
//
#define FVMW_MAX_MISSSHAPETOLLERANCE                    2
#define FVMW_MAX_MISS2TOUCHCOUNTTOLERANCE               5
#define FVMW_MAX_NONMOVETOLERANCE                       45
#define FVMW_MAX_UNEXPECTEDKNOWNMOV                     6
#define FVMW_MAX_UNEXPECTEDUNKNOWNMOV                   1
//

CFVMouseWheelStateMachine::CFVMouseWheelStateMachine(void)
{
    m_curState = WGR_IDLE_STATE;
}

CFVMouseWheelStateMachine::~CFVMouseWheelStateMachine(void)
{
}

bool CFVMouseWheelStateMachine::StartRun()
{

    m_curState = WGR_FVMOUSEWHEEL_LISTEN_STATE;
    return true;
}

bool CFVMouseWheelStateMachine::StopRun()
{
    m_curState = WGR_IDLE_STATE;

    return true;
}

IWGRStateMachine *CFVMouseWheelStateMachine::GetSubMachineCurStateContained()
{
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFVMouseWheelStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
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
    //        CFTwoTouchPublic::m_curMissShapeTolerance++;

    //        g_oWGRTraceLog.AddCurMissShapeTolerance(CFTwoTouchPublic::m_curMissShapeTolerance);

    //        if (CFTwoTouchPublic::m_curMissShapeTolerance > FVMW_MAX_MISSSHAPETOLLERANCE)
    //        {
    //            m_curState = WGR_FVMOUSEWHEEL_CLOSE_STATE;               
    //        }

    //         break;
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

    //        if (CFTwoTouchPublic::m_curMiss2TouchCountTolerance > FVMW_MAX_MISS2TOUCHCOUNTTOLERANCE)
    //        {
    //            m_curState = WGR_FVMOUSEWHEEL_CLOSE_STATE;                
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

    //        //if(pContext)
    //        //{//            
    //        //    pContext->TriggerHIDUp(&(CFTwoTouchPublic::m_ptCur[0]));
    //        //}

    //    }
    //    
    //    //强制由监听状态进入保持状态
    //    if (m_curState == WGR_FVMOUSEWHEEL_LISTEN_STATE)
    //    {
    //        m_curState = WGR_FVMOUSEWHEEL_KEEP_STATE;
    //    }

    //    /*
    //    @计算移动方向的方法是：两点的平均位置ptCurMid与上一次平均位置ptLastMid比较
    //    */       
    //    //

    //    E_WGR_MOVEDIR dir = CFTwoTouchPublic::CalcMoveDirection(aryLightSpots, nCount, 6, 6);

    //    if (dir == E_WGR_MDIR_None)
    //    {//未移动
    //        CFTwoTouchPublic::m_curMissMDirTolerance = 0;

    //        CFTwoTouchPublic::m_curNonMoveTolerance++;

    //        if (CFTwoTouchPublic::m_curNonMoveTolerance > FVMW_MAX_NONMOVETOLERANCE)
    //        {
    //            m_curState = WGR_FVMOUSEWHEEL_CLOSE_STATE;
    //        }
    //    }
    //    else if (dir == E_WGR_MDIR_Vertic)
    //    {//水平移动
    //        CFTwoTouchPublic::m_curNonMoveTolerance = 0;
    //        CFTwoTouchPublic::m_curMissMDirTolerance = 0;
    //        //m_curState = WGR_FVMOUSEWHEEL_LISTEN_STATE;
    //        //m_smHMouseWheel.StartRun();

    //        //触发滚轮事件
    //        if (pContext)
    //        {
    //            POINT ptCurMid;
    //            ptCurMid.x = (aryLightSpots[0].ptPosInScreen.x + aryLightSpots[1].ptPosInScreen.x + 1) >> 1;
    //            ptCurMid.y = (aryLightSpots[0].ptPosInScreen.y + aryLightSpots[1].ptPosInScreen.y + 1) >> 1;

    //            int rollDir = CFTwoTouchPublic::m_nDisplacement < 0 ? -1 : 1;
    //            pContext->TriggerVMouseWheelEvent(ptCurMid, rollDir);
    //        }

    //        CFTwoTouchPublic::m_ptLast[0] = CFTwoTouchPublic::m_ptCur[0];
    //        CFTwoTouchPublic::m_ptLast[1] = CFTwoTouchPublic::m_ptCur[1];

    //        CFTwoTouchPublic::ResetSomeMembers();
    //    }
    //    //else if (dir == E_WGR_MDIR_Vertic)
    //    //{
    //    //    CFTwoTouchPublic::m_curNonMoveTolerance = 0;
    //    //    CFTwoTouchPublic::m_curMissMDirTolerance = 0;
    //    //    //m_curState = WGR_FVMOUSEWHEEL_LISTEN_STATE;
    //    //    m_smVMouseWheel.StartRun();

    //    //    CFTwoTouchPublic::m_ptLast[0] = aryLightSpots[0].ptPosInScreen;
    //    //    CFTwoTouchPublic::m_ptLast[1] = aryLightSpots[0].ptPosInScreen;

    //    //    CFTwoTouchPublic::ResetSomeMembers();
    //    //}
    //    else
    //    {
    //        CFTwoTouchPublic::m_curNonMoveTolerance = 0;

    //        CFTwoTouchPublic::m_curMissMDirTolerance++;

    //        if(CFTwoTouchPublic::m_curMissMDirTolerance > FVMW_MAX_MISSDIR)
    //        {
    //            //CFTwoTouchPublic::ResetSomeMembers();
    //            m_curState = WGR_FVMOUSEWHEEL_CLOSE_STATE;
    //        }
    //    }

    //    g_oWGRTraceLog.AddMoveDirection(dir);
    //}while(false);

    //if (m_curState == WGR_FVMOUSEWHEEL_CLOSE_STATE)
    //{//WGR_FTWOTOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
    //    m_curState = WGR_IDLE_STATE;
    //    CFTwoTouchPublic::ResetSomeMembers();
    //}

    //g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFTwoTouchPublic::m_curNonMoveTolerance);
    //g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFTwoTouchPublic::m_curMissMDirTolerance);
    //g_oWGRTraceLog.AddSateToCovert(m_curState);

    ///*if(dir != )
    //{

    //}*/

    //return m_curState;

	return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFVMouseWheelStateMachine::TransitState(IWGRUtilContext *pContext)
{
	do
    {
        //g_oWGRTraceLog.AddSpotCount(nCount);

        //if (g_oWGRTraceLog.IsLogHasStarted())
        //{
        //    //
        //    POINT pt[16];
        //    for (int i = 0; i < nCount; i++)
        //    {
        //        pt[i] = aryLightSpots[i].ptPosInScreen;
        //    }

        //    g_oWGRTraceLog.AddSpotPos(pt, nCount);
        //}

        if (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Finger)
        {//非手指形状
            CWGRCommonUlity::m_curUnexpectedShapeTimes++;

            //g_oWGRTraceLog.AddCurMissShapeTolerance(CFTwoTouchPublic::m_curMissShapeTolerance);

            if ( CWGRCommonUlity::m_curUnexpectedShapeTimes > FVMW_MAX_MISSSHAPETOLLERANCE)
            {
                m_curState = WGR_FVMOUSEWHEEL_CLOSE_STATE;               
            }

             break;
        }
        else
        {
             CWGRCommonUlity::m_curUnexpectedShapeTimes = 0;
            //g_oWGRTraceLog.AddCurMissShapeTolerance(CFTwoTouchPublic::m_curMissShapeTolerance);
        }

        if(!CWGRCommonUlity::IsInputTouchsEqual(2))
        {//光点数不等于2
           CWGRCommonUlity::m_curUnexpectedTouchsTimes++;

            //g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFTwoTouchPublic::m_curMiss2TouchCountTolerance);

            if (CWGRCommonUlity::m_curUnexpectedTouchsTimes > FVMW_MAX_MISS2TOUCHCOUNTTOLERANCE)
            {
                m_curState = WGR_FVMOUSEWHEEL_CLOSE_STATE;                
            }

            break;
        }
        else
        {
            CWGRCommonUlity::m_curUnexpectedTouchsTimes = 0;
            //g_oWGRTraceLog.AddCurMissTouchCountTolerance(CFTwoTouchPublic::m_curMiss2TouchCountTolerance);
        }

        //g_oWGRTraceLog.AddEnterLess(CFTwoTouchPublic::m_bEnterLessThanOne);        
        
        //强制由监听状态进入保持状态
        if (m_curState == WGR_FVMOUSEWHEEL_LISTEN_STATE)
        {
            m_curState = WGR_FVMOUSEWHEEL_KEEP_STATE;
        }

        /*
        @计算移动方向的方法是：两点的平均位置ptCurMid与上一次平均位置ptLastMid比较
        */       
        //

        E_WGR_MOVEDIR dir = CWGRCommonUlity::CalcMoveDirectionByStatistics(6, 6, 2, 2, 3, 2);

        switch(dir)
        {
        case E_WGR_MDIR_None:
            {//未移动
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;

                CWGRCommonUlity::m_curUnexpectedNonMoveTimes++;   

                if (CWGRCommonUlity::m_curUnexpectedNonMoveTimes > FVMW_MAX_NONMOVETOLERANCE)
                {
                    m_curState = WGR_FHMOUSEWHEEL_CLOSE_STATE;
                }

            }
            break;

        case E_WGR_MDIR_UVertic:
            {//           
                //触发滚轮事件
                if (pContext)
                {                                
                    pContext->TriggerVMouseWheelEvent(CWGRCommonUlity::m_curPtMiddle, -1);
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();

                CWGRCommonUlity::ResetSomeMembers();

            }
            break;

        case E_WGR_MDIR_DVertic:
            {
                //触发滚轮事件
                if (pContext)
                {                                
                    pContext->TriggerVMouseWheelEvent(CWGRCommonUlity::m_curPtMiddle, 1);
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();

                CWGRCommonUlity::ResetSomeMembers();
            }
            break;

        case E_WGR_MDIR_Move:
        case E_WGR_MDIR_LHoriz:
        case E_WGR_MDIR_RHoriz:
            {                
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes    = 0;
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes        = 0;
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes++;

                if (CWGRCommonUlity::m_curUnexpectedKnownMoveTimes > FVMW_MAX_UNEXPECTEDKNOWNMOV)
                {
                    m_curState = WGR_FHMOUSEWHEEL_CLOSE_STATE;
                }

                CWGRCommonUlity::ResetDislaymentInLastTouchMap();
            }
            break;

        default:
            {
                CWGRCommonUlity::m_curUnexpectedNonMoveTimes = 0;  
                CWGRCommonUlity::m_curUnexpectedKnownMoveTimes      = 0;            
                CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes++;

                if(CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes > FVMW_MAX_UNEXPECTEDUNKNOWNMOV)
                {
                    //CFTwoTouchPublic::ResetSomeMembers();
                    m_curState = WGR_FHMOUSEWHEEL_CLOSE_STATE;
                }
            }
            break;

        }

        //g_oWGRTraceLog.AddMoveDirection(dir);
    }while(false);

    if (m_curState == WGR_FVMOUSEWHEEL_CLOSE_STATE)
    {//WGR_FTWOTOUCH_CLOSE_STATE只是过渡状态(用于清理或重置一些变量)，不持久性停留
        m_curState = WGR_IDLE_STATE;
        CWGRCommonUlity::ResetSomeMembers();
    }

	/*g_oWGRTraceLog.AddCurMissNonMoveTolerance(CFTwoTouchPublic::m_curNonMoveTolerance);
	g_oWGRTraceLog.AddCurUnexpectedUnknownDirTolerance(CFTwoTouchPublic::m_curMissMDirTolerance);*/
	g_oWGRTraceLog.AddSateToCovert(m_curState);   

    return m_curState;
}

