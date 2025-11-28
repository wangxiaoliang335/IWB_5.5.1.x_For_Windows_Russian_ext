#pragma once
#include "..\wgrstatemachineinterface.h"
#include ".\2Touches\FTwoTouchStateMachine.h"
#include ".\3~5Touches\FThree2FiveTouchStateMachine.h"
#include ".\6MoreTouches\FSixMoreTouchStateMachine.h"
#include "..\GlobalLog.h"

class CWGRFingerStateMachine :
    public IWGRStateMachine
{
public:
    CWGRFingerStateMachine(void);
    ~CWGRFingerStateMachine(void);

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
            //关闭日志
            g_oWGRTraceLog.StopLog();

            g_oLogHasStarted = false;
            g_oCurCollects = 0;
        }
    }

private:
    CFTwoTouchStateMachine              m_smTwoTouch;
    CFThree2FiveTouchStateMachine       m_smThree2FiveTouch;
    CFSixMoreTouchStateMachine          m_smSixMoreTouch;

    //
    WGR_STATE_TYPE                      m_curState;

    //log relation
    //bool                                m_bLogHasStarted     ;//本次日志已开启
    //int                                 m_nCurLogCollects    ;//本次日志开启后，采集的次数
};
