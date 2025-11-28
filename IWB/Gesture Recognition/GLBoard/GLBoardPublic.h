#pragma once

//////////////////////////////////////////////////////////////////////////
//两个光点移动方式
typedef enum
{
    E_2TOUCHS_MOVES_FALSE       , //非两点
    E_2TOUCHS_MOVES_NONE        , //未移动
    E_2TOUCHS_MOVES_SAME        , //同向移动，漫游时的移动形式
    E_2TOUCHS_MOVES_ROAM        ,
    E_2TOUCHS_MOVES_SELECTION   , 
    //E_2TOUCHS_MOVES_BACKWARD    , //背向移动，放大时的移动形式
    //E_2TOUCHS_MOVES_OPPOSITE    , //相向移动，缩小时的运动形式
    E_2TOUCHS_MOVES_ZOOM        , //缩放移动
    E_2TOUCHS_MOVES_RORATE      , //旋转移动
}E_2TOUCHS_MOVES;

//added by toxuke@gmail.com,  2014/01/08
//白板软件所处状态的枚举值
typedef enum 
{
    E_WBS_IDLE             ,    //无状态
    E_WBS_ERASER           ,    //板擦状态
    E_WBS_PEN              ,    //笔状态
    E_WBS_ROAM             ,    //漫游状态
    E_WBS_SPOTLIGHT        ,    //探照灯状态
    E_WBS_RETURNTODISKTOP  ,    //返回桌面状态
    E_WBS_NEXTPAGE         ,    //下一页状态
    E_WBS_PROPAGE          ,    //上一页状态
    E_WBS_ZOOM             ,    //缩放
    E_WBS_RORATE           ,    //旋转
    E_WBS_MASK             ,    //幕布
    E_WBS_SELECTION        ,    //选择笔画对象

}E_WBS;    

//**************************************************************
//定义手势形状
typedef enum
{
    E_GLGR_GShape_Unknown,
    E_GLGR_GShape_Finger,
    E_GLGR_GShape_Palm
}EGLGR_GESTURE_SHAPE;
//**************************************************************

enum GLMatchState
{
    E_GLMISMATCHED  = 0,
    E_GLMATCHED     = 1,
    E_GLNEW_ELEMENT = 2,
};

struct GLMatchInfo
{
    UINT  uId              ;//从0开始的匹配编号
    POINT ptPos            ;//坐标。
    GLMatchState eMatchState;//匹配状态
};