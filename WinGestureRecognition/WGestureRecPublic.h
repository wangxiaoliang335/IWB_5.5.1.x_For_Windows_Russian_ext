#pragma once

//**************************************************************
//下面的xxx__STATE_BEGIN和xxx__STATE_END只是用来分界的，程序中可能会用
//它们来确定curState所在的手势功能区
//<手势状态定义
#define WGR_BEGIN                                   0
#define WGR_IDLE_STATE                              0  //空闲状态

//*手指状态开始
#define WGR_FINGER_STATE_BEGIN                      WGR_BEGIN + 1
#define WGR_FINGER_STATE                            WGR_FINGER_STATE_BEGIN + 1
#define WGR_FINGER_CLOSE_STATE                      WGR_FINGER_STATE_BEGIN + 2

//**两触点状态开始
#define WGR_FTWOTOUCH_STATE_BEGIN                     WGR_FINGER_STATE_BEGIN + 10
#define WGR_FTWOTOUCH_LISTEN_STATE                    WGR_FTWOTOUCH_STATE_BEGIN + 1
#define WGR_FTWOTOUCH_CLOSE_STATE                     WGR_FTWOTOUCH_STATE_BEGIN + 2

    //***竖直鼠标滑轮状态开始
    #define WGR_FVMOUSEWHEEL_STATE_BEGIN                WGR_FTWOTOUCH_STATE_BEGIN + 10
    #define WGR_FVMOUSEWHEEL_LISTEN_STATE               WGR_FVMOUSEWHEEL_STATE_BEGIN + 1
    #define WGR_FVMOUSEWHEEL_KEEP_STATE                 WGR_FVMOUSEWHEEL_STATE_BEGIN + 2
    #define WGR_FVMOUSEWHEEL_EXE_STATE                  WGR_FVMOUSEWHEEL_STATE_BEGIN + 3
    #define WGR_FVMOUSEWHEEL_CLOSE_STATE                WGR_FVMOUSEWHEEL_STATE_BEGIN + 4
    #define WGR_FVMOUSEWHEEL_STATE_END                  WGR_FTWOTOUCH_STATE_BEGIN + 20
    //***竖直鼠标滑轮状态结束

    //***水平鼠标滑轮状态开始
    #define WGR_FHMOUSEWHEEL_STATE_BEGIN                WGR_FTWOTOUCH_STATE_BEGIN + 20
    #define WGR_FHMOUSEWHEEL_LISTEN_STATE               WGR_FHMOUSEWHEEL_STATE_BEGIN + 1
    #define WGR_FHMOUSEWHEEL_KEEP_STATE                 WGR_FHMOUSEWHEEL_STATE_BEGIN + 2
    #define WGR_FHMOUSEWHEEL_EXE_STATE                  WGR_FHMOUSEWHEEL_STATE_BEGIN + 3
    #define WGR_FHMOUSEWHEEL_CLOSE_STATE                WGR_FHMOUSEWHEEL_STATE_BEGIN + 4
    #define WGR_FHMOUSEWHEEL_STATE_END                  WGR_FTWOTOUCH_STATE_BEGIN + 30
    //***水平鼠标滑轮状态结束

#define WGR_FTWOTOUCH_STATE_END                       WGR_FINGER_STATE_BEGIN + 110
//**两触点状态结束

//** 3 ~ 5触点开始
#define WGR_FTHREE2FIVETOUCH_STATE_BEGIN              WGR_FINGER_STATE_BEGIN + 110
#define WGR_FTHREE2FIVETOUCH_LISTEN_STATE             WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 1
#define WGR_FTHREE2FIVETOUCH_CLOSE_STATE              WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 2

    //***最小化当前窗口开始
    #define WGR_FMINICURWND_STATE_BEGIN                      WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 10
    #define WGR_FMINICURWND_LISTEN_STATE                     WGR_FMINICURWND_STATE_BEGIN + 1
    #define WGR_FMINICURWND_CLOSE_STATE                      WGR_FMINICURWND_STATE_BEGIN + 3
    #define WGR_FMINICURWND_EXE_STATE                        WGR_FMINICURWND_STATE_BEGIN + 2
    #define WGR_FMINICURWND_KEEP_STATE                       WGR_FMINICURWND_STATE_BEGIN + 4
    #define WGR_FMINICURWND_STATE_END                        WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 20
    //***最小化当前窗口结束

    //***ALT+ARROW开始
    #define WGR_FALTARROW_STATE_BEGIN                     WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 20
    #define WGR_FALTARROW_LISTEN_STATE                    WGR_FALTARROW_STATE_BEGIN + 1
    #define WGR_FALTARROW_CLOSE_STATE                     WGR_FALTARROW_STATE_BEGIN + 2

        //****ALT+LARROW开始
        #define WGR_FALTLARROW_STATE_BEGIN                    WGR_FALTARROW_STATE_BEGIN + 10
        #define WGR_FALTLARROW_LISTEN_STATE                   WGR_FALTLARROW_STATE_BEGIN + 1
        #define WGR_FALTLARROW_CLOSE_STATE                    WGR_FALTLARROW_STATE_BEGIN + 2
        #define WGR_FALTLARROW_KEEP_STATE                     WGR_FALTLARROW_STATE_BEGIN + 3
        #define WGR_FALTLARROW_EXE_STATE                      WGR_FALTLARROW_STATE_BEGIN + 4
        #define WGR_FALTLARROW_STATE_END                      WGR_FALTARROW_STATE_BEGIN + 20
        //****ALT+LARROW结束

        //****ALT+RARROW开始
        #define WGR_FALTRARROW_STATE_BEGIN                    WGR_FALTARROW_STATE_BEGIN + 20
        #define WGR_FALTRARROW_LISTEN_STATE                   WGR_FALTRARROW_STATE_BEGIN + 1
        #define WGR_FALTRARROW_CLOSE_STATE                    WGR_FALTRARROW_STATE_BEGIN + 2
        #define WGR_FALTRARROW_KEEP_STATE                     WGR_FALTRARROW_STATE_BEGIN + 3
        #define WGR_FALTRARROW_EXE_STATE                      WGR_FALTRARROW_STATE_BEGIN + 4
        #define WGR_FALTRARROW_STATE_END                      WGR_FALTARROW_STATE_BEGIN + 30
        //****ALT+RARROW结束
        

    #define WGR_FALTARROW_STATE_END                       WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 55
    //***ALT+ARROW结束

    //***窗口切换开始
    #define WGR_FSWHWND_STATE_BEGIN                        WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 60
    #define WGR_FSWHWND_LISTEN_STATE                       WGR_FSWHWND_STATE_BEGIN + 1
    #define WGR_FSWHWND_CLOSE_STATE                        WGR_FSWHWND_STATE_BEGIN + 2
    #define WGR_FSWHWND_START_STATE                        WGR_FSWHWND_STATE_BEGIN + 3
    #define WGR_FSWHWND_KEEP_STATE                         WGR_FSWHWND_STATE_BEGIN + 4
    #define WGR_FSWHWND_EXE_STATE                          WGR_FSWHWND_STATE_BEGIN + 5
    #define WGR_FSWHWND_STATE_END                          WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 70
    //***窗口切换结束

    //***复杂动作开始
    #define WGR_FCOMPLEXMOTION_STATE_BEGIN                WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 255
    #define WGR_FCOMPLEXMOTION_LISTEN_STATE               WGR_FCOMPLEXMOTION_STATE_BEGIN + 1
    #define WGR_FCOMPLEXMOTION_CLOSE_STATE                WGR_FCOMPLEXMOTION_STATE_BEGIN + 2

        //****最大化当前窗口开始
        #define WGR_FMAXICURWND_STATE_BEGIN                   WGR_FCOMPLEXMOTION_STATE_BEGIN + 10
        #define WGR_FMAXICURWND_LISTEN_STATE                  WGR_FMAXICURWND_STATE_BEGIN + 1
        #define WGR_FMAXICURWND_CLOSE_STATE                   WGR_FMAXICURWND_STATE_BEGIN + 2
        #define WGR_FMAXICURWND_KEEP_STATE                    WGR_FMAXICURWND_STATE_BEGIN + 3
        #define WGR_FMAXICURWND_EXE_STATE                     WGR_FMAXICURWND_STATE_BEGIN + 4
        #define WGR_FMAXICURWND_STATE_END                     WGR_FCOMPLEXMOTION_STATE_BEGIN + 20
        //****最大化当前窗口结束

        //****显示桌面开始
        #define WGR_FDSPDESKTOP_STATE_BEGIN                   WGR_FCOMPLEXMOTION_STATE_BEGIN + 20
        #define WGR_FDSPDESKTOP_LISTEN_STATE                  WGR_FDSPDESKTOP_STATE_BEGIN + 1
        #define WGR_FDSPDESKTOP_CLOSE_STATE                   WGR_FDSPDESKTOP_STATE_BEGIN + 2
        #define WGR_FDSPDESKTOP_KEEP_STATE                    WGR_FDSPDESKTOP_STATE_BEGIN + 3
        #define WGR_FDSPDESKTOP_EXE_STATE                     WGR_FDSPDESKTOP_STATE_BEGIN + 4
        #define WGR_FDSPDESKTOP_STATE_END                     WGR_FCOMPLEXMOTION_STATE_BEGIN + 60
        //****显示桌面结束

    #define WGR_FCOMPLEXMOTION_STATE_END                  WGR_FTHREE2FIVETOUCH_STATE_BEGIN + 550
    //***复杂动作结束

#define WGR_FTHREE2FIVETOUCH_STATE_END                 WGR_FINGER_STATE_BEGIN + 710
//** 3 ~ 5触点结束

//**6点以上触点开始
#define WGR_FSIXMORETOUCH_STATE_BEGIN                  WGR_FINGER_STATE_BEGIN + 710
#define WGR_FSIXMORETOUCH_LISTEN_STATE                 WGR_FSIXMORETOUCH_STATE_BEGIN + 1 
#define WGR_FSIXMORETOUCH_CLOSE_STATE                  WGR_FSIXMORETOUCH_STATE_BEGIN + 2

    //***最小化所有窗口开始
    #define WGR_FMINIALLWND_STATE_BEGIN                    WGR_FSIXMORETOUCH_STATE_BEGIN + 10
    #define WGR_FMINIALLWND_LISTEN_STATE                   WGR_FMINIALLWND_STATE_BEGIN + 1
    #define WGR_FMINIALLWND_CLOSE_STATE                    WGR_FMINIALLWND_STATE_BEGIN + 2
    #define WGR_FMINIALLWND_KEEP_STATE                     WGR_FMINIALLWND_STATE_BEGIN + 3
    #define WGR_FMINIALLWND_EXE_STATE                      WGR_FMINIALLWND_STATE_BEGIN + 4
    #define WGR_FMINIALLWND_STATE_END                      WGR_FSIXMORETOUCH_STATE_BEGIN + 20
    //***最小化所有窗口结束

#define WGR_FSIXMORETOUCH_STATE_END                     WGR_FINGER_STATE_BEGIN + 910
//**6点以上触点结束

#define WGR_FINGER_STATE_END                           WGR_BEGIN + 1000
//*手指状态结束

//*手掌状态开始
#define WGR_PALM_STATE_BEGIN                           WGR_BEGIN + 1000
#define WGR_PALM_STATE                                 WGR_PALM_STATE_BEGIN + 1
#define WGR_PALM_CLOSE_STATE                           WGR_PALM_STATE_BEGIN + 2

//**特殊位置开始
#define WGR_PSPECLOC_STATE_BEGIN                       WGR_PALM_STATE_BEGIN + 10
#define WGR_PSPECLOC_LISTEN_STATE                      WGR_PSPECLOC_STATE_BEGIN + 1
#define WGR_PSPECLOC_CLOSE_STATE                       WGR_PSPECLOC_STATE_BEGIN + 2

    //***下边缘位置开始
    #define WGR_PLOWLIM_STATE_BEGIN                        WGR_PSPECLOC_STATE_BEGIN + 10
    #define WGR_PLOWLIM_LISTEN_STATE                       WGR_PLOWLIM_STATE_BEGIN + 1
    #define WGR_PLOWLIM_CLOSE_STATE                        WGR_PLOWLIM_STATE_BEGIN + 2

        //****并列显示窗口开始
        #define WGR_SHOWWNDSBS_STATE_BEGIN                     WGR_PLOWLIM_STATE_BEGIN + 6
        #define WGR_SHOWWNDSBS_LISTEN_STATE                    WGR_SHOWWNDSBS_STATE_BEGIN + 1
        #define WGR_SHOWWNDSBS_CLOSE_STATE                     WGR_SHOWWNDSBS_STATE_BEGIN + 2
        #define WGR_SHOWWNDSBS_KEEP_STATE                      WGR_SHOWWNDSBS_STATE_BEGIN + 3
        #define WGR_SHOWWNDSBS_EXE_STATE                       WGR_SHOWWNDSBS_STATE_BEGIN + 4
        #define WGR_SHOWWNDSBS_STATE_END                       WGR_PLOWLIM_STATE_BEGIN + 15
        //****并列显示窗口结束

    #define WGR_PLOWLIM_STATE_END                          WGR_PSPECLOC_STATE_BEGIN + 100
    //***下边缘位置结束

#define WGR_PSPECLOC_STATE_END                         WGR_PALM_STATE_BEGIN + 210
//**特殊位置结束

//**非特殊位置开始
#define WGR_PNONSPECLOC_STATE_BEGIN                    WGR_PALM_STATE_BEGIN + 210
#define WGR_PNONSPECLOC_LISTEN_STATE                   WGR_PNONSPECLOC_STATE_BEGIN + 1 
#define WGR_PNONSPECLOC_END_STATE                      WGR_PNONSPECLOC_STATE_BEGIN + 2

//***窗口刷新开始
#define WGR_PREFWND_STATE_BEGIN                        WGR_PNONSPECLOC_STATE_BEGIN + 5
#define WGR_PREFWND_LISTEN_STATE                       WGR_PREFWND_STATE_BEGIN + 1
#define WGR_PREFWND_CLOSE_STATE                        WGR_PREFWND_STATE_BEGIN + 2
#define WGR_PREFWND_SECMOVE_STATE                      WGR_PREFWND_STATE_BEGIN + 3
#define WGR_PREFWND_THDMOVE_STATE                      WGR_PREFWND_STATE_BEGIN + 4
#define WGR_PREFWND_KEEP_STATE                         WGR_PREFWND_STATE_BEGIN + 5
#define WGR_PREFWND_EXE_STATE                          WGR_PREFWND_STATE_BEGIN + 6
#define WGR_PREFWND_STATE_END                          WGR_PNONSPECLOC_STATE_BEGIN + 15
//***窗口刷新结束

//***关闭当前窗口开始
#define WGR_PCLSCWND_STATE_BEGIN                       WGR_PNONSPECLOC_STATE_BEGIN + 15
#define WGR_PCLSCWND_LISTEN_STATE                      WGR_PCLSCWND_STATE_BEGIN + 1
#define WGR_PCLSCWND_CLOSE_STATE                       WGR_PCLSCWND_STATE_BEGIN + 2
#define WGR_PCLSCWND_EXE_STATE                         WGR_PCLSCWND_STATE_BEGIN + 3
#define WGR_PCLSCWND_STATE_END                         WGR_PNONSPECLOC_STATE_BEGIN + 25
//***关闭当前窗口结束

//***移动窗口开始
#define WGR_PMOVWND_STATE_BEGIN                        WGR_PNONSPECLOC_STATE_BEGIN + 25
#define WGR_PMOVWND_LISTEN_STATE                       WGR_PMOVWND_STATE_BEGIN + 1
#define WGR_PMOVWND_CLOSE_STATE                        WGR_PMOVWND_STATE_BEGIN + 2
#define WGR_PMOVWND_START_STATE                        WGR_PMOVWND_STATE_BEGIN + 3
#define WGR_PMOVWND_KEEP_STATE                         WGR_PMOVWND_STATE_BEGIN + 4
#define WGR_PMOVWND_EXE_STATE                          WGR_PMOVWND_STATE_BEGIN + 5
#define WGR_PMOVWND_STATE_END                          WGR_PNONSPECLOC_STATE_BEGIN + 35
//***移动窗口结束

//***文档保存开始
#define WGR_PSVDOC_STATE_BEGIN                         WGR_PNONSPECLOC_STATE_BEGIN + 35
#define WGR_PSVDOC_LISTEN_STATE                        WGR_PSVDOC_STATE_BEGIN + 1
#define WGR_PSVDOC_CLOSE_STATE                         WGR_PSVDOC_STATE_BEGIN + 2
#define WGR_PSVDOC_EXE_STATE                           WGR_PSVDOC_STATE_BEGIN + 3
#define WGR_PSVDOC_STATE_END                           WGR_PNONSPECLOC_STATE_BEGIN + 45
//***文档保存结束

#define WGR_PNONSPECLOC_STATE_END                      WGR_PALM_STATE_BEGIN + 990
//**非特殊位置结束

#define WGR_PALM_STATE_END                             WGR_BEGIN + 2000
//*手掌状态结束

#define WGR_END                                        WGR_BEGIN + 4000
//手势状态定义>

//////////////////////////////////////////////////////////////////////////
#define KMAXTOUCHNUM        10
//////////////////////////////////////////////////////////////////////////

//**************************************************************

typedef unsigned int WGR_STATE_TYPE;

//**************************************************************
//定义手势形状
typedef enum
{
    E_WGR_GShape_Unknown,
    E_WGR_GShape_Finger,
    E_WGR_GShape_Palm
}EWGR_GESTURE_SHAPE;
//**************************************************************

//**************************************************************
//定义移动方向
typedef enum
{
    E_WGR_MDIR_Unknown      ,//未确定的运动
    E_WGR_MDIR_None         ,//未移动
    E_WGR_MDIR_Move         ,//移动了
    E_WGR_MDIR_Horiz        ,//水平移动
    E_WGR_MDIR_LHoriz       ,//水平向左
    E_WGR_MDIR_RHoriz       ,//水平向右
    E_WGR_MDIR_Vertic       ,//垂直移动
    E_WGR_MDIR_UVertic      ,//垂直向上
    E_WGR_MDIR_DVertic      ,//垂直向下
    E_WGR_MDIR_Complex      ,//复杂移动
    E_WGR_MDIR_Grab         ,//抓取动作
    E_WGR_MDIR_Open         ,//张开动作
}E_WGR_MOVEDIR;

//**************************************************************

//**************************************************************
//运算宏定义

//<32位整数绝对值宏
#ifndef WGR_INT32_ABS
#define WGR_INT32_ABS(n) (((n)^((n)>>31)) -((n)>>31))
#endif
//32位整数绝对值宏>

//<32位整数异号判断
#ifndef WGR_INT32_OPPSIGN
#define WGR_INT32_OPPSIGN(a, b) (((a) & 0x80000000) ^ ((b) & 0x80000000))
#endif
//32位整数异号判断>

//**************************************************************

struct ST_MOTIONINFO
{
    POINT       ptPos;  //坐标  
    int         nXDisplacement; //位移
    int         nYDisplacement; //位移
};

struct ST_TouchInfo
{
    UINT            uId;    //触点Id
    ST_MOTIONINFO   stMotionInfo; //移动信息
};

//////////////////////////////////////////////////////////////////////////
