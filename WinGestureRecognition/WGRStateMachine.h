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
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

    void StartLog()
    {
        if (g_oLogHasStarted)
        {
            //关闭日志
            g_oWGRTraceLog.StopLog();

            g_oLogHasStarted = false;
            g_oCurCollects = 0;
        }

        //开启日志
        g_oWGRTraceLog.StartLog();
        g_oLogHasStarted = true;
        g_oCurCollects = 0;
    }

    void StopLog()
    {
        if (g_oLogHasStarted)
        {
            //关闭日志
            g_oWGRTraceLog.StopLog();

            g_oLogHasStarted = false;
            g_oCurCollects = 0;
        }
    }

private:    

private:
    CWGRPalmStateMachine                m_smPalm;
    CWGRFingerStateMachine              m_smFinger;
    IWGRStateMachine                    *m_pIWGRSM;

    WGR_STATE_TYPE                      m_curState;     
};
