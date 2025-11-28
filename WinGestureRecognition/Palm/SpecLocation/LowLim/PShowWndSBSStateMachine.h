#pragma once
#include "..\..\..\wgrstatemachineinterface.h"

class CPShowWndSBSStateMachine :
    public IWGRStateMachine
{
public:
    CPShowWndSBSStateMachine(void);
    ~CPShowWndSBSStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);

private:
    WGR_STATE_TYPE      m_curState;
};
