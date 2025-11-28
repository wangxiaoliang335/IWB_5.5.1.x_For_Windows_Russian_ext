#include "StdAfx.h"
#include "SpotsLog.h"

//////////////////////////////////////////////////////////////////////////
const char *KTAG_COLLECT_PREFIX             = "COLLECT_";
const char *KTAG_SPOT_PREFIX                = "S_";
const char *KTAG_POS                        = "POS";
const char *KTAG_MASS                       = "MASS";
const char *KTAG_MULTI                      = "MULTI";
const char *KTAG_STDAREA                    = "STDAREA";
const char *KTAG_SHAPE                      = "SHAPE";
//////////////////////////////////////////////////////////////////////////
CSpotsLogXml::CSpotsLogXml()
{
    m_pXmlDoc = NULL;
    m_pEle_CurCollectTag = NULL;
}

CSpotsLogXml::~CSpotsLogXml()
{
    if (m_pXmlDoc)
    {
        StopLog("UnKnown.xml");
        delete m_pXmlDoc;
        m_pXmlDoc = NULL;
    }
}

void CSpotsLogXml::StartLog()
{
    StopLog("UnKnown.xml");

    m_pXmlDoc = new TiXmlDocument();
    TiXmlElement * element = new TiXmlElement("SPOTS");  
    m_pXmlDoc->LinkEndChild(element);
}

void CSpotsLogXml::StopLog(const std::string &xmlFileName)
{
    if (m_pXmlDoc)
    {
        m_pXmlDoc->SaveFile(xmlFileName.c_str());
        delete m_pXmlDoc;
        m_pXmlDoc = NULL;
    }

    m_pEle_CurCollectTag = NULL;
}

void CSpotsLogXml::AddSpotsInfo(int nCollect, const TLightSpot *arrSpots, int nCount, int nMulti)
{
    if (arrSpots == NULL || nCount == 0)
    {
        return ;
    }

    if (!m_pXmlDoc) return ;

    //////////////////////////////////////////////////////////////////////////
    std::string tagName = KTAG_COLLECT_PREFIX;    
    tagName += Int2String(nCollect);
    m_pEle_CurCollectTag = new TiXmlElement(tagName.c_str());
    m_pXmlDoc->LinkEndChild(m_pEle_CurCollectTag);

    //////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < nCount; i++)
    {
        tagName = KTAG_SPOT_PREFIX;
        tagName += Int2String(i+1);
        TiXmlElement *pSi = new TiXmlElement(tagName.c_str());

        std::string strValue = "Unknown";
        int MultiArea = arrSpots[i].lStdSpotAreaInVideo * nMulti;
        if (arrSpots[i].mass > MultiArea)
        {
            strValue = "PalmShape";
        }
        else
        {
            strValue = "FingerShape";
        }

        AddElementUnderTheTag(pSi, KTAG_SHAPE, strValue);

        strValue = PointArrayToString(&(arrSpots[i].ptPosInScreen), 1);
        AddElementUnderTheTag(pSi, KTAG_POS, strValue);

        AddElementUnderTheTag(pSi, KTAG_MASS, Int2String(arrSpots[i].mass));

        AddElementUnderTheTag(pSi, KTAG_MULTI, Int2String(MultiArea));

        AddElementUnderTheTag(pSi, KTAG_STDAREA, Int2String(arrSpots[i].lStdSpotAreaInVideo));       

        m_pEle_CurCollectTag->LinkEndChild(pSi);
    }
}

std::string CSpotsLogXml::PointArrayToString(const POINT *ptArray, int nCount)
{
    std::string strPoints = "[";
    if (!ptArray || nCount <= 0)
    {
        strPoints += "No Points";
        strPoints += "]";

        return strPoints;
    }

    for (int i = 0; i < nCount; i++)
    {
        if (i != 0) strPoints += "; ";
        strPoints += "(";
        strPoints += Int2String(ptArray[i].x);
        strPoints += ", ";
        strPoints += Int2String(ptArray[i].y);
        strPoints += ")";
    }

    strPoints += "]";

    return strPoints;
}
//////////////////////////////////////////////////////////////////////////
CSpotsLog::CSpotsLog(void)
{
    m_bHasStarted = NULL;
}

CSpotsLog::~CSpotsLog(void)
{
}

void CSpotsLog::StartLog()
{
    //#ifdef _DEBUG
    if (!m_bHasStarted)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        m_logName = "";
        m_logName += "_SpotsAnalysisLogDir\\";

        char szTime[64];
        _snprintf_s(szTime,sizeof(szTime),
            "%04d%02d%02d_%02d%02d_%02d_%03d",
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);

        m_logName += szTime;
        m_logName += ".xml";

        m_bHasStarted = true;

        m_LogXml.StartLog();
    }        
    //#else
    /*m_bHasStarted = false;
    return ;*/
    //#endif    
}

void CSpotsLog::StopLog()
{
    if (!m_bHasStarted) return;
    m_LogXml.StopLog(m_logName);

    m_bHasStarted = false;
}

void CSpotsLog::AddSpotsInfo(int nCollect, const TLightSpot *arrSpots, int nCount, int nMulti)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotsInfo(nCollect, arrSpots, nCount, nMulti);
}
