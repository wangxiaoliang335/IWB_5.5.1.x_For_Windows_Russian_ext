#include "StdAfx.h"
//#include <assert.h>
//#include<math.h>
#include "../ProcessUtility.h"
#include "../../headers.h"
#include "GLGRTraceLog.h"


CVirtualMouse &g_oRefVirtualMouse = GetVirtualMouse();


#define                MyXY_Begin                                    1
#define                MyXY_End                                      2
#define                MyWM_XY                                       1025
#define                MyWM_But                                      1026

#define                MyBut_Prev                                    1
#define                MyBut_Next                                    2
#define                MyBut_Select                                  3
#define                MyBut_Pen                                     10
#define                MyBut_SoftPen                                 11
#define                MyBut_Eraser                                  13
#define                MyBut_Windows                                 19
#define                MyBut_SmartPen                                20
#define                MyBut_MultiPen                                21
#define                MyWM_SpotLight                                22
#define                MyBut_Zoom                                    23
#define                MyBut_Rotation                                24
#define                MyBut_Mask                                    25
#define                MyBut_Selection                               26

#define                MyBut_NonMode                                 27


#define                MOUSE_MOVE_DISTANCE                           3
#define                COLLECT_POINT_COUNT                           30
#define                MOVE_DISTANCE                                 40
#define                ROAM_INTERVAL_DISTANCE                        100    //modify by vera 2020.5.9 150

#define                TWOTOUCHS_MISSTOLERANCE_OFFSTATE              18

//#pragma comment(lib,"../lib/ShareMemoryRegion.lib")

//////////////////////////////////////////////////////////////////////////
//
//<32位整数绝对值宏
#ifndef GLBGR_INT32_ABS
#define GLBGR_INT32_ABS(n) (((n)^((n)>>31)) -((n)>>31))
#endif
//32位整数绝对值宏>

//////////////////////////////////////////////////////////////////////////
//GLBoard程序进程名
const char *KGLBOARDPROCNAME = "JOSS.EXE";
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
CGLGRTraceLog   g_oGLTraceLog;
bool            g_bHasLogStarted = false;
int             g_nCurrentCollect = 0;
E_WBS           g_ePreState = E_WBS_IDLE;

void StartTraceLog()
{
    if (!g_bHasLogStarted)
    {
        g_oGLTraceLog.StartLog();

        g_bHasLogStarted = true;
    }
    else
    {
        g_oGLTraceLog.StopLog();
    }
}

void StopTraceLog()
{
    g_oGLTraceLog.StopLog();
    g_bHasLogStarted = false;
    g_nCurrentCollect = 0;
    g_ePreState = E_WBS_IDLE;
}

CVirtualHID &refVirtualHid = CSpotListProcessor::GetVirtualHID();

CGestureEventGenerator::CGestureEventGenerator(void)
:m_CollectCount(0),
m_eWBS(E_WBS_IDLE),
m_bGetstureDone(TRUE),
m_bCollectWheelState(FALSE),
m_bMouseWheelState(FALSE),
m_eRecordState(E_START),
m_eMouseWheelState(E_GESTUREEVENT_START_COLLECT),
m_eCollectState(E_COLLECT_START),
m_nCollectTotileCount(0),
m_nMobileDistance(0),
m_nOnePointCount(0),
m_nCollectMissCount(0),          //采集时的无效的坐标的个数
m_nOneMissCount(0),
IsMouseLeftEdge(FALSE),
m_Pre_CollectCount(0),
m_isGREnabled(true),
m_isMultiPenPermited(true),
m_isTouchPadMode(false),
m_nMultiEraser(4),
m_isMouseUp(false),
m_isSmartMatchReset(true),
m_isLogPermit(false),
m_isVMouseSelfCotrol(false),
m_isGLBoardProcExsit(false),
m_conNonOneTouch_when_penstate(0),
m_isInuptInGLBoard(false),
m_bHadResetAllGRState(true),
m_isPenTouchDevice(false),
m_isMultiPenMode(false)
{
	m_nScreenX =  GetSystemMetrics(SM_CXSCREEN)  ;
	m_nScreenY =  GetSystemMetrics(SM_CYSCREEN)  ;

#ifdef _DEBUG
    m_isLogPermit = true;
#endif
}

CGestureEventGenerator::~CGestureEventGenerator(void)
{

}

void CGestureEventGenerator::SetSpotProportion(int nSpot)
{
	m_nSpotProportion = nSpot;
}

void CGestureEventGenerator::SetMultiEraser(int nEraser)
{
	g_oWGRConfig.SetMultipleEraser2Std(nEraser);
}

int CGestureEventGenerator::GetMultiEraser()
{
    return g_oWGRConfig.GetMultipleEraser2Std();
}

void CGestureEventGenerator::EnableGestureRecognition(bool bEnable)
{
    g_oWGRConfig.SetGLBoardGRPermission(bEnable);
}

bool CGestureEventGenerator::IsGREnabled()
{
    return g_oWGRConfig.IsGLBoardGRPermit();
}

void CGestureEventGenerator::CancelMouseButtonUp(const TLightSpot * aryLightSpots,  int nCount)
{
    if (!m_isMouseUp)
    {
        if (nCount > 0)
        {
            GetVirtualMouse().GenEvent(aryLightSpots[0].ptPosInScreen, E_LBUTTON_UP);
            m_isMouseUp = true;
        }
    }
}

void CGestureEventGenerator::SimulHIDReset(const POINT &pt)
{
    m_oSmartMatch.Reset();
    refVirtualHid.Reset(); //首先Reset    
}

void CGestureEventGenerator::SimulHIDDown(const POINT &pt)
{          
   /* POINT ptTmp = pt;
    g_oRefVirtualMouse.GenEvent(ptTmp, E_LBUTTON_DOWN_MOVE);
    ptTmp.x += 10;
    ptTmp.y += 10;
    g_oRefVirtualMouse.GenEvent(ptTmp, E_LBUTTON_DOWN_MOVE);
    ptTmp.x += 10;
    ptTmp.y += 10;
    g_oRefVirtualMouse.GenEvent(ptTmp, E_LBUTTON_DOWN_MOVE);    
    g_oRefVirtualMouse.GenEvent(ptTmp, E_LBUTTON_UP);  
    ptTmp.x -= 20;
    ptTmp.y -= 20;*/

    m_oSmartMatch.Reset();
    refVirtualHid.Reset(); //首先Reset设备
   
    /*g_oRefVirtualMouse.GenEvent(pt, E_LBUTTON_UP);
    g_oRefVirtualMouse.GenEvent(pt, E_LBUTTON_DOWN);*/

    //Sleep(100);

    m_isVMouseSelfCotrol = true;
}

void CGestureEventGenerator::ResetSmartMathch()
{
    if (!m_isSmartMatchReset)
    {
        //m_oSmartMatch.Reset();
        if (StrStatus ==_T("multipen"))
        {
            m_GRMultiPen.ResetSmartMatch();
        }
        else
        {            
            Input2Device(NULL, 0);       
        }         

        m_isSmartMatchReset = true;
    }    
}

void CGestureEventGenerator::OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
{
    m_oStrokFilter.OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);
}

void CGestureEventGenerator::Input2Device(const POINT *ptArry, const int &nActualCount, bool needMatch)
{
    TContactInfo penInfo[MAXMATCH_NUM];
    int nElementCount = 0;

    if (needMatch)
    {
        m_oSmartMatch.DoMatch(ptArry, nActualCount);
        const TMatchInfo* pMatchInfo =  m_oSmartMatch.GetAllMatchInfo(&nElementCount);
 
        //g_oGLTraceLog.AddSpotInfo((GLMatchInfo*)pMatchInfo, nElementCount);

        if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);


        /*
        @说明：当板擦移动过程中，出现错误匹配时，uid=0的板擦弹起，而uid=*的板擦按下，而GLBoard中板擦是鼠标响应的，而鼠标接受的是uid=0笔
        */
        if (m_eWBS == E_WBS_ERASER && nElementCount > 1)
        {//
            for (int i = 0; i < nElementCount; i++)
            {
                const TMatchInfo &refMatchInfo = pMatchInfo[i];

                if (refMatchInfo.uId == 0)
                {
                    if (refMatchInfo.eMatchState == E_MISMATCHED)
                    {//uid = 0 橡皮擦失配了
                        //
                        m_oSmartMatch.Reset();
                        refVirtualHid.Reset();

                        m_oSmartMatch.DoMatch(ptArry, nActualCount);
                        pMatchInfo =  m_oSmartMatch.GetAllMatchInfo(&nElementCount);

                        if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);
                    }

                    break;
                }
            }
        }

        g_oGLTraceLog.AddSpotInfo((GLMatchInfo*)pMatchInfo, nElementCount);
        
        for(int i = 0; i < nElementCount; i++)
        {
            penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
            penInfo[i].uId       = pMatchInfo[i].uId;
            penInfo[i].pt        = pMatchInfo[i].ptPos; 

            /* if (nActualCount == 1)
            {
            AtlTrace(_T("########MatchState : %d  nElementCount\r\n"), pMatchInfo[i].eMatchState);            
            }*/
        }  
    }
    else if (nActualCount > 0)
    {
        penInfo[0].ePenState = E_PEN_STATE_DOWN;
        penInfo[0].uId = 0;
        penInfo[0].pt = ptArry[0];
        nElementCount = 1;
    }

    m_oStrokFilter.DoFilter(penInfo, nElementCount);

    refVirtualHid.InputPoints(penInfo, nElementCount);
}

BOOL CGestureEventGenerator::FindTboardWnd(const TLightSpot * aryLightSpots,  const int &nCount)
{       
    BOOL bRet = TRUE;
    //bool isGLBoardProcExist = false;
    //do 
    //{
    //    isGLBoardProcExist = CProcessUtility::IsTheSpecWatchProcessExisted();

    //    if (!isGLBoardProcExist)
    //    {            
    //        m_hwnd = NULL;

    //        bRet = FALSE;

    //        break;
    //    }

    //    ///*if (!m_isGLBoardProcExsit || m_hwnd == NULL) */
    //    m_hwnd = FindWindow(NULL,_T("tBoard"));
    //    if (!IsWindowVisible(m_hwnd)) {bRet = FALSE; break;}       

    //    //if (m_hwnd == NULL || !IsWindowVisible(m_hwnd)) {bRet = FALSE; break;}

    //    if (nCount <= 0)
    //    {
    //        if (m_eWBS == E_WBS_IDLE)
    //        {
    //            bRet = FALSE;
    //        }
    //        else
    //        {
    //            bRet = TRUE;
    //        }

    //        break;
    //    }

    //    bRet = CProcessUtility::IsPointInTheSpecialWatchProcess(aryLightSpots[0].ptPosInScreen);

    //    m_isSmartMatchReset = false;
    //   
    //} while (false);

    //m_isGLBoardProcExsit = isGLBoardProcExist;
    do 
    {
        if (nCount <= 0)
        {
            if (m_bHadResetAllGRState)
            {
                bRet = FALSE;
            }            

            break;
        }

        //新加坡客户
        //m_hwnd = FindWindow(NULL, _T("V-Notes 3.0"));
        //易视
        //m_hwnd = FindWindow(NULL, _T("tBoard"));
        //{{OEM}}
      m_hwnd = FindWindow(NULL, _T("tBoard"));
        //{{OEM}}
        

        if (m_hwnd == NULL || !IsWindowVisible(m_hwnd)) {bRet = FALSE; break;}    

        RECT rc;
        GetWindowRect(m_hwnd, &rc);

        if (!PtInRect(&rc, aryLightSpots[0].ptPosInScreen)) {bRet = FALSE; break;}    

        m_isSmartMatchReset = false;
    } while (false);

    m_isInuptInGLBoard = bRet?true:false;

	return bRet;
}

bool CGestureEventGenerator::DoDetection(const TLightSpot * aryLightSpots,  const int &nCount)
{
    bool isResponse = true;            
    //
    //允许多笔，检测当前白板是不是选择了多笔，
    //如果是，则只允许橡皮擦和多笔书写
    ///*
    //    多笔状态的需要满足如下条件之一：
    //    a.StrStatus = multipen;
    //    b.PreStatus = multipen, 且StrStatus != (Pencil || zineng || pen);
    //*/
    //bool isMultiPenMode = (StrStatus == _T("multipen"));
    //isMultiPenMode = (isMultiPenMode || ((PreStrStatus == _T("multipen")) && ((StrStatus != _T("Pen") && (StrStatus != _T("Pencil") && (StrStatus != _T("zineng"))))

    if (m_isMultiPenMode)
    {
        //白板软件正处于多笔模式 
        m_GRMultiPen.SetGLBoadHWnd(m_hwnd);
        m_GRMultiPen.SetTouchPadMode(m_isTouchPadMode);
        m_GRMultiPen.SetEableGR((g_oWGRConfig.IsGLBoardGRPermit()) && !m_isPenTouchDevice); //只有当手触模式且允许手势识别时才开启多笔手势识别
        isResponse = m_GRMultiPen.DoGestureRecognition(aryLightSpots, nCount, (g_oWGRConfig.GetMultipleEraser2Std()));

        m_bHadResetAllGRState = isResponse;

        return isResponse;
    }
    else
    {
        m_bHadResetAllGRState = false;
        if (false == (g_oWGRConfig.IsGLBoardGRPermit()))     //不允许手势识别
        {
            m_bHadResetAllGRState = true;
            return true;
        }
        if(nCount == 0)
        {
            m_bHadResetAllGRState = true;
        }
        else
        {
            m_bHadResetAllGRState = false;
        }

        //if (m_eWBS == E_WBS_IDLE && nCount == 0) return true;
        isResponse = EraserTransPen(aryLightSpots, nCount)?true:false;
        

        POINT pts[MAX_CAMERA_NUMBER*MAX_OBJ_NUMBER];
        int   nActualCount;        

        if (isResponse)
        {
            //////如果是在触屏模式下，且当前状态是图片旋转缩放操作，
            //////则直接使用微软多点触屏旋转缩放，否则仅进行把pptMouse单点作为输入传递到HID设备中
            ////if(!(m_isTouchPadMode && (m_eWBS == E_WBS_ZOOM || m_eWBS == E_WBS_RORATE)))      
            ////{
            ////    pts[0] = m_ptResponse;
            ////    nActualCount = 1; 

            ////    /*if (m_eWBS == E_WBS_SELECTION)
            ////    {
            ////        AtlTrace(_T("<<<Points Sets :\r\n"));
            ////        for (int i = 0; i < nCount; i++)
            ////        {     
            ////            AtlTrace(_T("Point%d = (%d, %d)"), i, aryLightSpots[i].ptPosInScreen.x, aryLightSpots[i].ptPosInScreen.y);
            ////        }
            ////        AtlTrace(_T("\r\nPoints Sets :\r\n>>>"));
            ////    }*/
            ////}
            ////else
            ////{
            ////    for(int i = 0; i < nCount; i++)
            ////    {
            ////        pts[i] = aryLightSpots[i].ptPosInScreen;
            ////    }
            ////    nActualCount = nCount;
            ////} 
            //g_oRefVirtualMouse.GenEvent(m_ptResponse, E_LBUTTON_DOWN_MOVE);
            Input2Device(&m_ptResponse, 1);
        }
        else
        {
            if (m_eWBS == E_WBS_ERASER && nCount > 0)
            {//橡皮擦仅使用最大光斑点作为输入  
                //m_ptResponse = aryLightSpots[0].ptPosInScreen;
                //g_oRefVirtualMouse.GenEvent(m_ptResponse, E_LBUTTON_DOWN_MOVE);      
                Input2Device(&m_ptResponse, 1);
            }
            else
            {
                if (m_isVMouseSelfCotrol)
                {
                    m_isVMouseSelfCotrol = false;
                    /*g_oRefVirtualMouse.GenEvent(m_ptResponse, E_LBUTTON_UP);*/
                    /*Input2Device(NULL, 0, false);*/
                    //refVirtualHid.Reset(); 
                    //m_oSmartMatch.Reset();
                }

                for(int i = 0; i < nCount; i++)
                {
                    pts[i] = aryLightSpots[i].ptPosInScreen;
                }
                nActualCount = nCount;

                Input2Device(pts, nActualCount);
            }                    
        }           

        g_oGLTraceLog.AddSpotCount(nCount);        

        if (((m_eWBS == E_WBS_IDLE || m_eWBS == E_WBS_SPOTLIGHT || m_eWBS == E_WBS_RETURNTODISKTOP || m_eWBS == E_WBS_MASK) && nCount == 0) && g_bHasLogStarted)
        {
            StopTraceLog();
        }

        return false;
    }        
}

void CGestureEventGenerator::Restore2OldPenState()
{   

    if (PreStrStatus == _T("Pencil"))
    {
        ::PostMessage(m_hwnd, MyWM_But,MyBut_Pen,MyBut_Pen);
    }
    else if (PreStrStatus == _T("zineng"))
    {
        ::PostMessage(m_hwnd, MyWM_But,MyBut_SmartPen,MyBut_SmartPen);
    }
    else if (PreStrStatus == _T("multipen"))
    {
        ::PostMessage(m_hwnd, MyWM_But,MyBut_MultiPen,MyBut_MultiPen);
    }
    else
    {
        ::PostMessage(m_hwnd, MyWM_But,MyBut_SoftPen,MyBut_SoftPen);
    }
}

//@功能:在白板软件中，笔和板擦之间的状态转换函数
//@参数:aryLightSpots,按照大小从大到小排列的光斑数组
//      nCount, 光斑个数
//      pptMouse, 指向保存"推荐鼠标"位置的内存空间的指针
BOOL  CGestureEventGenerator::EraserTransPen(const TLightSpot * aryLightSpots,  int nCount)
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	if (nCount == 0)
	{
        /*if(m_eWBS == E_WBS_SELECTION)
        {
            AtlTrace(_T("*******************Stroke Selection End : \r\n"));
        }*/

		if(m_eWBS == E_WBS_ERASER)
		{
			::PostMessage(m_hwnd,MyWM_XY,MyXY_End,0);
		}

        if (m_eWBS != E_WBS_IDLE && m_eWBS != E_WBS_SELECTION && m_eWBS != E_WBS_RORATE && m_eWBS != E_WBS_ZOOM && m_eWBS != E_WBS_SPOTLIGHT && m_eWBS != E_WBS_MASK && m_eWBS != E_WBS_RETURNTODISKTOP)
        {
            Restore2OldPenState();           
        }

        if(m_eWBS == E_WBS_ZOOM || m_eWBS == E_WBS_RORATE)
        {
            ::PostMessage(m_hwnd,MyWM_But,MyBut_Select,MyBut_Select);
        }

		if(m_eWBS != E_WBS_SPOTLIGHT || m_eWBS != E_WBS_MASK)
        {
            m_eWBS = E_WBS_IDLE ;
            m_isMouseUp = false;
        }
       /* else
        {
            m_oFingerDoubleClickChecker.SetDoubleClickStartState(TRUE)   ;
            m_oHandDoubleClickChecker.SetHandDoubleClickStartState(TRUE) ;
        }*/        

		m_CollectCount = 0 ;
		m_eRecordState = E_START;               //这个是板擦或者笔的采集
		m_eCollectState = E_COLLECT_START ;     //放大缩小或者是旋转或者是漫游的采集
		IsMouseLeftEdge =  FALSE ;

        /*::PostMessage(m_hwnd,MyWM_But,MyBut_Select,MyBut_Select);
        g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_MOVE);*/

		m_nCollectMissCount = 0    ;
		m_nCollectTotileCount  =0  ;
		m_Pre_CollectCount   =  0 ;
		m_bGetstureDone = TRUE;

        m_oFingerDoubleClickChecker.SetDoubleClickStartState(TRUE)   ;
        m_oHandDoubleClickChecker.SetHandDoubleClickStartState(TRUE) ;

        m_conNonOneTouch_when_penstate = 0;        
		
		return IsReponseByMyself(m_eWBS);
	}      

    if (m_eWBS == E_WBS_IDLE && nCount >= 1 && m_isLogPermit && !g_bHasLogStarted)
    {
        StartTraceLog();
    }

    if (g_bHasLogStarted)
    {
        g_nCurrentCollect++;            
    }

    POINT ptEraser = aryLightSpots[0].ptPosInScreen ;
    bool isEraser = false;

    const TLightSpot &refSpot = aryLightSpots[0];
    int nStdAreaByMulti = (g_oWGRConfig.GetMultipleEraser2Std()) * refSpot.lStdSpotAreaInVideo; 
    int nWeightedMass = g_oWGRConfig.GetWeightedSpotMass(refSpot.mass, refSpot.lAreaInVideo, nStdAreaByMulti);
    if (nWeightedMass > nStdAreaByMulti) isEraser = true;

	/////////////////////////////////////////////////////////
	////需要把所有的光斑加起来进行判断
 //   int nSumMass = aryLightSpots[0].mass;
 //   int nSumArea = aryLightSpots[0].lAreaInVideo;
 //   POINT ptEraser = aryLightSpots[0].ptPosInScreen ;
 //   bool isEraser = false;

 //   {
 //       //由于lAreaInVideo是外接矩形的面积，且该外接矩形与mass相差较大，尤其笔快速移动时，所以采用如下方法来平衡
 //       /*
 //       @平衡时的条件：mass < m_nMulti * lStdSpotAreaInVideo，且lAreaInVideo > m_nMulti * lStdSpotAreaInVideo
 //       @方法：
 //            令k0, k1为正数， k0 < k1 且 2*k0 > k1
 //              lambda = (mass / lAreaInVideo) * (k0 / k1)            

 //              T = (1 - lambda) * mass + lambda * lAreaInVideo

 //              T = [(k0+k1)*mass + k1 - 1] / k1 - (k0*mass*mass + k1*lAreaInVideo - 1) / k1*lAreaInVideo
 //       */
 //       int nStdAreaByMulti = m_nMultiEraser * aryLightSpots[0].lStdSpotAreaInVideo;
 //       if (nSumArea > nStdAreaByMulti)
 //       {
 //           if (nStdAreaByMulti >= nSumMass)
 //           {
 //               const int k0 = 1;
 //               const int k1 = 3;

 //               int k0_m = k0 * nSumMass;
 //               int k0_m_m = k0_m * nSumMass;
 //               int k1_m = k1 * nSumMass;
 //               int k1_r = k1 * nSumArea;

 //               int nWeightedMass = (k0_m + k1_m + k1 - 1) / k1 - (k0_m_m + k1_r - 1) / k1_r;

 //               if (nWeightedMass > nStdAreaByMulti) isEraser = true;                

 //               AtlTrace(_T("$$$$$$$ nSumArea = %d nSumMass = %d nWeightedMass = %d nStdAreaByMulti = %d \r\n"), nSumArea, nSumMass, nWeightedMass, nStdAreaByMulti);
 //           }
 //           else
 //               isEraser = true;
 //       }
 //       
 //   }
    ////AtlTrace(_T("++++++++Before Merge: nSumMass = %d std = %d\r\n"), nSumArea, aryLightSpots[0].lStdSpotAreaInVideo);
    //AtlTrace(_T("+++++++++Point%d = <%d, %d> \r\n"), 0, aryLightSpots[0].ptPosInScreen.x, aryLightSpots[0].ptPosInScreen.x);
    //int nMerged = 0;
    //if (((m_eWBS == E_WBS_IDLE || m_eWBS == E_WBS_PEN) && nSumArea > (m_nMultiEraser - 1) * aryLightSpots[0].lStdSpotAreaInVideo) || E_WBS_ERASER == m_eWBS)
    //{
    //    //POINT MaxPt = aryLightSpots[0].ptPosInScreen ;        

    //    AtlTrace(_T("*****nCount = %d \r\n"), nCount);

    //    //int xLimit = 120;
    //    //int yLimit = 120;

    //    ////
    //    //xLimit = g_oWGRConfig.MapStandardX2ActualValue(xLimit);
    //    //yLimit = g_oWGRConfig.MapStandardY2ActualValue(yLimit);
    //    int thresh_100 = g_oWGRConfig.MapStandardX2ActualValue(100);
    //    int MERGE_THRESHOLD = thresh_100 * thresh_100;//

    //    for (int i = 1 ;i< nCount ; i++)
    //    {
    //        AtlTrace(_T("*******Point%d = <%d, %d> \r\n"), i, aryLightSpots[i].ptPosInScreen.x, aryLightSpots[i].ptPosInScreen.x);
    //        const POINT &refPoint = aryLightSpots[i].ptPosInScreen;

    //        int dx = ptEraser.x - refPoint.x;
    //        int dy = ptEraser.y - refPoint.y;

    //        int R2 = dx*dx + dy*dy;//两个光斑之间的距离

    //        if (R2  < MERGE_THRESHOLD)
    //        {
    //            const int &refMass = aryLightSpots[i].mass;
    //            ptEraser.x = (ptEraser.x * nSumMass + refPoint.x * refMass) / (nSumMass + refMass);
    //            ptEraser.y = (ptEraser.y * nSumMass + refPoint.y * refMass) / (nSumMass + refMass);

    //            nSumArea += aryLightSpots[i].lAreaInVideo;
    //            nSumMass += aryLightSpots[i].mass;

    //            nMerged++;
    //        }
    //    }
    //}

    //g_oGLTraceLog.AddSpotArea(nSumArea);
    //g_oGLTraceLog.AddStdArea(aryLightSpots[0].lStdSpotAreaInVideo);

    //AtlTrace(_T("________After Merge: nSumMass = %d std*multi = %d, nMerged = %d\r\n"), nSumArea, aryLightSpots[0].lStdSpotAreaInVideo * m_nMultiEraser, nMerged);

	//AtlTrace(_T("aryLightSpots0mass =%d, SunMass =%d ,calArea = %d nCount = %d\r\n"),aryLightSpots[0].mass, nSumMass,aryLightSpots[0].lStdSpotAreaInVideo * m_nMultiEraser, nCount);
    //BOOL bRet = TRUE ;   

    //
    LONG  DiffX = 0;
    LONG  DiffY = 0;
    //

	switch(m_eWBS)
	{
	case E_WBS_SPOTLIGHT:      //已经判断在聚光灯模式下的操作
		{
			//bRet = TRUE ;

            if(m_oFingerDoubleClickChecker.FingerDoubleClick(aryLightSpots[0].ptPosInScreen))
            {
                //检测到是双击的时间。然后判断在此之前是那种笔的书写类型，根据书写类型变成响应的笔的类型

                Restore2OldPenState();

                m_eWBS = E_WBS_PEN;
            }
			break;
		}
	case E_WBS_RETURNTODISKTOP:   //已经返回桌面的模式下操作
		{
			//bRet = FALSE ;
			break;
		}

	case E_WBS_MASK:               //已经判断为幕布模式下操作
		{
			//bRet = FALSE ;
			break;

		}
	case E_WBS_NEXTPAGE:            //已经判断为下一页模式下操作
		{
    		//bRet = FALSE ;
			break;
		}

	case  E_WBS_PROPAGE:           //已经判断为上一页模式下操作
		{
			//如果已经判断出上一页的模式时，白板已经跳到了上一页，因此后面的鼠标就不做任何操作了。
			//bRet = FALSE ;
			break;
		}

	case E_WBS_ERASER:               //已经判断在板擦的模式下操作
		{
            m_ptResponse = ptEraser;
            //已经判断为板擦，再判断鼠标的坐标位置，如果鼠标的坐标位置在最左边的话，有可能是幕布的功能，在根据运动的距离来确定是不是幕布。
            //需要采集很多次判断的。
            if(/*nSumArea > aryLightSpots[0].lStdSpotAreaInVideo * m_nMultiEraser*/isEraser)
            {
                m_nCollectMissCount = 0;
                //已经判断为板擦，然后在判断是不是大的光斑在一个点上长时间的停留，如果是长时间的停留，那就是聚光灯的模式了，如果不是，仍然是板擦的模式
                if (m_oLongTimeClickTrigger.DetectLongTimeStay(aryLightSpots[0].ptPosInScreen))
                {
                    //聚光灯
                    PostMessage(m_hwnd,MyWM_SpotLight,aryLightSpots[0].ptPosInScreen.x ,aryLightSpots[0].ptPosInScreen.y );
                    m_eWBS = E_WBS_SPOTLIGHT ;
                }

                AtlTrace(_T("Cur Eraser Point = (%d, %d) \r\n"), ptEraser.x, ptEraser.y);

               // m_nCollectMissCount = 0;                

            }
            else
            {
                m_nCollectMissCount++;

                //AtlTrace("m_nCollectMissCount = %d \r\n", m_nCollectMissCount);

                if (m_nCollectMissCount > 100)
                {
                    m_nCollectMissCount = 0;
                    m_eWBS = E_WBS_IDLE;    
                    m_CollectCount = 0 ;
                    m_eRecordState = E_START ;                    
                    m_eCollectState = E_COLLECT_START ;     //放大缩小或者是旋转或者是漫游的采集
                    IsMouseLeftEdge =  FALSE ;

                    Restore2OldPenState();

                    //g_oVirtualMouse.GenEvent(CMouseEventGenerator::SIMUL_MOUSE_STATE_IDLE) ;
                    
                    m_nCollectTotileCount  =0  ;
                    m_Pre_CollectCount   =  0 ;
                    m_bGetstureDone = TRUE;
                    m_oFingerDoubleClickChecker.SetDoubleClickStartState(TRUE)   ;
                    //m_oHandDoubleClickChecker.SetHandDoubleClickStartState(TRUE) ;

                    //bRet = FALSE;
                    break;
                }

                ////已经判断为板擦，然后在判断是不是大的光斑在一个点上长时间的停留，如果是长时间的停留，那就是聚光灯的模式了，如果不是，仍然是板擦的模式
                //if (m_oLongTimeClickTrigger.DetectLongTimeStay(aryLightSpots[0].ptPosInScreen))
                //{
                //    //聚光灯
                //    PostMessage(m_hwnd,MyWM_SpotLight,aryLightSpots[0].ptPosInScreen.x ,aryLightSpots[0].ptPosInScreen.y );
                //    m_eWBS = E_WBS_SPOTLIGHT ;
                //}
            }

            //bRet = TRUE ;
			break;
		}

	case E_WBS_PEN:                          //已经判断为笔的模式了
		{
			////已经判断在笔的状态下，如果出现大的光斑的话，就得返回到空闲模式，在进行板擦的判断是不是板擦。
			//if(nSumMass > aryLightSpots[0].lStdSpotAreaInVideo * m_nMultiEraser)
			//{
			//	m_eWBS = E_WBS_IDLE ;
			//}
			////如果不是出现大的光斑，而是出现了多于一个的光斑，
			//else
			//{
			//	//说明在书写的过程中出现了多于1个的光斑，多于一个光斑的话就有可能是漫游或者是放大缩小或者是旋转都有可能，
			//	//漫游有距离的限制，两点之间的距离需要大于一定的数，但是放大或缩小还有旋转之间是根据图片的大小，这是没有
			//	//距离限制的，因此只能退回到空闲状态重新判断。
				if (nCount > 1)
				{
                    m_conNonOneTouch_when_penstate ++;

                    if (m_conNonOneTouch_when_penstate > 5)
                    {
                        m_eWBS = E_WBS_IDLE ;
                        m_conNonOneTouch_when_penstate = 0;
                    }					
				}
                else
                {
                    m_conNonOneTouch_when_penstate = 0;
                }
			//}

			m_bGetstureDone = TRUE ;  //手势判断结束
			//bRet = TRUE ;
			break;
		}
	case E_WBS_ROAM:                                //已经判断为漫游的模式了。
		{			
            //说明是正在漫游中，漫游的距离就跟随水平坐标最小的光斑
			//if (nCount == 2)
   //         {
   //             m_nCollectMissCount = 0;
   //             POINT ptRoam = aryLightSpots[0].ptPosInScreen ;
   //             for (int i = 1 ;i< nCount ;i++ )
   //             {
   //                 if (ptRoam.x >= aryLightSpots[i].ptPosInScreen.x )
   //                 {
   //                     ptRoam = aryLightSpots[i].ptPosInScreen ;
   //                 }
   //             }
   //             /////pptMouse是函数传递过来的指针鼠标
   //             ////把最小的鼠标值传递出去用来作为鼠标的模拟值的。
   //             //if (pptMouse)
   //             //{
   //             //    *pptMouse = ptRoam ;
   //             //}
   //             m_ptResponse = ptRoam;

   //             m_bGetstureDone = TRUE ;
   //             //bRet = TRUE ;                
   //         }	
            Match2Touchs(aryLightSpots, nCount);
			//else
   //         ////已经判断是漫游了，如果连续出现不是两个点次数超过10次时，说明漫游已经结束了。
   //         {
   //             m_nCollectMissCount++;
   //             if (m_nCollectMissCount > 10)
   //             {
   //                 ::PostMessage(m_hwnd,MyWM_XY,MyXY_End,0);
   //                 m_CollectCount = 0 ;
   //                 m_eRecordState = E_START ;
   //                 m_bGetstureDone = TRUE ;                    
   //                 m_nCollectMissCount = 0;
   //             }	

   //             m_bGetstureDone = TRUE ;
   //             //bRet = TRUE;                
   //         }
					
            //***************************************************

            //E_2TOUCHS_MOVES eMove = Calc2TouchsMoveDirection(Match2Touchs(aryLightSpots, nCount), pptMouse);
            //if (eMove != E_2TOUCHS_MOVES_NONE && eMove != E_2TOUCHS_MOVES_ROAM)
            //{
            //    m_nCollectMissCount++;
            //    if (m_nCollectMissCount > TWOTOUCHS_MISSTOLERANCE_OFFSTATE)
            //    {
            //        m_nCollectMissCount = 0;
            //        ::PostMessage(m_hwnd,MyWM_XY,MyXY_End,0);
            //        m_CollectCount = 0 ;
            //        m_eRecordState = E_START ;
            //        m_eWBS = E_WBS_IDLE;
            //        m_eCollectState = E_COLLECT_START ;     //放大缩小或者是旋转或者是漫游的采集
            //        IsMouseLeftEdge =  FALSE ;

            //        m_nCollectMissCount = 0    ;
            //        m_nCollectTotileCount  =0  ;
            //        m_Pre_CollectCount   =  0 ;
            //        m_bGetstureDone = TRUE;
            //        m_oFingerDoubleClickChecker.SetDoubleClickStartState(TRUE)   ;
            //        m_oHandDoubleClickChecker.SetHandDoubleClickStartState(TRUE) ;
            //    }            	            	
            //	m_bGetstureDone = TRUE ;
            //	bRet = FALSE ;                
            //}
            ////说明是正在漫游中，漫游的距离就跟随水平坐标最小的光斑
            //else
            //{
            //    m_nCollectMissCount = 0;
            ////	POINT ptRoam = aryLightSpots[0].ptPosInScreen ;
            ////	for (int i = 1 ;i< nCount ;i++ )
            ////	{
            ////		if (ptRoam.x >= aryLightSpots[i].ptPosInScreen.x )
            ////		{
            ////			ptRoam = aryLightSpots[i].ptPosInScreen ;
            ////		}
            ////	}
            //////	///pptMouse是函数传递过来的指针鼠标
            ////	if (pptMouse)
            ////	{
            ////		*pptMouse = ptRoam ;
            ////	}

            //    m_bGetstureDone = TRUE ;
            //    bRet = TRUE ;
            //}
			break;
		}
	case E_WBS_ZOOM:                         //已经判断为放大或者缩小的模式了、
		//{
		//	if (nCount == 1)
		//	{
		//		////////所有的变量都需要初始化
		//		bRet = FALSE  ;
		//		m_bGetstureDone = TRUE  ;
		//		g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_IDLE) ;

		//	}
		//	else
		//	{
		//		////旋转的话不能根据x轴的大小来判断，因为这个是变化的，不确定的，因此用距离来判断
		//		///用来判断是那个光斑用于鼠标的操作。
		//		long DisX0 = (m_Start_MousePoint[0].x - aryLightSpots[0].ptPosInScreen.x) * (m_Start_MousePoint[0].x - aryLightSpots[0].ptPosInScreen.x) 
		//			        +(m_Start_MousePoint[0].y - aryLightSpots[0].ptPosInScreen.y) * (m_Start_MousePoint[0].y - aryLightSpots[0].ptPosInScreen.y) ;

		//		long DisX1 = (m_Start_MousePoint[0].x - aryLightSpots[1].ptPosInScreen.x) * (m_Start_MousePoint[0].x - aryLightSpots[1].ptPosInScreen.x)
		//			        +(m_Start_MousePoint[0].y - aryLightSpots[1].ptPosInScreen.y) * (m_Start_MousePoint[0].y - aryLightSpots[1].ptPosInScreen.y) ;

		//		if (DisX0 < DisX1)
		//		{
		//			m_Start_MousePoint[0] = aryLightSpots[0].ptPosInScreen ;
		//		}
		//		else
		//		{
		//			m_Start_MousePoint[0] = aryLightSpots[1].ptPosInScreen ;
		//		}

		//		if (pptMouse)
		//		{
		//			*pptMouse = m_Start_MousePoint[0]  ;
		//		}

		//		m_bGetstureDone = TRUE ;

		//		bRet  = TRUE    ;
		//	}
  //          //**************************************************************************
  //          //E_2TOUCHS_MOVES eMove = Calc2TouchsMoveDirection(aryLightSpots, nCount, pptMouse);
  //          // //AtlTrace(_T("Zoom: %d \r\n"), eMove);
  //          // if (eMove != E_2TOUCHS_MOVES_NONE && eMove != E_2TOUCHS_MOVES_ZOOM)
  //          // {
  //          //     m_nCollectMissCount ++;
  //          //     if (m_nCollectMissCount > TWOTOUCHS_MISSTOLERANCE_OFFSTATE)
  //          //     {   
  //          //         m_nCollectMissCount = 0;
  //          //         //::PostMessage(m_hwnd,MyWM_But,MyBut_Select,MyBut_Select);                    
  //          //         m_CollectCount = 0 ;
  //          //         m_eRecordState = E_START ;
  //          //         m_eWBS = E_WBS_IDLE;
  //          //         m_eCollectState = E_COLLECT_START ;     //放大缩小或者是旋转或者是漫游的采集
  //          //         IsMouseLeftEdge =  FALSE ;

  //          //         m_nCollectMissCount = 0    ;
  //          //         m_nCollectTotileCount  =0  ;
  //          //         m_Pre_CollectCount   =  0 ;
  //          //         m_bGetstureDone = TRUE;
  //          //         m_oFingerDoubleClickChecker.SetDoubleClickStartState(TRUE)   ;
  //          //         m_oHandDoubleClickChecker.SetHandDoubleClickStartState(TRUE) ;
  //          //         //g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_MOVE) ;
  //          //     }
  //          //     
  //          //     bRet = FALSE  ;
  //          //     m_bGetstureDone = TRUE  ;
  //          // }
  //          // else
  //          // {
  //          // 	////旋转的话不能根据x轴的大小来判断，因为这个是变化的，不确定的，因此用距离来判断
  //          // 	///用来判断是那个光斑用于鼠标的操作。
  //          // 	/*long DisX0 = (m_Start_MousePoint[0].x - aryLightSpots[0].ptPosInScreen.x) * (m_Start_MousePoint[0].x - aryLightSpots[0].ptPosInScreen.x) 
  //          // 		        +(m_Start_MousePoint[0].y - aryLightSpots[0].ptPosInScreen.y) * (m_Start_MousePoint[0].y - aryLightSpots[0].ptPosInScreen.y) ;

  //          // 	long DisX1 = (m_Start_MousePoint[0].x - aryLightSpots[1].ptPosInScreen.x) * (m_Start_MousePoint[0].x - aryLightSpots[1].ptPosInScreen.x)
  //          // 		        +(m_Start_MousePoint[0].y - aryLightSpots[1].ptPosInScreen.y) * (m_Start_MousePoint[0].y - aryLightSpots[1].ptPosInScreen.y) ;

  //          // 	if (DisX0 < DisX1)
  //          // 	{
  //          // 		m_Start_MousePoint[0] = aryLightSpots[0].ptPosInScreen ;
  //          // 	}
  //          // 	else
  //          // 	{
  //          // 		m_Start_MousePoint[0] = aryLightSpots[1].ptPosInScreen ;
  //          // 	}

  //          // 	if (pptMouse)
  //          // 	{
  //          // 		*pptMouse = m_Start_MousePoint[0]  ;
  //          // 	}*/                 

  //          // 	m_bGetstureDone = TRUE ;

  //          // 	bRet  = TRUE    ;
  //          // }

		//	break;
		//}
	case E_WBS_RORATE:                        //已经判断为旋转的模式了。
		{			

            //Calc2TouchsMoveDirection(Match2Touchs(aryLightSpots, nCount), &m_ptResponse);
            Match2Touchs(aryLightSpots, nCount);

			break;
		}

	case E_WBS_SELECTION:                            //已经判断为选择对象的模式了。
		{            
            
            //*************************************
            int xSum = 0;
            int ySum = 0;

            for (int i = 0; i < nCount; i++)
            {
                if (abs(aryLightSpots[i].ptPosInScreen.x - m_ptResponse.x) < 100 && abs(aryLightSpots[i].ptPosInScreen.y - m_ptResponse.y) < 100)
                {//剔除干扰点（与上一次平均值相差过大的点）
                    xSum += aryLightSpots[i].ptPosInScreen.x;
                    ySum += aryLightSpots[i].ptPosInScreen.y;
                }
            }

            if (nCount > 0)
            {
                m_ptResponse.x = (xSum + nCount - 1) / nCount;
                m_ptResponse.y = (ySum + nCount - 1) / nCount;
            }

			m_bGetstureDone = TRUE  ;
			//bRet  = TRUE  ;
			break;
		}

	case E_WBS_IDLE:
		{
			//这里是手势不在如何的状态下，所有的手势是从头来判断的，
			//判断是橡皮擦的条件是只要出现的光斑的倍数大于计算光斑的响应倍数的话，说明就是出现了大光斑。
			if(isEraser)
			{
    //            //::PostMessage(m_hwnd,MyWM_But,MyBut_NonMode,MyBut_NonMode)   ;    //切换到白板无模式状态
				////////如果IsMouseLeftEdge为ture的话，说明两次采集到的鼠标是在屏幕的边缘的。
				//if (IsMouseLeftEdge)
				//{
				//	int deltAx = aryLightSpots[0].ptPosInScreen.x - m_ptInputCoord.x ;
				//	int deltAy = aryLightSpots[0].ptPosInScreen.y - m_ptInputCoord.y ;

    //                int deltaXLimit = 40;
    //                int deltaYLimit = 10;

    //                deltaXLimit = g_oWGRConfig.MapStandardX2ActualValue(deltaXLimit);
    //                deltaYLimit = g_oWGRConfig.MapStandardY2ActualValue(deltaYLimit);

				//	if (abs(deltAx) < deltaXLimit && abs(deltAy) < deltaYLimit)
				//	{
				//		/////说明只是在x轴上移动，Y轴基本上是不动的。
				//		m_ptInputCoord = aryLightSpots[0].ptPosInScreen  ;

				//		/////再根据距离来是否达到运动的距离的要求
    //                    int Dx = m_ptInputCoord.x - m_Start_MousePoint[0].x;
    //                    int Dy = m_ptInputCoord.y - m_Start_MousePoint[0].y;
    //                    
				//		if(abs(Dx) > g_oWGRConfig.MapStandardX2ActualValue(100) && (abs(Dx) > 2 * abs(Dy)))
				//		{
				//			///说明距离达到了要求。调用消息信息是幕布,                            
				//			::PostMessage(m_hwnd, MyWM_But, MyBut_Mask, MyBut_Mask ) ;
				//			 m_eWBS = E_WBS_MASK ;                             
				//		}

				//		m_bGetstureDone = FALSE  ;
				//		//bRet = m_bGetstureDone   ;
				//		break;
				//	}
				//	else
				//	{
				//		/////否则这个点是无效的点.

				//	}

				//}

				//开始进行板擦的光斑采集进行判断。
				//进行第一次的采集
				if (m_eRecordState == E_START)
				{
					m_ptInputCoord = /*aryLightSpots[0].ptPosInScreen*/ptEraser ;
					m_CollectCount = 1 ;
					m_eRecordState = E_IS_FINSHED ;
					m_bGetstureDone = FALSE ;   //说明正在采集中，不能判断是何种手势类型
					m_oLongTimeClickTrigger.Reset();

                    //<<Modified by jiqw
                    //原因：当手掌处于白板边缘位置擦处时，误触发双击事件
                      if (aryLightSpots[0].ptPosInScreen.x > g_oWGRConfig.MapStandardX2ActualValue(70)  && aryLightSpots[0].ptPosInScreen.x < m_nScreenX - g_oWGRConfig.MapStandardX2ActualValue(80))
                      {//白板左右边缘，不触发双击事件
                          //判断是不是双击产生的效果，如果是双击产生的话，说明是返回到桌面的情况。
                          if (m_oHandDoubleClickChecker.HandDoubleClick(aryLightSpots[0].ptPosInScreen))
                          {
                              ::PostMessage(m_hwnd,MyWM_But,MyBut_Windows,MyBut_Windows);
                              m_eWBS = E_WBS_RETURNTODISKTOP ;
                          }
                      }
					
                    //>>
				}
				//进行第二次采集
				else
				{
					int deltAx = /*aryLightSpots[0].ptPosInScreen.x*/ptEraser.x - m_ptInputCoord.x ;
					int deltAy = /*aryLightSpots[0].ptPosInScreen.y*/ptEraser.y - m_ptInputCoord.y ;

					if (abs(deltAx) < g_oWGRConfig.MapStandardX2ActualValue(100) && abs(deltAy) < g_oWGRConfig.MapStandardY2ActualValue(100))
					{
						m_ptInputCoord = aryLightSpots[0].ptPosInScreen ;
						m_CollectCount++ ;
					}
					else
					{
						//重新开始计算，一切置为初始值。
						m_eRecordState = E_START ;
						m_CollectCount = 0       ;
						m_bGetstureDone = TRUE   ;
						//bRet  = TRUE             ;

						break;
					}
				}

				////////采集结束了，就可以进行判断了。
				if (m_CollectCount >= 2 )
				{
					////////采集2次的话判断鼠标的坐标的位置。如果鼠标的位置在坐边缘的话，说明有可能是幕布的功能，根据具体的再判断。
					//if (aryLightSpots[0].ptPosInScreen.x < g_oWGRConfig.MapStandardX2ActualValue(70)  || aryLightSpots[0].ptPosInScreen.x > m_nScreenX - g_oWGRConfig.MapStandardY2ActualValue(80))
					//{
					//	IsMouseLeftEdge = TRUE    ;
					//	m_bGetstureDone = FALSE   ;    //说明需要进一步的判断。
					//	m_Start_MousePoint[0] = aryLightSpots[0].ptPosInScreen ;

					//}
					////////否则的话说明不是边缘的鼠标，因此需要响应板擦的功能了。
					//else /*if (m_CollectCount >= 5)*/
					{
						//g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_IDLE) ;
                        SimulHIDDown(/*aryLightSpots[0].ptPosInScreen*/ptEraser);
                        m_ptResponse = ptEraser;
						::PostMessage(m_hwnd, MyWM_But,MyBut_Eraser,MyBut_Eraser);     //响应按钮变成板擦
						m_eWBS  = E_WBS_ERASER   ;                        

                        AtlTrace(_T("Begin Eraser Point = (%d, %d) \r\n"), ptEraser.x, ptEraser.y);
					}

					m_bGetstureDone = TRUE   ;    //说明转换结束.响应鼠标信息
					m_eRecordState =E_START  ;
					m_CollectCount = 0       ;
				}

				//bRet = m_bGetstureDone       ;

			}


			////////这里出现的都是小光斑的信息了。
			else
			{
				//////说明是出现了2个或者2个以上的小光斑，这种情况有可能是漫游、缩放、旋转。或者什么也不是，这里就是需要判断
				//如果是漫游的话。那么所满足的条件是两个点之间存在着一定的距离并且两个点是同向运动的。
				//如果是缩放的话。两点的距离是没有限制的，只是两个点相向运动。
				//如果是旋转的话。两点的距离也是没有限制的，需要判断点的运动夹角
				if (nCount == 2)
				{
					//首先是得到这两个光斑的坐标信息
					switch(m_eCollectState)
					{
						///开始采集,因为知道有两个点的信息，因此直接用两个点进行判断。
					case E_COLLECT_START:
						{
                            memset(m_Start_MousePoint, 0, sizeof(m_Start_MousePoint));
							//m_start_mousepoint[0]存放的是x轴较小的坐标。m_start_mousepoint[1]存放的是x轴较大的坐标。
							if(aryLightSpots[0].ptPosInScreen.x < aryLightSpots[1].ptPosInScreen.x)
							{
								m_Start_MousePoint[0] = aryLightSpots[0].ptPosInScreen  ;
								m_Start_MousePoint[1] = aryLightSpots[1].ptPosInScreen  ;
							}
							else
							{
								m_Start_MousePoint[0] = aryLightSpots[1].ptPosInScreen  ;
								m_Start_MousePoint[1] = aryLightSpots[0].ptPosInScreen  ;
							}

							m_Vsumx[0] = m_Vsumx[1] = m_Vsumy[0] = m_Vsumy[1] = 0 ;
							m_eCollectState = E_COLLECT_CONTINUE                  ;

							m_Last_Multipoint[0] = m_Start_MousePoint[0]          ;
							m_Last_Multipoint[1] = m_Start_MousePoint[1]          ;

							m_nCollectTotileCount = 1;
							m_bGetstureDone = FALSE  ;
							//bRet = m_bGetstureDone   ;

							//return bRet              ;
							break;
						}

					case E_COLLECT_CONTINUE:
						{
							////把后面采集的数据和前面放置的数据位置对应起来。
							long dx0 = (aryLightSpots[0].ptPosInScreen.x - m_Last_Multipoint[0].x ) ;
							long dy0 = (aryLightSpots[0].ptPosInScreen.y - m_Last_Multipoint[0].y ) ;
							long dx1 = (aryLightSpots[0].ptPosInScreen.x - m_Last_Multipoint[1].x ) ;
							long dy1 = (aryLightSpots[0].ptPosInScreen.y - m_Last_Multipoint[1].y ) ;

							if ( dx0 *dx0 + dy0 * dy0  <dx1*dx1 +dy1*dy1 )
							{
								m_MultiPoint[0] = aryLightSpots[0].ptPosInScreen   ;
								m_MultiPoint[1] = aryLightSpots[1].ptPosInScreen   ;
							}
							else
							{
								m_MultiPoint[0] = aryLightSpots[1].ptPosInScreen   ;
								m_MultiPoint[1] = aryLightSpots[0].ptPosInScreen   ;
							}

							//////判断数据的有效性,如果在一定的范围内，说明数据是有效的
                            int xLimit = g_oWGRConfig.MapStandardX2ActualValue(100);
                            int yLimit = g_oWGRConfig.MapStandardY2ActualValue(100);;
							if(abs((m_Last_Multipoint[0].x -m_MultiPoint[0].x) < xLimit || (m_Last_Multipoint[0].y - m_MultiPoint[0].y)< yLimit 
								||(m_Last_Multipoint[1].x  - m_MultiPoint[1].x)<xLimit || (m_Last_Multipoint[1].y - m_MultiPoint[1].y)< yLimit))
							{
								//首先是错误光斑的个数置为0.总光斑数加一
								m_nCollectMissCount = 0   ;
								m_nCollectTotileCount++   ;
								m_bGetstureDone = FALSE   ;  //光斑正在采集中。
								//bRet  = m_bGetstureDone   ;

								//把这次的数据放在对应的数组中
								for (int i = 0 ; i < nCount ; i++ )
								{
									m_Last_Multipoint[i] = m_MultiPoint[i]   ;

									////计算运动的速度
									m_Vsumx[i] = m_Last_Multipoint[i].x - m_Start_MousePoint[i].x   ;
									m_Vsumy[i] = m_Last_Multipoint[i].y - m_Start_MousePoint[i].y   ;

                                    //AtlTrace(_T("***m_Last_Multipoint%d = <%d, %d>, m_Start_MousePoint%d = <%d, %d>\r\n"), i,  m_Last_Multipoint[i].x,  m_Last_Multipoint[i].y, i, m_Start_MousePoint[i].x, m_Start_MousePoint[i].y);
								}

                                int abs_Vsumx_0 = GLBGR_INT32_ABS(m_Vsumx[0]);
                                int abs_Vsumy_0 = GLBGR_INT32_ABS(m_Vsumy[0]);
                                int abs_Vsumx_1 = GLBGR_INT32_ABS(m_Vsumx[1]);
                                int abs_Vsumy_1 = GLBGR_INT32_ABS(m_Vsumy[1]);

                                int abs_maxVsum_0 = abs_Vsumx_0 > abs_Vsumy_0 ? abs_Vsumx_0 : abs_Vsumy_0;
                                int abs_maxVsum_1 = abs_Vsumx_1 > abs_Vsumy_1 ? abs_Vsumx_1 : abs_Vsumy_1;
                                                               

                                int xLimit = g_oWGRConfig.MapStandardX2ActualValue(8);
                                int yLimit = g_oWGRConfig.MapStandardY2ActualValue(8);
                                if (abs_maxVsum_0 > xLimit && abs_maxVsum_1 > yLimit/* && m_nCollectTotileCount >= 8*/)
                                {
                                    m_eCollectState = E_COLLECT_END  ;
                                }

                                //AtlTrace(_T("===m_nCollectTotileCount = %d, m_eCollectState  = %d vx = %d vy = %d\r\n"), m_nCollectTotileCount, m_eCollectState, GLBGR_INT32_ABS(m_Vsumx[0]), GLBGR_INT32_ABS(m_Vsumx[1]));

								///////采集20次，用着20次的数据进行分析看是那种类型的变化。
								//if (m_nCollectTotileCount > 5 && m_eCollectState != E_COLLECT_END)
								//{
								//	m_eCollectState = E_COLLECT_START  ;
								//}

								//return  bRet ;
							}
							////如果是在范围之外，说明这组数据是无效的。连续2次数据无效的话，就需要重新采集了
							else
							{
								//如果错误的光斑个数等于2次的话，需要重新采集。
								if(m_nCollectMissCount == 2)
								{
									m_nCollectMissCount = 0    ;
									m_nCollectTotileCount  =0  ;
									m_eCollectState = E_COLLECT_START  ;
									
								}
								else
								{
									m_nCollectMissCount++ ;
								}
							}
							break;
						}
					case E_COLLECT_END:
						{

							//////采集结束后就是进行判断了。如果是反向的话，说明是缩小或者放大的情况。根据两条直线的夹角来计算
							//////旋转时的力矩  M= FXL  =FxLy - FyLx

// 							/////计算两条直线之间的夹角用来判断是不是旋转的情况。
// 							//tg0 = k2-k1/1+k0k1 ;
// 							//k = (y2-y1)/(x2-x1)
// 							double k1 = double(m_Start_MousePoint[1].y - m_Start_MousePoint[0].y) /double(m_Start_MousePoint[1].x - m_Start_MousePoint[0].x)  ;
// 							double k2 = double(m_Last_Multipoint[1].y  - m_Last_Multipoint[0].y) / double(m_Last_Multipoint[1].x - m_Last_Multipoint[0].x)    ;
// 							double tgV0_V1  = abs(k2 - k1 ) ;
// 							double  tg10 =0.176326  * double(abs(1+ k2 * k1));   ///0.176326,0.0874887
  


// 							long V0_square = m_Vsumx[0] * m_Vsumx[0]  + m_Vsumy[0] * m_Vsumy[0]   ;
// 							long V1_square = m_Vsumx[1] * m_Vsumx[1]  + m_Vsumy[1] * m_Vsumy[1]   ;
 
// 							double cosV0_V1_squre = (double(dotProduct) * double(dotProduct))     ;
// 							double  cos120_squre  =  0.25 * double(V0_square) * double(V1_square) ;

							long dotProduct = m_Vsumx[0] * m_Vsumx[1] + m_Vsumy[0] * m_Vsumy[1]   ;       //用来判断方向的

							//m_Start_MidPoint.x = ( m_Start_MousePoint[0].x + m_Start_MousePoint[1].x)>>1 ;
							//m_Start_MidPoint.y = ( m_Start_MousePoint[0].y + m_Start_MousePoint[1].y)>>1 ;

							//long M_LX0 =(m_Last_Multipoint[0].x - m_Start_MidPoint.x)   ;
							//long M_LY0 =(m_Last_Multipoint[0].y - m_Start_MidPoint.y)   ;

							//long M_LX1 = (m_Last_Multipoint[1].x - m_Start_MidPoint.x)  ;
							//long M_LY1 = (m_Last_Multipoint[1].y - m_Start_MidPoint.y)  ;

							//long Moment0 =  m_Vsumx[0] * M_LY0  - m_Vsumy[0]  * M_LX0    ;
							//long Moment1 =  m_Vsumx[1] * M_LY1  - m_Vsumy[1]  * M_LX1    ;

							///*double sin_square0 = Moment0 * Moment0   ;
							//double  sin20_square0 = 0.342020 * (m_Vsumx[0] * m_Vsumx[0] + m_Vsumy[0]* m_Vsumy[0]) *(M_LX0 *M_LX0 + M_LY0 *M_LY0) ;

							//double sin_square1  = Moment1 *Moment1    ;
							//double  sin20_square1 = 0.342020 *  (m_Vsumx[1] * m_Vsumx[1] + m_Vsumy[1]* m_Vsumy[1]) *(M_LX1 *M_LX1 + M_LY1 *M_LY1) ;*/

       //                     double sin_square0 = Moment0 * Moment0 /*(m_Vsumx[0] * M_LY0) *  (m_Vsumx[0] * M_LY0) + (m_Vsumy[0]  * M_LX0) * (m_Vsumy[0]  * M_LX0)*/;
       //                     double  sin20_square0 = 0.342020 * (m_Vsumx[0] * m_Vsumx[0] + m_Vsumy[0]* m_Vsumy[0]) *(M_LX0 *M_LX0 + M_LY0 *M_LY0) ;

       //                     double sin_square1  = Moment1 *Moment1/* (m_Vsumx[1] * M_LY1) *  (m_Vsumx[1] * M_LY1) + (m_Vsumy[1]  * M_LX1) * (m_Vsumy[1]  * M_LX1)*/   ;
       //                     double  sin20_square1 = 0.342020 *  (m_Vsumx[1] * m_Vsumx[1] + m_Vsumy[1]* m_Vsumy[1]) *(M_LX1 *M_LX1 + M_LY1 *M_LY1) ;

							////这里存在一个问题是鼠标需要根据那个运动光斑来判断，因为白板的运动时根据鼠标的值的变化而产生的运动
							////因为存在着旋转。因此不能根据x轴或者Y轴的最小值来判断。因为旋转有时是一个光斑不动，另一个光斑做旋转运动的，
							////而放大缩小的功能的鼠标判断也是这样，这样就防止了鼠标的跳帧。
							////首先判断出移动的距离的最大值，说明是前面还是后面的光斑移动的位置大，选择最大的值作为用来移动的鼠标的值，
							////用这个值来作为鼠标移动或旋转的最初的鼠标值。后面的值根据离这个鼠标值的远近来盘算那个是鼠标值，这样就可以防止鼠标跳帧了。
							////在这四个运动的值中取最大的值。
							////计算是那个光斑的值作为鼠标的值
							////首先是X0的速度中最大的一个值与X1的速度中最大的一个值进行比较

							int Max0 , Max1;
							if (abs(m_Vsumx[0]) <abs(m_Vsumy[0]) )
							{
								Max0 = abs(m_Vsumy[0]);
							}
							else
							{
								Max0 = abs(m_Vsumx[0]);
							}
							if (abs(m_Vsumx[1]) <abs(m_Vsumy[1]) )
							{
								Max1 = abs(m_Vsumy[1]);
							}
							else
							{
								Max1 = abs(m_Vsumx[1]) ;
							}

							if (dotProduct < 0 )
							{
 //                           	g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_IDLE) ; 
                                ::PostMessage(m_hwnd,MyWM_But,MyBut_NonMode,MyBut_NonMode)   ;    //切换白板到无模式状态

                            	///////////这里判断出是0光斑移动的数大还是1光斑移动的数大
                            	if(Max0 < Max1)
                            	{
                            		//////说明是1移动的距离大
                            		//if (pptMouse)
                            		{
                            			m_ptResponse = m_Last_Multipoint[1];
                            			m_Start_MousePoint[0] = m_Last_Multipoint[1] ;
                            		}
                            	}
                            	else
                            	{
                            		///说明是0移动的距离大m_Start_MousePoint[0]这个是用来作为标准比较的点
                            		//if (pptMouse)
                            		{
                            			m_ptResponse = m_Last_Multipoint[0];
                            			m_Start_MousePoint[0] = m_Last_Multipoint[0];
                            		}
                            	}

                                SimulHIDDown(m_ptResponse);

                                ::PostMessage(m_hwnd,MyWM_But,MyBut_Rotation,MyBut_Rotation);
                                m_eWBS  =   E_WBS_RORATE     ;

                            	m_bGetstureDone = TRUE ;   //说明采集完成，判断完成了，说放大缩小的情况。
                            	m_nCollectMissCount = 0    ;
                            	m_nCollectTotileCount  =0  ;
                            	m_eCollectState = E_COLLECT_START  ;
                            	//bRet = m_bGetstureDone   ;

                            	//return bRet ;

                                //**********************************************					
							}

							/////说明方向是同向的，因此用来分析是不是漫游。
							else
							{
								/////////不满足其他的两个点就是漫游了首先检查两个点之间的距离，如果距离大于一定的值的话说明就是漫游了。

								POINT Max_pt = aryLightSpots[0].ptPosInScreen ;
								POINT Sec_pt = aryLightSpots[1].ptPosInScreen ;

                                int nXRoamLimit = g_oWGRConfig.MapStandardX2ActualValue(ROAM_INTERVAL_DISTANCE);
                                int nYRoamLimit = g_oWGRConfig.MapStandardY2ActualValue(ROAM_INTERVAL_DISTANCE);

								if (abs(Max_pt.x - Sec_pt.x) > nXRoamLimit || abs(Max_pt.y - Sec_pt.y) > nYRoamLimit )
								{
									if (m_eRecordState == E_START)
									{
										m_CollectCount = 1              ;
										m_eRecordState = E_IS_FINSHED  ;
										m_bGetstureDone = FALSE         ;
									}
									else
									{
										////说明是在采集中
										m_CollectCount++     ;
									}

									if (m_CollectCount == 1)
									{
										m_CollectCount = 0           ;
										m_eRecordState = E_START    ;
										//g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_IDLE) ;
                                        //
                                        ::PostMessage(m_hwnd,MyWM_But,MyBut_NonMode,MyBut_NonMode)   ;    //切换白板到无模式状态

										///鼠标跟随X坐标最小的值来进行判断
										POINT ptRoam = aryLightSpots[0].ptPosInScreen   ;

										for (int i =1 ;i < nCount ; i++ )
										{
											if (ptRoam.x >= aryLightSpots[i].ptPosInScreen.x)
											{
												ptRoam = aryLightSpots[i].ptPosInScreen    ;
											}
										}

										//if (pptMouse)
										{
											m_ptResponse  = ptRoam   ;
										}

                                        SimulHIDDown(m_ptResponse);

										::PostMessage(m_hwnd,MyWM_XY,MyXY_Begin,0)   ;    //1025是漫游的响应ID。
										m_CollectCount = 0         ;
										m_eRecordState = E_START   ;
										m_eWBS =E_WBS_ROAM         ;
									}
								}
								//else
								//{
								//	///////两个点的距离小于漫游的距离，那就是两个点的同时移动，两个点的距离在一定的范围内
								//	if (abs(Max_pt.x - Sec_pt.x) < 70 && (Max_pt.y - Sec_pt.y) < 70 )
								//	{
								//		//g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_IDLE) ;
        //                                ::PostMessage(m_hwnd,MyWM_But,MyBut_NonMode,MyBut_NonMode)   ;    //切换白板到无模式状态
        //                                
        //                                m_ptResponse.x = (Max_pt.x + Sec_pt.x) >> 1;
        //                                m_ptResponse.y = (Max_pt.y + Sec_pt.y) >> 1;
        //                                
        //                                SimulHIDDown(m_ptResponse);
								//		::PostMessage(m_hwnd,MyWM_But,MyBut_Selection,MyBut_Selection);                                                                                  

								//		m_eWBS = E_WBS_SELECTION ;

        //                                /*AtlTrace(_T("*******************Stroke Selection Begin : \r\n"));*/

								//	}
								//}

								m_nCollectMissCount = 0    ;
								m_nCollectTotileCount  =0  ;
								m_eCollectState = E_COLLECT_START  ;
								m_bGetstureDone = TRUE   ;
								//bRet = m_bGetstureDone   ;

								break;
							}
						}
					}//swich(m_eCollectState)

					break;    //swich(m_eWBS)
				}

				/*else if(nCount >= 3)
				{					

				}*/



				///////////ncount == 1是光斑的操作
				//if (pptMouse)
				{
					m_ptResponse =aryLightSpots[0].ptPosInScreen ;
				}

                m_conNonOneTouch_when_penstate = 0;

				////第一次采集
				if(m_eRecordState ==E_START)
				{
					m_ptInputCoord = m_ptResponse       ;
					m_CollectCount =  1              ;
					m_eRecordState =   E_IS_FINSHED  ;
					m_bGetstureDone = FALSE          ;
				}
				//第二次采集
				else
				{
					int deltAx = m_ptResponse.x - m_ptInputCoord.x   ;
					int deltAy = m_ptResponse.y - m_ptInputCoord.y   ;

					m_ptInputCoord = m_ptResponse  ;
                    

                    int deltXLimit = g_oWGRConfig.MapStandardX2ActualValue(100);
                    int deltYLimit = g_oWGRConfig.MapStandardY2ActualValue(100);;
					if (abs(deltAx) < deltXLimit && abs(deltAy) < deltYLimit)
					{
						m_CollectCount++ ;
					}
					else
					{
						//从新开始计算
						m_eRecordState = E_START   ;
						m_CollectCount  = 0 ;
						m_bGetstureDone  = TRUE ;
						//bRet = TRUE ;
						break;
					}
				}                

                m_eCollectState = E_COLLECT_START;   

				if (m_CollectCount >= 1)
				{
					///////首先在此检测是不是双击事件
					if(m_oFingerDoubleClickChecker.FingerDoubleClick(aryLightSpots[0].ptPosInScreen))
					{
						//检测到是双击的时间。然后判断在此之前是那种笔的书写类型，根据书写类型变成响应的笔的类型

						Restore2OldPenState();
					}

					///////// 从板擦的状态变成笔的状态，也是之前笔的状态是什么状态，则变成相应的笔的类型
					//if (StrStatus == _T("EraseStroke"))
					//{
					//	Restore2OldPenState();

					//	m_eWBS = E_WBS_PEN          ;
					//}

					m_CollectCount   =  0       ;
					m_bGetstureDone  = TRUE     ;
					m_eRecordState   = E_START  ;

				}
				//bRet = m_bGetstureDone ;
			}

			break;
		}

	}//swich
    
    if (g_ePreState != m_eWBS)
    {
        g_oGLTraceLog.AddCollectTag(g_nCurrentCollect, m_eWBS);        
    }

    g_ePreState = m_eWBS;    

	return   IsReponseByMyself(m_eWBS);

}


//读取缓冲区的数据，读出当前板擦的当前状态，返回当前状态的字符串
CString  CGestureEventGenerator::ReadShareMemory()
{
	DWORD dwWaitResult = SMR_WaitForWriteEvent(0);
	CString strAllData;
	if(WAIT_OBJECT_0 == dwWaitResult)
	{
		char m_Namesz[30];
		memset(m_Namesz,0,sizeof(m_Namesz));
		char m_versionsz[10];
		memset(m_versionsz,0,sizeof(m_versionsz));

		UINT nDataLen = SMR_ReadDataBlockInfo(m_Namesz,sizeof(m_Namesz),m_versionsz,sizeof(m_versionsz));
		BYTE *buf = new BYTE[nDataLen];

		int length = SMR_Read(NULL,NULL,buf,nDataLen,0);

		for (int i =0; i<length;i++)
		{
			CString strVal;
			strVal.Format(_T("%c"),buf[i]);
			strAllData += strVal;
		}

        delete []buf;
        buf = NULL;
	}
	return strAllData;
}

///进行鼠标滑轮滚动手势的判断
BOOL CGestureEventGenerator::DetectMouseWheelAction(TLightSpot * aryLightSpots, int nCount)
{
	//说明没有光斑出现，这样或者是一次运动结束，在下次运动开始时需要重新修正某些参数。

	if (nCount != 2)//说明一次采集结束了。
	{
		if (nCount == 0) //等于0的话，说明是完全结束的。
		{
			m_nCollectTotileCount    =      0 ;
			m_bMouseWheelState       =  FALSE ; 
			m_bCollectWheelState     =  FALSE ; //当一次判断结束后。所有的状态置为FALSE.
			m_eMouseWheelState = E_GESTUREEVENT_START_COLLECT;

			m_nDistanceCount_Big    = 0 ;
			m_nDistanceCount_Small  = 0 ;
			m_nOneMissCount         = 0 ;
			m_nOnePointCount        = 0 ;
			return FALSE;
		}
		else //说明不是0的话，如果鼠标的状态是上滑或者下滑或者是放大或者是缩小的话，说明还是在运行阶段。
		{
			if (m_nOnePointCount >  40) //说明前面40次的采集都是1个点的情况，那么说明在这次运动中是一个点的情况，不对鼠标进行滑轮的判断了。
			{
				return FALSE;
			}

			if (m_bMouseWheelState)    
			{
				//如果采集10次都是一个光斑的情况的话，就说明不是滚轮的情况了，那么就需要置状态为非滚轮状态。
				if(m_nOneMissCount < 10)
				{
					m_bCollectWheelState      = TRUE;
					m_nOneMissCount++    ;
					return TRUE;
				}
				else
				{
					m_bCollectWheelState  = FALSE;
					m_bMouseWheelState    = FALSE  ;
		            m_nOneMissCount = 0 ;
					/*if (g_oMouseEventGen.m_eSimulMouseState == CMouseEventGenerator::SIMUL_MOUSE_STATE_WHEEL_SIMULATE )
					{
						g_oMouseEventGen.SetSimulMouseResponseState(CMouseEventGenerator::SIMUL_MOUSE_STATE_IDLE);
					}*/
					return FALSE;
				}
			}
			else //说明鼠标的状态不是在如何的状态下，说明这个点不是鼠标滑轮操作状态。因此不执行鼠标滑轮的操作。                                       
			{
				//说明是1个点的情况出现。如果一个出现了几秒后，说明一直是鼠标的响应状态
				m_nOnePointCount++ ;

				m_nCollectTotileCount    =      0 ;
				m_bMouseWheelState       =  FALSE ; 
				m_bCollectWheelState     =  FALSE ;                                
				m_eMouseWheelState = E_GESTUREEVENT_START_COLLECT;   //当一次判断结束后。所有的状态置为初始状态 
				m_nDistanceCount_Big    = 0 ;
				m_nDistanceCount_Small  = 0 ;
				m_nOneMissCount         = 0 ;
				return FALSE;
			}
		}
	}

    //出现的是2个点的情况
	if (m_nOnePointCount >  40) //说明前面40次的采集都是1个点的情况，那么说明在这次运动中是一个点的情况，不对鼠标进行滑轮的判断了。
	{
		return FALSE;
	}

	m_nCollectTotileCount++;
	m_nOneMissCount = 0    ;
	m_nOnePointCount = 0   ;

	switch(m_eMouseWheelState)
	{
	case E_GESTUREEVENT_START_COLLECT:
		{
			for (int i =0 ; i< nCount ; i++ )
			{
                m_MultiPoint[i] = aryLightSpots[i].ptPosInScreen ;

				m_Vsumx[i] =  0  ;
				m_Vsumy[i] =  0  ;                           //首先这个速度和运动距离记为0；
			}
			if (m_MultiPoint[0].x < m_MultiPoint[1].x)     //根据X坐标的前后来把数据存储起来
			{
				m_Start_MousePoint[0] = m_MultiPoint[0] ;
				m_Start_MousePoint[1] = m_MultiPoint[1] ; 
			}
			else
			{
				m_Start_MousePoint[1] = m_MultiPoint[0];
				m_Start_MousePoint[0] = m_MultiPoint[1];
			}

			for(int i = 0 ; i < nCount ; i++)
			{
				m_Last_Multipoint[i] = m_MultiPoint[i] = m_Start_MousePoint[i] ;
			}

			m_Start_MidPoint.x = (m_Start_MousePoint[0].x + m_Start_MousePoint[1].x)>>1 ;
			m_Start_MidPoint.y = (m_Start_MousePoint[0].y + m_Start_MousePoint[1].y)>>1 ;

			int nDx = m_MultiPoint[0].x - m_MultiPoint[1].x;
			int nDy = m_MultiPoint[0].y - m_MultiPoint[1].y ;

			m_lFirstDistance = nDx * nDx + nDy * nDy ;
			m_bCollectWheelState = TRUE;                          //说明采集已经开始了。
			m_eMouseWheelState = E_GESTUREEVENT_CONTINUE_COLLECT;
		}
		break;

	case E_GESTUREEVENT_CONTINUE_COLLECT:
	{
	    //把对应的位置的鼠标对应起来
        //MultiPoint[0]存放水平坐标小的光斑坐标
        //MultiPoint[1]存放水平坐标大的光斑坐标


        if (abs(aryLightSpots[0].ptPosInScreen.x - m_Start_MousePoint[0].x) < abs(aryLightSpots[0].ptPosInScreen.x - m_Start_MousePoint[1].x) )
		{
			m_MultiPoint[0]  = aryLightSpots[0].ptPosInScreen;
			m_MultiPoint[1]  = aryLightSpots[1].ptPosInScreen;

		}
		else
		{
			m_MultiPoint[0]  = aryLightSpots[1].ptPosInScreen;
			m_MultiPoint[1] = aryLightSpots[0].ptPosInScreen;
		}



		int nDx = m_MultiPoint[0].x - m_MultiPoint[1].x;
		int nDy = m_MultiPoint[0].y - m_MultiPoint[1].y;

		m_lSecondDistance  = nDx * nDx + nDy * nDy ;
		
        //判断两次的距离不能差的太远，差的太远的话，就默认这个数据是无效的数据
		if(abs( m_Last_Multipoint[0].x - m_MultiPoint[0].x) < 100 || abs( m_Last_Multipoint[0].y - m_MultiPoint[0].y) < 100 
			|| abs( m_Last_Multipoint[1].x - m_MultiPoint[1].x) < 100 || abs( m_Last_Multipoint[1].y - m_MultiPoint[1].y) < 100 )
		{
			m_bCollectWheelState = TRUE;            //正在采集中，这个标志是为TURE;
			m_nCollectMissCount = 0;

			m_Last_Multipoint[0] =  m_MultiPoint[0]   ;
			m_Last_Multipoint[1] =  m_MultiPoint[1]  ;

			for(int i = 0 ; i< nCount ; i++)
			{
				m_Vsumx[i] =   (m_Start_MousePoint[i].x - m_MultiPoint[i].x );           //计算运动的速度;
				m_Vsumy[i] =  (m_Start_MousePoint[i].y - m_MultiPoint[i].y );
			}
			//////////////////////////////////////////计算出鼠标移动的速度，来决定采集的数量。
			////////////////////////////////////////////首先计算出这四个坐标的最大值。如果最大值大于4个像素的话，就说明这次采集结束了。

			if (m_nCollectTotileCount == COLLECT_POINT_COUNT)
			{
				m_eMouseWheelState = E_GESTUREEVENT_DATA_CALCULATE;
			}

			int nMaxPoint = max( abs( m_Vsumx[0]),max( abs(m_Vsumx[1]),max( abs(m_Vsumy[0]) ,abs(m_Vsumy[1])))) ;    //找出移动的最大距离
			if(nMaxPoint >= MOUSE_MOVE_DISTANCE)
			{
				m_nCollectTotileCount = COLLECT_POINT_COUNT;
				m_eMouseWheelState = E_GESTUREEVENT_DATA_CALCULATE;
			}

			if( ( m_lSecondDistance - m_lFirstDistance) > 0 )
			{
				m_nDistanceCount_Big++ ;	
			}
			if( (m_lSecondDistance  - m_lFirstDistance ) < 0 )
			{
				m_nDistanceCount_Small++ ;
			}
		}
		else
		{
			/////////////////////////如果不满足条件的话，就说明这次采集时错误的。
			if(m_nCollectMissCount == 2 )
			{
				m_nCollectMissCount = 0;
				m_eMouseWheelState = E_GESTUREEVENT_START_COLLECT;
				m_nCollectTotileCount =  0;
				m_bCollectWheelState = FALSE;
				m_nDistanceCount_Big = m_nDistanceCount_Small  = 0 ;
				return FALSE;
			}
			else
			{
				m_nCollectMissCount++;
				m_nCollectTotileCount--;       //这次采集不采用
			}
		}
	}
	break;

	case E_GESTUREEVENT_DATA_CALCULATE:
		{
			//判断，如果是同向的话，说明是鼠标下滑或上升，如果是相反方向的话，说明是放大或缩小；
			long dotProduct= m_Vsumx[0] * m_Vsumx[1] + m_Vsumy[0] * m_Vsumy[1] ;
			long V0_square = m_Vsumx[0] * m_Vsumx[0] + m_Vsumy[0] * m_Vsumy[0] ;
			long V1_square = m_Vsumx[1] * m_Vsumx[1] + m_Vsumy[1] * m_Vsumy[1] ;

			if (V0_square == 0)
			{
				if (m_Vsumx[1] <= 0)                        //放大           
				{
					if (abs(m_Vsumx[1]) > abs(m_Vsumy[1]))
					{
						m_nMobileDistance  =  m_Vsumx[1];
					}
					else
					{ 
						m_nMobileDistance  =  m_Vsumy[1];    //向下
					}
				}
				else                                     //缩小
				{
					if (abs(m_Vsumx[1]) > abs(m_Vsumy[1]))
					{
						m_nMobileDistance  =  m_Vsumx[1];
					}
					else
					{ 
						m_nMobileDistance  =  m_Vsumy[1];
					}

				}
			}
			else if(V1_square == 0)
			{
				if(m_Vsumx[0] > 0 )           
				{
					if (abs(m_Vsumx[0]) > abs(m_Vsumy[0]))       //放大
					{
						m_nMobileDistance  =  m_Vsumx[0];
					}
					else
					{ 
						m_nMobileDistance  =  m_Vsumy[0];
					}			

				}
				else                           
				{
					if (abs(m_Vsumx[0]) > abs(m_Vsumy[0]))        //缩小
					{
						m_nMobileDistance  =  m_Vsumx[0];
					}
					else
					{ 
						m_nMobileDistance  =  m_Vsumy[0];
					}	
				}
			}
			else
			{
				double cosV0_V1_square = (double(dotProduct) * double(dotProduct));
				double cos120_squre = 0.25 * double(V0_square) *double(V1_square);                                         /*0.57101007166283436652204980734113*/
				double cos45_squre  = 0.5 * double(V0_square) *double(V1_square) ;

				if (dotProduct < 0 && cosV0_V1_square > cos120_squre)  //cos(v1^v2)<0, 说明范围在145~180度之间
				{
					//////说明是放大或者缩小
					//m_bDistanceState = TRUE  ;
					/////////////////////////////把数据中的符号保留下来。减少计算量
					long  Vector_X[2] , Vector_Y[2] ;
					Vector_X[0] =  (m_Start_MousePoint[0].x - m_Start_MidPoint.x ) ;
					if (Vector_X[0] < 0)   {Vector_X[0] = -1 ;}
					else{Vector_X[0] = 1 ;}

					Vector_Y[0] =  (m_Start_MousePoint[0].y - m_Start_MidPoint.y ) ;
					if (Vector_Y[0] < 0)
					{	Vector_Y[0] = -1;	}
					else
					{	Vector_Y[0] = 1;	}

					Vector_X[1] =  ( m_Start_MousePoint[1].x - m_Start_MidPoint.x );
					if (Vector_X[1] < 0)
					{  Vector_X[1] = -1 ;}
					else
					{  Vector_X[1]  = 1;}
					Vector_Y[1] =  ( m_Start_MousePoint[1].y - m_Start_MidPoint.y );
					if (Vector_Y[1] < 0)
					{   Vector_Y[1] = -1 ;	}
					else
					{	Vector_Y[1] = 1 ;	}

					long dotProduct1 = Vector_X[0] * m_Vsumx[0] + Vector_Y[0] * m_Vsumy[0] ;
					long dotProduct2 = Vector_X[1] * m_Vsumx[1] + Vector_Y[1] * m_Vsumy[1] ;

					if(dotProduct1 < 0 && dotProduct2 < 0)    //说明是放大的情况出现
					{
						int nMaxPoint = max( abs( m_Vsumx[0]),max( abs(m_Vsumx[1]),max( abs(m_Vsumy[0]) ,abs(m_Vsumy[1])))) ;
						m_nMobileDistance = nMaxPoint;

					}
					else if (dotProduct1 > 0 && dotProduct2 > 0)   //说明是缩小的情况出现
					{
						int nMaxPoint = max( abs( m_Vsumx[0]),max( abs(m_Vsumx[1]),max( abs(m_Vsumy[0]) ,abs(m_Vsumy[1])))) ;
						m_nMobileDistance = -nMaxPoint;
					}
					else                                          //不满足放大或者缩小的条件
					{
						m_nCollectTotileCount    =      0 ;
						m_bMouseWheelState       =  FALSE ; 
						m_bCollectWheelState     =  FALSE ;                                
						m_eMouseWheelState = E_GESTUREEVENT_START_COLLECT;
						m_nDistanceCount_Big = m_nDistanceCount_Small  = 0 ;

						return FALSE;
					}
				}
				else if (dotProduct > 0 &&  cosV0_V1_square > cos45_squre )
				{
					/////说明是上滑或者是下滑
					if(m_Vsumy[0] > 0 && m_Vsumy[1] > 0)       //说明是向上滑动的
					{
						m_bMouseWheelState = TRUE;
						int MaxPoint = max( m_Vsumy[0] ,m_Vsumy[1]);
						m_nMobileDistance = MaxPoint;
					}
					else if (m_Vsumy[0] < 0 && m_Vsumy[1] < 0)    //说明是向下滑动的
					{
						m_bMouseWheelState = TRUE;
						int MaxPoint= min(m_Vsumy[0] ,m_Vsumy[1]);
						m_nMobileDistance = MaxPoint;
					}
					else                                        //不满足任何条件
					{
						m_nCollectTotileCount    =      0 ;
						m_bMouseWheelState       =  FALSE ;
						m_bCollectWheelState     =  FALSE ;
						m_eMouseWheelState = E_GESTUREEVENT_START_COLLECT;
						m_nDistanceCount_Big = m_nDistanceCount_Small  = 0 ;
						return FALSE;
					}
				}
				else
				{
					///////////////////////////////////计算距离
					if (m_nDistanceCount_Big >= m_nCollectTotileCount/2  )
					{
						int nMaxPoint = max( abs( m_Vsumx[0]),max( abs(m_Vsumx[1]),max( abs(m_Vsumy[0]) ,abs(m_Vsumy[1])))) ;
						m_nMobileDistance = nMaxPoint;
					}
					else if (m_nDistanceCount_Small >= m_nCollectTotileCount/2 )
					{
						int nMaxPoint = max( abs( m_Vsumx[0]),max( abs(m_Vsumx[1]),max( abs(m_Vsumy[0]) ,abs(m_Vsumy[1])))) ;
						m_nMobileDistance = -nMaxPoint;
					}
					else
					{
						m_eMouseWheelState = E_GESTUREEVENT_START_COLLECT;
						m_nCollectTotileCount =  0;
						m_bCollectWheelState = FALSE;
						m_nDistanceCount_Big = m_nDistanceCount_Small  = 0 ;
						return FALSE;
					}
				}
			} //else
			////////////////////////////这一次的判断完成了，
				m_eMouseWheelState = E_GESTUREEVENT_START_COLLECT;
				m_nCollectTotileCount =    0;
				m_bCollectWheelState = FALSE;
				m_nDistanceCount_Big = m_nDistanceCount_Small  = 0 ;

				return TRUE;
		}	

  }  //switch

  return TRUE;
}


int  CGestureEventGenerator::GetMouseWheelLength()
{
	return m_nMobileDistance * MOVE_DISTANCE ;
}

void  CGestureEventGenerator::GetGLboardButtonState()
{
    CString Read_Str = ReadShareMemory();
    if ( Read_Str !=_T("") )           //说明按钮的状态有变化，把这个记录下来。
	{
		if ( StrStatus == _T("multipen") && Read_Str == _T("EraseStroke"))
		{
			//if (!bGestureEnable)
			{
				PreStrStatus = StrStatus;
				StrStatus = Read_Str;
			}
		}
		else
		{
			PreStrStatus = StrStatus;
			StrStatus = Read_Str;
		}
	}

}
BOOL  CGestureEventGenerator::MultiplePenExecute(const  TLightSpot *aryLightSpots, int &nCount)
{
	if (StrStatus ==_T("multipen"))
	{
		//if(!bGestureEnable)//不需要手势识别
		//{
		//	m_oMulPenMatch.BeforeMulPenMatch();

		//	for(int j = 0; j < nCount ; j ++)
		//	{
		//		m_oMulPenMatch.DoMulMatch(m_hwnd,aryLightSpots[j].ptPosInScreen);
		//	}
		//	m_oMulPenMatch.AfterMulPenMatch(m_hwnd);

		//	return FALSE;
		//}

		//else
		//{
		//	m_oMulPenMatch.BeforeMulPenMatch();
	 //       m_oMulEarserMatch.BeforeEarserMatch();

		//	POINT  ptNow;
		//	ptNow.x = ptNow.y = 0;
		//	BOOL bIsExistBigSpot = FALSE;//是否存在大的光斑.是板擦的状态
	 //       int  nCounter        = 0  ;

		//	for(int i = 0 ; i < nCount ; i++)
		//	{
  //              LONG nActualAreas    = aryLightSpots[i].mass  ;
  //              LONG nCalculateAreas = aryLightSpots[i].lStdSpotAreaInVideo  ;

		//		if (nActualAreas > nCalculateAreas * m_nMultiEraser)
		//		{
		//			bIsExistBigSpot =    TRUE;
		//			ptErase = aryLightSpots[i].ptPosInScreen;
		//		}
		//	}

		//	for(int j = 0; j < nCount ; j ++)
		//	{	
  //               //当前光斑位置的坐标
		//		ptNow = aryLightSpots[j].ptPosInScreen ;

		//		if(bIsExistBigSpot)                                                     //为TRUE说明是存在大的光斑的，因此是需要变成板擦的
		//		{
		//			if(ptNow == ptErase)                                               //说明这个坐标是大光斑的坐标
		//			{
		//				m_oMulEarserMatch.DoEarserMatch(m_hwnd,ptNow);
		//			}

		//			else if(abs(ptNow.x - ptErase.x) < 100  && abs(ptNow.y - ptErase.y) < 100)
		//			{
		//				////////////////////////////////////////////////////////////////do nothing 说明是大光斑周边存在的小光斑
		//				int i = 0;
		//			}
		//			else
		//			{
		//				m_oMulPenMatch.DoMulMatch(m_hwnd,ptNow);
		//			}
		//		}
		//		else
		//		{
		//			if (abs(ptNow.x - ptErase.x) < 100  && abs(ptNow.y - ptErase.y) < 100)
		//			{
		//				m_oMulEarserMatch.DoEarserMatch(m_hwnd,ptNow);
		//				ptErase = ptNow;
		//			}
		//			else
		//			{
		//				m_oMulPenMatch.DoMulMatch(m_hwnd,ptNow);
		//				nCounter++ ;
		//			}
		//		}
		//	}

		//	//说明所有的都是小光斑，因此板擦的值清零。
		//	if (nCount == nCounter)
		//	{
		//		ptErase.x = (std::numeric_limits<long>::min)();
		//		ptErase.y =(std::numeric_limits<long>::min)();
		//	}

		//	m_oMulPenMatch.AfterMulPenMatch(m_hwnd);
		//	m_oMulEarserMatch.AfterEarserMatch(m_hwnd);
		//	return FALSE;
		//}

        return TRUE;

	}
	else
	{
		return FALSE;
	}	
}

bool CGestureEventGenerator::Match2Touchs(const TLightSpot * aryLightSpots,  int nCount)
{
    if (nCount < 2)
    {
        return false;
    }    

    /*
    @根据最近距离进行匹配，匹配的条件
      a.离上一次点m_ptResponse最近；
      b.离上一次点m_ptResponse最近距离应该小于值T,否则认为是干扰点
    */

    const int nStdMaxLimit = 60;
    int nActualMaxLimit = g_oWGRConfig.MapStandardX2ActualValue(nStdMaxLimit); 
    int nActualMaxLimit2 = nActualMaxLimit * nActualMaxLimit;

    long dx = aryLightSpots[0].ptPosInScreen.x - m_ptResponse.x;
    long dy = aryLightSpots[0].ptPosInScreen.y - m_ptResponse.y;

    long nMinDis2 = dx * dx + dy * dy;

    int nMatchPos = 0; //匹配点在aryLightSpots中的位置

    for (int i = 1; i < nCount; i++)
    {
        dx = aryLightSpots[i].ptPosInScreen.x - m_ptResponse.x;
        dy = aryLightSpots[i].ptPosInScreen.y - m_ptResponse.y;
        int tmp2 = dx * dx + dy * dy;

        if (tmp2 < nMinDis2)
        {
            nMinDis2 = tmp2;

            nMatchPos = i;
        }
    }

    if (nMinDis2 < nActualMaxLimit2)
    {
        //
        m_ptResponse = aryLightSpots[nMatchPos].ptPosInScreen;

        return true;
    }

    /*long dx0 = (aryLightSpots[0].ptPosInScreen.x - m_Last_Multipoint[0].x ) ;
    long dy0 = (aryLightSpots[0].ptPosInScreen.y - m_Last_Multipoint[0].y ) ;
    long dx1 = (aryLightSpots[0].ptPosInScreen.x - m_Last_Multipoint[1].x ) ;
    long dy1 = (aryLightSpots[0].ptPosInScreen.y - m_Last_Multipoint[1].y ) ;

    long lDistSquare0 = dx0 * dx0 + dy0 * dy0;
    long lDistSquare1 = dx1 * dx1 + dy1 * dy1; 

    if(lDistSquare0 < lDistSquare1)
    {
        m_MultiPoint[0] = aryLightSpots[0].ptPosInScreen   ;
        m_MultiPoint[1] = aryLightSpots[1].ptPosInScreen   ;      
    }
    else
    {
        m_MultiPoint[0] = aryLightSpots[1].ptPosInScreen   ;
        m_MultiPoint[1] = aryLightSpots[0].ptPosInScreen   ;    
    }   */

    return false;
}

bool CGestureEventGenerator::LeastDistanceMatch(const TLightSpot * aryLightSpots, int nCount, POINT &pt)
{
    if (nCount != 2)
    {
        return false;
    }

    long dx0 = (aryLightSpots[0].ptPosInScreen.x - pt.x ) ;
    long dy0 = (aryLightSpots[0].ptPosInScreen.y - pt.y ) ;
    long dx1 = (aryLightSpots[1].ptPosInScreen.x - pt.x ) ;
    long dy1 = (aryLightSpots[1].ptPosInScreen.y - pt.y ) ;

    long lDistSquare0 = dx0 * dx0 + dy0 * dy0;
    long lDistSquare1 = dx1 * dx1 + dy1 * dy1;   

    if(lDistSquare0 < lDistSquare1)
    {
        pt = aryLightSpots[0].ptPosInScreen;  
    }
    else
    {
        pt = aryLightSpots[1].ptPosInScreen;   
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_2TOUCHS_MOVES CGestureEventGenerator::Calc2TouchsMoveDirection(bool matchResult, POINT* pptMouse)
{    
    if(!matchResult)
    {
        if (pptMouse)
        {
            *pptMouse = m_Start_MousePoint[0];
        }
        AtlTrace(_T("E_2TOUCHS_MOVES_FALSE!!!\r\n"));

        return E_2TOUCHS_MOVES_FALSE;
    }


    //判断数据的有效性,如果在一定的范围内，说明数据是有效的
    long dx0 = m_MultiPoint[0].x - m_Last_Multipoint[0].x ;
    long dy0 = m_MultiPoint[0].y - m_Last_Multipoint[0].y;
    long dx1 = m_MultiPoint[1].x  - m_Last_Multipoint[1].x;
    long dy1 = m_MultiPoint[1].y - m_Last_Multipoint[1].y;


    long lDistSquare0 = dx0 * dx0 + dy0 * dy0;
    long lDistSquare1 = dx1 * dx1 + dy1 * dy1;

    int xLimit_100 = g_oWGRConfig.MapStandardX2ActualValue(100);
    int yLimit_100 = xLimit_100;

    int nLimit_2 = g_oWGRConfig.MapStandardX2ActualValue(2);

    if(abs(dx0) > xLimit_100 || abs(dy0) > yLimit_100 
        || abs(dx1) > xLimit_100 || abs(dy1) > yLimit_100)
    {
        if (pptMouse)
        {
            *pptMouse = m_Start_MousePoint[0];            
        }

        AtlTrace(_T("E_2TOUCHS_MOVES_FALSE!!!\r\n"));
        return E_2TOUCHS_MOVES_FALSE;
    }
    else
    {        
        if(lDistSquare0 < nLimit_2 && lDistSquare1 < nLimit_2)
        {
            if (pptMouse)
            {
                *pptMouse = m_Start_MousePoint[0];
            }

            AtlTrace(_T("E_2TOUCHS_MOVES_NONE!!!\r\n"));
            return E_2TOUCHS_MOVES_NONE;
        }
    }     

    //由于某个点未移动，而选择的比较标准m_Start_MousePoint[0]就是该点，
    //就需要通过相对移动来获取pptMouse了
    //比如0点未移动，m_Start_MousePoint[0]现在是0点，1点移动了，
    //pptMouse就要根据relative_offsetx、relative_offsety和m_Start_MousePoint共同决定了
    long relative_offsetx = 0;
    long relative_offsety = 0;

    long dotProduct = dx0 * dx1 + dy0 * dy1;

    int nLimit_4 = g_oWGRConfig.MapStandardX2ActualValue(4);

    ///////////////////////////////
    if(lDistSquare0 < nLimit_4)
    {
        //
        dx0 = m_Last_Multipoint[1].x - m_MultiPoint[0].x;
        dy0 = m_Last_Multipoint[1].y - m_MultiPoint[0].y;

        if(m_eWBS == E_WBS_RORATE || m_eWBS == E_WBS_ZOOM)
        {
            dotProduct = -1;
        }
        else if(m_eWBS == E_WBS_ROAM || E_WBS_SELECTION)
        {
            dotProduct = 1;
        }
        else
        {
            dotProduct = dx0 * dx1 + dy0 * dy1;
        }

        if(dx1 < 0)
        {
            relative_offsetx = (dx1 - 1)>>1;
        }
        else
        {
            relative_offsetx = (dx1 + 1)>>1;
        }

        if(dy1 < 0)
        {
            relative_offsety = (dy1 - 1)>>1;
        }
        else
        {
            relative_offsety = (dx1 + 1)>>1;
        }

        //lDsitSquare0 = dx0 * dx0 + dy0 * dy0;
    }
    else if(lDistSquare1 < nLimit_4)
    {
        dx1 = m_Last_Multipoint[0].x - m_MultiPoint[1].x;
        dy1 = m_Last_Multipoint[0].y - m_MultiPoint[1].y;

        if(m_eWBS == E_WBS_RORATE || m_eWBS == E_WBS_ZOOM)
        {
            dotProduct = -1;
        }
        else if(m_eWBS == E_WBS_ROAM || E_WBS_SELECTION)
        {
            dotProduct = 1;
        } 
        else
        {
            dotProduct = dx0 * dx1 + dy0 * dy1;
        }

        if(dx0 < 0)
        {
            relative_offsetx = (dx0 - 1)>>1;
        }
        else
        {
            relative_offsetx = (dx0 + 1)>>1;
        }

        if(dy0 < 0)
        {
            relative_offsety = (dy0 - 1)>>1;
        }
        else
        {
            relative_offsety = (dx0 + 1)>>1;
        }

        //lDsitSquare0 = dx1 * dx1 + dy1 * dy1;
    }

    //把这次的数据放在对应的数组中
    for (int i = 0 ; i < 2 ; i++ )
    {                
        ////计算运动的速度
        /* m_Vsumx[i] = m_Last_Multipoint[i].x - m_MultiPoint[i].x   ;
        m_Vsumy[i] = m_Last_Multipoint[i].y - m_MultiPoint[i].y   ;*/

        m_Last_Multipoint[i] = m_MultiPoint[i];
    }    

    if(pptMouse)
    {
        long DisX0 = (m_Start_MousePoint[0].x - m_MultiPoint[0].x) * (m_Start_MousePoint[0].x - m_MultiPoint[0].x) 
            +(m_Start_MousePoint[0].y - m_MultiPoint[0].y) * (m_Start_MousePoint[0].y - m_MultiPoint[0].y) ;

        long DisX1 = (m_Start_MousePoint[0].x - m_MultiPoint[1].x) * (m_Start_MousePoint[0].x - m_MultiPoint[1].x)
            +(m_Start_MousePoint[0].y - m_MultiPoint[1].y) * (m_Start_MousePoint[0].y - m_MultiPoint[1].y) ;

        if(DisX0 < DisX1)
        {
            if(lDistSquare0 < nLimit_4)
            {
                (*pptMouse).x = m_Start_MousePoint[0].x - relative_offsetx;
                (*pptMouse).y = m_Start_MousePoint[0].y - relative_offsety;
            }
            else if(lDistSquare1 < nLimit_4)
            {
                (*pptMouse).x = m_Start_MousePoint[0].x + relative_offsetx;
                (*pptMouse).y = m_Start_MousePoint[0].y + relative_offsety;             
            }
            else
            {
                *pptMouse = m_MultiPoint[0];
            }

            m_Start_MousePoint[0] = *pptMouse;
        }
        else
        {
            if(lDistSquare0 < nLimit_4)
            {
                (*pptMouse).x = m_Start_MousePoint[0].x + relative_offsetx;
                (*pptMouse).y = m_Start_MousePoint[0].y + relative_offsety;
            }
            else if(lDistSquare1 < nLimit_4)
            {
                (*pptMouse).x = m_Start_MousePoint[0].x - relative_offsetx;
                (*pptMouse).y = m_Start_MousePoint[0].y - relative_offsety;             
            }
            else
            {
                *pptMouse = m_MultiPoint[1];
            }

            m_Start_MousePoint[0] = *pptMouse;
        } 
    }


    return E_2TOUCHS_MOVES_FALSE;
}