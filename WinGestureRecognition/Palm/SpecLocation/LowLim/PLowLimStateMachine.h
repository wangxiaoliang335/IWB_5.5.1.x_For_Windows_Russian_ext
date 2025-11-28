#pragma once
#include "..\..\..\wgrstatemachineinterface.h"
#include "PShowWndSBSStateMachine.h"

class CPLowLimStateMachine :
    public IWGRStateMachine
{
public:
    CPLowLimStateMachine(void);
    ~CPLowLimStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);

private:
    CPShowWndSBSStateMachine    m_smShowWndSBS;
    WGR_STATE_TYPE              m_curState;
};
