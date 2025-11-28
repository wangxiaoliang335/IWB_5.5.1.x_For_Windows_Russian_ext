#include "StdAfx.h"
//#include <Atlbase.h>
#include "WGRCommonUlity.h"
#include "GlobalLog.h"
//#include "..\inc\PerfDetector.h"

int CWGRCommonUlity::m_curUnexpectedShapeTimes      = 0;                //形状缺失容忍度
int CWGRCommonUlity::m_curUnexpectedUnknownShapeTimes = 0;
int CWGRCommonUlity::m_curUnexpectedTouchsTimes     = 0;                //丢失预期触点数目容忍度
int CWGRCommonUlity::m_curUnexpectedTouchsDisTimes  = 0;                //非预期的光点间距离出现的次数
int CWGRCommonUlity::m_curUnexpectedNonMoveTimes    = 0;                //非预期的光点不运动的次数
int CWGRCommonUlity::m_curUnexpectedKnownMoveTimes  = 0;                //非预期的运动次数
int CWGRCommonUlity::m_curUnexpectedUnKnownMoveTimes= 0;                //非预期的运动次数

std::map<UINT, ST_MOTIONINFO> CWGRCommonUlity::m_lastTouchInfoMap;//上一帧采集的触点存储图
ST_TouchInfo CWGRCommonUlity::m_curMatchedTInfo[KMAXTOUCHNUM];    //本帧采集的匹配的触点
ST_TouchInfo CWGRCommonUlity::m_curMisMatchedTInfo[KMAXTOUCHNUM]; //本帧采集的匹配失败的触点
ST_TouchInfo CWGRCommonUlity::m_curNewAddedTInof[KMAXTOUCHNUM];   //本帧采集的新增的触点

int  CWGRCommonUlity::m_nCurMatchedTouchs           = 0;
int  CWGRCommonUlity::m_nCurMisMatchedTouchs        = 0;
int  CWGRCommonUlity::m_nCurNewAddedTouchs          = 0;

int CWGRCommonUlity::m_lastXDisplacement            = 0;                //上一次x方向位移
int CWGRCommonUlity::m_lastYDisplacement            = 0;                //上一次y方向位移

int CWGRCommonUlity::m_curXDisplacement             = 0;                //本次x方向位移
int CWGRCommonUlity::m_curYDisplacement             = 0;                //本次y方向位移
POINT CWGRCommonUlity::m_curPtMiddle;

bool CWGRCommonUlity::m_havedEnteredTheSumSM        = false;            //是否第一次进入该子状态机
bool CWGRCommonUlity::m_haveCalcedDisplacement      = false;

EWGR_GESTURE_SHAPE CWGRCommonUlity::m_curGestureShape = E_WGR_GShape_Unknown;
bool               CWGRCommonUlity::m_haveGSDetermined = false;

IWGRUtilContext    *CWGRCommonUlity::m_pWGRUtilContext = NULL;

int                 CWGRCommonUlity::m_nCurSameDirTouchs = 0;

//////////////////////////////////////////////////////////////////////////
/*
用到的运动方向种类：
E_WGR_MDIR_Unknown，E_WGR_MDIR_None，E_WGR_MDIR_Move，E_WGR_MDIR_LHoriz
E_WGR_MDIR_RHoriz， E_WGR_MDIR_UVertic，E_WGR_MDIR_DVertic
*/
const int KMAXDIRTYPES = 7;
const int KPOS_DIR_UNKNOWN = 0;
const int KPOS_DIR_NONE = 1;
const int KPOS_DIR_MOVE = 2;
const int KPOS_DIR_LHORIZ = 3;
const int KPOS_DIR_RHORIZ = 4;
const int KPOS_DIR_UVERTIC = 5;
const int KPOS_DIR_DVERTIC = 6;


const E_WGR_MOVEDIR    KDIRMAPS[KMAXDIRTYPES] = {E_WGR_MDIR_Unknown, E_WGR_MDIR_None, E_WGR_MDIR_Move,
                        E_WGR_MDIR_LHoriz, E_WGR_MDIR_RHoriz, E_WGR_MDIR_UVertic, E_WGR_MDIR_DVertic}; //运动方向的映射图
//////////////////////////////////////////////////////////////////////////

CWGRCommonUlity::CWGRCommonUlity(void)
{
}

CWGRCommonUlity::~CWGRCommonUlity(void)
{
}

bool CWGRCommonUlity::CalcDisplacement(const int &xMaxDisLimit, const int &yMaxDisLimit, bool toBeAverage)
{     
    m_curXDisplacement = m_curYDisplacement = 0;
	m_curPtMiddle.x = m_curPtMiddle.y = 0;

    //g_oWGRTraceLog.AddLastSpotMap(m_lastTouchInfoMap);

    std::map<UINT, ST_MOTIONINFO>::iterator it = m_lastTouchInfoMap.end();    

    for(int i = 0; i < m_nCurMatchedTouchs; i++)
    {
        UINT key = m_curMatchedTInfo[i].uId;

        it = m_lastTouchInfoMap.find(key);
        if (it != m_lastTouchInfoMap.end())
        {
            ST_MOTIONINFO &refCurMInfo = m_curMatchedTInfo[i].stMotionInfo;
            ST_MOTIONINFO &refLastMInfo = it->second;

            m_curXDisplacement += (refCurMInfo.nXDisplacement = (refCurMInfo.ptPos.x - refLastMInfo.ptPos.x) + refLastMInfo.nXDisplacement);
            m_curYDisplacement += (refCurMInfo.nYDisplacement = (refCurMInfo.ptPos.y - refLastMInfo.ptPos.y) + refLastMInfo.nYDisplacement);        

            m_curPtMiddle.x += refCurMInfo.ptPos.x;
            m_curPtMiddle.y += refCurMInfo.ptPos.y;

            refLastMInfo = refCurMInfo;
        }		
    }    

    if (m_nCurMatchedTouchs > 0 && toBeAverage)
    {
        m_curXDisplacement = (m_curXDisplacement + m_nCurMatchedTouchs - 1) / m_nCurMatchedTouchs;
        m_curYDisplacement = (m_curYDisplacement + m_nCurMatchedTouchs - 1) / m_nCurMatchedTouchs;

		m_curPtMiddle.x = (m_curPtMiddle.x + m_nCurMatchedTouchs - 1) / m_nCurMatchedTouchs;
		m_curPtMiddle.y = (m_curPtMiddle.y + m_nCurMatchedTouchs - 1) / m_nCurMatchedTouchs;
    }
    
    //删除MisMatched触点
    for (int i = 0; i < m_nCurMisMatchedTouchs; i++)
    {  
        UINT key = m_curMisMatchedTInfo[i].uId;
        it = m_lastTouchInfoMap.find(key);

        if (it != m_lastTouchInfoMap.end())
        {
            m_lastTouchInfoMap.erase(it);
        }
    }

    //插入新增加的触点
    for (int i = 0; i < m_nCurNewAddedTouchs; i++)
    {
        UINT key = m_curNewAddedTInof[i].uId;
        m_curNewAddedTInof[i].stMotionInfo.nXDisplacement = m_curNewAddedTInof[i].stMotionInfo.nYDisplacement = 0;
        m_lastTouchInfoMap[key] = m_curNewAddedTInof[i].stMotionInfo;
    }

    g_oWGRTraceLog.AddSpotInfo(0, m_curMisMatchedTInfo, m_nCurMisMatchedTouchs);
    g_oWGRTraceLog.AddSpotInfo(1, m_curMatchedTInfo, m_nCurMatchedTouchs);
    g_oWGRTraceLog.AddSpotInfo(2, m_curNewAddedTInof, m_nCurNewAddedTouchs);    

    m_haveCalcedDisplacement = true;

    return true;
}

E_WGR_MOVEDIR CWGRCommonUlity::CalcMoveDirectionByMiddlePoint(const UINT &nXMoveStandard, const UINT &nYMoveStandard, int nXDisMulti, int nYDisMulti)
{
    E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;

    int nActualXMoveStandard = nXMoveStandard;
    int nActualYMoveStandard = nYMoveStandard;
    int nActualXDisMulti = nXDisMulti;
    int nActualYDisMulti = nYDisMulti;

    if (m_pWGRUtilContext)
    {
        nActualXMoveStandard = m_pWGRUtilContext->MapStandX2Actual(nActualXMoveStandard);
        nActualYMoveStandard = m_pWGRUtilContext->MapStandY2Actual(nActualYMoveStandard);
       /* nActualXDisMulti = m_pWGRUtilContext->MapStandX2Actual(nActualXDisMulti);
        nActualYDisMulti = m_pWGRUtilContext->MapStandY2Actual(nYDisMulti);*/
    }

    if (!m_haveCalcedDisplacement)
    {//如果外部没有计算
        CalcDisplacement();
    }

    if (nActualXMoveStandard > WGR_INT32_ABS(m_curXDisplacement) && nActualYMoveStandard > WGR_INT32_ABS(m_curYDisplacement))
    {//未移动
        dir = E_WGR_MDIR_None;
    }
    else 
    {
        dir = E_WGR_MDIR_Move;

        if (WGR_INT32_ABS(m_curXDisplacement) > WGR_INT32_ABS(m_curYDisplacement))
        {//水平移动距离更大            
            if(WGR_INT32_ABS(m_curXDisplacement) > nActualXDisMulti * WGR_INT32_ABS(m_curYDisplacement))
            {//水平运动
                //运动方向判断
                if(m_curXDisplacement < 0)
                {
                    dir = E_WGR_MDIR_LHoriz;
                }
                else
                {
                    dir = E_WGR_MDIR_RHoriz;
                }
            }
        }
        else if(WGR_INT32_ABS(m_curYDisplacement) > nActualYDisMulti * WGR_INT32_ABS(m_curXDisplacement))
        {//竖直运动
            //运动方向判断
            if(m_curYDisplacement < 0)
            {
                dir = E_WGR_MDIR_UVertic;
            }
            else
            {
                dir = E_WGR_MDIR_DVertic;
            }
        }
    }

    m_haveCalcedDisplacement = false;
    return dir;
}

E_WGR_MOVEDIR CWGRCommonUlity::CalcMoveDirectionByStatistics(const UINT &nXMoveStandard, const UINT &nYMoveStandard, const int &nMinTouchsLimit, const int &nMaxTouchsLimit, int nXDisMulti, int nYDisMulti)
{
    E_WGR_MOVEDIR dir = E_WGR_MDIR_Unknown;    

    int nActualXMoveStandard = nXMoveStandard;
    int nActualYMoveStandard = nYMoveStandard;
    int nActualXDisMulti = nXDisMulti;
    int nActualYDisMulti = nYDisMulti;

    if (m_pWGRUtilContext)
    {
        nActualXMoveStandard = m_pWGRUtilContext->MapStandX2Actual(nActualXMoveStandard);
        nActualYMoveStandard = m_pWGRUtilContext->MapStandY2Actual(nActualYMoveStandard);
        /*nActualXDisMulti = m_pWGRUtilContext->MapStandX2Actual(nActualXDisMulti);
        nActualYDisMulti = m_pWGRUtilContext->MapStandY2Actual(nYDisMulti);*/
    }

    /*
        用到的运动方向种类：
        E_WGR_MDIR_Unknown，E_WGR_MDIR_None，E_WGR_MDIR_Move，E_WGR_MDIR_LHoriz
        E_WGR_MDIR_RHoriz， E_WGR_MDIR_UVertic，E_WGR_MDIR_DVertic
    */    
    byte dirVoteStatis[KMAXDIRTYPES]; //方向统计
    short xDispVoteStatis[KMAXDIRTYPES]; //x方向位移统计
    short yDispVoteStatis[KMAXDIRTYPES]; //y方向位移统计
	POINT ptVoteStatis[KMAXDIRTYPES];	//投票点位置的统计

    memset(dirVoteStatis, 0, KMAXDIRTYPES * sizeof(byte));
    memset(xDispVoteStatis, 0, KMAXDIRTYPES * sizeof(short));
    memset(yDispVoteStatis, 0, KMAXDIRTYPES * sizeof(short));
	memset(ptVoteStatis, 0, KMAXDIRTYPES * sizeof(POINT));

    if (!m_haveCalcedDisplacement)
    {//如果外部没有计算
        CalcDisplacement(100, 100,false);
    }

	dirVoteStatis[KPOS_DIR_UNKNOWN] = m_nCurMatchedTouchs; //把所有触点的运动方向都初始化unknown
	xDispVoteStatis[KPOS_DIR_UNKNOWN] = m_curXDisplacement;
	yDispVoteStatis[KPOS_DIR_UNKNOWN] = m_curYDisplacement;
	ptVoteStatis[KPOS_DIR_UNKNOWN] = m_curPtMiddle;

    for(int i = 0; i < m_nCurMatchedTouchs; i++)
    {
        int curXDisplacement = m_curMatchedTInfo[i].stMotionInfo.nXDisplacement;
        int curYDisplacement = m_curMatchedTInfo[i].stMotionInfo.nYDisplacement;
		int curXPos			 = m_curMatchedTInfo[i].stMotionInfo.ptPos.x;
		int curYPos			 = m_curMatchedTInfo[i].stMotionInfo.ptPos.y;

        if (nActualXMoveStandard > WGR_INT32_ABS(curXDisplacement) && nActualYMoveStandard > WGR_INT32_ABS(curYDisplacement))
        {//未移动
            dirVoteStatis[KPOS_DIR_NONE]++;
            dirVoteStatis[KPOS_DIR_UNKNOWN]--;

            xDispVoteStatis[KPOS_DIR_NONE] += curXDisplacement; 
            yDispVoteStatis[KPOS_DIR_NONE] += curYDisplacement;

            xDispVoteStatis[KPOS_DIR_UNKNOWN] -= curXDisplacement; 
            yDispVoteStatis[KPOS_DIR_UNKNOWN] -= curYDisplacement;

			ptVoteStatis[KPOS_DIR_UNKNOWN].x -= curXPos;
			ptVoteStatis[KPOS_DIR_UNKNOWN].y -= curYPos;

			ptVoteStatis[KPOS_DIR_NONE].x += curXPos;
			ptVoteStatis[KPOS_DIR_NONE].y += curYPos;
        }
        else 
        {
            dirVoteStatis[KPOS_DIR_MOVE]++;
            dirVoteStatis[KPOS_DIR_UNKNOWN]--;

            xDispVoteStatis[KPOS_DIR_MOVE] += curXDisplacement; 
            yDispVoteStatis[KPOS_DIR_MOVE] += curYDisplacement;

            xDispVoteStatis[KPOS_DIR_UNKNOWN] -= curXDisplacement; 
            yDispVoteStatis[KPOS_DIR_UNKNOWN] -= curYDisplacement;

			ptVoteStatis[KPOS_DIR_UNKNOWN].x -= curXPos;
			ptVoteStatis[KPOS_DIR_UNKNOWN].y -= curYPos;

			ptVoteStatis[KPOS_DIR_MOVE].x += curXPos;
			ptVoteStatis[KPOS_DIR_MOVE].y += curYPos;

            if(WGR_INT32_ABS(curXDisplacement) > nActualXDisMulti * WGR_INT32_ABS(curYDisplacement))
            {//水平运动
                dirVoteStatis[KPOS_DIR_MOVE]--;
                xDispVoteStatis[KPOS_DIR_MOVE] -= curXDisplacement; 
                yDispVoteStatis[KPOS_DIR_MOVE] -= curYDisplacement;
				ptVoteStatis[KPOS_DIR_MOVE].x -= curXPos;
				ptVoteStatis[KPOS_DIR_MOVE].y -= curYPos;

                //运动方向判断
                if(curXDisplacement < 0)
                {
                    dirVoteStatis[KPOS_DIR_LHORIZ]++;

                    xDispVoteStatis[KPOS_DIR_LHORIZ] += curXDisplacement; 
                    yDispVoteStatis[KPOS_DIR_LHORIZ] += curYDisplacement;

					ptVoteStatis[KPOS_DIR_LHORIZ].x += curXPos;
					ptVoteStatis[KPOS_DIR_LHORIZ].y += curYPos;
                }
                else
                {
                    dirVoteStatis[KPOS_DIR_RHORIZ]++;

                    xDispVoteStatis[KPOS_DIR_RHORIZ] += curXDisplacement; 
                    yDispVoteStatis[KPOS_DIR_RHORIZ] += curYDisplacement;

					ptVoteStatis[KPOS_DIR_RHORIZ].x += curXPos;
					ptVoteStatis[KPOS_DIR_RHORIZ].y += curYPos;
                }
            }
            else if(WGR_INT32_ABS(curYDisplacement) > nActualYDisMulti * WGR_INT32_ABS(curXDisplacement))
            {//竖直运动
                dirVoteStatis[KPOS_DIR_MOVE]--;
                xDispVoteStatis[KPOS_DIR_MOVE] -= curXDisplacement; 
                yDispVoteStatis[KPOS_DIR_MOVE] -= curYDisplacement;

				ptVoteStatis[KPOS_DIR_MOVE].x -= curXPos;
				ptVoteStatis[KPOS_DIR_MOVE].y -= curYPos;

                //运动方向判断
                if(curYDisplacement < 0)
                {
                    dirVoteStatis[KPOS_DIR_UVERTIC]++;

                    xDispVoteStatis[KPOS_DIR_UVERTIC] += curXDisplacement; 
                    yDispVoteStatis[KPOS_DIR_UVERTIC] += curYDisplacement;

					ptVoteStatis[KPOS_DIR_UVERTIC].x += curXPos;
					ptVoteStatis[KPOS_DIR_UVERTIC].y += curYPos;
                }
                else
                {                    
                    dirVoteStatis[KPOS_DIR_DVERTIC]++;

                    xDispVoteStatis[KPOS_DIR_DVERTIC] += curXDisplacement; 
                    yDispVoteStatis[KPOS_DIR_DVERTIC] += curYDisplacement;

					ptVoteStatis[KPOS_DIR_DVERTIC].x += curXPos;
					ptVoteStatis[KPOS_DIR_DVERTIC].y += curYPos;
                }
            }
        }
    }

    int nPos = KPOS_DIR_UNKNOWN;
    short maxVoter = 0;
    
    if (IsInputTouchsBetween(nMinTouchsLimit, nMaxTouchsLimit))    
    {
        for (int i = 0; i < KMAXDIRTYPES; i++)
        {
            byte curVoter = dirVoteStatis[i];
            if (curVoter > maxVoter)
            {
                maxVoter = curVoter;
                nPos = i;
            }
        }
    }

    dir = KDIRMAPS[nPos];

    m_nCurSameDirTouchs = maxVoter; //（通过统计计算得到）当前移动方向中，符合该条件的移动方向的点数
    
    if (maxVoter > 0)
    {
        m_curXDisplacement = (xDispVoteStatis[nPos] + maxVoter - 1) / maxVoter;
        m_curYDisplacement = (yDispVoteStatis[nPos] + maxVoter - 1) / maxVoter;

		m_curPtMiddle.x = (ptVoteStatis[nPos].x + maxVoter - 1) / maxVoter;
		m_curPtMiddle.y = (ptVoteStatis[nPos].y + maxVoter - 1) / maxVoter;
    }
    else
    {
        m_curXDisplacement = 0;
        m_curYDisplacement = 0;

		m_curPtMiddle.x = 0;
		m_curPtMiddle.y = 0;
    }

    g_oWGRTraceLog.AddPTMiddle(m_curPtMiddle);
    g_oWGRTraceLog.AddElectee(dir, maxVoter);

    g_oWGRTraceLog.AddXShift(m_curXDisplacement);
    g_oWGRTraceLog.AddYShift(m_curYDisplacement);

    m_haveCalcedDisplacement = false;
    return dir;
}

bool CWGRCommonUlity::IsTheEventPermit(IWGRUtilContext *pContext, const E_TRIGGEREVENT &theEvent/*, const int &nMinTouchsLimit, const int &nMaxTouchsLimit*/)
{    
    /*return true;*/

    //CPerfDetector perf(_T("IsTheEventPermit"));
	int nPermitStatis = 0;
	if (!pContext) return false;

	if(m_nCurMatchedTouchs + m_nCurNewAddedTouchs <= 0) return false;

	bool perm = false;
	if (m_nCurMatchedTouchs > 0)
	{
		perm = pContext->Get_TriggerEvent_Permission(theEvent, m_curMatchedTInfo[0].stMotionInfo.ptPos);
	}
	else
	{
		perm = pContext->Get_TriggerEvent_Permission(theEvent, m_curNewAddedTInof[0].stMotionInfo.ptPos);
	}

	return perm;
}

void CWGRCommonUlity::InitWhenFirstEnter(IWGRUtilContext *pContext)
{	
	m_lastTouchInfoMap.clear();

	for(int i = 0; i < m_nCurMatchedTouchs; i++)
	{
		m_lastTouchInfoMap[m_curMatchedTInfo[i].uId] = m_curMatchedTInfo[i].stMotionInfo;        
	}

	for(int i = 0; i < m_nCurNewAddedTouchs; i++)
	{
		m_lastTouchInfoMap[m_curNewAddedTInof[i].uId] = m_curNewAddedTInof[i].stMotionInfo;
	}

	if (pContext)
	{
		pContext->TriggerHIDUp(NULL);
	}

	m_havedEnteredTheSumSM = true;
}

void CWGRCommonUlity::ResetDislaymentInLastTouchMap()
{
	std::map<UINT, ST_MOTIONINFO>::iterator it = m_lastTouchInfoMap.begin();

	for (; it != m_lastTouchInfoMap.end(); it++)
	{
		(*it).second.nXDisplacement = 0;
		(*it).second.nYDisplacement = 0;
	}
}