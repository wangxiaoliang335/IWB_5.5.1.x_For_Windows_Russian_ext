#pragma once
#include "..\..\wgrstatemachineinterface.h"
#include "FMaxiCurWndStateMachine.h"
//#include "FDspDesktopStateMachine.h"

class CFComplexMotionStateMachine :
    public IWGRStateMachine
{
public:
    CFComplexMotionStateMachine(void);
    ~CFComplexMotionStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);

private:
    //CFMaxiCurWndStateMachine        m_smMaxiCurWnd;
    //CFDspDesktopStateMachine        m_smDspDesktop;
};
