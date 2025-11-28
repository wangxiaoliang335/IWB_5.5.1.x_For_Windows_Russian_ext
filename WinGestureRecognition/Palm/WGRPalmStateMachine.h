#pragma once
#include "..\wgrstatemachineinterface.h"
#include ".\NonSpecLocation\PNonSpeccLocStateMachine.h"
#include ".\SpecLocation\LowLim\PLowLimStateMachine.h"
#include "..\GlobalLog.h"

class CWGRPalmStateMachine :
    public IWGRStateMachine
{
public:
    CWGRPalmStateMachine(void);
    ~CWGRPalmStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

public:
    void StopLog()
    {
        if (g_oLogHasStarted)
        {
            //πÿ±’»’÷æ
            g_oWGRTraceLog.StopLog();

            g_oLogHasStarted = false;
            g_oCurCollects = 0;
        }
    }

private:
    CPNonSpecLocStateMachine               m_smNonSpecLoc;
    CPLowLimStateMachine                   m_smLowLim;

    WGR_STATE_TYPE                         m_curState;    
};
