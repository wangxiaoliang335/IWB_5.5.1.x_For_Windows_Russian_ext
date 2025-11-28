#include "StdAfx.h"
#include "FAltArrowStateMachine.h"

CFAltArrowStateMachine::CFAltArrowStateMachine(void)
{
}

CFAltArrowStateMachine::~CFAltArrowStateMachine(void)
{
}

bool CFAltArrowStateMachine::StartRun()
{
    return true;
}

bool CFAltArrowStateMachine::StopRun()
{
    return true;
}

IWGRStateMachine *CFAltArrowStateMachine::GetSubMachineCurStateContained()
{
    return (IWGRStateMachine *)this;
}

WGR_STATE_TYPE CFAltArrowStateMachine::TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape)
{
    return WGR_IDLE_STATE;
}

WGR_STATE_TYPE CFAltArrowStateMachine::TransitState(IWGRUtilContext *pContext)
{
	return WGR_IDLE_STATE;
}
