#pragma once
#include "..\inc\EASI_TOUCHSCREEN_INTERFACE.h"

//
enum EPenState
{
    E_PEN_STATE_UP   = 0,
    E_PEN_STATE_DOWN = 1,
};

struct PenInfo
{
    int  nId;//从0开始的触控点编号
    POINT pt;//触控点的屏幕坐标
    
    EPenState ePenState;//笔的状态。
};

class CVirtualTouchPad
{
public:

    CVirtualTouchPad();

    ~CVirtualTouchPad();

    BOOL OpenDevice();

    BOOL CloseDevice();

    BOOL InputTouchPoints(const PenInfo* penInfos, int nPenCount);

protected:
    HANDLE m_hDev;


    static const int MAX_TOUCH_POINT_COUNT  = 10;
    EASI_TouchPoint m_TouchPoints[MAX_TOUCH_POINT_COUNT];
};