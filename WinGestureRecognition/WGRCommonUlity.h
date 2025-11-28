#pragma once

#include <map>
#include <atlbase.h>
//#include <list>

#include "WGestureRecPublic.h"
#include "../inc/wgr/IWGRUtilContext.h"
#include "../inc/wgr/WGRPermissionPublic.h"
#include "../inc/PerfDetector.h"

//struct ST_MOTIONINFO
//{
//    POINT       ptPos;  //坐标  
//    int         nXDisplacement; //位移
//    int         nYDisplacement; //位移
//};
//
//struct ST_TouchInfo
//{
//    UINT            uId;    //触点Id
//    ST_MOTIONINFO   stMotionInfo; //移动信息
//};

class CWGRCommonUlity
{
private:
    CWGRCommonUlity(void);
    ~CWGRCommonUlity(void);

public:
    static void ResetSomeMembers()
    {
        m_curUnexpectedShapeTimes       = 0;
        m_curUnexpectedTouchsTimes      = 0;        
        m_curUnexpectedTouchsDisTimes   = 0;
        m_curUnexpectedNonMoveTimes     = 0;
        m_curUnexpectedKnownMoveTimes   = 0;
        m_curUnexpectedUnKnownMoveTimes = 0;
        m_curUnexpectedUnknownShapeTimes = 0;
        m_havedEnteredTheSumSM          = false;        
    }

    static void ResetCurrentTouchInfo()
    { 
        m_nCurMatchedTouchs = 0;
        m_nCurMisMatchedTouchs = 0;
        m_nCurNewAddedTouchs = 0;
		m_curGestureShape = E_WGR_GShape_Unknown;
        m_haveGSDetermined = false;

        m_nCurSameDirTouchs = 0;
    }

	static bool IsInputTouchsEqual(const int nCount)
	{
		return ((m_nCurNewAddedTouchs + m_nCurMatchedTouchs) == nCount);
	}

    /*
    @功能：输入点是否等于nCount，并且这些输入点的在一定的范围内
    */
    static bool IsInputTouchsEqualWithinRange(const int &nCount, const int &nXRange, const int &nYRange);    

	static bool IsInputTouchsBetween(const int &nBegin, const int &nEnd)
	{
		int nTouchs = m_nCurNewAddedTouchs + m_nCurMatchedTouchs;
		if (nTouchs >= nBegin && nTouchs <= nEnd) return true;
		
		return false;
	}

	static bool IsTheEventPermit(IWGRUtilContext *pContext, const E_TRIGGEREVENT &theEvent);

	static void InitWhenFirstEnter(IWGRUtilContext *pContext);

	static void ResetDislaymentInLastTouchMap();


    /*
    @功能：计算运动方向，计算的位置是根据采集触点的平均位置
    @参数：
           nXMoveStard，nYMoveStard，移动距离的标准，在这里，超过任一标准，就被认为移动了
           nXDisMulti，nYDisMulti，水平位移与竖直位移的倍值，至少满足这个倍值关系，才能确定运动是水平还是竖直——
                |xDis| >= nXDisMulti * |yDis| 水平运动
                |yDis| >= nYDisMulti * |xDis| 竖直运动
    */
    static E_WGR_MOVEDIR CalcMoveDirectionByMiddlePoint(const UINT &nXMoveStandard, const UINT &nYMoveStandard, int nXDisMulti = 3, int nYDisMulti = 3);

    /*
    @功能：计算运动方向，计算的方法是根据多个触点运动趋势统计投票决定的方式
           触点运动趋势统计方式——那种运动方向统计较多，且不超出用户指定范围的运动方向为本次运动的方向
    @参数：
        nXMoveStard，nYMoveStard，移动距离的标准，在这里，超过任一标准，就被认为移动了
        nMinTouchsLimit，nMaxTouchsLimit，用户指定的触点范围
    */
    static E_WGR_MOVEDIR CalcMoveDirectionByStatistics(const UINT &nXMoveStandard, const UINT &nYMoveStandard, const int &nMinTouchsLimit, const int &nMaxTouchsLimit, int nXDisMulti = 3, int nYDisMulti = 3);

    /*
    @功能：计算触点的位移
    @参数：
        xMaxDisLimit，当前光斑x方向所允许的最大距离限制
        yMaxDisLimit，当前光斑y方向所允许的最大距离限制
        nMinMatchedLimit，至少要匹配的光斑个数
        nMaxMatchedLimit，至多要匹配的光斑个数
        toBeAverage，是否计算平均位移
    @返回值：true，计算成功；false，计算失败
    */
    static bool  CalcDisplacement(const int &xMaxDisLimit = 100, const int &yMaxDisLimit = 100, bool toBeAverage = true);

public:
    static int                              m_curUnexpectedShapeTimes;          //形状缺失容忍度
    static int                              m_curUnexpectedUnknownShapeTimes;   //无任何形状缺失容忍度
    static int                              m_curUnexpectedTouchsTimes;         //丢失预期触点数目容忍度
    //static int                              m_curUnexpectedMovDirTimes;         //非预期的运动次数    
    static int                              m_curUnexpectedTouchsDisTimes;      //非预期的光点间距离出现的次数
    static int                              m_curUnexpectedNonMoveTimes;        //非预期的光点不运动的次数
    static int                              m_curUnexpectedKnownMoveTimes;      //非预期的运动次数(该运动方向可知)
    static int                              m_curUnexpectedUnKnownMoveTimes;    //非预期的运动次数(该运动方向未知)

    static std::map<UINT, ST_MOTIONINFO>    m_lastTouchInfoMap;                 //上一帧采集的触点存储图
    static ST_TouchInfo                     m_curMatchedTInfo[KMAXTOUCHNUM];    //本帧采集的匹配的触点
    static ST_TouchInfo                     m_curMisMatchedTInfo[KMAXTOUCHNUM]; //本帧采集的匹配失败的触点
    static ST_TouchInfo                     m_curNewAddedTInof[KMAXTOUCHNUM];   //本帧采集的新增的触点

	static POINT							m_curPtMiddle;	//当前匹配触点的中间值

    static int                              m_nCurMatchedTouchs;
    static int                              m_nCurMisMatchedTouchs;
    static int                              m_nCurNewAddedTouchs;

    static int                              m_lastXDisplacement;  //上一次x方向平均位移
    static int                              m_lastYDisplacement;  //上一次y方向平均位移

    static int                              m_curXDisplacement;   //本次x方向平均位移
    static int                              m_curYDisplacement;   //本次y方向平均位移

    static bool                             m_havedEnteredTheSumSM;             //是否进入该子状态机   

	static EWGR_GESTURE_SHAPE				m_curGestureShape;		//当前输入的形状
    static bool                             m_haveGSDetermined;     //当前帧是否判定过输入形状，只有判定之后才允许进入识别处理

    static IWGRUtilContext                  *m_pWGRUtilContext;

    static int                              m_nCurSameDirTouchs; //当前移动方向（通过CalcMoveDirectionByStatistics）的触点数目

private:
    static bool                             m_haveCalcedDisplacement;           //位移是否已经计算过了
};
