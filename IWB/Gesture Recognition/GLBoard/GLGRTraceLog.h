#pragma once
#include "../../TinyXML/tinyxml.h"
#include "GLBoardPublic.h"
#include <Windows.h>
#include <string>
#include <map>

typedef enum 
{
    EMT_Unknown,
    EMT_Pre,
    EMT_After
}EMATCHTIME;

//CGLGRTraceLogXML
class CGLGRTraceLogXML
{
public:
    CGLGRTraceLogXML();
    ~CGLGRTraceLogXML();

public:
    void StartLog();
    void StopLog(const std::string &xmlFileName);

    void AddCollectTag(int curCollect, E_WBS curState);
    void AddSpotCount(int nSpotCount);
    void AddSpotPos(const POINT *ptArray, int nCount);
    void AddGestureShape(EGLGR_GESTURE_SHAPE gShape);    
    void AddMoveDirection(E_2TOUCHS_MOVES dir);
    void AddSateToCovert(E_WBS stateCoverted);
    void AddLastDisplacement(int lstXDisplacement, int lstYDisplacement);
    void AddCurrentDisplacement(int curXDisplacement, int curYDisplacement);
    void AddStartMiddlePoint(const POINT &pt);
    void AddLastMiddlePoint(const POINT &pt);
    void AddSpotArea(const int &nActualArea);
    void AddStdArea(const int &nStdAra);

    void AddXShift(int xShift);
    void AddYShift(int yShift);

    //void AddLastSpotMap(const std::map<UINT, ST_MOTIONINFO> &refMap);
    void AddSpotInfo(const GLMatchInfo *matchInfo, const int &nSize);
    //void AddPTMiddle(const POINT &ptMiddle);
    ///*void AddDirVoteStatis(const byte *voteStatis, const int &nSize);
    //void AddDispVoteStatis(const short *voteStatis, const int &nSize);
    //void AddPTVoteStatis(const POINT *voteStatis, const int &nSize);*/
    //void AddElectee(const E_WGR_MOVEDIR &dir, const int &votes);

    void AddCollectTag(int curCollect);

    void AddRawSpotInfo(const TLightSpot *aryLightSpots,  const int &nCount);
    void AddAlterErasrPos(const int *posAlterEraserArry, const int &nPosEraserCount, const TLightSpot *aryLightSpots,  const int &nCount);
    void AddSplitEraserInfo(const POINT *ptArray, int nCount);
    void AddSplitPenInfo(const POINT *ptArray, int nCount);

private:
    /*
    @功能：把点数组转化为如下格式的string
    [(x0, y0)，（x1, y1), ..., (xn, yn)]
    */
    std::string PointArrayToString(const POINT *ptArray, int nCount);

    /*
    */
    void AddElementUnderCollectTag(const char *eleTag, const std::string &eleValue)
    {
        TiXmlElement *ele = new TiXmlElement(eleTag);
        TiXmlText *text = new TiXmlText(eleValue.c_str());

        ele->LinkEndChild(text);
        m_pEle_CurCollectTag->LinkEndChild(ele);
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
    TiXmlElement    *m_pEle_CurCollects;   //用于记录采集次数的元素
    
    TiXmlElement    *m_pEle_EWBS_Eraser;  
    TiXmlElement    *m_pEle_EWBS_Spotlight;    
    TiXmlElement    *m_pEle_EWBS_Mask;  
    TiXmlElement    *m_pEle_EWBS_RETURNTODISKTOP;    

    
    TiXmlElement    *m_pEle_EWBS_Pen;    
    TiXmlElement    *m_pEle_EWBS_Selection;    
    TiXmlElement    *m_pEle_EWBS_Roate;    
    TiXmlElement    *m_pEle_EWBS_Roam;        

    TiXmlElement    *m_pEle_MultiPen;      

    TiXmlElement    *m_pEle_CurCollectTag; //用于记录采集数据的元素，不要与m_pEle_CurCollects混淆       
};

class CGLGRTraceLog
{
public:
    CGLGRTraceLog(void);
    ~CGLGRTraceLog(void);

public:
    void StartLog();
    void StopLog();

    void AddCollectTag(int curCollect, E_WBS curState);
    void AddSpotCount(int nSpotCount);
    void AddSpotPos(const POINT *ptArray, int nCount);
    void AddGestureShape(EGLGR_GESTURE_SHAPE gShape);    
    void AddMoveDirection(E_2TOUCHS_MOVES dir);
    void AddSateToCovert(E_WBS stateCoverted);
    void AddLastDisplacement(int lstXDisplacement, int lstYDisplacement);
    void AddCurrentDisplacement(int curXDisplacement, int curYDisplacement);
    void AddStartMiddlePoint(const POINT &pt);
    void AddLastMiddlePoint(const POINT &pt);

    void AddXShift(int xShift);
    void AddYShift(int yShift);

    //void AddLastSpotMap(const std::map<UINT, ST_MOTIONINFO> &refMap);
    void AddSpotInfo(const GLMatchInfo *matchInfo, const int &nSize);

    void AddSpotArea(const int &nActualArea);
    void AddStdArea(const int &nStdAra);

private:
    CGLGRTraceLogXML    m_LogXml;
    std::string         m_logName;

    bool                m_bHasStarted;

};
