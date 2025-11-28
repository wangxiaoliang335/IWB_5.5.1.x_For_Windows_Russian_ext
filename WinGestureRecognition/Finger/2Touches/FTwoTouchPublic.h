#pragma once

#include "..\..\WGRCommonUlity.h"

//class CFTwoTouchPublic
//{
//private:
//    CFTwoTouchPublic(void);
//    ~CFTwoTouchPublic(void);
//
//public:
//    static void ResetSomeMembers()
//    {
//        m_curMiss2TouchCountTolerance = 0;
//        m_curMiss2TouchDisLimitTolerance = 0;
//        m_curMissMDirTolerance = 0;
//        m_curMissShapeTolerance = 0;
//        m_curNonMoveTolerance = 0;   
//        m_nDisplacement = 0;
//        m_bEnterLessThanOne = TRUE;
//    }
//
//    /*
//    @功能：计算运动方向，计算的位置是根据采集触点的平均位置
//    @参数：ptCur，当前位置，这个点实际上是两触点平均位置点，即ptCur = (ptTouch1 + ptTouch2) / 2;
//           nXMoveStard，nYMoveStard，移动距离的标准，在这里，超过任一标准，就被认为移动了
//    */
//    static E_WGR_MOVEDIR CalcMoveDirection(const TLightSpot * aryLightSpots,  int nCount, const int nXMoveStandard, const int nYMoveStandard);
//
//    /*
//    @功能：光点匹配，即当前的两个光点与上一次采集的两个光点间匹配（对应关系）
//    @参数：aryLightSpots，光点数组
//           nCount，光点个数
//           xMaxDisLimit，当前光斑x方向所允许的最大距离限制
//           yMaxDisLimit，当前光斑y方向所允许的最大距离限制
//    @返回值：true，匹配成功；false，匹配失败
//    */
//    static bool  MatchTouches(const TLightSpot * aryLightSpots,  int nCount, int xMaxDisLimit, int yMaxDisLimit);
//
//public:
//    //
//    static int                             m_curMissShapeTolerance; //形状缺失容忍度，即非手指形状容忍度（连续非手指形状累计值）
//                                                                    //，如果它大于最大容忍度，将结束两点的监听状态
//    static int                             m_curMiss2TouchCountTolerance; //光点个数非两点的容忍度（连续非两点次数累计值），如果它大于
//                                                                            //最大容忍值，将结束两点的监听状态
//    static int                             m_curMiss2TouchDisLimitTolerance; //两触点违背距离容忍值，即两光点之间的距离连续超过限制的累计值，如果它大于
//                                                                            //最大容忍值，借结束两点的监听状态
//    static int                             m_curMissMDirTolerance;  //移动方向容忍度(它强调是移动了)，即移动方向非水平或垂直的容忍度（连续非水平向左或
//                                                                    //向右累计值)，如果它大于最大容忍值，将结束两点的监听状态
//    static int                             m_curNonMoveTolerance;   //不移动容忍度，即光斑连续不移动的累计值，如果它大于最大容忍值，将结束两点的监听状态    
//
//    //
//    static POINT                           m_ptCur[2];            //开始坐标点
//    static POINT                           m_ptLast[2];             //上一次坐标点
//
//    static int                             m_nDisplacement;         //触点位移，
//
//    static bool                            m_bEnterLessThanOne;           //进入两触点子状态机次数小于1次，即还未进入过两触点子状态机
//};
