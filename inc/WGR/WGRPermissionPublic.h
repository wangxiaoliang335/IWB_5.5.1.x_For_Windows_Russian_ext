#pragma once

//////////////////////////////////////////////////////////////////////////
//
//***
#define PERMFLAGSET_A_REGION                        0x00000003
#define PERMFLAGSET_A_REGION_0                      0x00000001
#define PERMFLAGSET_A_REGION_1                      0x00000002
//#define PERMFLAGSET_A_REGION_2                      0x00000004
//#define PERMFLAGSET_A_REGION_3                      0x00000008

#define PERMFLAGSET_B_REGION                        0x00000070
#define PERMFLAGSET_B_REGION_0                      0x00000010
#define PERMFLAGSET_B_REGION_1                      0x00000020
#define PERMFLAGSET_B_REGION_2                      0x00000040
//#define PERMFLAGSET_B_REGION_3                      0x00000080

#define PERMFLAGSET_C_REGION                        0x00000100
#define PERMFLAGSET_C_REGION_0                      0x00000100
//#define PERMFLAGSET_C_REGION_1                      0x00000200
//#define PERMFLAGSET_C_REGION_2                      0x00000400
//#define PERMFLAGSET_C_REGION_3                      0x00000800

#define PERMFLAGSET_D_REGION                        0x00001000
#define PERMFLAGSET_D_REGION_0                      0x00001000
//#define PERMFLAGSET_D_REGION_1                      0x00002000
//#define PERMFLAGSET_D_REGION_2                      0x00004000
//#define PERMFLAGSET_D_REGION_3                      0x00008000

#define PERMFLAGSET_E_REGION                        0x00070000
#define PERMFLAGSET_E_REGION_0                      0x00010000
#define PERMFLAGSET_E_REGION_1                      0x00020000
#define PERMFLAGSET_E_REGION_2                      0x00040000
//#define PERMFLAGSET_E_REGION_3                      0x00080000

#define PERMFLAGSET_F_REGION                        0x00F00000
//#define PERMFLAGSET_F_REGION_0                      0x00100000
//#define PERMFLAGSET_F_REGION_1                      0x00200000
//#define PERMFLAGSET_F_REGION_2                      0x00400000
//#define PERMFLAGSET_F_REGION_3                      0x00800000

#define PERMFLAGSET_G_REGION                        0x0G000000

#define PERMFLAGSET_H_REGION                        0xH0000000
//***

/*
@方式：使用一个DWORD存储手势识别许可值
@功能：存储事件是否允许触发的标志位
@使用与分配：
|0000|0000|0000|0000|0000|0000|0000|0000|
| H  | G  | F  | E  | D  |  C |  B | A  |

A区域——对应两触点
A = 0 , 意味着两触点所有的功能全部禁止；
A = 1 , 意味着两触点的水平滚轮功能允许，其他功能禁止；
A = 2 , 意味着两触点的竖直滚轮功能允许，其他功能禁止；
A = 3 , 意味着两触点的水平和竖直滚轮功能允许，其他功能禁止；
A = ...

B区域——对应3 ~ 5触点
B = 0 , 意味着3 ~ 5触点所有功能全部禁止；
B = 1 , 意味着3 ~ 5触点的窗口切换功能开启；
B = 2 , 意味着3 ~ 5触点的最大化当前窗口功能开启；
B = 4 , 意味着3 ~ 5触点的最小化当前窗口功能开启；
B = 8 , 意味着3 ~ 5触点的ALT_RARROW功能开启；

C区域——对应6个以上的触点
C = 0 , 意味着6个以上触点所有功能全部禁止；
C = 1 , 意味着6个以上触点的显示桌面功能开启

D区域——对应手掌下边缘位置
D = 0 , 意味着下边缘位置所有功能全部禁止；
D = 1 , 意味着下边缘位置平铺窗口功能开启；

E区域——对应手掌非特殊位置
E = 0 , 意味着手掌非特殊位置所有功能全部禁止；
E = 1 , 意味着手掌非特殊位置关闭当前窗口功能开启；
E = 2 , 意味着手掌非特殊位置刷新窗口功能开启；
E = 4 , 意味着手掌非特殊位置移动窗口功能开启；
//E = 8 , 意味着手掌非特殊位置保存文档功能开启；
*/

typedef enum
{
    //////////////////////////////////////////////////////////////////////////
    E_TE_ALL,
    E_TE_PART,                      //与E_TE_ALL相对不相斥，用来表明所有手势中部分的许可状态
    //手指形状
    E_TEF_ALL,                      //所有手指形状支持事件
    E_TEF_PART,                     //与E_TEF_ALL相对不相斥

    E_TEF_2TOUCHS_ALL,              //两触点所有事件
    E_TEF_2TOUCHS_PART,
    E_TEF_2TOUCHS_HMW,              //两触点水平鼠标滚轮
    E_TEF_2TOUCHS_VMW,              //两触点垂直鼠标滚轮

    E_TEF_35TOUCHS_ALL,             //3 ~ 5触点所有事件
    E_TEF_35TOUCHS_PART,
    E_TEF_35TOUCHS_SWHWND,            //3 ~ 5触点窗口切换    
    E_TEF_35TOUCHS_MAXICURWND,      //3 ~ 5触点最大化当前窗口
    E_TEF_35TOUCHS_MINICURWND,      //3 ~ 5触点最小化当前窗口
    E_TEF_35TOUCHS_ALTRARROW,       //3 ~ 5触点Alt+RArrow

    E_TEF_6MTOUCHS_ALL,             //6以上触点所有事件
    E_TEF_6MTOUCHS_PART,
    E_TEF_6MTOUCHS_DESPDESKTOP,     //6以上触点显示桌面
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //手掌形状
    E_TEP_ALL,                      //所有手掌形状支持事件
    E_TEP_PART,

    E_TEP_LOWLIM_ALL,               //下边缘位置所有事件
    E_TEP_LOWLIM_PART,
    E_TEP_LOWLIM_SHOWWNDSBS,        //下边缘水平平铺窗口

    E_TEP_NSPLOC_ALL,               //非特殊位置所有事件
    E_TEP_NSPLOC_PART,
    E_TEP_NSPLOC_CLSCURWND,         //关闭当前窗口
    E_TEP_NSPLOC_REFRESHWND,        //刷新窗口
    E_TEP_NSPLOC_MOVWND,            //移动窗口
    E_TEP_NSPLOC_SVDOC,             //保存文档
    //////////////////////////////////////////////////////////////////////////
}E_TRIGGEREVENT;

#ifndef INIT_WITH_ALL_PERMITED
#define INIT_WITH_ALL_PERMITED  (PERMFLAGSET_A_REGION | PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION | PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION)
#endif

#ifndef INIT_WITH_ALL_PROHIBITED
#define INIT_WITH_ALL_PROHIBITED    0
#endif

#ifndef INIT_WITH_FINGER_PERMITED
#define INIT_WITH_FINGER_PERMITED  (PERMFLAGSET_A_REGION | PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION)
#endif

#ifndef INIT_WITH_PALM_PERMITED
#define INIT_WITH_PALM_PERMITED  (PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION)
#endif

#ifndef ALLPERMITED_EXCEPT_2TOUCHS
#define ALLPERMITED_EXCEPT_2TOUCHS (PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION | PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION)
#endif