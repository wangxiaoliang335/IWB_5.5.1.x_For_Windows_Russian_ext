#pragma once
#include "..\..\wgrstatemachineinterface.h"

class CFHMouseWheelStateMachine :
    public IWGRStateMachine
{
public:
    CFHMouseWheelStateMachine(void);
    ~CFHMouseWheelStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

private:
    WGR_STATE_TYPE                  m_curState;    //µ±Ç°×´Ì¬ 
};
