#pragma once
#include "wgrstatemachineinterface.h"
#include ".\Finger\WGRFingerStateMachine.h"
#include ".\Palm\WGRPalmStateMachine.h"

class CWGRStateMachine :
    public IWGRStateMachine
{
public:
    CWGRStateMachine(void);
    ~CWGRStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

private:    

private:
    CWGRPalmStateMachine                m_smPalm;
    CWGRFingerStateMachine              m_smFinger;
    IWGRStateMachine                    *m_pIWGRSM;

    WGR_STATE_TYPE                      m_curState;     
};
