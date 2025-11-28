#pragma once

#include "..\..\WGestureRecPublic.h"
#include "..\..\WGRCommonUlity.h"

//class CFSixMoreTouchPublic
//{
//
//    CFSixMoreTouchPublic(void);
//    ~CFSixMoreTouchPublic(void);
//
//public:
//    static void ResetSomeMembers()
//    {
//        m_curMissTouchCountTolerance = 0;
//        m_curMissDisLimitTolerance = 0;
//        m_curMissMDirTolerance = 0;
//        m_curMissShapeTolerance = 0;
//        m_curNonMoveTolerance = 0;       
//
//        m_bEnterLessThanOne = true;
//    }
//
//    /*
//    @功能：计算运动方向，计算的位置是根据采集触点的平均位置
//    @参数：aryLightSpots，光斑数组;
//    nCount，光斑个数
//    nXMoveStard，nYMoveStard，移动距离的标准，在这里，超过任一标准，就被认为移动了
//    */
//    static E_WGR_MOVEDIR CalcMoveDirection(const TLightSpot * aryLightSpots,  int nCount, const int nXMoveStandard, const int nYMoveStandard);
//
//    /*
//    @功能：光点匹配，即当前的两个光点与上一次采集的两个光点间匹配（对应关系）
//    @参数：aryLightSpots，光点数组
//    nCount，光点个数
//    xMaxDisLimit，当前光斑x方向所允许的最大距离限制
//    yMaxDisLimit，当前光斑y方向所允许的最大距离限制
//    @返回值：true，匹配成功；false，匹配失败
//    */
//    static bool  MatchTouches(const TLightSpot * aryLightSpots,  int nCount, int xMaxDisLimit, int yMaxDisLimit);
//
//    /*
//    @功能：计算光斑集的中心
//    @参数：aryLightSpots，光点数组
//    nCount，光点个数
//    ptMid[in, out]，光斑集的中心点
//    @返回值：true,计算成功；false,计算失败
//    */
//    static bool CalcMiddlePoint(const TLightSpot * aryLightSpots,  int nCount, POINT &ptMid);   
//
//public:
//    static int                  m_curMissShapeTolerance; //形状缺失容忍度，即非手指形状容忍度（连续非手指形状累计值）
//                                                            //，如果它大于最大容忍度，将结束两点的监听状态
//    static int                  m_curMissTouchCountTolerance;//光点个数不满足要求的容忍度，如果它大于
//                                                            //最大容忍值，将结束监听状态
//    static int                  m_curMissDisLimitTolerance; //触点违背距离容忍值，即光点之间的距离连续超过限制的累计值，如果它大于
//                                                            //最大容忍值，借结束监听状态
//    static int                  m_curMissMDirTolerance;     //移动方向容忍度(它强调是移动了)，即移动方向非水平或垂直的容忍度（连续非水平向左或
//
//    static int                  m_curNonMoveTolerance;      //不移动容忍度，即光斑连续不移动的累计值，如果它大于最大容忍值，将结束监听状态
//
//    static int                  m_nXDisplacement;           //x方向的位移
//    static int                  m_nYDisplacement;           //y方向的位移
//    
//    static POINT                m_ptSartMid;               //开始采集坐标点的中间值，实质上是本次移动（移动距离超过一定的阈值才认为移动）采集的开始
//    static POINT                m_ptLastMid;               //上一次采集坐标点的中间值    
//
//    static bool                 m_bEnterLessThanOne;           //进入子状态机次数小于1次，即还未进入过子状态机
//};
