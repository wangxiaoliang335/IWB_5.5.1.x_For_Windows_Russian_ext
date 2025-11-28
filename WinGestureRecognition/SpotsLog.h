#pragma once
#include "TinyXML/tinyxml.h"
#include "../inc/LightSpot.h"
#include <Windows.h>
#include <string>

class CSpotsLogXml
{
public:
    CSpotsLogXml();
    ~CSpotsLogXml();

public:
    void StartLog();
    void StopLog(const std::string &xmlFileName);
    void AddSpotsInfo(int nCollect, const TLightSpot *arrSpots, int nCount, int nMulti);

private:
    /*
    @功能：把点数组转化为如下格式的string
    [(x0, y0)，（x1, y1), ..., (xn, yn)]
    */
    std::string PointArrayToString(const POINT *ptArray, int nCount);   

    /*
    */
    void AddElementUnderTheTag(TiXmlElement *theTag, const char *eleTag, const std::string &eleValue)
    {
        if (theTag == NULL)
        {
            return;
        }

        TiXmlElement *ele = new TiXmlElement(eleTag);
        TiXmlText *text = new TiXmlText(eleValue.c_str());

        ele->LinkEndChild(text);
        theTag->LinkEndChild(ele);
    }


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

private:
    TiXmlDocument   *m_pXmlDoc;
    TiXmlElement    *m_pEle_CurCollectTag;
};

class CSpotsLog
{
public:
    CSpotsLog(void);
    ~CSpotsLog(void);

public:
    void StartLog();
    void StopLog();

    bool IsLogHasStarted() {return m_bHasStarted;}

    void AddSpotsInfo(int nCollect, const TLightSpot *arrSpots, int nCount, int nMulti);

private:
    CSpotsLogXml        m_LogXml;
    std::string         m_logName;

    bool                m_bHasStarted;
};
