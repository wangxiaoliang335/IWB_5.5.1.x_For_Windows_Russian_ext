#include "stdafx.h"
#include "TouchPadDeviceAPI.h"
#pragma comment(lib, "..\\lib\\EASI_TOUCHSCREEN_INTERFACE.lib")


CVirtualTouchPad::CVirtualTouchPad()
:
m_hDev(INVALID_HANDLE_VALUE)
{


}


CVirtualTouchPad::~CVirtualTouchPad()
{
    if(m_hDev != INVALID_HANDLE_VALUE)
    {
        EASI_CloseDevice(m_hDev);
    }
}


BOOL CVirtualTouchPad::OpenDevice()
{
    m_hDev = EASI_OpenDevice();

    return INVALID_HANDLE_VALUE == m_hDev;
}

BOOL CVirtualTouchPad::CloseDevice()
{
     BOOL bRet = EASI_CloseDevice(m_hDev);

     m_hDev = INVALID_HANDLE_VALUE;

     return bRet;
}


BOOL CVirtualTouchPad::InputTouchPoints(const PenInfo* penInfos, int nPenCount)
{

    if(nPenCount > _countof(m_TouchPoints))
    {
        nPenCount = _countof(m_TouchPoints);
    }

    if(nPenCount == 0) return FALSE;

    if(m_hDev == INVALID_HANDLE_VALUE)
    {
        if(!OpenDevice())
        {
            return FALSE;
        }
    }


    int nCxScreen      = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCyScreen      = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int nScreenLeft    = GetSystemMetrics(SM_XVIRTUALSCREEN );
    int nScreenTop     = GetSystemMetrics(SM_YVIRTUALSCREEN );


    for(int i=0; i < nPenCount; i++)
    {
        m_TouchPoints[i].ContactId = penInfos[i].nId;
        m_TouchPoints[i].bStatus   = penInfos[i].ePenState == E_PEN_STATE_DOWN?TIP_DOWN:TIP_UP;
        m_TouchPoints[i].wXData    = USHORT((penInfos[i].pt.x - nScreenLeft) * EASI_TOUCH_MAXIMUM_X /nCxScreen);
        m_TouchPoints[i].wYData    = USHORT((penInfos[i].pt.y - nScreenTop ) * EASI_TOUCH_MAXIMUM_Y /nCyScreen);
    }

    BOOL bRet = EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[0], nPenCount);


    return bRet;
}