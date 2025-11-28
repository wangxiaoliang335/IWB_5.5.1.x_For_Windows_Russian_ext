#pragma once
#include "..\..\wgrstatemachineinterface.h"
#include "FHMouseWheelStateMachine.h"
#include "FVMouseWheelStateMachine.h"

class CFTwoTouchStateMachine :
    public IWGRStateMachine
{
public:
    CFTwoTouchStateMachine(void);
    ~CFTwoTouchStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

private:   

private:
    CFHMouseWheelStateMachine       m_smHMouseWheel;
    CFVMouseWheelStateMachine       m_smVMouseWheel;
    
    WGR_STATE_TYPE                  m_curState;    //µ±Ç°×´Ì¬    
};
