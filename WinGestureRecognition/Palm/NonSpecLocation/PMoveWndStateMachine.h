#pragma once
#include "..\..\wgrstatemachineinterface.h"
#include "PSaveDocStateMachine.h"

class CPMoveWndStateMachine :
    public IWGRStateMachine
{
public:
    CPMoveWndStateMachine(void);
    ~CPMoveWndStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);

private:
    //CPSaveDocStateMachine    m_smSVDoc;


    WGR_STATE_TYPE  m_curState;

    int             m_nConMoves; //连续移动的次数
	POINT			m_ptLast;
};
