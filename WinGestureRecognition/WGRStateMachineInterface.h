#pragma once

#include "../inc/LightSpot.h"
#include "WGestureRecPublic.h"
#include "../inc/wgr/IWGRUtilContext.h"

class IWGRStateMachine
{
public:
    /*
     @功能：运行自动机
     @返回值：true，运行成功；否则，运行失败
    */
    virtual bool StartRun() = 0;

    /*
    @功能：停止自动机
    @返回值：true，停止成功；否则，停止失败
    */
    virtual bool StopRun() = 0;

    /*
    @功能：获取当前状态所在的子自动机接口
    @返回值：当前状态所在的子自动机接口
    */
    virtual IWGRStateMachine *GetSubMachineCurStateContained() = 0;

    /*
    @功能：状态转换，并且状态转换后，执行的相应的动作
    @参数：aryLightSpots，光斑数组；
           nCount，光斑数组大小；
           gestureShape，手势形状；
           pContext，指向调用者，主要用来执行回调函数
    @返回值：返回当前状态
    */
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown) = 0;

	/*
	@功能：状态转换，并且状态转换后，执行相应的功能
	@参数：
	@返回值：返回当前状态
	*/
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext) = 0;

    ///*
    //@功能：状态转换后，执行的动作，所以它和TransitState成对出现，并且一定在TransitState后调用
    //@参数：pContext，指向调用者，主要用来执行回调函数
    //@返回值：返回当前状态，之所以在该处返回，而不在TransitState后返回，是因为状态机中有一些临时状态，在DoAction后再次变化，所以此时的当前
    //        状态，才是具有持久性的状态
    //*/
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext)  = 0;
};