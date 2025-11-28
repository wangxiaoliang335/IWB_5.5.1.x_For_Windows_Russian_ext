#pragma once
#ifdef EASISDKREG_INTERFACE_EXPORTS
    #define EASISDKREG_INTERFACE_API _declspec(dllexport)
#else
    #define EASISDKREG_INTERFACE_API _declspec(dllimport)

#endif

#include <string>
extern "C"
{

//@功能:SDK初始化
EASISDKREG_INTERFACE_API HRESULT  SKDREG_Init();


//@功能:SDK反初始化
EASISDKREG_INTERFACE_API void  SKDREG_Uninit();

//@功  能:通过读取USB软件狗,遍历检查所有正确的软件狗, 判断基本功能是否可用
//@参  数:nFunctionNo,1~15,基本功能编号
//@返回值:S_OK,基本功能使能
//        -1,软件狗不存在
//        -2,软件狗数据检验失败
//        -3,超过期限了
//        -4,软件狗硬件ID错误
//        -5,未使能
EASISDKREG_INTERFACE_API HRESULT SDKREG_IsBasicFunctionEnabled(int nFunctionNo);


//@功  能:通过读取USB软件狗,判断PPT索引SDK是否可用
//@参 数:nUSBKeyIndex, 从0开始的USB Key索引号
//@返回值:S_OK,PPT索引SDK可用
//        -1,软件狗不存在
//        -2,软件狗数据检验失败
//        -3,超过期限了
//        -4,软件狗硬件ID错误
//        -5,未使能
EASISDKREG_INTERFACE_API HRESULT SDKREG_PPTIndexSDKEnabled(UINT uUSBKeyIndex = 0);



//@功  能:通过读取USB软件狗,判断鼠标键盘事件钩子SDK是否可用
//@参 数:nUSBKeyIndex, 从0开始的USB Key索引号
//@返回值:S_OK,鼠标键盘事件钩子SDK可用
//        -1,软件狗不存在
//        -2,软件狗数据检验失败
//        -3,超过期限了
//        -4,软件狗硬件ID错误
//        -5,未使能
EASISDKREG_INTERFACE_API HRESULT SDKREG_KMEventHookSDKEnabled(UINT uUSBKeyIndex = 0);


//@功  能:通过读取USB软件狗,判断鼠标控制云台SDK是否可用
//@参 数:nUSBKeyIndex, 从0开始的USB Key索引号
//@返回值:S_OK,鼠标控制云台SDK可用
//        -1,软件狗不存在
//        -2,软件狗数据检验失败
//        -3,超过期限了
//        -4,软件狗硬件ID错误
//        -5,未使能
EASISDKREG_INTERFACE_API HRESULT SDKREG_MousePTZControlSDKEnabled(UINT uUSBKeyIndex = 0);



//@功  能:检查USB软件狗的状态, 遍历检查所有正确的软件狗。
//@返回值:S_OK,USB软件狗状态正常
//        -1,软件狗不存在
//        -2,软件狗数据检验失败
//        -3,超过期限了
//        -4,软件狗硬件ID错误,USB软件狗数据区中记录的HID和真实的HID不一致
//        -5,未使能
//        -6,系统时间错误
//        -7,写入软件狗失败
//@说明:每次检测完毕后,会将系统时间写入USB数据区
EASISDKREG_INTERFACE_API HRESULT SDKREG_CheckUSBSoftwareDog();


//@功  能:返回错误信息
//@参  数:lErrorCode,错误代码
//@返回值:错误代码对应的描述信息
EASISDKREG_INTERFACE_API const WCHAR* SDKREG_GetErrorMsg(HRESULT lErrorCode);

//@功  能:获取加密锁版本号
//@参  数:pVersion, 存放加密狗版本的数据缓冲区。
//        nUSBKeyIndex, 从0开始的USB Key索引号
//@返回值:S_OK,鼠标控制云台SDK可用
//        -1,软件狗不存在
//        -2,软件狗数据检验失败
//        -3,超过期限了
//        -4,软件狗硬件ID错误
//        -5,未使能,
//        -6,系统时间错误
//        -7,写入错误
EASISDKREG_INTERFACE_API HRESULT SDKREG_GetVersion(float* pVersion, UINT uUSBKeyIndex = 0);


//@功  能:获取加密锁对应的应用程序类型
//@参  数:pAppType, 存放应用程序类型的数据缓冲区。
//        nUSBKeyIndex, 从0开始的USB Key索引号
//@返回值:S_OK,鼠标控制云台SDK可用
//        -1,软件狗不存在
//        -2,软件狗数据检验失败
//        -3,超过期限了
//        -4,软件狗硬件ID错误
//        -5,未使能
//        -6,系统时间错误
//        -7,写入错误
//@说明:白板定位软件类型,
//      第一个字节定义
//      bit0: 定位软件类型
//            0:笔触电子白板
//            1:手触电子白板
//            2:手指触控
//            3:手掌互动
//
//      第二字节定义
//      bit0:双屏拼接功能使能位
//            0，无多双屏拼接功能
//            1，有多屏拼接功能
//      bit1:0:windows
//           1:android
//       其他位保留未用
EASISDKREG_INTERFACE_API HRESULT SDKREG_GetAppType(int* pAppType, UINT uUSBKeyIndex = 0);


//@功 能:返回USB Key的数目
EASISDKREG_INTERFACE_API UINT SDKREG_GetUSBKeyCount();


//@功 能:返回USB Key的路径
//@参数:uUSBKeyIndex, USBKey的索引号
//      devPath, 保存设备路径的缓冲区
//      bufSize, 缓冲区尺寸
EASISDKREG_INTERFACE_API HRESULT SDKREG_GetUSBKeyDevPath(UINT uUSBKeyIndex, char* devPathBuf, int bufSize);


//@功  能:读取多屏拼接类型,和SDKREG_GetAppType返回的多屏拼接使能配合使用
//@返回值:B0~31的定义如下
//       B0:双屏拼接
//       B1:3屏拼接
//       B2:4屏拼接
//       B3:5屏拼接
//       B31~4:预留
EASISDKREG_INTERFACE_API UINT SDKREG_GetMultiScreenMergeType(UINT uUSBKeyIndex = 0);


//@功  能:返回应用参数类型
//@返回值:
//       对手指触控:0:F0; 1:F1; 2:F2; 3:F3; 4:F4;  5:F5
//       对手掌触控:0:P0; 1:P1; 2:P2; 3:P3; 4:P4;  5:P5;
//                 6:T0; 7:T1; 8:T2; 9:T3;10:T4; 11:T5
//                 12:TX1;13:TX2
//
//
//
EASISDKREG_INTERFACE_API UINT SDKREG_GetParamType(UINT uUSBKeyIndex = 0);



#define COMPENSATE_PARAM_COUNT   6
#define DATA_PREFIX_BYTE_COUNT 3
#define DATA_SUFFIX_BYTE_COUNT 3


//@功  能:读出补偿的校正参数的数值
//@参 数:pParamsBuf, 保存补偿参数的首地址
//       nParamsBufCount, 参数个数
//       uUSBKeyIndex, USB设备索引.
//       devPathBuf, 保存设备路径的缓冲区, 用来标识不同的设备。
//@返回值:读取的double参数个数
EASISDKREG_INTERFACE_API  UINT SDKREG_ReadE2PROMCompensateParams(double* pParamsBuf, int nParamsBufCount, UINT uUSBKeyIndex, char* devPathBuf = NULL, int bufSize = 0);

//@功  能:写入补偿的校正参数的数值
//@参  数:
//        pParams, 要写入的参数值首地址
//        nParamCount, 写入的参数个数
//         uUSBKeyIndex, USB设备索引.
//@返回值:  S_OK, 成功
//         E_FAIL, 失败
EASISDKREG_INTERFACE_API  HRESULT SDKREG_WriteE2PROMCompensateParams(const double* pParams, int nParamCount, UINT uUSBKeyIndex);

}