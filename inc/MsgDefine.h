#pragma once
#define WM_MANUAL_CALIBRATE_DATA       (WM_USER+1)
//手动校正完成通知消息
//WPARAM, 1:成功; 0:失败
//LPARAM, 保留
#define WM_FINISH_MAUNUAL_CALIBRATING  (WM_USER+2)
//#define WM_ABORT_MANUAL_CALIBRATING  (WM_USER+3)
#define WM_GRAPHNOTIFY                 (WM_USER+4)

//帧率通知消息
//WPARAM, 当前帧率
//LPARAM, 摄像头编号
#define WM_FPSNOTIFY                 (WM_USER+5)



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
#define  CLUTTER_DETECTION_DONE_MSG  (WM_USER + 11)

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
//#define  WM_CALIBRATE_FRAME_DATA  (WM_USER+15)
//设置采集光斑
#define  WM_COLLECT_SPOT_DATA     (WM_USER+16) 

//完成采集光斑的工作
//WPARAM,采样个数
#define  WM_FINISH_COLLECTSPOT    (WM_USER+17) 

//中断采集光斑的工作
#define  WM_BREAK_COLLECTSPOT     (WM_USER+18)

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

#define WM_MANUAL_COLLECT_SPOT      (WM_USER+23)
#define WM_DISPLAYWINDOW             (WM_USER+24)
#define  WM_CHANGE_FIXEDBOLBSETTIME  (WM_USER+25)

//////在运行的过程中，发现有固定的光斑存在，

#define WM_STATGE_DISABLE_OPTICAL_PEN_FOR_CLUTTER (WM_USER + 26)
#define WM_CHANGE_AUTOMASKDETECTTHRESHOLD     (WM_USER + 27)
//<<added by toxuke@gmail.com, 2013/04/19
//#define WM_CHANGE_ENABLE_GESTURE_RECOGNITION (WM_USER + 27)
//>>
#define WM_CHANGE_NORMALUSER_BRIGHTNESS (WM_USER + 28)

#define WM_CHANGE_ENABLE_GESTURE_TOUCH        (WM_USER + 29)
#define WM_CHANGE_ENABLE_PEN_TOUCH            (WM_USER + 30)
#define WM_RECORD_VIDEO                       (WM_USER + 31)

#define WM_CHANGE_AUTOCALIBRIATION_AVERAGE_BRIGHTNESS  (WM_USER + 32)
#define WM_CHANGE_AUTOCALIBRIATION_LIGHTGRAY           (WM_USER+ 33)
#define WM_REAR_PROJECTION                             (WM_USER+ 34)
#define WM_CHANGE_VIDEODISPLAYDELAY                    (WM_USER+ 35)


//摄像头状态
//WPARAM, LPCTSTR字符串
//LPARAM, 摄像头编号
#define WM_CAMERA_STATUS_NOTIFY (WM_USER + 35)


//////////////////////////////////////////////////////////////////////////
//更换视频显示窗口，主要在安装调试时，把视频绘画到其他窗口上
#define WM_CHANGE_DISPLAY_WINDOW          (WM_USER + 36)
////////////////

//////////////////////////////////////////////////////////////////////////
#define WM_PUT_DETECT_BACKSPLASH_DATA       (WM_USER + 37)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//安装向导过程中，通知开始/停止自动检测光带是否消失
#define WM_START_DETECT_BACKSPLASH_VANISH   (WM_USER + 38)
#define WM_STOP_DETECT_BACKSPLASH_VANISH    (WM_USER + 39)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//禁用或使能某窗口
#define WM_ENABLEORDISABLE_WINDOW           (WM_USER + 40)
//////////////////////////////////////////////////////////////////////////




//added by 15077726@qq.com, 2015/11/3
//<<校正精度调试使用
#define WM_CALIBRATE_MANUAL_RESAMPLE       (WM_USER + 41)
//>>


#define WM_APPLY_SENSOR_CONFIG            (WM_USER + 42)


#define WM_CREEN_LAYOUT_DESIGN_BUTTON_CLICK       (WM_USER + 43)

//四点标定结束标志
#define WM_END_4_BASE_POINT_CALIBRATE       (WM_USER + 44)

//摄像头参数设置
#define WM_CHANGE_INSTALL_BRIGHTNESS  (WM_USER+ 45)
#define WM_CHANGE_INSTALL_GRAMMA      (WM_USER+ 46)