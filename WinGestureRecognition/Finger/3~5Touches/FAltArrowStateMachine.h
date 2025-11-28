#pragma once
#include "..\..\wgrstatemachineinterface.h"
#include "FAltLArrowStateMachine.h"
#include "FAltRArrowStateMachine.h"

class CFAltArrowStateMachine :
    public IWGRStateMachine
{
public:
    CFAltArrowStateMachine(void);
    ~CFAltArrowStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
    virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);

private:
    CFAltLArrowStateMachine     m_smAltLArrow;
    CFAltRArrowStateMachine     m_smAltRArrow;
};
