// OpticalPen.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "UsbCameraDeviceList.h"
// CIWBApp:
// See OpticalPen.cpp for the implementation of this class

#include "../inc/EasiSDKReg_Interface.h"

#include <unordered_map>
//@功能:返回实际的触控类型
EDeviceTouchType GetActualTouchType();

//@功  能:返回实际的屏幕控制区域
//@参  数:
//@返回值:
SIZE GetActualScreenControlSize();

struct USBKeyInformation
{
    EDeviceTouchType        eUSBKeyTouchType     ;//
    EPalmTouchControlType   ePalmTouchControlType;//
    EFingerTouchControlType eFingerTouchControlType;
    EScreenMode             eScreenModeFromUsbKey;//
    BOOL                    bEnableFourPointCalibrate;

    USBKeyInformation()
    {
        eUSBKeyTouchType        = E_DEVICE_NOT_FOUND;
        ePalmTouchControlType   = E_PLAM_TOUCHCONTROL_UNKNOWN;
        eFingerTouchControlType = E_FINGER_TOUCHCONTROL_UNKNOWN;
        eScreenModeFromUsbKey   = EScreenModeSingle;
        bEnableFourPointCalibrate = FALSE;
    }
};


struct TAutoCalibrateCompensateData
{
    TAutoCalibCompCoefs coefs;//自动补偿系数
    double throwRatioOfLens  ;//采样镜头的投射比	
};

class CIWBApp : public CWinApp
{
public:
    CIWBApp();

    ~CIWBApp();
    // Overrides

    virtual BOOL InitInstance();

    BOOL IsStartFromAutoRun()const
    {
        return m_bAutoRunMode;
    }

    BOOL IsForAllUsers()const
    {
        return m_bForAllUser;
    }


    EDeviceTouchType  GetUSBKeyTouchType() const;
    EPalmTouchControlType GetPalmTouchType()const;
    EFingerTouchControlType GetFingerTouchType() const;

    BOOL IsFourPointCalibrateEnabled()const;
    //int  GetAllUSBKeyTouchTypeCount();
    //const USBKeyInformation* GetAllUSBKeyInformations() const;
    const  std::unordered_map<std::string, USBKeyInformation>& GetAllUSBKeyInformations() const;
    //int GetDoubleScreenMerge() const
    //{
    //  return m_nDoubleScreenType ;
    //}

    //慢速版本
    EScreenMode GetScreenMode() const ;

    //快速版本
    EScreenMode ScreenMode() const
    {
        return m_eScreenMode;
    }

    EScreenMode GetScreenModeFromUSBKey()const;

    int GetScreenCount() const;
    int GetScreenModeFromUSBKeyCount()const;

    BOOL IsHardwareKeyExist()const
    {
        //return m_bFoundHardwareUSBKey;
        return m_uHardwareUSBKeyCount > 0 ? TRUE : FALSE;
    }

    BOOL IsOnlineRegistered()const
    {
        return m_bIsOnlineRegistered;
    }
    LPCTSTR GetLangeCode()const
    {
        return (LPCTSTR)m_strLanguageCode;
    }
    // Implementation
    BOOL ParseCmdLine(LPCTSTR lpCmdLine);

    //@功能: 载入OEM资源。
    //@说明:OEM, Original Equipment Manufacture
    //OEM,  manufactures products or components that are purchased by a company 
    //      and retailed under that purchasing company's brand name.
    //
    HINSTANCE LoadOEMResource();


    //<<added by toxuke@gmail.com, 2013/10/10
    //@功能:初始化程序依赖的路径名称
    //      包括配置文件的目录路径和调试输出文件保存的目录路径。
    void  InitDirectoryInformation();

    CDispDevFinder& GetMonitorFinder() { return m_oDispMonitorFinder; }

    CString m_strSettingsDir;//配置文件存放目录
    CString m_strSystemDir; //安装程序下系统目录的位置
    CString m_strIntermediatOutputDir;//调试输出文件存放目录
    CString m_strFirmwareDirectory;//固件文件目录

    //@功能:从USBKey中读取信息
    //@参数:bFirstTime, 第一次检测UsbKey的存在
    //@说明:第一次检测UsbKey时允许弹出对话框, 并记录日志信息。
    //      第二次及以后则不再弹出兑换框。
    void ReadUSBKey(BOOL bFirstTime = FALSE);

    BOOL ReadUSBKeyData(UINT uKeyIndex);

    void OnPlugOutUSBKey(const char* szDevPath);
    void OnPlugInUSBKey(const char* szDevPath);

    //@功能:根据设备路径查找自动校准补偿系数
    const TAutoCalibrateCompensateData* GetCompensateData(const char* strDevPath)const;

    //@功能:重置自动校准补偿系数
    void  ResetCompensateData();

    //@功能:获取所有自动补偿校正系数
    void GetAllCompensateData(std::vector<TAutoCalibrateCompensateData>& compensateData);

    UINT& GetBetweenInstanceMsg()  { return  m_uBetweenInstanceMsg; }

protected:
    DECLARE_MESSAGE_MAP()


    HANDLE m_hMutex;
    BOOL   m_bAutoRunMode;
    BOOL   m_bForAllUser ;//所有用户标志

    //int    m_nUSBKeyTouchType;////0:为3DTouch电子白板,1:为手指触控电子白板，2:为高清手指触控，3:为手掌互动  
    //EDeviceTouchType m_eUSBKeyTouchType;//

   // EPalmTouchControlType    m_ePalmTouchControlType;   //手掌互动类型
    //EFingerTouchControlType  m_eFingerTouchControlType; //手指触控类型

    //std::vector<USBKeyInformation>  m_VecAllUsbKeyTouchType ;

    std::unordered_map<std::string, USBKeyInformation> m_AllUSbKeyInformations;
    
    //EScreenType    m_eScreenType; //0:为单屏，1：为双屏

    //EScreenMode    m_eScreenModeFromUsbKey;//从机密狗注册信息中得到的屏幕模式
   

    //BOOL           m_bFoundHardwareUSBKey;//发现硬件USB Key标志
    UINT           m_uHardwareUSBKeyCount;//发现硬件USB Key标志
    BOOL           m_bIsOnlineRegistered;//是否在线注册了。
    CString        m_strLanguageCode;//语言编码

    CDispDevFinder m_oDispMonitorFinder;

    



    std::unordered_map<std::string, TAutoCalibrateCompensateData> m_allCompensateCoefs;
    
    
    
    //const static   int     COMPENSATE_NUM = 6;
    //double    m_pParams[COMPENSATE_NUM];
    //double    m_nCollectType;
    //static const int m_nStartDelayTime = 5000;//10000ms,最大启动延迟时间

    UINT m_uBetweenInstanceMsg;

    EScreenMode m_eScreenMode;

};

extern CIWBApp theApp;