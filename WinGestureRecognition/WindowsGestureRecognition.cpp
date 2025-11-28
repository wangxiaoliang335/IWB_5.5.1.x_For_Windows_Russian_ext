#include "StdAfx.h"
#include "WGRStateMachine.h"
#include "WindowsGestureRecognition.h"
#include "WGRCommonUlity.h"
#include "InputTouchClusterAlg.h"

#include <atlbase.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//
//
//////////////////////////////////////////////////////////////////////////
//
CWGRStateMachine            m_gSMWGR              ;//手势识别状态机
bool                        g_needToBeFilter = false; //是否需要过滤干扰点                                                        

//////////////////////////////////////////////////////////////////////////
//


//
bool CWindowsGestureRecognition::DetermineWhethertoEnterWGR(const TLightSpot * aryLightSpots,  int nCount)
{
    bool bDetermineRes = true;

    EWGR_GESTURE_SHAPE gesShape = E_WGR_GShape_Unknown;

    do 
    {      

        if (m_pContextImpl == NULL)
        {
            gesShape = E_WGR_GShape_Unknown;
            m_curState = WGR_IDLE_STATE;

            bDetermineRes = false;

            break;
        }

        if (nCount <= 0)
        {
            gesShape = E_WGR_GShape_Unknown;

            if(m_curState == WGR_IDLE_STATE)
            {
                bDetermineRes = false;
            }

            break;
        }
        
        if (m_pContextImpl->IsAllGestureClosed())
        {
            bDetermineRes = false;

            break;
        }

        /*
        @判断为手掌，需要满足的条件
        a.最大光斑的质量至少是标准质量的nMulti倍
        //b.当光斑的个数大于等于2个时，最大光斑的质量m1与次大光斑的质量m2满足：m1 * ld2 > m2 * ld1 * (4 / 3)
        */
        gesShape = E_WGR_GShape_Finger;

        const TLightSpot &refSpot = aryLightSpots[0];
        int nStdAreaByMulti = m_pContextImpl->GetMultiOfPalm2Std() * refSpot.lStdSpotAreaInVideo; 
        int nWeightedMass = m_pContextImpl->GetWeightedSpotMass(refSpot.mass, refSpot.lAreaInVideo, nStdAreaByMulti);        
        //if (aryLightSpots[0].mass > m_pContextImpl->GetMultiOfPalm2Std() * aryLightSpots[0].lStdSpotAreaInVideo)
        if (nWeightedMass > nStdAreaByMulti)
        {        
            {
                gesShape = E_WGR_GShape_Palm;
            }
        }

        //AtlTrace(_T("###aryLightSpots[0].mass = %d nWeightedMass = %d nStdAreaByMulti = %d \r\n"), refSpot.mass, nWeightedMass, nStdAreaByMulti);
        //AtlTrace(_T("###aryLightSpots[0].mass = %d m_pContextImpl->GetMultiOfPalm2Std() * aryLightSpots[0].lStdSpotAreaInVideo = %d \r\n"), aryLightSpots[0].mass, m_pContextImpl->GetMultiOfPalm2Std() * aryLightSpots[0].lStdSpotAreaInVideo);

        if (gesShape == E_WGR_GShape_Finger)
        {
            if (m_pContextImpl->IsAllFingerBasedGestureClosed())
            {
                bDetermineRes = false;
                break;
            }            
        }
        else
        {
            if(m_pContextImpl->IsAllPalmBasedGestureClosed())
            {
                bDetermineRes = false;
                break;
            }            
        }

        if (gesShape != E_WGR_GShape_Palm && nCount <= 1 && m_curState == WGR_IDLE_STATE)
        {
            bDetermineRes = false;
        }

    } while (false);

    if (bDetermineRes)
    {
        if (!g_oLogHasStarted && m_pContextImpl->IsLogPermited())
        {
            m_gSMWGR.StartLog();
        }  

        if(m_curState == WGR_IDLE_STATE) CWGRCommonUlity::ResetCurrentTouchInfo();        
    }
    else
    {
        if (g_oLogHasStarted)
        {
            m_gSMWGR.StopLog();
        }
    }

    CWGRCommonUlity::m_curGestureShape = gesShape;

    CWGRCommonUlity::m_haveGSDetermined = true;

    g_needToBeFilter = ((gesShape == E_WGR_GShape_Finger) && (nCount <= 5) && (nCount > 2) && (m_curState == WGR_IDLE_STATE)) && bDetermineRes; //过滤干扰点的条件

    /*if ((m_curState != WGR_IDLE_STATE) && gesShape == E_WGR_GShape_Unknown)
    {
        int a = 0;
        a = 10;
    }*/

    return bDetermineRes;
}

//bool CWindowsGestureRecognition::DeterminationShape(const TLightSpot * aryLightSpots,  int nCount)
//{
//    do
//    {
//        if(nCount <= 0)
//        {
//            CWGRCommonUlity::m_curGestureShape = E_WGR_GShape_Unknown;
//
//            break;
//        }    
//
//        ////AtlTrace("\r\n nCount = %d ; mass = %d ; m_nMultiEraser * aryLightSpots[0].lStdSpotAreaInVideo = %d", nCount, aryLightSpots[0].mass, aryLightSpots[0].lStdSpotAreaInVideo);
//
//        /*
//          @判断为手掌，需要满足的条件
//            a.最大光斑的质量至少是标准质量的nMulti倍
//            b.当光斑的个数大于等于2个时，最大光斑的质量m1与次大光斑的质量m2满足：m1 * ld2 > m2 * ld1 * (4 / 3)
//        */
//        CWGRCommonUlity::m_curGestureShape = E_WGR_GShape_Finger;
//
//        //int nTmpMulti = nMulti > 1 ? (nMulti - 1) : nMulti;
//
//        if (m_pContextImpl) m_nMultiEraser = m_pContextImpl->GetMultiOfPalm2Std();
//        
//        if (aryLightSpots[0].mass > m_nMultiEraser * aryLightSpots[0].lStdSpotAreaInVideo)
//        {        
//            {
//                CWGRCommonUlity::m_curGestureShape = E_WGR_GShape_Palm;
//            }
//        }
//
//        if (m_curState == WGR_IDLE_STATE)
//        {
//            if (nCount == 1 && CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Palm)
//            {//单个手指在WGR_IDLE_STATE状态下视为Unkown shape
//                CWGRCommonUlity::m_curGestureShape = E_WGR_GShape_Unknown;
//            }
//            else
//            {//第一次进入手势识别时，先重置先前存储的触点
//                CWGRCommonUlity::ResetCurrentTouchInfo();
//            }
//        }
//    }while(false);
//
//    return (CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Unknown) || (m_curState != WGR_IDLE_STATE);
//}


CWindowsGestureRecognition::CWindowsGestureRecognition(void)
{
    m_nMultiEraser = 3;    
    m_curState = WGR_IDLE_STATE;

    m_pContextImpl = NULL;
    m_bLogPermited = false; //默认不开启日志

    CWGRCommonUlity::m_curGestureShape = E_WGR_GShape_Unknown;    
}

CWindowsGestureRecognition::~CWindowsGestureRecognition(void)
{
}

//开启识别器
bool CWindowsGestureRecognition::Start()
{
    m_curState = WGR_IDLE_STATE;
    m_pContextImpl = NULL;
    m_bLogPermited = false; //默认不开启日志    
    
    return true;
}

//停止识别器
bool CWindowsGestureRecognition::Stop()
{
    m_curState = WGR_IDLE_STATE;
    m_pContextImpl = NULL;
    m_bLogPermited = false; //默认不开启日志

    /*if (g_oLogHasStarted)
    {
        g_oSpotsLog.StopLog();
        g_oLogHasStarted = false;
    }*/
    return true;
}

void CWindowsGestureRecognition::AttachExceContext(IWGRUtilContext *pContext)
{
    m_pContextImpl = pContext;

    if (m_pContextImpl)
    {
        int nClusterMaxLimit = m_pContextImpl->MapStandX2Actual(70);
        CInputTouchClusterAlg::UpdateMaxDistanceLimit(nClusterMaxLimit, nClusterMaxLimit);
    }
}

//切断功能的执行者
void CWindowsGestureRecognition::DetachExceContext()
{
    m_pContextImpl = NULL;
}

////执行识别
//bool CWindowsGestureRecognition::DoRecongition(const TLightSpot * aryLightSpots,  int nCount)
//{      
//    if (m_pContextImpl == NULL) 
//    {
//        m_curState = WGR_IDLE_STATE;
//
//        return false;
//    }
//
//    if ((nCount <= 0 || nCount > 12) && m_curState == WGR_IDLE_STATE)
//    {
//        m_gSMWGR.StopLog();
//        return false;
//    }
//
//    //AtlTrace(_T("Here!!"));    
//
//    if (nCount == 1 && CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Palm && m_curState == WGR_IDLE_STATE)
//    {
//        return false;
//    }
//
//    if (m_pContextImpl->IsLogPermited())
//    {
//        if(!g_oLogHasStarted && CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Unknown)
//        {
//            m_gSMWGR.StartLog();
//        }
//    }    
//
//    m_curState = m_gSMWGR.TransitState(aryLightSpots, nCount, m_pContextImpl, CWGRCommonUlity::m_curGestureShape); 
//
//    CWGRCommonUlity::ResetCurrentTouchInfo();   //重置当前的输入的触点，等待新触点输入
//        
//    return m_curState != WGR_IDLE_STATE;
//}

bool CWindowsGestureRecognition::DoRecongition()
{
    //bool bRet = false;    

    do 
    {
        if (CWGRCommonUlity::m_haveGSDetermined == false) 
        {
            m_curState = WGR_IDLE_STATE;
            break;
        }

        CWGRCommonUlity::m_pWGRUtilContext = m_pContextImpl;

        //if ( (!CWGRCommonUlity::IsInputTouchsBetween(1, 10)) && m_curState == WGR_IDLE_STATE)
        //{
        //    m_gSMWGR.StopLog();
        //    m_curState = WGR_IDLE_STATE;
        //    break;
        //}

        ////AtlTrace(_T("Here!!"));    

        ///*if (CWGRCommonUlity::IsInputTouchsEqual(1) && CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Palm && m_curState == WGR_IDLE_STATE)
        //{
        //    m_curState = WGR_IDLE_STATE;
        //    break;
        //}*/

        //if (m_pContextImpl->IsLogPermited())
        //{
        //    if(!g_oLogHasStarted && CWGRCommonUlity::m_curGestureShape != E_WGR_GShape_Unknown)
        //    {
        //        m_gSMWGR.StartLog();
        //    }
        //}   
        //bool needToFilter = (m_curState == WGR_IDLE_STATE) && CWGRCommonUlity::m_curGestureShape == E_WGR_GShape_Finger && (g_curTouchCount <= 5);
        if (g_needToBeFilter)
        {//过滤干扰点
            CInputTouchClusterAlg::GetValidInputAfterCluster();

            memcpy(CWGRCommonUlity::m_curNewAddedTInof, CInputTouchClusterAlg::m_validTouchsAdded, sizeof(ST_TouchInfo) * CInputTouchClusterAlg::m_nValidTouchsAdded);
            CWGRCommonUlity::m_nCurNewAddedTouchs = CInputTouchClusterAlg::m_nValidTouchsAdded;

            memcpy(CWGRCommonUlity::m_curMatchedTInfo, CInputTouchClusterAlg::m_validTouchsMatched, sizeof(ST_TouchInfo) * CInputTouchClusterAlg::m_nValidTouchsMatched);
            CWGRCommonUlity::m_nCurMatchedTouchs = CInputTouchClusterAlg::m_nValidTouchsMatched;
        }
        m_curState = m_gSMWGR.TransitState(m_pContextImpl);                 

        CWGRCommonUlity::ResetCurrentTouchInfo();   //重置当前输入的触点，等待新触点的输入

        if (g_needToBeFilter) {CInputTouchClusterAlg::ResetClusterInfo(); g_needToBeFilter = false;}
    } while (false);        
    

    return m_curState != WGR_IDLE_STATE;
}

void CWindowsGestureRecognition::AddTouchAfterMatch(const UINT &uId, const POINT &ptPos, int matchState)
{
    /*
     @因为用户操作时，可能存在干扰点（特别对于手指形状的手势），所以在当前状态为idle时，对于判定为手指形状的输入进行过滤，
      去除干扰点，干扰点的去除方法是——先把输入点存入CInputTouchClusterAlg中，然后在DoRecongition时，进行聚类，通过获取最大聚类集来
      去除干扰点
    */
    if (g_needToBeFilter)
    {//过滤干扰点
        CInputTouchClusterAlg::AddTouch(uId, ptPos, matchState);

        return;
    }

    ST_TouchInfo *pRefTouchInfo = NULL;
    int *pRefTouchCount = NULL;

    switch(matchState)
    {
    case 0:
        //Mismatched Touch
        {
            pRefTouchInfo = (CWGRCommonUlity::m_curMisMatchedTInfo);
            pRefTouchCount = &(CWGRCommonUlity::m_nCurMisMatchedTouchs);
        }
        break;

    case 1:
        //Matched Touch
        {
            pRefTouchInfo = (CWGRCommonUlity::m_curMatchedTInfo);
            pRefTouchCount = &(CWGRCommonUlity::m_nCurMatchedTouchs);
        }
        break;

    case 2:
        //New Added Touch
        {
            pRefTouchInfo = (CWGRCommonUlity::m_curNewAddedTInof);
            pRefTouchCount = &(CWGRCommonUlity::m_nCurNewAddedTouchs);
        }
        break;

    default:
        break;
    }

    if (pRefTouchInfo != NULL && pRefTouchCount != NULL && (*pRefTouchCount) < KMAXTOUCHNUM)
    {
        pRefTouchInfo[*pRefTouchCount].uId                          = uId;
        pRefTouchInfo[*pRefTouchCount].stMotionInfo.ptPos           = ptPos;
        pRefTouchInfo[*pRefTouchCount].stMotionInfo.nXDisplacement  = 0;
        pRefTouchInfo[*pRefTouchCount].stMotionInfo.nYDisplacement  = 0;
        (*pRefTouchCount)++;
    }    
}
