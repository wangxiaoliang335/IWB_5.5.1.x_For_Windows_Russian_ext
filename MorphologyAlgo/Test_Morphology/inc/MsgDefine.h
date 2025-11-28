#pragma once
#define WM_MANUAL_CALIBRATE_DATA      (WM_USER+1)
#define WM_FINISH_CALIBRATING (WM_USER+2)
#define WM_BREAK_CALIBRATING  (WM_USER+3)
#define WM_GRAPHNOTIFY        (WM_USER+4)
#define WM_FPSNOTIFY          (WM_USER+5)


//请求校正矩阵计算的消息
#define WM_CALC_ADJUST_MATRIX (WM_USER+6)

//设置检测门限
//WPARAM 门限值 0~100
#define WM_SET_DETECT_THRESHOLD (WM_USER+7)

//自动校正数据
#define WM_AUTO_CALIBRATE_DATA  (WM_USER+8)


//自动校正完毕
#define WM_AUTO_CALIBRATE_DONE  (WM_USER+9)

#define TRAY_NOTIFY_MSG      (WM_USER + 10)
#define  AUTO_MASK_DONE_MSG  (WM_USER + 11)

//初始阶段发现亮斑, 自动禁用光笔
#define WM_INITIAL_STATGE_DISABLE_OPTICAL_PEN_FOR_CLUTTER (WM_USER + 12)


//设置导引线位置
//WPARAM, 第一位0表示第一条导引线,1表示第二条导引线
//LPARAM, 低字表示垂直位置
#define WM_CHANGE_GUIDELINE_Y_POS (WM_USER + 13)


//设置导引线位置
//WPARAM, 第一位0表示第一条导引线,1表示第二条导引线
//LPARAM, 第一位表示是否显示导引线
#define WM_SHOW_GUIDELINE (WM_USER + 14)

//设置取得图像的信息
#define  WM_CALIBRATE_FRAME_DATA  (WM_USER+15)
//设置采集光斑
#define  WM_COLLECT_SPOT_DATA     (WM_USER+16) 
//完成采集光斑的工作
#define  WM_FINISH_COLLECTSPOT    (WM_USER+17) 
//中断采集光斑的工作
#define  WM_BREAK_COLLECTSPOT     (WM_USER+18)
//光斑缺省值的设置
//#define  WM_SET_DETECT_BOLB       (WM_USER+19)

#define  WM_CHANGE_SPOTPROPORTION  (WM_USER+19)
#define  WM_CHANGE_MULTERASER      (WM_USER+20)


//结束屏幕区域的侦测
//WPARAM, 0
//LPARAM, 0:侦测失败
//               1:侦测成功
#define WM_SEARCH_SCREEN_AREA_DONE (WM_USER+21)


//手动人工修正结束
//WPARAM, 0
//LPARAM, 0: 失败
//               1: 成功
#define WM_MANUAL_CORRECT_DONE     (WM_USER+22)

