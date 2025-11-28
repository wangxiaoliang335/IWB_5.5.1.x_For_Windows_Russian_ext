#include <list>
#include  <streams.h>
#include <vector>
#include <queue>
#include <map>
#include <fstream>
#include <limits>
#include <stack>

#include  <utility>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <time.h>
#include <atlbase.h>

#include "../inc/RectF.h"

#include "../inc/VideoProcAmpProperty.h"
#include "../inc/MultiDimensionPoint.h"
#include "../inc/LightSpot.h"
#include "../inc/MsgDefine.h"
#include "../inc/ImageFrame.h"
#include "../inc/DIB/Dib.h"
#include "../inc/DIB/DibCanvas.h"
#include "../inc/AVI/AVIWriter.h"
#include "../inc/AVI/AVIReader.h"
#include "../inc/NewFrameSink.h"
#include "../inc/MJPG/MJPG_Decoder.h"

#include "../inc/PerfDetector.h"
#include "../inc/DispMonitorFinder.h"

#include "../inc/Algolib.h"
#include "../inc/Algorithm/HeapSort.h"
#include "../inc/ImageDib.h"
#include "../inc/ShareMemoryRegion.h"
#include "../inc/BitFrame.h"
#include "./TinyXml/TinyXml.h"
#include "../inc/CommonDefine.h"
#include "../inc/IWB_API_Def.h"
#include "../inc/log.h"
#include "../inc/Mf2StdGf.h"
#include "../inc/Algorithm/MaxWeightBipartiteMatch.h"
#include "../inc/Round-RobinQueue.h"
#include "../inc/MyAPI.h"

//////////////////////////////////////////////////////////////////////////
//系统下手势识别相关头文件
#include "..\inc\wgr\IWGRUtilContext.h"
#include "..\inc\wgr\WinShellToolKit.h"
#include "..\inc\wgr\WGRPermissionParse.h"
#include "..\inc\wgr\WindowsGestureRecognition.h"

//////////////////////////////////////////////////////////////////////////

#include <atlcoll.h>
#include <atlstr.h>
#include <vector>
#include <atlstr.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#include <cfgmgr32.h>

#include <amvideo.h>
#include <dvdmedia.h>
#include <ks.h>
#include <d3d9.h>
#include <vmr9.h>

#include "DirectShowFunc.h"

#include "EnumDevices.h"

#include "VideoPlayer.h"

#include "Shortcut.h"


#include "FrameIntervalDetector.h"
#include "YUY2ToRGB.h"
#include "ARGBToDib.h"
//#include "LightSpot.h"

#include "Config.h"

#include "ContactPtFinder.h"
#include "ContactInfo.h"
#include "VirtualMouse.h"
#include "VirtualHID.h"



#include "IWB.h"
#include "USBDevDetector.h"
#include "USBCameraDeviceList.h"
#include "MultiLang.h"


#include "CalibrationDataDef.h"
#include "VideoToScreenMap.h"
#include "BezierInterpolator.h"
#include "StrokeSmoother.h"
#include "Akima_Interpolate.h"
#include "VelocityCompensator.h"
//#include "MouseEventGenerator.h"
//#include "GestureEventGenerator.h"

//#include "OpticalPenMatch_RedLeaf.h"


#include "SmartMatch.h"
#include "Gesture Recognition\WGRExeContext\WGRUtilContextImpl.h"
#include "Gesture Recognition\WGRExeContext\WGRCofig.h"

//#include "OpticalPenDoubleMatch.h"
#include "CollectSpotSize.h"
#include "SpotMerger.h"

#include "StrokeFilter.h"
#include "Gesture Recognition\GLBoard\MultiPenGestureRecognition.h"
#include "Gesture Recognition\GLBoard\GestureEventGenerator.h"
#include "SpotListProcessor.h"

#include "BaseStoneMarker.h"
#include "PenPosDetector.h"
#include "LostFrameDetector.h"

#include "DebugMediaType.h"
#include "InterceptInputPin.h"
#include "InterceptFilter.h"

#include "VideoPlayer.h"


#include "ManualCorrectWnd.h"
#include "TrayImpl.h"

//<<added by toxuke@gmail.com, 2015/09/09
#include "CalibrateSymbolChecker.h"
#include "CalibrateSymbolManualSampler.h"
//>>

#include "ManualCalibrationWnd.h"

//屏幕布局编辑工具
#include "ScreenLayoutDesigner.h"



#include "SensorTimerAction.h"
#include "IWBSensor.h"
#include "IWBSensorManager.h"


#include "OwnerDrawMenuBase.h"
#include "ColorCursor.h"
#include "PlayWnd.h"

#include "SimulateCalibarationParamDlg.h"
#include "HyperLinkWnd.h"
#include "ScreenRecognition.h"


#include "AdvancedSettingsDlg.h"

#include "GestureSettingDlg.h"
#include "ParamsSettingPropertySheet.h"
#include "SpotSettingDlg.h"
#include "LangSelectDlg.h"
#include "StatusDlg.h"

//...........
//安装提示处理模块

//...........

#include "IWBDlg.h"

//运算宏定义

//<32位整数绝对值宏
#ifndef IWB_INT32_ABS
#define IWB_INT32_ABS(n) (((n)^((n)>>31)) -((n)>>31))
#endif
//32位整数绝对值宏>

#include "VirtualTUIO.h"
