#pragma once

//@功能:检索扩展的显示器信息


//显示器信息
struct MonitorInfo
{
    POINT ptLefTop;
    int   nWidth  ;
    int   nHeigh  ;
    //BOOL  bValid  ;//正确标志
};


class CDispMonitorFinder
{
public:
    CDispMonitorFinder();

    ~CDispMonitorFinder();
    
    //@功  能:搜索显示器
    //@返回之:成功, TRUE
    //        失败, FALSE
    BOOL SearchMonitor();


    //@功  能:搜索显示器
    int GetMonitorCount()const;



    //@功能:返回指定编号的显示器信息。
    //@参数:nNum, 显示其编号
    //@说明:当输入的编号参数大于实际检测到的显示器个数则返回NULL
    const MonitorInfo* GetMointorInfo(int nNum)const;

protected:
    int m_nMonitorCount;
    MonitorInfo* m_pMonitorInfos; 

    static BOOL CALLBACK MonitorEnumProc(  HMONITOR hMonitor,  // handle to display monitor
              HDC hdcMonitor,     // handle to monitor DC
              LPRECT lprcMonitor, // monitor intersection rectangle
              LPARAM dwData);     // data;

private:

};