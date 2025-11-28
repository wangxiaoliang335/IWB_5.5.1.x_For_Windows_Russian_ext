#include "StdAfx.h"
#include "FComplexMotionStateMachine.h"

CFComplexMotionStateMachine::CFComplexMotionStateMachine(void)
{
}

CFComplexMotionStateMachine::~CFComplexMotionStateMachine(void)
{
}

bool CFComplexMotionStateMachine::StartRun()
{
    return true;
}

bool CFComplexMotionStateMachine::StopRun()
{
    return true;
}

IWGRStateMachine *CFComplexMotionStateMachine::GetSubMachineCurStateContained()
{
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFComplexMotionStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFComplexMotionStateMachine::TransitState(IWGRUtilContext *pContext)
{
	return WGR_IDLE_STATE;
}

