#pragma once
#include "..\..\wgrstatemachineinterface.h"
#include "PRefreshWndStateMachine.h"
#include "PMoveWndStateMachine.h"
#include "PCloseCurWndStateMachine.h"

class CPNonSpecLocStateMachine :
    public IWGRStateMachine
{
public:
    CPNonSpecLocStateMachine(void);
    ~CPNonSpecLocStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);

private:
    CPRefreshWndStateMachine    m_smRefreshWnd;
    CPMoveWndStateMachine       m_smMOVWnd;
    CPCloseCurWndStateMachine   m_smCloseCurWnd;

    WGR_STATE_TYPE              m_curState;

    int                         m_nCurConMoves;  //进入Refresh window的条件时，必须连续移动两次

	DWORD						m_dwNonMoveStartTime;   
};
