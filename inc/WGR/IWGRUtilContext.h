#pragma once

#include "WGRPermissionPublic.h"

//窗口切换相关枚举量
typedef enum
{
    E_SWHWND_NONEMODE,   //未定义方式
    E_SWHWND_STARTMODE, //开始窗口切换
    E_SWHWND_EXECMODE,  //执行窗口切换
    E_SWHWND_EXELMODE,  //向左切换窗口
    E_SWHWND_EXERMODE,  //向右切换窗口
    E_SWHWND_OKMODE,    //确定
    E_SWHWND_CANCLEMODE,//取消切换
    E_SWHWND_ENDMODE    //结束窗口切换
}E_SWHWND_MODE;

//窗口移动相关枚举量
typedef enum
{
    E_MOVWND_NONMODE,
    E_MOVWND_STARTMODE,
    E_MOVWND_EXECMODE,
    E_MOVWND_ENDMODE,
}E_MOVWND_MODE;


//手势识别后，用于执行相关功能的接口
//主要是一些回调函数
class IWGRUtilContext
{
public:
    /*
    @功能：按下虚拟设备
    @参数：按下位置
    */
    virtual void TriggerHIDDown(const POINT *pt) = 0;
    /*
    @功能：弹起虚拟设备
    @参数：弹起位置
    */
    virtual void TriggerHIDUp(const POINT *pt) = 0;


    /*
    @功能：触发鼠标水平滚轮事件
    @参数：rollDistance，滚动距离，默认为120;
           dir，滚动方向，-1向左，1向右，其他不移动
    */
    virtual void TriggerHMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120) = 0;
    /*{
        return;
    }*/

    /*
    @功能：触发鼠标竖直滚轮事件
    @参数：rollDistance，滚动距离，默认为120;
    dir，滚动方向，-1向上，1向下，其他不移动
    */
    virtual void TriggerVMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120) = 0;
    /*{
        return;
    }*/

    /*
    @功能：触发Alt + Left Arrow事件
    @参数：ptCur，光斑位置
    */
    virtual void TriggerAltLArrowEvent(const POINT &ptCur) = 0;

    /*
    @功能：触发Alt + Right Arrow事件
    @参数：ptCur，光斑位置
    */
    virtual void TriggerAltRArrowEvent(const POINT &ptCur) = 0;

    /*
    @功能：触发最小化当前窗口事件
    @参数：ptCur，光斑位置
    */
    virtual void TriggerMiniCurWindow(const POINT &ptCur) = 0;

    /*
    @功能：触发最大化当前窗口事件
    @参数：ptCur，光斑位置
    */
    virtual void TriggerMaxiCurWindow(const POINT &ptCur) = 0;

    /*
    @功能：触发显示桌面事件
    @参数：ptCur，光斑位置
    */
    virtual void TriggerDisplayDesktop(const POINT &ptCur) = 0;

    //////////////////////////////////////////////////////////////////////////
    /*
    @功能：触发并列显示窗口事件
    @参数：ptCur，光斑位置
    */
    virtual void TriggerShowWndSBS(const POINT &ptCur) = 0;

    /*
    @功能：触发刷新窗口事件
    @参数：ptCur，光斑位置
    */
    virtual void TriggerRefreshWnd(const POINT &ptCur) = 0;

    /*
       @功能：触发开启窗口切换事件
       @参数：ptCur，光斑位置
              eSWMode，窗口切换的模式（即是开启，还是执行，还是其他）
    */
    virtual void TriggerSwitchWindowEvent(const POINT &ptCur, E_SWHWND_MODE eSWMode = E_SWHWND_NONEMODE) = 0;

    /*
       @功能：触发提示可以进行窗口切换（目前是改变鼠标形状）
       @参数：ptCur，光斑位置
              bHint，true，开启提示；false，关闭提示
    */
    virtual void TriggerSwitchWindowHintEvent(POINT ptCur, bool bHint) = 0;

    /*
        @功能：触发文档保存事件
        @参数：ptCur
    */
    virtual void TriggerDocSaveEvent(const POINT &ptCur) = 0;

    /*
        @功能：触发关闭当前窗口事件
        @参数：ptCur
    */
    virtual void TriggerCloseCurrentWindowEvent(const POINT &ptCur) = 0;

    /*
        @功能：触发提示可以进行窗口移动（目前是改变鼠标形状）
        @参数：ptCur，光斑位置
               bHint，true，开启提示；false，关闭提示
    */
    virtual void TriggerMoveWindowHintEvent(POINT ptCur, bool bHint) = 0;

    /*
        @功能：触发窗口移动事件
        @参数：ptCur，当前光斑位置
               ptLast,先前光斑位置
               eMode，窗口移动方式
    */
    virtual void TriggerMoveWindowEvent(POINT ptCur, POINT ptLast, E_MOVWND_MODE eMode) = 0;

public:    
    /*
        @功能：获取“是否允许触发**事件”属性
    */
    virtual bool Get_TriggerEvent_Permission(E_TRIGGEREVENT eEvent, const POINT &pt) = 0;

    /*
        @功能：是否关闭了所有手势
    */
    virtual bool IsAllGestureClosed() = 0;

    /*
        @功能：是否关闭了所有基于手掌的手势
    */
    virtual bool IsAllPalmBasedGestureClosed() = 0;

    /*
        @功能：是否关闭了所有基于手指的手势
    */
    virtual bool IsAllFingerBasedGestureClosed() = 0;

    /*
        @功能：是否允许开启日志
    */
    virtual bool IsLogPermited() = 0;

    /*
        @功能：获取手掌与标准采样光斑的倍数
    */
    virtual int GetMultiOfPalm2Std() = 0;

    /*
    //@功能:屏幕分辨率变化事件响应函数
    //@参数:nScreenWidth, 新的屏幕宽度
    //      nScreenHeight,新的屏幕高度
    */
    virtual void OnDisplayChange(int nScreenWidth, int nScreenHeight) = 0;

    /*
    //@功能:物理尺寸变化时间响应函数
    //@参数:物理尺寸
    */
    virtual void OnPhysicalLengthChange(float fPhysicalLength) = 0;

    /*
        @功能：映射x方向的标准距离到本环境的实际距离
        @附加说明：该标准为——80英寸白板、1024 * 768分辨率
    */
    virtual int MapStandX2Actual(int xStandard) = 0;

    /*
        @功能：映射y方向的标准距离到本环境的实际距离
        @附加说明：该标准为——80英寸白板、1024 * 768分辨率
    */
    virtual int MapStandY2Actual(int yStandard) = 0;

    /*
        @功能：获取加权光斑质量
    */
    virtual int GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti) = 0;
};