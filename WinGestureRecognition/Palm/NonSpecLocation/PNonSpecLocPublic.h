#pragma once
#include "..\..\WGestureRecPublic.h"
#include "..\..\WGRCommonUlity.h"

////
//#define PMaxWInterSwitch    1800            //窗口切换最大等待间隔
//#define PMinWInterSave      1800            //文档保存最小等待间隔
////

//class CPNonSpecLocPublic
//{
//private:
//    CPNonSpecLocPublic(void);
//    ~CPNonSpecLocPublic(void);
//
//public:
//    static void ResetSomeMembers()
//    {
//        m_curMissShapeTolerance     = 0;
//        m_curMissMDirTolerance      = 0;
//        m_curMissShapeTolerance     = 0;
//        m_curNonMoveTolerance       = 0;
//
//        m_bEnterLessThanOne         = true;
//    }
//
//    static void ResetStartTime()
//    {
//        m_dwNonMoveStartTime = 0;
//    }
//
//    static void UpdateStartTime()
//    {
//        m_dwNonMoveStartTime = GetTickCount();
//    }
//
//    /*
//    @功能：主要判断是否移动
//    @参数：aryLightSpots，光斑数组
//    nCount，光斑数组元素个数
//    nXMoveStard，nYMoveStard，移动距离的标准，在这这里，只有超过任一标准，才被认为移动了
//    */
//    static E_WGR_MOVEDIR CalcMoveDirection(const TLightSpot *aryLightSpots,  int nCount,  const int nXMoveStandard, const int nYMoveStandard);
//
//    /*
//    @功能：匹配手掌（由于同时可能出现多个手掌，需要匹配）
//    @参数：aryLightSpots，光斑数组
//           nCount，光斑数组元素个数
//           ptCurPalm【in, out】，当前手掌的位置
//    @返回值：true，匹配成功；false，匹配失败
//    */
//    static bool MatchPalm(const TLightSpot *aryLightSpots, int nCount, POINT &ptCurPalm);
//
//public:
//    static int                  m_curMissShapeTolerance;    //形状缺失容忍度，即非手指形状容忍度（连续非手指形状累计值）
//    static int                  m_curMissMDirTolerance;     //移动方向容忍度(它强调是移动了)，即移动方向非水平或垂直的容忍度（连续非水平向左或
//                                                            //向右累计值)，如果它大于最大容忍值，将结束监听状态
//    static int                  m_curNonMoveTolerance;      //不移动容忍度，即光斑连续不移动的累计值，如果它大于最大容忍值，将结束监听状态
//
//    static int                  m_nXDisplacement;           //上次采集x方向的位移
//    static int                  m_nYDisplacement;           //上次采集y方向的位移
//
//    static DWORD                m_dwNonMoveStartTime;       //一次移动结束或者未移动的开始时间，
//
//    static POINT                m_ptStart;                  //一次移动结束，本次移动的手掌开始坐标点，即每次符合移动条件后，更新一次
//    static POINT                m_ptCur;                    //本次采集的位置
//    static POINT                m_ptLast;                   //上一次采集手掌的位置，每次采集更新一次
//    static int                  m_nLastMass;                //上一次采集手掌的质量
//    static int                  m_nLastStdMass;             //上一次采集手掌的标准质量
//
//    static bool                 m_bEnterLessThanOne;        //进入子状态机
//};
