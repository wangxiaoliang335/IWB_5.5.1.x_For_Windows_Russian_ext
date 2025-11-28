#pragma once
#include "..\..\wgrstatemachineinterface.h"
#include "FMiniAllWndStateMachine.h"
//#include "FDspDesktopStateMachine.h"

class CFSixMoreTouchStateMachine :
    public IWGRStateMachine
{
public:
    CFSixMoreTouchStateMachine(void);
    ~CFSixMoreTouchStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

private:
    CFMiniAllWndStateMachine    m_smMiniAllWnd;
    WGR_STATE_TYPE              m_curState; 
};
