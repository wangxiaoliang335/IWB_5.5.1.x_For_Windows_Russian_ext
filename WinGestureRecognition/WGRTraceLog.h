#pragma once

#include "TinyXML/tinyxml.h"
#include "WGestureRecPublic.h"
#include <Windows.h>
#include <string>
#include <map>

//CWGRTraceLogXML
class CWGRTraceLogXML
{
public:
    CWGRTraceLogXML();
    ~CWGRTraceLogXML();

public:
    void StartLog();
    void StopLog(const std::string &xmlFileName);

    void AddCollectTag(int curCollect, int curState);
    void AddSpotCount(int nSpotCount);
    void AddSpotPos(const POINT *ptArray, int nCount);
    void AddGestureShape(EWGR_GESTURE_SHAPE gShape);
    void AddCurMissShapeTolerance(int nCMissShapTol);
    void AddCurMissTouchCountTolerance(int nCMissTCountTol);
    void AddCurMissDisLimitTolerance(int nCMissDisLimitTol);
    void AddCurMissNonMoveTolerance(int nCMissNonMoveTol);
    void AddCurUnexpectedKnownDirTolerance(int nTol);
    void AddCurUnexpectedUnknownDirTolerance(int nTol);
    //void AddCurUnexpectedUnknownDirTolerance(int nCMissDirTol);
    void AddEnterLess(bool bEnterLess);
    void AddMoveDirection(E_WGR_MOVEDIR dir);
    void AddSateToCovert(int stateCoverted);
    void AddLastDisplacement(int lstXDisplacement, int lstYDisplacement);
    void AddCurrentDisplacement(int curXDisplacement, int curYDisplacement);
    void AddStartMiddlePoint(const POINT &pt);
    void AddLastMiddlePoint(const POINT &pt);
    void AddCurrentMiddlePoint(const POINT &pt);
    void AddSpotArea(int nCMoments);
    void AddStdArea(int nCMoments);
    void AddTimesCMIncreased(int nTimes);
    void AddTimesCMDecreased(int nTimes);
    void AddTotCollects(int nTotCollects);

    void AddLastPoint(const POINT *ptArray, int nCount);
    void AddStartPoint(const POINT *ptArray, int nCount);
    void AddXShift(int xShift);
    void AddYShift(int yShift);

    void AddLastSpotMap(const std::map<UINT, ST_MOTIONINFO> &refMap);
    void AddSpotInfo(const int &type, const ST_TouchInfo *spotInfo, const int &nSize);
    void AddPTMiddle(const POINT &ptMiddle);
    /*void AddDirVoteStatis(const byte *voteStatis, const int &nSize);
    void AddDispVoteStatis(const short *voteStatis, const int &nSize);
    void AddPTVoteStatis(const POINT *voteStatis, const int &nSize);*/
    void AddElectee(const E_WGR_MOVEDIR &dir, const int &votes);

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
    TiXmlElement    *m_pEle_FT2F_LISTEN_STATE;
    TiXmlElement    *m_pEle_FMINICURWND_LISTEN_STATE;
    TiXmlElement    *m_pEle_FMINICURWND_KEEP_STATE;
    TiXmlElement    *m_pEle_FALTLARROW_LISTEN_STATE;
    TiXmlElement    *m_pEle_FALTLARROW_KEEP_STATE;
    TiXmlElement    *m_pEle_FALTRARROW_LISTEN_STATE;
    TiXmlElement    *m_pEle_FALTRARROW_KEEP_STATE;
    TiXmlElement    *m_pEle_FDSPDESKTOP_LISTEN_STATE;
    TiXmlElement    *m_pEle_FDSPDESKTOP_KEEP_STATE;
    TiXmlElement    *m_pEle_FMAXICURWND_LISTEN_STATE;
    TiXmlElement    *m_pEle_FMAXICURWND_KEEP_STATE;
    TiXmlElement    *m_pEle_FTWOTOUCH_LISTEN_STATE;
    TiXmlElement    *m_pEle_FVMOUSEWHEEL_LISTEN_STATE;
    TiXmlElement    *m_pEle_FVMOUSEWHEEL_KEEP_STATE;
    TiXmlElement    *m_pEle_FHMOUSEWHEEL_LISTEN_STATE;
    TiXmlElement    *m_pEle_FHMOUSEWHEEL_KEEP_STATE;

    TiXmlElement    *m_pEle_CurCollectTag; //用于记录采集数据的元素，不要与m_pEle_CurCollects混淆   

    //
    TiXmlElement    *m_pEle_PLOWLIM_LISTEN_STATE;
    TiXmlElement    *m_pEle_PSHOWWNDSBS_LISTEN_STATE;
    TiXmlElement    *m_pEle_PSHOWWNDSBS_KEEP_STATE;
    TiXmlElement    *m_pEle_PSHOWWNDSBS_EXE_STATE;

    TiXmlElement    *m_pEle_PNONSPECLOC_LISTEN_STATE;
    
    TiXmlElement    *m_pEle_PMOVWND_LISTEN_STATE;
    TiXmlElement    *m_pEle_PMOVWND_KEEP_STATE;
    TiXmlElement    *m_pEle_PMOVWND_EXE_STATE;
};

//CWGRTraceLog
class CWGRTraceLog
{
public:
    CWGRTraceLog(void);
    ~CWGRTraceLog(void);

public:
    void StartLog();
    void StopLog();

    bool IsLogHasStarted() {return m_bHasStarted;}

    void AddCollectTag(int curCollect, int curState);
    void AddSpotCount(int nSpotCount);
    void AddSpotPos(const POINT *ptArray, int nCount);
    void AddGestureShape(EWGR_GESTURE_SHAPE gShape);
    void AddCurMissShapeTolerance(int nCMissShapTol);
    void AddCurMissTouchCountTolerance(int nCMissTCountTol);
    void AddCurMissDisLimitTolerance(int nCMissDisLimitTol);
    void AddCurMissNonMoveTolerance(int nCMissNonMoveTol);
    void AddCurUnexpectedKnownDirTolerance(int nTol);
    void AddCurUnexpectedUnknownDirTolerance(int nTol);
    void AddEnterLess(bool bEnterLess);
    void AddMoveDirection(E_WGR_MOVEDIR dir);
    void AddSateToCovert(int stateCoverted);
    void AddLastDisplacement(int lstXDisplacement, int lstYDisplacement);
    void AddCurrentDisplacement(int curXDisplacement, int curYDisplacement);
    void AddStartMiddlePoint(const POINT &pt);
    void AddLastMiddlePoint(const POINT &pt);
    void AddCurrentMiddlePoint(const POINT &pt);
    void AddSpotArea(int nCMoments);
    void AddStdArea(int nCMoments);
    void AddTimesCMIncreased(int nTimes);
    void AddTimesCMDecreased(int nTimes);
    void AddTotCollects(int nTotCollects);

    void AddLastPoint(const POINT *ptArray, int nCount);
    void AddStartPoint(const POINT *ptArray, int nCount);
    void AddXShift(int xShift);
    void AddYShift(int yShift);

    void AddLastSpotMap(const std::map<UINT, ST_MOTIONINFO> &refMap);
    void AddSpotInfo(const int &type, const ST_TouchInfo *spotInfo, const int &nSize);
    void AddPTMiddle(const POINT &ptMiddle);
    void AddElectee(const E_WGR_MOVEDIR &dir, const int &votes);

private:
    CWGRTraceLogXML     m_LogXml;
    std::string         m_logName;

    bool                m_bHasStarted;
};