#pragma once

#include <vector>
#include <wingdi.h>

//@功能:显示适配器属性
struct DiplayAdpaterInfo
{
    DISPLAYCONFIG_PATH_TARGET_INFO   targetInfo;//目标监视器信息
    DISPLAYCONFIG_TARGET_MODE        targetMode;//显卡输给监视器的格式
    DISPLAYCONFIG_SOURCE_MODE        sourceMode;//计算机桌面的源格式
                        TCHAR        monitorDevicePath[128];//显示器设备路径
                        TCHAR        moniotrName[CCHDEVICENAME];//显示器名称
};
//显示器信息
struct DisplayDevInfo
{
	HMONITOR hMonitor;
    RECT   rcMonitor;
    TCHAR  szDevice[CCHDEVICENAME];
	BOOL   bIsPrimary             ;//是否是主屏标志
//	DWORD  physicalMaxWidthInMM   ;//屏幕的物理尺寸最大宽度, 单位:mm
//	DWORD  physicalMaxHeightInMM  ;//屏幕的物理尺寸最大高度, 单位:mm

	DWORD  MaxWidthInPixel        ;//最大像素宽度
	DWORD  MaxHeightInPixel       ;//最大像素高度

	DEVMODE CurrentSetings        ;//当前显示模式

	DWORD OptimalWidthInPixel     ;//最优宽度
	DWORD OptimalHeightInPixel    ;//最优高度

	//DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName;
	//DISPLAYCONFIG_TARGET_DEVICE_NAME targetName;

	//<<2017/10/27,新的监视器数据结构
	DISPLAYCONFIG_PATH_TARGET_INFO   targetInfo;//目标监视器信息
	DISPLAYCONFIG_TARGET_MODE        targetMode;//显卡输给监视器的格式
	DISPLAYCONFIG_SOURCE_MODE        sourceMode;//计算机桌面的源格式
	//>>

    std::vector<DiplayAdpaterInfo> displayAdapterInfos;

};

struct MonitorSignalFormat
{
	SIZE ActiveSize;
	SIZE PrimSurfSize;
};

class CDispDevFinder
{
public:
    CDispDevFinder();

    ~CDispDevFinder();
    
    //@功  能:搜索显示器SearchDisplayDev
    //@返回之:成功, TRUE
    //        失败, FALSE
    BOOL SearchDisplayDev();

    //@功  能:搜索显示器
    int GetDisplayDevCount()const;


    //@功能:返回指定编号的显示器信息。
    //@参数:nNum, 显示其编号
    //@说明:当输入的编号参数大于实际检测到的显示器个数则返回NULL
    const DisplayDevInfo* GetDisplayDevInfo(int nNum)const;


	//@功能:提取坐标点所在屏幕的信息
	const DisplayDevInfo* GetDisplayDevInfo(int x, int y) const;


    //@功能:使能屏幕拼接模式
    //@参数:bEnable,是否使能拼接模式
    //void EnableScreenMergeMode(BOOL bEnable);

	//@功能:获取扩展屏的信息
	const DisplayDevInfo* GetFirstExtendMonitorInfo()const;

	//@功能:获取主屏幕信息·
	const DisplayDevInfo* GetPrimaryMonitorInfo()const;

protected:
    

    std::vector<DisplayDevInfo> m_vecDisplayDevInfo;

    static BOOL CALLBACK MonitorEnumProc(  HMONITOR hMonitor,  // handle to display monitor
              HDC hdcMonitor,     // handle to monitor DC
              LPRECT lprcMonitor, // monitor intersection rectangle
              LPARAM dwData);     // data;

private:
    BOOL m_bScreenMergeMode;
};