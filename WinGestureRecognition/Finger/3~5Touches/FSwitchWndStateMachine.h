#pragma once
#include "..\..\wgrstatemachineinterface.h"

class CFSwitchWndStateMachine :
    public IWGRStateMachine
{
public:
    CFSwitchWndStateMachine(void);
    ~CFSwitchWndStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);

private:    
    WGR_STATE_TYPE  m_curState;

    int             m_nConLMoves; //连续向左移动的次数
    int             m_nConRMoves; //连续向右移动的次数
};
