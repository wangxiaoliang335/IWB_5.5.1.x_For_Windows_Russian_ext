#pragma once
#ifdef EASI_TOUCHSCREEN_INTERFACE_EXPORTS
    #define EASI_TOUCHSCREEN_API _declspec(dllexport)
#else
    #define EASI_TOUCHSCREEN_API _declspec(dllimport)
#endif

#define TIP_UP   0x02   //提笔
#define TIP_DOWN 0x03   //下笔

//#define EASI_TOUCH_MAXIMUM_X 0x7FFF
//#define EASI_TOUCH_MAXIMUM_Y 0x7FFF
#define EASI_TOUCH_MAXIMUM_X (1920*8)
#define EASI_TOUCH_MAXIMUM_Y (1080*8)

//最大触控点个数
#define MAX_CONTACT_COUNT 30

//每个Report中包含的最大触控点个数
#define MAX_CONTACT_COUNT_PER_REPORT 10

struct EASI_TouchPoint
{
    UCHAR  bStatus  ;//bit0: Tip Switch; bit1:In Range; bit2...7: Padding bits
    UCHAR  ContactId;//contact indentifiers
    USHORT wXData   ;//水平坐标,取值范围:0~0x7FFF(EASI_TOUCH_MAXIMUM_X)
    USHORT wYData   ;//垂直坐标,取值范围:0~0x7FFF(EASI_TOUCH_MAXIMUM_Y
};

#define ALL_BTN_UP      0x00
#define LEFT_BTN_DOWN   0x01
#define RIGHT_BTN_DOWN  0x02
#define MIDDLE_BTN_DOWN 0x04
#define BACK_BTN_DOWN   0x08
#define FOWARD_BTN_DWON 0x40

#define EASI_MOUSE_MAXIMUM_X  0x7FFF
#define EASI_MOUSE_MAXIMUM_Y  0x7FFF


struct EASI_MouseInput
{
    //Buttions bit map:
    //D0: Left Button
    //D1: Right Button
    //D2: Middle Button
    //D3: Back Button
    //D4: Forward Button;
    //D5..7: Padding bits
    UCHAR Buttons;
    USHORT X     ;//水平坐标,取值范围:0~0x7FFF(EASI_MOUSE_MAXIMUM_X)
    USHORT Y     ;//垂直坐标,取值范围:0~0x7FFF(EASI_MOUSE_MAXIMUM_X)
    UCHAR Wheel  ;//垂直滚动,取值范围:(-127)0x81~0x7F(127)
    UCHAR ACPan  ;//水平滚动,取值范围:(-127)0x81~0x7F(127) Application Control Pan, Horizontal Scrolling
};


enum ETouchScreenMode
{
    EASI_MOUSE = 0,
    EASI_SINGLE_OUCH = 1,
    EASI_MULTI_TOUCH = 2

};

//@功能:返回虚拟驱动的设备接口GUID号。
extern "C" EASI_TOUCHSCREEN_API const GUID& EASI_GetDeviceInterfaceGUID();

//@功能:打开设备,返回设备句柄
extern "C" EASI_TOUCHSCREEN_API HANDLE  EASI_OpenDevice();

//取消不用
//extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_SelectDeviceMode(ETouchScreenMode eMode);

//@功能:写入触屏数据到虚拟驱动
//@参数:hDevice, 调用EASI_OpenDevice返回的设备句柄
//      pTouchPoint, 指向EASI_TouchPoint数组的指针
//      nPointCount, 触控点的数目
extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_WriteVirtualTouchScreen(HANDLE hDevice, const EASI_TouchPoint *pTouchPoint, int nPointCount);

//@功能:写入鼠标数据到虚拟驱动中去
//@参数:hDevice, 调用函数EASI_OpenDevice返回的设备句柄
//      pMouseInput， 指向鼠标输入数据的指针
extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_WriteVirtualMouse(HANDLE hDevice, const EASI_MouseInput *pMouseInput);

//@功能:关闭设备句柄
//@参数:hDevice, 调用函数EASI_OpenDevice返回的设备句柄
extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_CloseDevice(HANDLE hDevice);


extern "C" EASI_TOUCHSCREEN_API const GUID  GUID_DEVINTERFACE_EASI_UMDF_TouchScreen;
