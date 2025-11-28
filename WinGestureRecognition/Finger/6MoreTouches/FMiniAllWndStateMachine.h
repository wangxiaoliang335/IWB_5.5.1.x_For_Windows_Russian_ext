#pragma once
#include "..\..\wgrstatemachineinterface.h"

class CFMiniAllWndStateMachine :
    public IWGRStateMachine
{
public:
    CFMiniAllWndStateMachine(void);
    ~CFMiniAllWndStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE CFMiniAllWndStateMachine::TransitState(IWGRUtilContext *pContext);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

private:
    WGR_STATE_TYPE m_curState;   

    int            m_nConMoves; //连续向下移动的次数
};
