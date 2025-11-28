#include "stdafx.h"
//#include "headers.h"

CVirtualMouse::CVirtualMouse()
:
m_hDriverDevice(INVALID_HANDLE_VALUE),
m_oDoubleClickChecker(this),
m_oRightClickTrigger(this),
m_bForceUseWinAPI(FALSE)
{
    Reset();
}

//@功能:设置驱动设备句柄
//@参数:hDriverDevice， 驱动设备句柄
//@说明:当设备句柄为INVALID_HANDLE_VALUE时，使用Window API模拟鼠标消息。
void CVirtualMouse::SetDeviceHandle(HANDLE hDriverDevice)
{
    m_hDriverDevice = hDriverDevice;
}


//功能:给虚拟鼠标输入光笔数据
//参数:bPenOn, 光笔亮灭标志
//     pPos, 光笔位置,屏幕坐标
//     hDriverDevice, 驱动设备句柄
void CVirtualMouse::Input(BOOL bPenOn, const POINT* pPos, BOOL bForceUseWinAPI)
{
    POINT ptPenPos;
    ptPenPos.x = ptPenPos.y =0;
    if(pPos)
    {
        ptPenPos.x = pPos->x /*+ g_tSysCfgData.globalSettings.nXCoordOffset * 2*/;
        ptPenPos.y = pPos->y /*+ g_tSysCfgData.globalSettings.nYCoordOffset * 2*/;
    }

	m_bForceUseWinAPI = bForceUseWinAPI;
    //状态机描述

    char szBUf[128] = { 0 };
    sprintf(szBUf, " Input m_eVirutalMouseState:%d, bPenOn:%d\n", m_eVirutalMouseState, bPenOn);
    OutputDebugStringA(szBUf);
    switch(m_eVirutalMouseState)
    {
    case VIRTUAL_MOUSE_STATE_IDLE:

        if(bPenOn)
        {
            GenEvent(ptPenPos,E_MOUSE_MOVE);

            m_ptLastPenAbsolutePos  = ptPenPos;

            GenEvent(ptPenPos,E_LBUTTON_DOWN);

            //双击事件检测
            m_oDoubleClickChecker.Check(ptPenPos, CDoubleClickChecker::E_MOUSE_STATE_LEFT_DOWN);
            m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_LEFT_DOWN;

            //复位右击检测器,重新开始右击检测
            m_oRightClickTrigger.Reset();
        }

        break;

    case VIRTUAL_MOUSE_STATE_LEFT_DOWN:
        if(bPenOn)
        {
            if(m_oRightClickTrigger.Process(ptPenPos))
            {
                //m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_RIGHT_CONTROL;
                m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_MOVE;
                
            }
            else
            {
                GenEvent(ptPenPos,E_LBUTTON_DOWN_MOVE);//

                m_oDoubleClickChecker.Check(ptPenPos, CDoubleClickChecker::E_MOUSE_STATE_LEFT_DOWN_MOVE);

                m_ptLastPenAbsolutePos = ptPenPos;
            }
        }
        else
        {
            GenEvent(ptPenPos,E_LBUTTON_UP);//
            m_oDoubleClickChecker.Check(ptPenPos, CDoubleClickChecker::E_MOUSE_STATE_LEFT_UP);
            m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
        }

        break;

    //case VIRTUAL_MOUSE_STATE_RIGHT_CONTROL:
    //    if(bPenOn)
    //    {
    //        if(m_ptLastPenAbsolutePos != ptPenPos)
    //        {
    //            GenEvent(ptPenPos,E_MOUSE_MOVE);
    //            m_ptLastPenAbsolutePos = ptPenPos;
    //        }

    //    }
    //    else
    //    {
    //        //mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
    //        m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
    //    }
    //    break;

    case VIRTUAL_MOUSE_STATE_MOVE://模拟鼠标的移动功能
        if (bPenOn)
        {
             if(m_ptLastPenAbsolutePos != ptPenPos)
            {
                GenEvent(ptPenPos,E_MOUSE_MOVE);
                m_ptLastPenAbsolutePos = ptPenPos;
            }
        }
        else
        {
            m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
        }
        break;
    }//switch

}


//功能:给虚拟鼠标输入光笔数据
//参数:bPenOn, 光笔亮灭标志
//     pPos, 光笔位置,屏幕坐标
//     hDriverDevice, 驱动设备句柄
void CVirtualMouse::Input_AirOperate(BOOL bPenOn, const POINT* pPos, EAIROPERATE_CLICKMODE eClickMode, BOOL bForceUseWinAPI)
{
	POINT ptPenPos;
	ptPenPos.x = ptPenPos.y = 0;
	if (pPos)
	{
		ptPenPos.x = pPos->x /*+ g_tSysCfgData.globalSettings.nXCoordOffset * 2*/;
		ptPenPos.y = pPos->y /*+ g_tSysCfgData.globalSettings.nYCoordOffset * 2*/;
	}

	m_bForceUseWinAPI = bForceUseWinAPI;
	//状态机描述

    char szBUf[128] = { 0 };
    sprintf(szBUf, " Input_AirOperate m_eVirutalMouseState:%d, bPenOn:%d\n", m_eVirutalMouseState, bPenOn);
    OutputDebugStringA(szBUf);

	switch (m_eVirutalMouseState)
	{
	case VIRTUAL_MOUSE_STATE_IDLE:

		if (bPenOn)
		{
			GenEvent(ptPenPos, E_MOUSE_MOVE);
			m_ptLastPenAbsolutePos = ptPenPos;
			m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_MOVE;
		}
		break;

	case VIRTUAL_MOUSE_STATE_MOVE://模拟鼠标的移动功能

		if (bPenOn)
		{
			if (m_ptLastPenAbsolutePos != ptPenPos)
			{
				GenEvent(ptPenPos, E_MOUSE_MOVE);
				m_ptLastPenAbsolutePos = ptPenPos;
			}
		}
		else
		{
			switch(eClickMode)
			{
			   case E_MODE_CLICK:

				    GenEvent(ptPenPos, E_LBUTTON_DOWN);
					GenEvent(ptPenPos, E_LBUTTON_UP);
				  break;

			   case E_MODE_DOUBLE_CLICK:

				   GenEvent(ptPenPos, E_LBUTTON_DOWN);
				   GenEvent(ptPenPos, E_LBUTTON_UP);

				   GenEvent(ptPenPos, E_LBUTTON_DOWN);
				   GenEvent(ptPenPos, E_LBUTTON_UP);

				  break;
			   default:
				   break;

			}
			m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
		}
		break;

	default:
		break;
	}//switch

}

//功能:状态复位
void CVirtualMouse::Reset()
{
     if(m_hDriverDevice != INVALID_HANDLE_VALUE)
     {   //使用驱动模拟鼠标事件
         if(m_eVirutalMouseState == VIRTUAL_MOUSE_STATE_LEFT_DOWN)
         {
            int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
            int nCyScreen = GetSystemMetrics(SM_CYSCREEN);

            EASI_MouseInput mouseInput;
            ZeroMemory(&mouseInput, sizeof(mouseInput));

            mouseInput.Buttons = ALL_BTN_UP;
            mouseInput.X = USHORT(EASI_MOUSE_MAXIMUM_X * m_ptLastPenAbsolutePos.x / nCxScreen);
            mouseInput.Y = USHORT(EASI_MOUSE_MAXIMUM_Y * m_ptLastPenAbsolutePos.y / nCyScreen);
            
            EASI_WriteVirtualMouse(
                m_hDriverDevice,
                &mouseInput
                );
         }
     }
     else
     {//使用API模拟鼠标事件
        
         if(m_eVirutalMouseState == VIRTUAL_MOUSE_STATE_LEFT_DOWN)
         {
             //!注意!:使用API模拟鼠标时, 归一化坐标要使用虚拟屏幕坐标
            int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

            INPUT input;
            memset(&input, 0 ,sizeof(input));
            input.type    = INPUT_MOUSE;

            //归一化到0~65535范围内
            input.mi.dx   = m_ptLastPenAbsolutePos.x * 65535/nCxVirtualScreen;
            input.mi.dy   = m_ptLastPenAbsolutePos.y * 65535/nCyVirtualScreen;

            input.mi.dwFlags  = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;


            SendInput(1, &input, sizeof(input));
         }
     }


    //复位状态
    m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;

    m_ptLastPenAbsolutePos.x = 0;
    m_ptLastPenAbsolutePos.y = 0;
}

//@功能:产生指定的鼠标事件。
//@参数:  pos, 鼠标位置
//     eEvent, 鼠标事件枚举
//     nMouseData,  E_VERT_WHEEL, 为鼠标滚轮移动量, 是WHEEL_DELTA的整数倍。正数表示向前滚动，负数表示向后滚动。
//                  E_HORZ_WHEEL, 为鼠标滚轮移动量, 是WHEEL_DELTA的整数倍。正数表示右滚动，负数表示向左滚动。
//
void CVirtualMouse::GenEvent(const POINT& ptMouse, EVirtualMouseEvent eEvent, int nMouseData)
{
    char szBUf[128] = { 0 };
    sprintf(szBUf, " GenEvent m_hDriverDevice:%d, m_bForceUseWinAPI:%d\n", m_hDriverDevice, m_bForceUseWinAPI);
    OutputDebugStringA(szBUf);

    if(m_hDriverDevice != INVALID_HANDLE_VALUE && !m_bForceUseWinAPI)
    {
        int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
        int nCyScreen = GetSystemMetrics(SM_CYSCREEN);

        int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int nVSLeft          = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int nVSTop           = GetSystemMetrics(SM_YVIRTUALSCREEN);

        //使用驱动模拟鼠标事件
        BOOL bSuccess = FALSE;
        EASI_MouseInput mouseInput;
        ZeroMemory(&mouseInput, sizeof(mouseInput));

        //mouseInput.X = (USHORT)(EASI_MOUSE_MAXIMUM_X * ptMouse.x / nCxScreen);
        //mouseInput.Y = (USHORT)(EASI_MOUSE_MAXIMUM_Y * ptMouse.y / nCyScreen);

        //win10 1709 (OS内部版本 16299.125)
        if(IsWin10OrGreater())
        {//win10下的鼠标输入坐标的归一化
            mouseInput.X = (USHORT)(EASI_MOUSE_MAXIMUM_X * (ptMouse.x - nVSLeft) / nCxVirtualScreen);
            mouseInput.Y = (USHORT)(EASI_MOUSE_MAXIMUM_Y * (ptMouse.y - nVSTop) / nCyVirtualScreen);
        }
        else 
        {//win7下的鼠标输入坐标的归一化
            mouseInput.X = (USHORT)(EASI_MOUSE_MAXIMUM_X * (ptMouse.x - 0) / nCxScreen);
            mouseInput.Y = (USHORT)(EASI_MOUSE_MAXIMUM_Y * (ptMouse.y - 0) / nCyScreen);
        }

        char szBUf[128] = { 0 };
        sprintf(szBUf, " GenEvent mouseInput.X:%d, mouseInput.Y:%d\n", mouseInput.X, mouseInput.Y);
        OutputDebugStringA(szBUf);

        //mouseInput.X += g_tSysCfgData.globalSettings.nXCoordOffset * 5;
        //mouseInput.Y += g_tSysCfgData.globalSettings.nYCoordOffset * 5;

        switch(eEvent)
        {

        case E_LBUTTON_DOWN:
            mouseInput.Buttons = LEFT_BTN_DOWN;
            //AtlTrace(_T("E_LBUTTON_DOWN \n"));
            break;

        case E_LBUTTON_UP:
            mouseInput.Buttons = ALL_BTN_UP;
            //AtlTrace(_T("E_LBUTTON_UP \n"));
            break;

        case E_RBUTTON_DOWN:
            mouseInput.Buttons = RIGHT_BTN_DOWN;
            AtlTrace(_T("E_RBUTTON_DOWN \n"));
            break;

        case E_RBUTTON_UP:
            mouseInput.Buttons = ALL_BTN_UP;
            AtlTrace(_T("E_RBUTTON_UP \n"));
            break;


        case E_MOUSE_MOVE:
            mouseInput.Buttons = ALL_BTN_UP;
            AtlTrace(_T("E_MOUSE_MOVE \n"));
            break;

        case E_LBUTTON_DOWN_MOVE:
            mouseInput.Buttons = LEFT_BTN_DOWN;
            //AtlTrace(_T("E_LBUTTON_DOWN_MOVE \n"));
            break;

        case E_RBUTTON_DOWN_MOVE:
            mouseInput.Buttons = RIGHT_BTN_DOWN;
            AtlTrace(_T("E_RBUTTON_DOWN_MOVE \n"));
            break;

        case E_VERT_WHEEL://垂直滚动
            mouseInput.Wheel = (UCHAR)(nMouseData/WHEEL_DELTA);
            AtlTrace(_T("E_VERT_WHEEL \n"));
            break;

        case E_HORZ_WHEEL://水平滚动
            mouseInput.ACPan = (UCHAR)(nMouseData/WHEEL_DELTA);
            AtlTrace(_T("E_HORZ_WHEEL \n"));
            break;
        default:
            AtlTrace(_T("Unkown Mouse Event Type.\n"));

        }//switch

        bSuccess =
            EASI_WriteVirtualMouse(
            m_hDriverDevice,
            &mouseInput
            );

    }
    else
    {   //使用Windows API模拟鼠标
        //!注意!:使用API模拟鼠标时, 归一化坐标要使用虚拟屏幕坐标
        int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int nVSLeft          = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int nVSTop           = GetSystemMetrics(SM_YVIRTUALSCREEN);

        INPUT input;
        memset(&input, 0 ,sizeof(input));
        input.type    = INPUT_MOUSE;

        //归一化到0~65535范围内
        input.mi.dx   = (ptMouse.x - nVSLeft ) * 65535/nCxVirtualScreen;
        input.mi.dy   = (ptMouse.y - nVSTop) * 65535/nCyVirtualScreen;

        //input.mi.dx += g_tSysCfgData.globalSettings.nXCoordOffset * 10;
        //input.mi.dy += g_tSysCfgData.globalSettings.nYCoordOffset * 10;

        char szBUf[128] = { 0 };
        sprintf(szBUf, " GenEvent input.X:%d, input.Y:%d\n", input.mi.dx, input.mi.dy);
        OutputDebugStringA(szBUf);

        switch(eEvent)
        {

        case E_LBUTTON_DOWN:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;

            break;

        case E_LBUTTON_UP:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;

            break;

        case E_RBUTTON_DOWN:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;

            break;

        case E_RBUTTON_UP:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
            break;


        case E_MOUSE_MOVE:
        case E_LBUTTON_DOWN_MOVE:
        case E_RBUTTON_DOWN_MOVE:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
            break;


        case E_VERT_WHEEL://垂直滚轮
            input.mi.mouseData = (DWORD)nMouseData;
            input.mi.dwFlags   = MOUSEEVENTF_WHEEL;

            break;

        case E_HORZ_WHEEL://水平滚轮
            input.mi.mouseData = (DWORD)nMouseData;
            input.mi.dwFlags   = MOUSEEVENTF_HWHEEL;
            break;

        }//switch

        SendInput(1, &input, sizeof(input));
    }
    
}    

