#pragma once
#include "..\..\wgrstatemachineinterface.h"
#include "FMiniCurWndStateMachine.h"
#include "FComplexMotionStateMachine.h"
#include "FAltArrowStateMachine.h"
#include "FSwitchWndStateMachine.h"

class CFThree2FiveTouchStateMachine :
    public IWGRStateMachine
{
public:
    CFThree2FiveTouchStateMachine(void);
    ~CFThree2FiveTouchStateMachine(void);

public:
    virtual bool StartRun();
    virtual bool StopRun();
    virtual IWGRStateMachine *GetSubMachineCurStateContained();
    virtual WGR_STATE_TYPE TransitState(const TLightSpot * aryLightSpots,  int nCount,  IWGRUtilContext *pContext, EWGR_GESTURE_SHAPE gestureShape = E_WGR_GShape_Unknown);
	virtual WGR_STATE_TYPE TransitState(IWGRUtilContext *pContext);
    //virtual WGR_STATE_TYPE DoAction(IWGRUtilContext *pContext) ;

private:
    CFMiniCurWndStateMachine        m_smMiniCurWnd;
    //CFComplexMotionStateMachine     m_smComplexMotion;
    //CFDspDesktopStateMachine        m_smDspDesktop;
    CFMaxiCurWndStateMachine        m_smMaxiCurWnd;
    //CFAltArrowStateMachine          m_smAltArrow;
    /*CFAltLArrowStateMachine         m_smAltLArrow;
    CFAltRArrowStateMachine         m_smAltRArrow;*/

    CFSwitchWndStateMachine         m_smSWHWnd;             //以前是手掌手势下的功能，后来为操作友好方便，改成了手指手势下的功能

    WGR_STATE_TYPE                  m_curState;
};
