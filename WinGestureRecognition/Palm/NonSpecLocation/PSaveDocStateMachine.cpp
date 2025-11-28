#include "StdAfx.h"
//#include "PSaveDocStateMachine.h"
//#include "PNonSpecLocPublic.h"
//#include "..\..\GlobalLog.h"
//
////
//#define PSAVEDOC_MAX_MISSDIR                               1
//#define PSAVEDOC_MAX_MISSSHAPETOLLERANCE                   2
//#define PSAVEDOC_MAX_NONMOVETOLERANCE                      15
////******
//
//CPSaveDocStateMachine::CPSaveDocStateMachine(void)
//{
//    m_curState = WGR_IDLE_STATE;
//}
//
//CPSaveDocStateMachine::~CPSaveDocStateMachine(void)
//{
//}
//
//bool CPSaveDocStateMachine::StartRun()
//{
//    m_curState = WGR_PSVDOC_LISTEN_STATE;
//    return true;
//}
//
//bool CPSaveDocStateMachine::StopRun()
//{
//    m_curState = WGR_IDLE_STATE;
//    return false;
//}
//
//IWGRStateMachine *CPSaveDocStateMachine::GetSubMachineCurStateContained()
//{    
//    return (IWGRStateMachine *)this;
//}
//
//WGR_STATE_TYPE CPSaveDocStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
//{   
//    do 
//    {
//        g_oWGRTraceLog.AddGestureShape(gestureShape);
//        
//        if (gestureShape != E_WGR_GShape_Palm)
//        {
//            CPNonSpecLocPublic::m_curMissShapeTolerance++;
//
//            if (CPNonSpecLocPublic::m_curMissShapeTolerance > PSAVEDOC_MAX_MISSSHAPETOLLERANCE)
//            {               
//                m_curState = WGR_PSVDOC_CLOSE_STATE;               
//                CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
//            }
//
//            break;
//        }
//        else
//        {
//            CPNonSpecLocPublic::m_curMissShapeTolerance = 0;
//        }
//
//        //if(pContext)
//        //{//
//        //    //pContext->TriggerMouseLeftButtonDown(&(CPNonSpecLocPublic::m_ptStart));
//        //    pContext->TriggerHIDUp(&(CPNonSpecLocPublic::m_ptStart));
//        //}
//
//        //光斑的实际大小和标准大小
//        g_oWGRTraceLog.AddSpotArea(aryLightSpots[0].mass);
//        g_oWGRTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);
//
//
//        E_WGR_MOVEDIR dir = CPNonSpecLocPublic::CalcMoveDirection(aryLightSpots, nCount, 7, 7);
//
//        switch(dir)
//        {
//        case E_WGR_MDIR_None:
//            {                
//                CPNonSpecLocPublic::m_curMissMDirTolerance = 0;
//
//                CPNonSpecLocPublic::m_curNonMoveTolerance++;
//
//                if (CPNonSpecLocPublic::m_curNonMoveTolerance > PSAVEDOC_MAX_NONMOVETOLERANCE)
//                {
//                    //执行保存文档
//                    if (pContext)
//                    {
//                        pContext->TriggerDocSaveEvent(CPNonSpecLocPublic::m_ptStart);
//                    }
//
//                    m_curState = WGR_PSVDOC_CLOSE_STATE; 
//                }
//            }
//            break;
//
//        case E_WGR_MDIR_Move:
//            {                 
//                CPNonSpecLocPublic::m_curMissMDirTolerance = 0;
//                CPNonSpecLocPublic::m_curNonMoveTolerance = 0;
//                m_curState = WGR_PSVDOC_CLOSE_STATE;                 
//            }
//            break;
//
//        default:
//            {
//                CPNonSpecLocPublic::m_curNonMoveTolerance = 0;
//
//                CPNonSpecLocPublic::m_curMissMDirTolerance++;
//
//                if (CPNonSpecLocPublic::m_curMissMDirTolerance > PSAVEDOC_MAX_MISSDIR)
//                {
//                    m_curState = WGR_PSVDOC_CLOSE_STATE;   
//                }
//            }
//            break;
//        }
//
//        g_oWGRTraceLog.AddMoveDirection(dir);
//        //switch()
//    } while (false);
//
//    if (m_curState == WGR_PSVDOC_CLOSE_STATE)
//    {
//        m_curState = WGR_IDLE_STATE;
//
//        CPNonSpecLocPublic::ResetSomeMembers();
//    }
//
//    g_oWGRTraceLog.AddSateToCovert(m_curState);
//
//    return m_curState;
//}
