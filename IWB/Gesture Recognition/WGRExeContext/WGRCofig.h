#pragma once

#include <string>
#include <list>
#include <map>
#include "..\..\..\inc\wgr\WGRPermissionPublic.h"


//typedef enum
//{
//    E_MT_NONE       ,//未修改
//    E_MT_ADD        ,//新增加
//    E_MT_DEL        ,//待删除
//    E_MT_UPDATE     ,//更新
//}E_MODIFIED_TYPE;
#define KSTANDAR_XSCRNRESOLUTION        1024
#define KSTANDAR_YSCRNRESOLUTION        768
//#define KSTANDAR_XSCRNPHYSIZE    85
//#define KSTANDAR_YSCRNPHYSIZE    85
#define KSTANDAR_SCRNDLGPHYSICALLENGTH  (80.0*25.4f)

typedef struct _ST_WGR_ACTUALPARAMS
{
    //屏幕分辨率
    int xScreenResolution;
    int yScreenResolution;
    //屏幕物理尺寸
    float     fScreenDiagonalPhysicalLength;//触屏对角线物理尺寸,单位:毫米(mm);
}ST_WGR_ACTUALPARAMS;

typedef struct _ST_WGR_EXCEPTION
{
    std::string procName;
    std::string apppName;
    DWORD		permissionSets;	
    //E_MODIFIED_TYPE modifiedType;

    _ST_WGR_EXCEPTION()
    {
        permissionSets = 0;
        //modifiedType = E_MT_NONE;
    }
}ST_WGR_EXCEPTION;

typedef struct _ST_WGR_PERMISSION_CONFIG
{
    DWORD generalPermissionSets;
    std::list<ST_WGR_EXCEPTION> lstExceptions;

    _ST_WGR_PERMISSION_CONFIG()
    {
        //默认常用程序支持所有手势
        generalPermissionSets = PERMFLAGSET_A_REGION | PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION 
                        | PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION;

        //lstExceptions.resize(4);        
    }
}ST_WGR_PERMISSION_CONFIG;

typedef struct _ST_WGR_CONFIGDATA
{
    ST_WGR_PERMISSION_CONFIG permissionConfig;
    bool                     isLogPermitted;
    int                      multiplePalmtoStd;          //手掌光斑与标准采样光斑的倍数
}ST_WGR_CONFIGDATA;

typedef struct _ST_GR_WEIGHTCONF
{//要求k0不大于k1
    byte k0;
    byte k1;
}ST_GR_WEIGHTCONF; 

//////////////////////////////////////////////////////////////////////////
//GLBoard
typedef struct _ST_GLBOARDGR_CONFIG
{
    /*std::string procName;
    std::string apppName;*/
    std::string clsName;
    std::string wndTitle;    
    DWORD		permissionSets;
}ST_GLBOARDGR_CONFIG;

typedef struct _ST_GLBOARDGR_CONFIGDATA
{
    //ST_GLBOARDGR_CONFIG permissionConfig;
    bool                isGLBoardGRPermit;
    bool                isGLBoardGRLogPermitted;
    int                 multipleEraserToStd;          //手掌光斑与标准采样光斑的倍数

    _ST_GLBOARDGR_CONFIGDATA()
    {
        isGLBoardGRPermit = true;
        isGLBoardGRLogPermitted = false;
        multipleEraserToStd = 4;
    }
}ST_GLBOARDGR_CONFIGDATA;

typedef struct _ST_GR_CONFIGDATA
{
    ST_WGR_CONFIGDATA       wgrConfig;
    ST_GLBOARDGR_CONFIGDATA glboardConfig;
}ST_GR_CONFIGDATA;

class CWGRCofig
{
public:
    CWGRCofig(void);
    ~CWGRCofig(void);

public:
    bool IsLogPermitted()
    {
        return m_wgr_CfgData.isLogPermitted;
    }

    void SetLogPermitted(bool bPermssion)
    {
        m_wgr_CfgData.isLogPermitted = bPermssion;
    }

    void SetHIDMode(bool bIsTouchPad);

    int GetMultiplePalm2Std() {return m_wgr_CfgData.multiplePalmtoStd;}
    void SetMultiplePalm2Std(int multiple) {m_wgr_CfgData.multiplePalmtoStd = multiple;}

    //<added by Jiqw 2015/05/15 11:12
    //由于未给GLBoard单独创建配置文件，为了偷懒，暂时把GLBoard的板擦倍值和是否开启GLBoard手势配置放在Windows手势配置文件中
    /*
       @功能：获取或设置板擦和标准采样光斑的倍值
    */
    int GetMultipleEraser2Std() {return m_glbgr_cfgData.multipleEraserToStd;}
    void SetMultipleEraser2Std(int multiple) {m_glbgr_cfgData.multipleEraserToStd = multiple;}

    /*
       @功能：获取或设置是否开启GLBoard手势
    */
    bool IsGLBoardGRPermit() {return m_glbgr_cfgData.isGLBoardGRPermit;}
    void SetGLBoardGRPermission(bool bEnable) {m_glbgr_cfgData.isGLBoardGRPermit = bEnable;}
    //>

    /*
    @功能：获取当前位置窗口所属进程手势许可值
    @参数：ptCur，当前的位置
    @返回值：当前进程所有的手势识别许可值
    */
    DWORD GetPermission(const POINT &pt);

    /*
    @功能：获取一般进程的手势识别许可值    
    @返回值：一般进程的手势识别许可值
    */
    DWORD GetGeneralPermission();    
    void SetGeneralPermission(const DWORD &perm);

    bool IsGerneralGRPermit() {return m_wgr_CfgData.permissionConfig.generalPermissionSets != INIT_WITH_ALL_PROHIBITED;}
    void SetGeneralPermission(bool bFlag) {
        if(bFlag) {m_wgr_CfgData.permissionConfig.generalPermissionSets = INIT_WITH_ALL_PERMITED;}
        else      {m_wgr_CfgData.permissionConfig.generalPermissionSets = INIT_WITH_ALL_PROHIBITED;}
    }
    /*
    @功能：获取例外进程的手势识别许可值
    @参数：procName，例外进程名称
    @返回值：例外进程的手势识别许可值
    */
    DWORD GetExceptionPermission(const std::string &procName);
    void SetExcetionPermission(const DWORD &perm, const std::string &procName); 

    /*
    @功能：获取例外进程列表
    */
    const std::list<ST_WGR_EXCEPTION> &GetExceptionalProcessList()  const;
    /*
    @功能：添加例外进程
    @参数：procName，例外进程名称
           appName【可为空】，例外进程所属程序名称
           perm，例外进程的许可值
    @返回值：true，添加成功；false，添加失败
    */
    bool AddExceptionalProcess(const std::string &procName, const std::string &appName, const DWORD &perm);
    /*
    @功能：删除例外进程
    @参数：procName，例外进程名称    
    */
    void DeleteExceptionalProcess(const std::string &procName);

    bool IsTheProcessHasAdded(const std::string &procName);

    //int GetCurrentProcessInfo(std::string *procNameArry, const int &arrySize);
    int GetCurrentProcessInfo(char (*ppProcNameArry)[256], const int &nMaxArrySize);

    /*
        @更新屏幕分辩参数
    */
    void UpdateScreenResolutionParams(int nScreenWidth, int nScreenHeight)
    {
        m_wgr_ActualParams.xScreenResolution = nScreenWidth;
        m_wgr_ActualParams.yScreenResolution = nScreenHeight;    
    }

    /*
        @更新物理尺寸
    */
    void UpdateScreenDiagonalPhysicalLength(float fPhysicalLength)
    {
        m_wgr_ActualParams.fScreenDiagonalPhysicalLength = fPhysicalLength;
    }

    int MapStandardX2ActualValue(int xStandard);
    int MapStandardY2ActualValue(int yStandard);

    /*
       @功能：获取加权的光斑质量值
       @参数：nActualMass，实际的光斑质量值；
       @参数：nExtRectArea，光斑的外接矩形面积；
       @参数：nStdAreaMulti,标准采样面积乘以相应倍数后的标准面积倍值

       @说明：
       //由于nExtRectArea是外接矩形的面积，且该外接矩形与nActualMass相差较大，尤其笔快速移动时，所以采用如下方法来平衡
       //
       @平衡时的条件：nActualMass < nStdAreaMulti，且nExtRectArea > nStdAreaMulti
       @方法：
       令k0, k1为正数， k0 < k1 且 2*k0 > k1
       lambda = (mass / lAreaInVideo) * (k0 / k1)            

       T = (1 - lambda) * mass + lambda * lAreaInVideo

       T = [(k0+k1)*mass + k1 - 1] / k1 - (k0*mass*mass + k1*lAreaInVideo - 1) / k1*lAreaInVideo
       //
    */
    int GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti);

    void SetDoubleScreen(bool isDoubleFlag);

    //默认情况下，如果常规进程的手势关闭了，则相应的特例进程的手势也会强制关闭
    bool IsAllGestureClosed();
    bool IsAllPalmBasedGestureClosed();
    bool IsAllFingerBasedGestureClosed();

	//added by 15077726@qq.com
	//<<2017/12/26
	//有客户直接断电重启计算机，导致CWGRCofig没有机会在析构函数中保存文件。
	//因此需要在配置对话框中立即保存设置。
	BOOL Save();
	//>>
private:
    //@功能:载入配置文件
    //@参数:lpszConfigFilePath, 配置文件的完整路路径
    BOOL LoadConfig(LPCTSTR lpszConfigFilePath);


    //@功能:保存配置文件
    //@参数:lpszConfigFilePath, 配置文件的完成路径
    BOOL SaveConfig(LPCTSTR lpszConfigFilePath);


private:
    ST_WGR_CONFIGDATA   m_wgr_CfgData;
    ST_GR_WEIGHTCONF    m_gr_WeightConf;

    //<added by Jiqw 2015/05/15 11:12
    //由于未给GLBoard单独创建配置文件，为了偷懒，暂时把GLBoard的板擦倍值和是否开启GLBoard手势配置放在Windows手势配置文件中
    ST_GLBOARDGR_CONFIGDATA m_glbgr_cfgData;
    //>

    bool                m_bIsTouchPad;      //当前是否是触屏模式，触屏模式下，不开启两触点手势

    ST_WGR_ACTUALPARAMS m_wgr_ActualParams; //当前环境的白板屏幕实际参数
    bool                m_isDoubleScreen;   //双屏情况下，获取的分辩率值要减半
};

extern CWGRCofig g_oWGRConfig;
