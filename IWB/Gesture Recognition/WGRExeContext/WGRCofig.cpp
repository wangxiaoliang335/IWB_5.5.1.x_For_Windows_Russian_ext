#include "StdAfx.h"
#include "..\..\Profile.h"
#include "..\ProcessUtility.h"
#include "..\..\headers.h"

//////////////////////////////////////////////////////////////////////////
//常量定义
const char *KTAG_WGRSETTING         = "WGRSetting";
const char *KTAG_PERSETTING         = "PERSetting";
const char *KTAG_EXCEPTIONS         = "Exceptions";
const char *KTAG_EXCEPTIONPRFIX     = "E_";
const char *KTAG_GERNERAL           = "General";

const char *KTAG_LOGSETTING         = "LOGSETTING";

const char *KTAG_MULTISETTING       = "MULTIPLESETTING";

const char *KTAG_WEIGHTCONF         = "WEIGHTCONF";

const char *KTAG_GLBGRPERMIT        = "GLBGRPERMIT";
const char *KTAG_ERASERMULTIPLE     = "ERASERMULTIPLE";
//const char *KTAG_

//const char *KTAG_PHYSICALSIZE       = "PhysicalSize";

const char *KATTR_EXCEPTIONS        = "count";
const char *KATTR_PROCNAME          = "procName";
const char *KATTR_APPNAME           = "appName";
const char *KATTR_PERMISSION        = "permission";

const char *KATTR_LOGPERMISSION     = "logPermission";

const char *KATTR_MULTIPLE          = "value";

const char *KATTR_k0              = "k0";
const char *KATTR_k1              = "k1";

const char *KATTR_GLBGRPERMIT     = "value";
const char *KATTR_ERASERMULTIPLE  = "value";

//const char *KATTR_PHYSICALSIZE      = "value";

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
CWGRCofig g_oWGRConfig;

//////////////////////////////////////////////////////////////////////////
/*
@功能：int转化为string
*/
std::string Int2String(int toBeConverted)
{
    char temp[16];
    memset(temp, 0, 16);

    sprintf_s(temp, 16, "%d", toBeConverted);

    return temp;
}
//////////////////////////////////////////////////////////////////////////

CWGRCofig::CWGRCofig(void)
{
    m_wgr_CfgData.isLogPermitted = false;
    m_wgr_CfgData.multiplePalmtoStd = 3;
    m_bIsTouchPad = false;   

    m_wgr_ActualParams.xScreenResolution = KSTANDAR_XSCRNRESOLUTION;
    m_wgr_ActualParams.yScreenResolution = KSTANDAR_YSCRNRESOLUTION;
    m_wgr_ActualParams.fScreenDiagonalPhysicalLength = KSTANDAR_SCRNDLGPHYSICALLENGTH;
    m_isDoubleScreen = false;

    m_gr_WeightConf.k0 = 1;
    m_gr_WeightConf.k1 = 3;

    //
    m_glbgr_cfgData.isGLBoardGRLogPermitted = false;
    m_glbgr_cfgData.isGLBoardGRPermit = true;
    m_glbgr_cfgData.multipleEraserToStd = 4;

    LoadConfig(PROFILE::WGR_CONFIG_FILE_NAME);
}

CWGRCofig::~CWGRCofig(void)
{
    SaveConfig(PROFILE::WGR_CONFIG_FILE_NAME);
}

BOOL LoadExceptionsConfig(TiXmlNode *pNode, ST_WGR_CONFIGDATA &cfgData)
{
    if (pNode == NULL) return FALSE;

    TiXmlNode *pTmpNode = NULL;           
    TiXmlElement *pTmpEle = (TiXmlElement *)pNode;

    int nCount = 0;
    const char *pCount = pTmpEle->Attribute(KATTR_EXCEPTIONS, &nCount);
    if (pCount != NULL)
    {                       
        if (nCount != 0)
        {//载入例外进程列表
            std::string prefix = KTAG_EXCEPTIONPRFIX;

            for (int i = 0; i < nCount; i++)
            {
                pTmpNode = pNode->IterateChildren(pTmpNode);

                if (pTmpNode == NULL) break;

                const char *lpszElementName = pTmpNode->Value();

                std::string tag = prefix + Int2String(i + 1);
                if (_stricmp(lpszElementName, tag.c_str()) == 0)
                {
                    pTmpEle = (TiXmlElement *)pTmpNode;

                    ST_WGR_EXCEPTION excption;
                    excption.procName = pTmpEle->Attribute(KATTR_PROCNAME);
                    if (excption.procName != "")
                    {
                        excption.apppName = pTmpEle->Attribute(KATTR_APPNAME);

                        int nPermission = 0;
                        const char *pPermission = pTmpEle->Attribute(KATTR_PERMISSION, &nPermission);

                        if (pPermission)
                        {
                            excption.permissionSets = nPermission;                                    
                        }                                            

                        cfgData.permissionConfig.lstExceptions.push_back(excption);
                    }
                }
            }
        }                
    }   

    return TRUE;
}

BOOL SaveExceptionsConfig(TiXmlNode *pNode)
{
    return TRUE;
}

BOOL LoadPermissionSettingConfig(TiXmlNode *pNode, ST_WGR_CONFIGDATA &cfgData)
{
    if (pNode == NULL) return FALSE;

    TiXmlNode *pTmpNode = NULL;           

    do 
    {
        pTmpNode = pNode->IterateChildren(pTmpNode);

        if(NULL == pTmpNode )
        {
            break;
        }
        const char* lpszElementName = pTmpNode->Value();

        if(_stricmp(lpszElementName, KTAG_EXCEPTIONS) == 0)
        {
            LoadExceptionsConfig(pTmpNode, cfgData);
        }
        else if (_stricmp(lpszElementName, KTAG_GERNERAL) == 0)
        {                      
            TiXmlElement *pTmpEle = (TiXmlElement *)pTmpNode;

            int nPemssion = 0;
            const char *pRes = pTmpEle->Attribute(KATTR_PERMISSION, &nPemssion);            

            if (pRes != NULL)
            {
                cfgData.permissionConfig.generalPermissionSets = nPemssion;
            }            
        }

    } while (true);

    return TRUE;
}


//BOOL OpenWGRConfig(LPCTSTR lpszConfigFilePath, TiXmlDocument &oXMLDoc)
//{
//    if (oXMLDoc.LoadFile(CT2A(lpszConfigFilePath),TIXML_ENCODING_UTF8))
//    {//文件存在，直接打开
//        return TRUE;
//    }
//
//    TiXmlElement *element = new TiXmlElement(KTAG_WGRSETTING);
//    oXMLDoc.LinkEndChild(element);
//
//    TiXmlElement *elePermSetting = new TiXmlElement(KTAG_PERSETTING);
//    element->LinkEndChild(elePermSetting);
//
//    TiXmlElement *eleExceptions = new TiXmlElement(KTAG_EXCEPTIONS);
//    eleExceptions->SetAttribute(KATTR_EXCEPTIONS, 0);
//    
//    element->LinkEndChild(eleExceptions);
//
//    TiXmlElement *eleGeneral = new TiXmlElement(KTAG_GERNERAL);
//
//    element->SetAttribute(KATTR_PERMISSION, 0);
//
//    return TRUE;
//}

BOOL CWGRCofig::LoadConfig(LPCTSTR lpszConfigFilePath)
{
    TiXmlDocument oXMLDoc;
    if (!oXMLDoc.LoadFile(CT2A(lpszConfigFilePath)))
    {//配置文件不存在，则      
        return FALSE;
    }

    TiXmlElement *pRootElement = oXMLDoc.RootElement();
    if(pRootElement == NULL)
    {
        return FALSE;
    }

    TiXmlNode *pNode = NULL;

    //do
    //{        
    //    pNode = pRootElement->IterateChildren(pNode);
    //    if(NULL == pNode )
    //    {
    //        break;
    //    }
    //    const char* lpszElementName = pNode->Value();

    //    if(_stricmp(lpszElementName, KTAG_WGRSETTING) == 0)
    //    {  //载入全局设置
    //        break;
    //    }       

    //}while(pNode);

    //if (pNode == NULL) return FALSE; 

    do
    {
        pNode = pRootElement->IterateChildren(pNode);
        if(NULL == pNode )
        {
            break;
        }
        const char* lpszElementName = pNode->Value();

        if (_stricmp(lpszElementName, KTAG_PERSETTING) == 0)
        {
            LoadPermissionSettingConfig(pNode, m_wgr_CfgData);
        }        
        else if (_stricmp(lpszElementName, KTAG_LOGSETTING) == 0)
        {
            TiXmlElement *pTmpEle = (TiXmlElement *)pNode;

            int nPemssion = 0;
            const char *pRes = pTmpEle->Attribute(KATTR_LOGPERMISSION, &nPemssion);            

            if (pRes != NULL)
            {
                m_wgr_CfgData.isLogPermitted = (nPemssion != 0);
            }            
        }
        else if (_stricmp(lpszElementName, KTAG_MULTISETTING) == 0)
        {
            TiXmlElement *pTmpEle = (TiXmlElement *)pNode;

            int nMultiple = 0;
            const char *pRes = pTmpEle->Attribute(KATTR_MULTIPLE, &nMultiple);            

            if (pRes != NULL)
            {
                m_wgr_CfgData.multiplePalmtoStd = nMultiple > 3 ? nMultiple : 3;
            }            
        }
        else if (_stricmp(lpszElementName, KTAG_WEIGHTCONF) == 0)
        {
            TiXmlElement *pTmpEle = (TiXmlElement *)pNode;

            int nK0 = 1;
            const char *pRes0 = pTmpEle->Attribute(KATTR_k0, &nK0);

            int nK1 = 3;
            const char *pRes1 = pTmpEle->Attribute(KATTR_k1, &nK1); 

            if (pRes0 != NULL && pRes1 != NULL)
            {
                m_gr_WeightConf.k0 = nK0 > nK1 ? nK1 : nK0;
                m_gr_WeightConf.k1 = nK0 > nK1 ? nK0 : nK1;
            }            
        }
        else if (_stricmp(lpszElementName, KTAG_GLBGRPERMIT) == 0)
        {
            TiXmlElement *pTmpEle = (TiXmlElement *)pNode;

            int nPemssion = 0;
            const char *pRes = pTmpEle->Attribute(KATTR_GLBGRPERMIT, &nPemssion);            

            if (pRes != NULL)
            {
                m_glbgr_cfgData.isGLBoardGRPermit = (nPemssion != 0);
            }            
        }
        else if (_stricmp(lpszElementName, KTAG_ERASERMULTIPLE) == 0)
        {
            TiXmlElement *pTmpEle = (TiXmlElement *)pNode;

            int nMultiple = 0;
            const char *pRes = pTmpEle->Attribute(KATTR_ERASERMULTIPLE, &nMultiple);            

            if (pRes != NULL)
            {
                m_glbgr_cfgData.multipleEraserToStd = nMultiple > 3 ? nMultiple : 3;
            }            
        }
        //else if (_stricmp(lpszElementName, KTAG_PHYSICALSIZE) == 0)
        //{
        //    TiXmlElement *pTmpEle = (TiXmlElement *)pNode;

        //    int nPhysicalSize = 0;
        //    const char *pRes = pTmpEle->Attribute(KATTR_PHYSICALSIZE, &nPhysicalSize);            

        //    if (pRes != NULL)
        //    {
        //        //限制物理尺寸是45~125英寸之间，默认是85
        //        if (nPhysicalSize < 45)
        //        {
        //            nPhysicalSize = 45;
        //        }

        //        if (nPhysicalSize > 125)
        //        {
        //            nPhysicalSize = 125;
        //        }
        //        m_wgr_ActualParams.xScreenSize = m_wgr_ActualParams.yScreenSize = nPhysicalSize;
        //    }            
        //}
    }while(pNode);    

    return TRUE;
}

BOOL CWGRCofig::SaveConfig(LPCTSTR lpszConfigFilePath)
{
    TiXmlDocument oXMLDoc;
    TiXmlElement *element = new TiXmlElement(KTAG_WGRSETTING);
    oXMLDoc.LinkEndChild(element);

    TiXmlElement *elePermSetting = new TiXmlElement(KTAG_PERSETTING);
    element->LinkEndChild(elePermSetting);

    TiXmlElement *eleExceptions = new TiXmlElement(KTAG_EXCEPTIONS);    

    elePermSetting->LinkEndChild(eleExceptions);

    //写入例外进程列表
    std::list<ST_WGR_EXCEPTION> &lst = m_wgr_CfgData.permissionConfig.lstExceptions;
    std::list<ST_WGR_EXCEPTION>::iterator itr = lst.begin();

    int index = 0;
    for (; itr != lst.end(); itr++)
    {
        if ((*itr).procName != "")
        {
            index++;
            std::string tag = KTAG_EXCEPTIONPRFIX;
            tag += Int2String(index);

            TiXmlElement *eleException = new TiXmlElement(tag.c_str());
            eleException->SetAttribute(KATTR_PROCNAME, (*itr).procName.c_str());
            eleException->SetAttribute(KATTR_APPNAME, (*itr).apppName.c_str());
            eleException->SetAttribute(KATTR_PERMISSION, (*itr).permissionSets);

            eleExceptions->LinkEndChild(eleException);
        }
    }

    eleExceptions->SetAttribute(KATTR_EXCEPTIONS, index);

    TiXmlElement *eleGeneral = new TiXmlElement(KTAG_GERNERAL);
    eleGeneral->SetAttribute(KATTR_PERMISSION, m_wgr_CfgData.permissionConfig.generalPermissionSets);

    elePermSetting->LinkEndChild(eleGeneral);

    TiXmlElement *eleLogSetting = new TiXmlElement(KTAG_LOGSETTING);
    int logPermssion = (m_wgr_CfgData.isLogPermitted ? 1 : 0);
    eleLogSetting->SetAttribute(KATTR_LOGPERMISSION, logPermssion);

    element->LinkEndChild(eleLogSetting);

    TiXmlElement *eleMultipleSetting = new TiXmlElement(KTAG_MULTISETTING);    
    eleMultipleSetting->SetAttribute(KATTR_MULTIPLE, m_wgr_CfgData.multiplePalmtoStd);

    element->LinkEndChild(eleMultipleSetting);

    TiXmlElement *eleWEIGHTCONF = new TiXmlElement(KTAG_WEIGHTCONF);    
    eleWEIGHTCONF->SetAttribute(KATTR_k0, m_gr_WeightConf.k0);
    eleWEIGHTCONF->SetAttribute(KATTR_k1, m_gr_WeightConf.k1);

    element->LinkEndChild(eleWEIGHTCONF);

    TiXmlElement *eleGLBGRPermit = new TiXmlElement(KTAG_GLBGRPERMIT);
    int glbgrPermit = (m_glbgr_cfgData.isGLBoardGRPermit ? 1 : 0);
    eleGLBGRPermit->SetAttribute(KATTR_GLBGRPERMIT, glbgrPermit);

    element->LinkEndChild(eleGLBGRPermit);

    TiXmlElement *eleEraserMultiple = new TiXmlElement(KTAG_ERASERMULTIPLE);    
    eleEraserMultiple->SetAttribute(KATTR_ERASERMULTIPLE, m_glbgr_cfgData.multipleEraserToStd);

    element->LinkEndChild(eleEraserMultiple);

    /*TiXmlElement *elePhysicalSize = new TiXmlElement(KTAG_PHYSICALSIZE);    
    elePhysicalSize->SetAttribute(KATTR_PHYSICALSIZE, m_wgr_ActualParams.xScreenSize);

    element->LinkEndChild(elePhysicalSize);*/



    oXMLDoc.SaveFile(CT2A(lpszConfigFilePath));

    return TRUE;
}


BOOL CWGRCofig::Save()
{
	return SaveConfig(PROFILE::WGR_CONFIG_FILE_NAME);
}
//////////////////////////////////////////////////////////////////////////
DWORD CWGRCofig::GetPermission(const POINT &pt)
{   
    std::string procName = CProcessUtility::GetProcessNameFromPoint(pt);

    DWORD perm = GetExceptionPermission(procName);

    if (m_bIsTouchPad)
    {//触屏模式下关闭两触点手势
        perm &= ALLPERMITED_EXCEPT_2TOUCHS;
    }

    return perm;
}

void CWGRCofig::SetHIDMode(bool bIsTouchPad)
{
    //
    m_bIsTouchPad = bIsTouchPad;    
}

bool CWGRCofig::IsAllGestureClosed()
{
    DWORD perm = m_wgr_CfgData.permissionConfig.generalPermissionSets;
    if (m_bIsTouchPad)
    {//触屏模式下强制关闭两触点手势
        perm &= ALLPERMITED_EXCEPT_2TOUCHS;
    }

    return (perm == INIT_WITH_ALL_PROHIBITED);
}

bool CWGRCofig::IsAllPalmBasedGestureClosed()
{
    DWORD perm = m_wgr_CfgData.permissionConfig.generalPermissionSets;
    
    return ((perm & INIT_WITH_PALM_PERMITED) == 0);
}

bool CWGRCofig::IsAllFingerBasedGestureClosed()
{
    DWORD perm = m_wgr_CfgData.permissionConfig.generalPermissionSets;
    if (m_bIsTouchPad)
    {//触屏模式下强制关闭两触点手势
        perm &= ALLPERMITED_EXCEPT_2TOUCHS;
    }

    return ((perm & INIT_WITH_FINGER_PERMITED) == 0);
}

DWORD CWGRCofig::GetGeneralPermission()
{
    //DWORD generalPerm = 
    ////if (m_bIsTouchPad)
    ////{//触屏模式下关闭两触点手势
    ////    generalPerm &= ALLPERMITED_EXCEPT_2TOUCHS;
    ////}

    return m_wgr_CfgData.permissionConfig.generalPermissionSets;
}

void CWGRCofig::SetGeneralPermission(const DWORD &perm)
{
    m_wgr_CfgData.permissionConfig.generalPermissionSets = perm;
}

DWORD CWGRCofig::GetExceptionPermission(const std::string &procName)
{
    DWORD dwPermission = GetGeneralPermission();

    std::list<ST_WGR_EXCEPTION> &lst = m_wgr_CfgData.permissionConfig.lstExceptions;

    std::list<ST_WGR_EXCEPTION>::iterator itr = lst.begin();

    for (; itr != lst.end(); itr++)
    {
        if (_stricmp(procName.c_str(), ((*itr).procName).c_str()) == 0)
        {
            dwPermission = (*itr).permissionSets;
        }
    }

    return dwPermission;
}

void CWGRCofig::SetExcetionPermission(const DWORD &perm, const std::string &procName)
{
    DWORD dwPermission = m_wgr_CfgData.permissionConfig.generalPermissionSets;

    std::list<ST_WGR_EXCEPTION> &lst = m_wgr_CfgData.permissionConfig.lstExceptions;

    std::list<ST_WGR_EXCEPTION>::iterator itr = lst.begin();

    for (; itr != lst.end(); itr++)
    {
        if (_stricmp(procName.c_str(), ((*itr).procName).c_str()) == 0)
        {
            (*itr).permissionSets = perm;

            break;
        }
    }

}

const std::list<ST_WGR_EXCEPTION> & CWGRCofig::GetExceptionalProcessList() const
{
    return m_wgr_CfgData.permissionConfig.lstExceptions;
}

bool CWGRCofig::IsTheProcessHasAdded(const std::string &procName)
{
    std::list<ST_WGR_EXCEPTION> &lst = m_wgr_CfgData.permissionConfig.lstExceptions;

    std::list<ST_WGR_EXCEPTION>::iterator itr = lst.begin();

    for (; itr != lst.end(); itr++)
    {
        if (_stricmp(procName.c_str(), ((*itr).procName).c_str()) == 0)
        {            
            return true;
        }
    }

    return false;
}

bool CWGRCofig::AddExceptionalProcess(const std::string &procName, const std::string &appName, const DWORD &perm)
{
    DWORD dwPermission = m_wgr_CfgData.permissionConfig.generalPermissionSets;

    std::list<ST_WGR_EXCEPTION> &lst = m_wgr_CfgData.permissionConfig.lstExceptions;

    std::list<ST_WGR_EXCEPTION>::iterator itr = lst.begin();

    for (; itr != lst.end(); itr++)
    {
        if (_stricmp(procName.c_str(), ((*itr).procName).c_str()) == 0)
        {
            (*itr).permissionSets = perm;

            return false;
        }
    }

    ST_WGR_EXCEPTION stException;
    stException.procName = procName;
    stException.apppName = appName;
    stException.permissionSets = perm;

    lst.push_back(stException);

    return true;
}

void CWGRCofig::DeleteExceptionalProcess(const std::string &procName)
{
    DWORD dwPermission = m_wgr_CfgData.permissionConfig.generalPermissionSets;

    std::list<ST_WGR_EXCEPTION> &lst = m_wgr_CfgData.permissionConfig.lstExceptions;

    std::list<ST_WGR_EXCEPTION>::iterator itr = lst.begin();

    for (; itr != lst.end(); itr++)
    {
        if (_stricmp(procName.c_str(), ((*itr).procName).c_str()) == 0)
        {
            lst.erase(itr);

            break;
        }
    }
}

int CWGRCofig::GetCurrentProcessInfo(char (*ppProcNameArry)[256], const int &nMaxArrySize)
{
    return CProcessUtility::GetCurrentProcessInfo(ppProcNameArry, nMaxArrySize);
}

//////////////////////////////////////////////////////////////////////////
//void CWGRCofig::UpdateScreenResolutionParams(int nScreenWidth, int nScreenHeight)
//{
//    m_wgr_ActualParams.xScreenResolution = nScreenWidth;
//    m_wgr_ActualParams.yScreenResolution = nScreenHeight;    
//}

int CWGRCofig::MapStandardX2ActualValue(int xStandard)
{   
    int nDenom = int(m_wgr_ActualParams.fScreenDiagonalPhysicalLength * KSTANDAR_XSCRNRESOLUTION + 0.5);

    return (int(xStandard * m_wgr_ActualParams.xScreenResolution * KSTANDAR_SCRNDLGPHYSICALLENGTH + 0.5) + nDenom - 1) / nDenom;         
}

int CWGRCofig::MapStandardY2ActualValue(int yStandard)
{
    int nDenom = int(m_wgr_ActualParams.fScreenDiagonalPhysicalLength * KSTANDAR_XSCRNRESOLUTION + 0.5);

    return (int(yStandard * m_wgr_ActualParams.xScreenResolution * KSTANDAR_SCRNDLGPHYSICALLENGTH + 0.5) + nDenom - 1) / nDenom;              
}

int CWGRCofig::GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti)
{
    if (nActualMass < nStdAreaMulti && nExtRectArea > nStdAreaMulti)
    {
        const int &k0 = m_gr_WeightConf.k0;
        const int &k1 = m_gr_WeightConf.k1;

        int k0_m = k0 * nActualMass;
        int k0_m_m = k0_m * nActualMass;
        int k1_m = k1 * nActualMass;
        int k1_r = k1 * nExtRectArea;

        return (k0_m + k1_m + k1 - 1) / k1 - (k0_m_m + k1_r - 1) / k1_r;
    }

    return nActualMass;
}

void CWGRCofig::SetDoubleScreen(bool isDoubleFlag)
{
    m_wgr_ActualParams.xScreenResolution = GetSystemMetrics(SM_CXSCREEN);
    m_wgr_ActualParams.yScreenResolution = GetSystemMetrics(SM_CYSCREEN);

    /*if (isDoubleFlag)
    {
        m_wgr_ActualParams.xScreenResolution = (m_wgr_ActualParams.xScreenResolution + 1) << 1;
        m_wgr_ActualParams.yScreenResolution = (m_wgr_ActualParams.yScreenResolution + 1) << 1;
    }*/
}