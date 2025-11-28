#pragma once
//@Date:2011/04/24
//@Author: toxuke@gmail.com

#ifdef ALGOLIB_EXPORTS	   
	#define ALGOLIB_API _declspec(dllexport)
#else
    #define ALGOLIB_API _declspec(dllimport)
#endif

#include <vector>
//Part 3. typedef std::vector<POINT> Points;

#include "ImageFrame.h"

#define ERR_SOFTKEY_VERSION  (-8)

#define ERR_SOFTKEY_APP_TYPE (-9)

#define ERR_START_CALIBRATING_FAILED (-10)

#define ERR_INVALID_PARAMETER (-11)


//Used In Part 1 and Part 2.
//
//================================================
//Part 1. 自动校正对象
enum E_AutoCalibrateGridsNumber
{
	E_CALIBRATE_GRIDS_NUMBER_9X9   = 0,
	E_CALIBRATE_GRIDS_NUMBER_11X11 = 1,
	E_CALIBRATE_GRIDS_NUMBER_13X13 = 2,
	E_CALIBRATE_GRIDS_NUMBER_15X15 = 3,
	E_CALIBRATE_GRIDS_NUMBER_17X17 = 4,
};


//自动校正错误代码
enum EAutoCalibrateError
{
	E_AUTO_CALIBRATOR_OK,
	E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED,//未检测到屏幕区域
	E_AUTO_CALIBRATOR_ERROR_CORNER_DETECT_FAILED,//角点检测失败
	E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED,//角点匹配失败
	E_AUTO_CALIBRATOR_ERROR_UNDEF,//未知错误
};

//自动校正单元坐标
struct TCalibrateMarkerCoord
{
	POINT ptScreenCoord;//
	POINT ptImageCoord ;//
	BOOL bIsMatched    ;//屏幕坐标和图像坐标已经匹配标志 

	TCalibrateMarkerCoord()
	{	
		ptScreenCoord.x = ptScreenCoord.y = 0;
		ptImageCoord.x  = ptImageCoord.y  = 0;
		bIsMatched = FALSE;
	}
};

//校正调试级别
enum ECalibrationDebugLevel
{
	E_CALIB_DEBUG_LEVEL_CONCISE,//精简检出
	E_CALIB_DEBUG_LEVEL_VERBOSE,//详细输出
};

class ALGOLIB_API CAutoCalibrator
{
public:
	//@功能:开始自动校正
	//@参数:hNotifyWnd, 当校正结束后, 接收校正完毕消息WM_AUTO_CALIBRATE_DONE的窗体句柄。
	//      eGridNumber, 自动校正时, 显示的棋盘格校正图案的模式
	//      eDebugLevel, 调试级别
	//@返回值:成功返回 0;
	//        <0, 错误代码
	//              
	//        -1,加密锁错误,详见加密狗接口库
	//         ...
	//        -7, 加密锁错误,详见加密狗接口库
	//        -8, 加密锁版本错误
	//        -9, 加密锁对应的应用程序类型错误
	//        -100,其他错误
	//@说明:开始自动校正后, 会生成校正窗体, "自动校正"状态机回到初始状态。
	LONG StartCalibrating(HWND hNotifyWnd, E_AutoCalibrateGridsNumber eGridsNumber, ECalibrationDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE);
	
	////@功能:外部程序开启自动校正后, 将来自摄像头的图片帧送入自动校正模块, 驱动自动校正状态机的运行。
	////@参数:pYUVFrame, 指向YUY2格式的位图帧
	////@返回值:成功返回TRUE
	////        失败返回FALSE
	//BOOL ProcessImage(const CImageFrame* pYUVFrame);

	//@功能:结束自动校正,
	//@说明:
	void EndCalibrating();
	

	 //@功能:获取自动校正生成的计算屏幕影像的屏蔽图
	 //@说明:自动校正算法会自动识别出计算机屏幕在摄像头图像中所占的区域
	//       屏蔽图是一个黑白二值图, 值为0XFF的区域对应屏幕区域，0x00为
	//       需要遮蔽区域。
	 const CImageFrame& GetMaskFrame()const;


	 //@功能:获取自动校正成功匹配的校正点个数
	 int GetValidCalibratePtNumber() const;

	 //@功能:获取"校正标志"的屏幕坐标和在摄像头图像中的图像坐标
	 //@参数:"校正标志"的索引好
	 //@说明:"校正标志"仅指相邻四个格子边组成的"十"字的中心
	const TCalibrateMarkerCoord* GetCalibrateMarkerCoord(int nIndex) const;

	//@功能:获取校正窗体的句柄
	//@说明:外部程序获取的校正图片帧通过发送消息WM_MANUAL_CALIBRATE_DATA通知给校正窗体, 校正窗体
	//      将数据送进"校正状态机", 驱动状态机的运行。
	HWND GetCalibrateHWnd()const;


	//调试函数
	//@功能:通过分析校正过程中录制的录像文件, 再现校正过程, 以便于调试校正算法。
	//@参数:lpszAVIFilePath, 录制下的AVI文件路径
	//      hNotifyWnd,  当校正结束后, 接收校正完毕消息WM_AUTO_CALIBRATE_DONE的窗体句柄。
	//      eGridNumber, 自动校正时, 显示的棋盘格校正图案的模式

	BOOL DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd,  E_AutoCalibrateGridsNumber eGridsNumber=E_CALIBRATE_GRIDS_NUMBER_9X9,  ECalibrationDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_VERBOSE);
};




//================================================
//Part 2. 光点检测算法
//目标信息
struct TBlobObject
{
	int nObjectId;//目标Id
	int mass     ;//质量
	int mx       ;//对x轴的力矩
	int my       ;//对y轴的力矩

	POINT GPos    ;//重心坐标
	RECT rcArea   ;//区域
	BOOL bIsValid ;//正确标志


	bool operator < (TBlobObject& right)
	{
		return (mass < right.mass);
	}

	bool operator <= (TBlobObject& right)
	{
		return (mass <= right.mass);
	}


	bool operator > (TBlobObject& right)
	{
		return (mass > right.mass);
	}

	bool operator >= (TBlobObject& right)
	{
		return (mass >= right.mass);
	}

};

#define MAX_OBJ_NUMBER 1024
class ALGOLIB_API CBlobDetector
{
public:

	//@功能:检测位图中亮度大于指定门限值的不规则区域的最大外接矩形, 并计算此区域的重心。
	//@参数:frame, YUY2格式的位图帧
	//      YThreshold, 检测的亮度门限
    //      pClipArea, 可选参数, 屏蔽裁减区域尺寸
	//@返回:检测到的目标的个数
	//@说明:
	UINT operator() (const CYUY2Frame& YUY2Frame, BYTE YThreshold, const RECT* pClipArea=NULL);

	//@功能:获取目标列表
	const TBlobObject* GetObjs()const;

protected:	
};



//====================================================
//Part 3.坐标校正算法
typedef std::vector<POINT> Points;

//
//@功能:计算摄像头图象坐标到屏幕坐标的映射的单应矩阵(Homography Matrix),
//@输入:screenPts ,输入, 屏幕坐标
//      imagePts  ,输入, 图象坐标
//      screenSize,输入, 屏幕尺寸
//      imageSize ,输入, 图象尺寸
//      mapMatrix ,输出,图象映射矩阵, 其尺寸=sizeof(POINT)*IMAGE_WIDTH*IAMGE_HEIGHT.
//      lpClipRect,输出,保存屏幕区域外接矩形
//@输出:错误代码
//      0:成功, 其他:失败。
//      错误码   含义
//        -1,加密锁错误,详见加密狗接口库
//         ...
//        -7, 加密锁错误,详见加密狗接口库
//        -10,输入数据错误
//
//
//@说明:screenPts[i] 与imagePts[i](i=1,...,N)是一对匹配点。
//      mapMatrix[i]中的数据已经归一化到0~65535范围内  
extern "C" ALGOLIB_API LONG CoordCalibrate(const Points& screenPts, const Points& imagePts, const SIZE& screenSize, const SIZE& imageSize, POINT* mapMatrix, RECT* lpClipRect);



//====================================================
#include "NewFrameSink.h"
//Part 4. 屏幕区域侦测
class ALGOLIB_API CScreenAreaDetector
{
public:
	//@功 能:开启侦测,同时显示全屏窗口
	//@参 数:hNotifyWnd, 接收侦测结束通知的窗体句柄
	//@返回值;	
	BOOL StartDetect(HWND hNotifyWnd);


	
	//@功  能:结束侦测功能,
	//@参  数:
	//@返回指:
	BOOL End();


	//@功能:返回屏幕区域屏蔽位图
	const CImageFrame& GetMaskFrame()const;

	operator INewFrameEventSink*();
    operator INewFrameEventSink&();
};


