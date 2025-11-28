#include "StdAfx.h"
#include "WGRTraceLog.h"

//////////////////////////////////////////////////////////////////////////
//常量定义
const char *KTAG_FINGERSHAPE                = "FINGERSHAPE";
const char *KTAG_FTHREE2FIVETOUCH           = "FTHREE2FIVETOUCH";
const char *KTAG_FT2F_LISTEN_STATE          = "FTHREE2FIVETOUCH_LISTEN_STATE";
const char *KTAG_FMINICURWND_LISTEN_STATE   = "FMINICURWND_LISTEN_STATE";
const char *KTAG_FMINICURWND_KEEP_STATE     = "FMINICURWND_KEEP_STATE";
const char *KTAG_FALTLARROW_LISTEN_STATE    = "FALTLARROW_LISTEN_STATE";
const char *KTAG_FALTLARROW_KEEP_STATE      = "FALTLARROW_KEEP_STATE";
const char *KTAG_FALTRARROW_LISTEN_STATE    = "FALTRARROW_LISTEN_STATE";
const char *KTAG_FALTRARROW_KEEP_STATE      = "FALTRARROW_KEEP_STATE";
const char *KTAG_FDSPDESKTOP_LISTEN_STATE   = "FDSPDESKTOP_LISTEN_STATE";
const char *KTAG_FDSPDESKTOP_KEEP_STATE     = "FDSPDESKTOP_KEEP_STATE";
const char *KTAG_FMAXICURWND_LISTEN_STATE   = "FMAXICURWND_LISTEN_STATE";
const char *KTAG_FMAXICURWND_KEEP_STATE     = "FMAXICURWND_KEEP_STATE";

const char *KTAG_FTWOTOUCH                  = "FTWOTOUCH";
const char *KTAG_FTWOTOUCH_LISTEN_STATE     = "FTWOTOUCH_LISTEN_STATE";
const char *KTAG_FVMOUSEWHEEL_LISTEN_STATE  = "FVMOUSEWHEEL_LISTEN_STATE";
const char *KTAG_FVMOUSEWHEEL_KEEP_STATE    = "FVMOUSEWHEEL_KEEP_STATE";
const char *KTAG_FHMOUSEWHEEL_LISTEN_STATE  = "FHMOUSEWHEEL_LISTEN_STATE";
const char *KTAG_FHMOUSEWHEEL_KEEP_STATE    = "FHMOUSEWHEEL_KEEP_STATE";

const char *KTAG_COLLECT                    = "COLLECT_";

const char *KTAG_SpotCount                  = "SpotCount";
const char *KTAG_SpotPos                    = "SpotPos";
const char *KTAG_GestureShape               = "GestureShape";
const char *KTAG_CMissShapeTol              = "CMissShapeTol";
const char *KTAG_CMissTCountTol             = "CMissTCountTol";
const char *KTAG_CMissDisLimitTol           = "CMissDisLimitTol";
const char *KTAG_CMissNonMoveTol            = "CMissNonMoveTol";
const char *KTAG_CUnexpectedKnownDirTol     = "CUnexpectedKnownDirTol";
const char *KTAG_CUnexpectedUnknownDirTol   = "CUnexpectedUnknownDirTol";
const char *KTAG_MDIR                       = "MDIR";
const char *KTAG_SateCovert                 = "SateCovert";
const char *KTAG_EnterLess                  = "EnterLess";

const char *KTAG_LDisplacement              = "LDisplacement";
const char *KTAG_CDisplacement              = "CDisplacement";
const char *KTAG_ptSartMid                  = "ptSartMid";
const char *KTAG_ptLastMid                  = "ptLastMid";
const char *KTAG_ptCurMid                   = "ptCurMid";
const char *KTAG_SpotArea                   = "SpotArea";                 
const char *KTAG_StdArea                    = "StdArea";                //以前有些手势要根据中心距计算，后来取消了；现在虽然Tag还是中心距，实质上是手掌标准大小
const char *KTAG_TimesCMIn                  = "TimesCMIn";
const char *KTAG_TimesCMDe                  = "TimesCMDe";
const char *KTAG_TotCollects                = "TotCollects";

const char *KTAG_ptStart                    = "ptStart";
const char *KTAG_ptCur                      = "ptCur";
const char *KTAG_ptLast                     = "ptLast";
const char *KTAG_xShift                     = "xShift";
const char *KTAG_yShift                     = "yShift";

//
const char *KTAG_PALMSHAPE                  = "PALMSHAPE";
const char *KTAG_PLOWLIM                    = "PLOWLIM";
const char *KTAG_PNONSPECLOC                = "PNONSPECLOC";

const char *KTAG_PLOWLIM_LISTEN_STATE       = "PLOWLIM_LISTEN_STATE";
const char *KTAG_PSHOWWNDSBS_LISTEN_STATE   = "PSHOWWNDSBS_LISTEN_STATE";
const char *KTAG_PSHOWWNDSBS_KEEP_STATE     = "PSHOWWNDSBS_LISTEN_STATE";
const char *KTAG_PSHOWWNDSBS_EXE_STATE      = "PSHOWWNDSBS_EXE_STATE";

const char *KTAG_PNONSPECLOC_LISTEN_STATE   = "PNONSPECLOC_LISTEN_STATE";

const char *KTAG_PMOVWND_LISTEN_STATE       = "PMOVWND_LISTEN_STATE";
const char *KTAG_PMOVWND_KEEP_STATE         = "KTAG_PMOVWND_KEEP_STATE";
const char *KTAG_PMOVWND_EXE_STATE          = "KTAG_PMOVWND_EXE_STATE";

//
const char *KTAG_TINFOLASTMAP               = "TInfoLastMap";
const char *KTAG_MATCHEDSPOTTINFO           = "MatchedSpotInfo";
const char *KTAG_MISMATCHEDSPOTINFO         = "MisMatchedSpotInfo";
const char *KTAG_NEWADDEDSPOTINFO           = "NewAddedSpotInfo";
const char *KTAG_LASTDISP                   = "LastDisp";
const char *KTAG_CURDISP                    = "CurDisp";
const char *KTAG_LASTPTMIDDLE               = "LastPTMiddle";
const char *KTAG_CURPTMIDDLE                = "CurPTMiddle";
const char *KTAG_DIRVOTESTATIS              = "DirVoteStatis";
const char *KTAG_DISPVOTESTATIS             = "DispVoteStatis";
const char *KTAG_PTVOTESTATIS               = "PTVoteStatis";
const char *KTAG_ELECTEE                    = "Electee";

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CWGRTraceLogXML

CWGRTraceLogXML::CWGRTraceLogXML()
{
    m_pXmlDoc = NULL;
    m_pEle_FT2F_LISTEN_STATE = NULL;
    m_pEle_CurCollects = NULL;   //用于记录采集次数的元素
    m_pEle_FT2F_LISTEN_STATE = NULL;
    m_pEle_FMINICURWND_LISTEN_STATE = NULL;
    m_pEle_FMINICURWND_KEEP_STATE = NULL;
    m_pEle_FALTLARROW_LISTEN_STATE = NULL;
    m_pEle_FALTLARROW_KEEP_STATE = NULL;
    m_pEle_FALTRARROW_LISTEN_STATE = NULL;
    m_pEle_FALTRARROW_KEEP_STATE = NULL;
    m_pEle_FDSPDESKTOP_LISTEN_STATE = NULL;
    m_pEle_FDSPDESKTOP_KEEP_STATE = NULL;
    m_pEle_FMAXICURWND_LISTEN_STATE = NULL;
    m_pEle_FMAXICURWND_KEEP_STATE = NULL;
    m_pEle_FTWOTOUCH_LISTEN_STATE = NULL;
    m_pEle_FVMOUSEWHEEL_LISTEN_STATE = NULL;
    m_pEle_FVMOUSEWHEEL_KEEP_STATE = NULL;
    m_pEle_FHMOUSEWHEEL_LISTEN_STATE = NULL;
    m_pEle_FHMOUSEWHEEL_KEEP_STATE = NULL;

    m_pEle_CurCollectTag = NULL; //用于记录采集数据的元素，不要与m_pEle_CurCollects混淆

    m_pEle_PLOWLIM_LISTEN_STATE = NULL;
    m_pEle_PSHOWWNDSBS_LISTEN_STATE = NULL;
    m_pEle_PSHOWWNDSBS_KEEP_STATE = NULL;
    m_pEle_PSHOWWNDSBS_EXE_STATE = NULL;

    m_pEle_PNONSPECLOC_LISTEN_STATE = NULL;
}

CWGRTraceLogXML::~CWGRTraceLogXML()
{
    if (m_pXmlDoc)
    {
        StopLog("UnKnown.xml");
    }
}

void CWGRTraceLogXML::StartLog()
{
    StopLog("UnKnown.xml");

    m_pXmlDoc = new TiXmlDocument();
    TiXmlElement * element = new TiXmlElement("WGR");  
    m_pXmlDoc->LinkEndChild(element);

    m_pEle_CurCollects = new TiXmlElement("CURRENTCOLLET");
    element->LinkEndChild(m_pEle_CurCollects);

    TiXmlElement *eleFinger = new TiXmlElement(KTAG_FINGERSHAPE);
    TiXmlElement *elePalm   = new TiXmlElement(KTAG_PALMSHAPE);

    element->LinkEndChild(eleFinger);
    element->LinkEndChild(elePalm);

    TiXmlElement *eleFT2F = new TiXmlElement(KTAG_FTHREE2FIVETOUCH);
    eleFinger->LinkEndChild(eleFT2F);

    m_pEle_FT2F_LISTEN_STATE = new TiXmlElement(KTAG_FT2F_LISTEN_STATE);
    eleFT2F->LinkEndChild(m_pEle_FT2F_LISTEN_STATE);

    m_pEle_FMINICURWND_LISTEN_STATE = new TiXmlElement(KTAG_FMINICURWND_LISTEN_STATE);
    eleFT2F->LinkEndChild(m_pEle_FMINICURWND_LISTEN_STATE);

    m_pEle_FMINICURWND_KEEP_STATE = new TiXmlElement(KTAG_FMINICURWND_KEEP_STATE);
    eleFT2F->LinkEndChild(m_pEle_FMINICURWND_KEEP_STATE);

    m_pEle_FALTLARROW_LISTEN_STATE = new TiXmlElement(KTAG_FALTLARROW_LISTEN_STATE);
    eleFT2F->LinkEndChild(m_pEle_FALTLARROW_LISTEN_STATE);

    m_pEle_FALTLARROW_KEEP_STATE = new TiXmlElement(KTAG_FALTLARROW_KEEP_STATE);
    eleFT2F->LinkEndChild(m_pEle_FALTLARROW_KEEP_STATE);

    m_pEle_FALTRARROW_LISTEN_STATE = new TiXmlElement(KTAG_FALTRARROW_LISTEN_STATE);
    eleFT2F->LinkEndChild(m_pEle_FALTRARROW_LISTEN_STATE);

    m_pEle_FALTRARROW_KEEP_STATE = new TiXmlElement(KTAG_FALTRARROW_KEEP_STATE);
    eleFT2F->LinkEndChild(m_pEle_FALTRARROW_KEEP_STATE);

    m_pEle_FDSPDESKTOP_LISTEN_STATE = new TiXmlElement(KTAG_FDSPDESKTOP_LISTEN_STATE);
    eleFT2F->LinkEndChild(m_pEle_FDSPDESKTOP_LISTEN_STATE);

    m_pEle_FDSPDESKTOP_KEEP_STATE = new TiXmlElement(KTAG_FDSPDESKTOP_KEEP_STATE);
    eleFT2F->LinkEndChild(m_pEle_FDSPDESKTOP_KEEP_STATE);

    m_pEle_FMAXICURWND_LISTEN_STATE = new TiXmlElement(KTAG_FMAXICURWND_LISTEN_STATE);
    eleFT2F->LinkEndChild(m_pEle_FMAXICURWND_LISTEN_STATE);

    m_pEle_FMAXICURWND_KEEP_STATE = new TiXmlElement(KTAG_FMAXICURWND_KEEP_STATE);
    eleFT2F->LinkEndChild(m_pEle_FMAXICURWND_KEEP_STATE);

    //////////////////////////////////////////////////////////////////////////
    TiXmlElement *eleFTWOTOUCH = new TiXmlElement(KTAG_FTWOTOUCH);
    eleFinger->LinkEndChild(eleFTWOTOUCH);

    m_pEle_FTWOTOUCH_LISTEN_STATE = new TiXmlElement(KTAG_FTWOTOUCH_LISTEN_STATE);
    eleFTWOTOUCH->LinkEndChild(m_pEle_FTWOTOUCH_LISTEN_STATE);

    m_pEle_FVMOUSEWHEEL_LISTEN_STATE = new TiXmlElement(KTAG_FVMOUSEWHEEL_LISTEN_STATE);
    eleFTWOTOUCH->LinkEndChild(m_pEle_FVMOUSEWHEEL_LISTEN_STATE);

    m_pEle_FVMOUSEWHEEL_KEEP_STATE = new TiXmlElement(KTAG_FVMOUSEWHEEL_KEEP_STATE);
    eleFTWOTOUCH->LinkEndChild(m_pEle_FVMOUSEWHEEL_KEEP_STATE);

    m_pEle_FHMOUSEWHEEL_LISTEN_STATE = new TiXmlElement(KTAG_FHMOUSEWHEEL_LISTEN_STATE);
    eleFTWOTOUCH->LinkEndChild(m_pEle_FHMOUSEWHEEL_LISTEN_STATE);

    m_pEle_FHMOUSEWHEEL_KEEP_STATE = new TiXmlElement(KTAG_FHMOUSEWHEEL_KEEP_STATE);
    eleFTWOTOUCH->LinkEndChild(m_pEle_FHMOUSEWHEEL_KEEP_STATE);

    TiXmlElement *eleLowLim = new TiXmlElement(KTAG_PLOWLIM);
    TiXmlElement *eleNonSpecLoc = new TiXmlElement(KTAG_PNONSPECLOC);

    elePalm->LinkEndChild(eleLowLim);
    elePalm->LinkEndChild(eleNonSpecLoc);

    m_pEle_PLOWLIM_LISTEN_STATE = new TiXmlElement(KTAG_PLOWLIM_LISTEN_STATE);
    eleLowLim->LinkEndChild(m_pEle_PLOWLIM_LISTEN_STATE);

    m_pEle_PSHOWWNDSBS_LISTEN_STATE = new TiXmlElement(KTAG_PSHOWWNDSBS_LISTEN_STATE);
    eleLowLim->LinkEndChild(m_pEle_PSHOWWNDSBS_LISTEN_STATE);

    m_pEle_PSHOWWNDSBS_KEEP_STATE = new TiXmlElement(KTAG_PSHOWWNDSBS_KEEP_STATE);
    eleLowLim->LinkEndChild(m_pEle_PSHOWWNDSBS_KEEP_STATE);

    m_pEle_PSHOWWNDSBS_EXE_STATE = new TiXmlElement(KTAG_PSHOWWNDSBS_EXE_STATE);
    eleLowLim->LinkEndChild(m_pEle_PSHOWWNDSBS_EXE_STATE);

    m_pEle_PNONSPECLOC_LISTEN_STATE = new TiXmlElement(KTAG_PNONSPECLOC_LISTEN_STATE);
    eleNonSpecLoc->LinkEndChild(m_pEle_PNONSPECLOC_LISTEN_STATE);

    m_pEle_PMOVWND_LISTEN_STATE = new TiXmlElement(KTAG_PMOVWND_LISTEN_STATE);
    eleNonSpecLoc->LinkEndChild(m_pEle_PMOVWND_LISTEN_STATE);

    m_pEle_PMOVWND_KEEP_STATE = new TiXmlElement(KTAG_PMOVWND_KEEP_STATE);
    eleNonSpecLoc->LinkEndChild(m_pEle_PMOVWND_KEEP_STATE);

    m_pEle_PMOVWND_EXE_STATE = new TiXmlElement(KTAG_PMOVWND_EXE_STATE);
    eleNonSpecLoc->LinkEndChild(m_pEle_PMOVWND_EXE_STATE);
}

void CWGRTraceLogXML::StopLog(const std::string &xmlFileName)
{
    if (m_pXmlDoc)
    {
        m_pXmlDoc->SaveFile(xmlFileName.c_str());
        delete m_pXmlDoc;        
    }

    m_pXmlDoc = NULL;
    m_pEle_FT2F_LISTEN_STATE = NULL;
    m_pEle_CurCollects = NULL;   //用于记录采集次数的元素
    m_pEle_FT2F_LISTEN_STATE = NULL;
    m_pEle_FMINICURWND_LISTEN_STATE = NULL;
    m_pEle_FMINICURWND_KEEP_STATE = NULL;
    m_pEle_FALTLARROW_LISTEN_STATE = NULL;
    m_pEle_FALTLARROW_KEEP_STATE = NULL;
    m_pEle_FALTRARROW_LISTEN_STATE = NULL;
    m_pEle_FALTRARROW_KEEP_STATE = NULL;
    m_pEle_FDSPDESKTOP_LISTEN_STATE = NULL;
    m_pEle_FDSPDESKTOP_KEEP_STATE = NULL;
    m_pEle_FMAXICURWND_LISTEN_STATE = NULL;
    m_pEle_FMAXICURWND_KEEP_STATE = NULL;
    m_pEle_FTWOTOUCH_LISTEN_STATE = NULL;
    m_pEle_FVMOUSEWHEEL_LISTEN_STATE = NULL;
    m_pEle_FVMOUSEWHEEL_KEEP_STATE = NULL;
    m_pEle_FHMOUSEWHEEL_LISTEN_STATE = NULL;
    m_pEle_FHMOUSEWHEEL_KEEP_STATE = NULL;

    m_pEle_CurCollectTag = NULL; //用于记录采集数据的元素，不要与m_pEle_CurCollects混淆
}

void CWGRTraceLogXML::AddCollectTag(int curCollect, int curState)
{
    if (!m_pXmlDoc) return ;
    
    std::string tagName = KTAG_COLLECT;    
    
    tagName += Int2String(curCollect);
    
    TiXmlElement *pTmpEle = NULL;
    switch(curState)
    {
    case WGR_FTHREE2FIVETOUCH_LISTEN_STATE:
        pTmpEle = m_pEle_FT2F_LISTEN_STATE;
        break;

    case WGR_FMINICURWND_LISTEN_STATE:
        pTmpEle = m_pEle_FMINICURWND_LISTEN_STATE;
        break;

    case WGR_FMINICURWND_KEEP_STATE:
        pTmpEle = m_pEle_FMINICURWND_KEEP_STATE;
        break;

    case WGR_FALTLARROW_LISTEN_STATE:
        pTmpEle = m_pEle_FALTLARROW_LISTEN_STATE;
        break;

    case WGR_FALTLARROW_KEEP_STATE:
        pTmpEle = m_pEle_FALTLARROW_KEEP_STATE;
        break;

    case WGR_FALTRARROW_LISTEN_STATE:
        pTmpEle = m_pEle_FALTRARROW_LISTEN_STATE;
        break;

    case WGR_FALTRARROW_KEEP_STATE:
        pTmpEle = m_pEle_FALTRARROW_KEEP_STATE;
        break;

    case WGR_FDSPDESKTOP_LISTEN_STATE:
        pTmpEle = m_pEle_FDSPDESKTOP_LISTEN_STATE;
        break;

    case WGR_FDSPDESKTOP_KEEP_STATE:
        pTmpEle = m_pEle_FDSPDESKTOP_KEEP_STATE;
        break;

    case WGR_FMAXICURWND_LISTEN_STATE:
        pTmpEle = m_pEle_FMAXICURWND_LISTEN_STATE;
        break;

    case WGR_FMAXICURWND_KEEP_STATE:
        pTmpEle = m_pEle_FMAXICURWND_KEEP_STATE;
        break;

    case WGR_FTWOTOUCH_LISTEN_STATE:
        pTmpEle = m_pEle_FTWOTOUCH_LISTEN_STATE;
        break;

    case WGR_FVMOUSEWHEEL_LISTEN_STATE:
        pTmpEle = m_pEle_FVMOUSEWHEEL_LISTEN_STATE;
        break;

    case WGR_FVMOUSEWHEEL_KEEP_STATE:
        pTmpEle = m_pEle_FVMOUSEWHEEL_KEEP_STATE;
        break;

    case WGR_FHMOUSEWHEEL_LISTEN_STATE:
        pTmpEle = m_pEle_FHMOUSEWHEEL_LISTEN_STATE;
        break;

    case WGR_FHMOUSEWHEEL_KEEP_STATE:
        pTmpEle = m_pEle_FHMOUSEWHEEL_KEEP_STATE;
        break;

        //
    case WGR_PLOWLIM_LISTEN_STATE:
        pTmpEle = m_pEle_PLOWLIM_LISTEN_STATE;
        break;

    case WGR_SHOWWNDSBS_LISTEN_STATE:
        pTmpEle = m_pEle_PSHOWWNDSBS_LISTEN_STATE;
        break;

    case WGR_SHOWWNDSBS_KEEP_STATE:
        pTmpEle = m_pEle_PSHOWWNDSBS_KEEP_STATE;
        break;

    case WGR_SHOWWNDSBS_EXE_STATE:
        pTmpEle = m_pEle_PSHOWWNDSBS_EXE_STATE;
        break;

    case WGR_PNONSPECLOC_LISTEN_STATE:
        pTmpEle = m_pEle_PNONSPECLOC_LISTEN_STATE;
        break;


    case WGR_PMOVWND_LISTEN_STATE:
        pTmpEle = m_pEle_PMOVWND_LISTEN_STATE;
        break;

    case WGR_PMOVWND_KEEP_STATE:
        pTmpEle = m_pEle_PMOVWND_KEEP_STATE;
        break;

    case WGR_PMOVWND_EXE_STATE:
        pTmpEle = m_pEle_PMOVWND_EXE_STATE;
        break;
        

    default:
        break;
    }

    if (pTmpEle)
    {
        m_pEle_CurCollectTag = new TiXmlElement(tagName.c_str());
        pTmpEle->LinkEndChild(m_pEle_CurCollectTag);
    }
    else
    {
        m_pEle_CurCollectTag = NULL;
    }

    std::string strValue = Int2String(curCollect);
    TiXmlText *CollectTimes = new TiXmlText(strValue.c_str());
    m_pEle_CurCollects->LinkEndChild(CollectTimes);
}

void CWGRTraceLogXML::AddSpotCount(int nSpotCount)
{
    if (!m_pEle_CurCollectTag) return;

    //TiXmlElement *ele = new TiXmlElement(KTAG_SpotCount);    

    std::string strValue = Int2String(nSpotCount);
    
    AddElementUnderCollectTag(KTAG_SpotCount, strValue);
}

std::string CWGRTraceLogXML::PointArrayToString(const POINT *ptArray, int nCount)
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

void CWGRTraceLogXML::AddSpotPos(const POINT *ptArray, int nCount)
{
    if (!m_pEle_CurCollectTag) return;

    //TiXmlElement *ele = new TiXmlElement(KTAG_SpotPos);

    std::string strPoints = PointArrayToString(ptArray, nCount);
    //TiXmlText *text = new TiXmlText(strPoints.c_str());

    AddElementUnderCollectTag(KTAG_SpotPos, strPoints);
}

void CWGRTraceLogXML::AddGestureShape(EWGR_GESTURE_SHAPE gShape)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "Unknown Shape";
    switch(gShape)
    {
    case E_WGR_GShape_Palm:
        strValue = "Palm Shape";
        break;

    case E_WGR_GShape_Finger:
        strValue = "Finger Shape";
        break;

    default:        
        break;
    }

    AddElementUnderCollectTag(KTAG_GestureShape, strValue);
}

void CWGRTraceLogXML::AddCurMissShapeTolerance(int nCMissShapTol)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nCMissShapTol);

    AddElementUnderCollectTag(KTAG_CMissShapeTol, strValue);
}

void CWGRTraceLogXML::AddCurMissTouchCountTolerance(int nCMissTCountTol)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nCMissTCountTol);

    AddElementUnderCollectTag(KTAG_CMissTCountTol, strValue);
}

void CWGRTraceLogXML::AddCurMissDisLimitTolerance(int nCMissDisLimitTol)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nCMissDisLimitTol);

    AddElementUnderCollectTag(KTAG_CMissDisLimitTol, strValue);
}

void CWGRTraceLogXML::AddCurMissNonMoveTolerance(int nCMissNonMoveTol)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nCMissNonMoveTol);

    AddElementUnderCollectTag(KTAG_CMissNonMoveTol, strValue);
}

void CWGRTraceLogXML::AddCurUnexpectedKnownDirTolerance(int nTol)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nTol);

    AddElementUnderCollectTag(KTAG_CUnexpectedKnownDirTol, strValue);
}

void CWGRTraceLogXML::AddCurUnexpectedUnknownDirTolerance(int nTol)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nTol);

    AddElementUnderCollectTag(KTAG_CUnexpectedUnknownDirTol, strValue);
}

void CWGRTraceLogXML::AddEnterLess(bool bEnterLess)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "";

    if (bEnterLess)
    {
        strValue = "1";
    }
    else
    {
        strValue = "0";
    }

    AddElementUnderCollectTag(KTAG_EnterLess, strValue);
}

void CWGRTraceLogXML::AddMoveDirection(E_WGR_MOVEDIR dir)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "E_WGR_MDIR_Unknown!";
    switch(dir)
    {
    case E_WGR_MDIR_None:
        strValue = "E_WGR_MDIR_None!";
        break;

    case E_WGR_MDIR_Horiz:
        strValue = "E_WGR_MDIR_Horiz!";
        break;

    case E_WGR_MDIR_LHoriz:
        strValue = "E_WGR_MDIR_LHoriz!";
        break;

    case E_WGR_MDIR_RHoriz:
        strValue = "E_WGR_MDIR_RHoriz!";
        break;

    case E_WGR_MDIR_Vertic:
        strValue = "E_WGR_MDIR_Vertic!";
        break;

    case E_WGR_MDIR_UVertic:
        strValue = "E_WGR_MDIR_UVertic!";
        break;

    case E_WGR_MDIR_DVertic:
        strValue = "E_WGR_MDIR_DVertic!";
        break;

    case E_WGR_MDIR_Complex:
        strValue = "E_WGR_MDIR_Complex!";
        break;

    case E_WGR_MDIR_Grab:
        strValue = "E_WGR_MDIR_Grab!";
        break;

    case E_WGR_MDIR_Open:
        strValue = "E_WGR_MDIR_Open!";
        break;

    case E_WGR_MDIR_Move:
        strValue = "E_WGR_MDIR_Move";
        break;

    default:
        break;
    }

    AddElementUnderCollectTag(KTAG_MDIR, strValue);
}

void CWGRTraceLogXML::AddSateToCovert(int stateCoverted)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "WGR_IDLE_STATE";

    switch(stateCoverted)
    {
    case WGR_FTHREE2FIVETOUCH_LISTEN_STATE:
        strValue = "WGR_FTHREE2FIVETOUCH_LISTEN_STATE";
        break;

    case WGR_FMINICURWND_LISTEN_STATE:
        strValue = "WGR_FMINICURWND_LISTEN_STATE";
        break;

    case WGR_FMINICURWND_KEEP_STATE:
        strValue = "WGR_FMINICURWND_KEEP_STATE";
        break;

    case WGR_FALTLARROW_LISTEN_STATE:
        strValue = "WGR_FALTLARROW_LISTEN_STATE";
        break;

    case WGR_FALTLARROW_KEEP_STATE:
        strValue = "WGR_FALTLARROW_KEEP_STATE";
        break;

    case WGR_FALTRARROW_LISTEN_STATE:
        strValue = "WGR_FALTRARROW_LISTEN_STATE";
        break;

    case WGR_FALTRARROW_KEEP_STATE:
        strValue = "WGR_FALTRARROW_KEEP_STATE";
        break;

    case WGR_FDSPDESKTOP_LISTEN_STATE:
        strValue = "WGR_FDSPDESKTOP_LISTEN_STATE";
        break;

    case WGR_FDSPDESKTOP_KEEP_STATE:
        strValue = "WGR_FDSPDESKTOP_KEEP_STATE";
        break;

    case WGR_FMAXICURWND_LISTEN_STATE:
        strValue = "WGR_FMAXICURWND_LISTEN_STATE";
        break;

    case WGR_FMAXICURWND_KEEP_STATE:
        strValue = "WGR_FMAXICURWND_KEEP_STATE";
        break;

    case WGR_FTWOTOUCH_LISTEN_STATE:
        strValue = "WGR_FTWOTOUCH_LISTEN_STATE";
        break;

    case WGR_FVMOUSEWHEEL_LISTEN_STATE:
        strValue = "WGR_FVMOUSEWHEEL_LISTEN_STATE";
        break;

    case WGR_FVMOUSEWHEEL_KEEP_STATE:
        strValue = "WGR_FVMOUSEWHEEL_KEEP_STATE";
        break;

    case WGR_FHMOUSEWHEEL_LISTEN_STATE:
        strValue = "WGR_FHMOUSEWHEEL_LISTEN_STATE";
        break;

    case WGR_FHMOUSEWHEEL_KEEP_STATE:
        strValue = "WGR_FHMOUSEWHEEL_KEEP_STATE";
        break;

    case WGR_PLOWLIM_LISTEN_STATE:
        strValue = "WGR_PLOWLIM_LISTEN_STATE";
        break;

    case WGR_SHOWWNDSBS_LISTEN_STATE:
        strValue = "WGR_PSHOWWNDSBS_LISTEN_STATE";
        break;

    case WGR_SHOWWNDSBS_KEEP_STATE:
        strValue = "WGR_PSHOWWNDSBS_KEEP_STATE";
        break;

    case WGR_SHOWWNDSBS_EXE_STATE:
        strValue = "WGR_PSHOWWNDSBS_EXE_STATE";
        break;

    case WGR_PNONSPECLOC_LISTEN_STATE:
        strValue = "WGR_PNONSPECLOC_LISTEN_STATE";
        break;

    case WGR_PMOVWND_LISTEN_STATE:
        strValue = "WGR_PMOVWND_LISTEN_STATE";
        break;

    case WGR_PMOVWND_KEEP_STATE:
        strValue = "WGR_PMOVWND_KEEP_STATE";
        break;

    case WGR_PMOVWND_EXE_STATE:
        strValue = "WGR_PMOVWND_EXE_STATE";
        break;

    case WGR_PREFWND_LISTEN_STATE:
        strValue = "WGR_PREFWND_LISTEN_STATE";
        break;

    case WGR_PREFWND_SECMOVE_STATE:
        strValue = "WGR_PREFWND_SECMOVE_STATE";
        break;

    case WGR_PREFWND_THDMOVE_STATE:
        strValue = "WGR_PREFWND_THDMOVE_STATE";
        break;

    case WGR_PREFWND_KEEP_STATE:
        strValue = "WGR_PREFWND_KEEP_STATE";
        break;

    case WGR_PREFWND_EXE_STATE:
        strValue = "WGR_PREFWND_EXE_STATE";
        break;

    case WGR_PCLSCWND_LISTEN_STATE:
        strValue = "WGR_PCLSCWND_LISTEN_STATE";
        break;

    case WGR_PCLSCWND_EXE_STATE:
        strValue = "WGR_PCLSCWND_EXE_STATE";
        break;

    case WGR_FSWHWND_LISTEN_STATE:
        strValue = "WGR_FSWHWND_LISTEN_STATE";
        break;

    case WGR_FSWHWND_START_STATE:
        strValue = "WGR_FSWHWND_START_STATE";
        break;

    case WGR_FSWHWND_KEEP_STATE:
        strValue = "WGR_FSWHWND_KEEP_STATE";
        break;

    case WGR_FSWHWND_EXE_STATE:
        strValue = "WGR_FSWHWND_EXE_STATE";
        break;

    case WGR_PSVDOC_LISTEN_STATE:
        strValue = "WGR_PSVDOC_LISTEN_STATE";
        break;

    case WGR_PSVDOC_CLOSE_STATE:
        strValue = "WGR_PSVDOC_CLOSE_STATE";
        break;

    case WGR_PSVDOC_EXE_STATE:
        strValue = "WGR_PSVDOC_EXE_STATE";
        break;   

    default:
        break;
    }

    AddElementUnderCollectTag(KTAG_SateCovert, strValue);
}

void CWGRTraceLogXML::AddLastDisplacement(int lstXDisplacement, int lstYDisplacement)
{
    if(!m_pEle_CurCollectTag) return;

    std::string strValue = "";
    strValue += "[";
    strValue += Int2String(lstXDisplacement);
    strValue += ", ";
    strValue += Int2String(lstYDisplacement);
    strValue += "]";

    AddElementUnderCollectTag(KTAG_LDisplacement, strValue);
}

void CWGRTraceLogXML::AddCurrentDisplacement(int curXDisplacement, int curYDisplacement)
{
    if(!m_pEle_CurCollectTag) return;

    std::string strValue = "";
    strValue += "[";
    strValue += Int2String(curXDisplacement);
    strValue += ", ";
    strValue += Int2String(curYDisplacement);
    strValue += "]";

    AddElementUnderCollectTag(KTAG_CDisplacement, strValue);
}

void CWGRTraceLogXML::AddStartMiddlePoint(const POINT &pt)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "";
    strValue += "(";
    strValue += Int2String(pt.x);
    strValue += ", ";
    strValue += Int2String(pt.y);;
    strValue += ")";

    AddElementUnderCollectTag(KTAG_ptSartMid, strValue);
    
}

void CWGRTraceLogXML::AddLastMiddlePoint(const POINT &pt)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "";
    strValue += "(";
    strValue += Int2String(pt.x);
    strValue += ", ";
    strValue += Int2String(pt.y);;
    strValue += ")";

    AddElementUnderCollectTag(KTAG_ptLastMid, strValue);

}

void CWGRTraceLogXML::AddCurrentMiddlePoint(const POINT &pt)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "";
    strValue += "(";
    strValue += Int2String(pt.x);
    strValue += ", ";
    strValue += Int2String(pt.y);;
    strValue += ")";

    AddElementUnderCollectTag(KTAG_ptCurMid, strValue);

}

void CWGRTraceLogXML::AddSpotArea(int nCMoments)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nCMoments);

    AddElementUnderCollectTag(KTAG_SpotArea, strValue);
}

void CWGRTraceLogXML::AddStdArea(int nCMoments)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nCMoments);

    AddElementUnderCollectTag(KTAG_StdArea, strValue);
}

void CWGRTraceLogXML::AddTimesCMIncreased(int nTimes)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nTimes);

    AddElementUnderCollectTag(KTAG_TimesCMIn, strValue);
}

void CWGRTraceLogXML::AddTimesCMDecreased(int nTimes)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nTimes);

    AddElementUnderCollectTag(KTAG_TimesCMDe, strValue);
}

void CWGRTraceLogXML::AddTotCollects(int nTotCollects)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(nTotCollects);

    AddElementUnderCollectTag(KTAG_TotCollects, strValue);
}

void CWGRTraceLogXML::AddLastPoint(const POINT *ptArray, int nCount)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = PointArrayToString(ptArray, nCount);

    AddElementUnderCollectTag(KTAG_ptLast, strValue);
}

void CWGRTraceLogXML::AddStartPoint(const POINT *ptArray, int nCount)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = PointArrayToString(ptArray, nCount);

    AddElementUnderCollectTag(KTAG_ptStart, strValue);
}

void CWGRTraceLogXML::AddXShift(int xShift)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(xShift);

    AddElementUnderCollectTag(KTAG_xShift, strValue);
}

void CWGRTraceLogXML::AddYShift(int yShift)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue =Int2String(yShift);

    AddElementUnderCollectTag(KTAG_yShift, strValue);
}

//*********************************
void CWGRTraceLogXML::AddLastSpotMap(const std::map<UINT, ST_MOTIONINFO> &refMap)
{
    ;
    if (!m_pEle_CurCollectTag) return;

    std::map<UINT, ST_MOTIONINFO>::const_iterator it = refMap.begin();

    std::string strValue = "";
    for(; it != refMap.end(); it++)
    {
        strValue += "{(uId:";
        strValue += Int2String(it->first);

        const ST_MOTIONINFO &refMotion = it->second;

        strValue += "), ";

        strValue += "[(Pos:|";
        strValue += Int2String(refMotion.ptPos.x);
        strValue += ", ";
        strValue += Int2String(refMotion.ptPos.y);
        strValue += "|), (Disp:|";

        strValue += Int2String(refMotion.nXDisplacement);
        strValue += ", ";
        strValue += Int2String(refMotion.nYDisplacement);
        strValue += "|)}";

        strValue += "; ";       
    }

    AddElementUnderCollectTag(KTAG_TINFOLASTMAP, strValue);
}

void CWGRTraceLogXML::AddSpotInfo(const int &type, const ST_TouchInfo *spotInfo, const int &nSize)
{
    if (!m_pEle_CurCollectTag) return;

    const char *pTag = NULL;

    switch(type)
    {
    case 0:
        pTag = KTAG_MISMATCHEDSPOTINFO;
        break;

    case 1:
        pTag = KTAG_MATCHEDSPOTTINFO;
        break;

    case 2:
        pTag = KTAG_NEWADDEDSPOTINFO;
        break;

    default:
        break;
    }

    if (!pTag) return;
    
    std::string strValue = "";

    for (int i = 0; i < nSize; i++)
    {
        strValue += "{(uId:";
        strValue += Int2String(spotInfo[i].uId);

        const ST_MOTIONINFO &refMotion = spotInfo[i].stMotionInfo;

        strValue += "), ";

        strValue += "[(Pos:|";
        strValue += Int2String(refMotion.ptPos.x);
        strValue += ", ";
        strValue += Int2String(refMotion.ptPos.y);
        strValue += "|), (Disp:|";

        strValue += Int2String(refMotion.nXDisplacement);
        strValue += ", ";
        strValue += Int2String(refMotion.nYDisplacement);
        strValue += "|)}";

        if (i != nSize - 1) strValue += "; ";       
    }

    AddElementUnderCollectTag(pTag, strValue);
}

void CWGRTraceLogXML::AddPTMiddle(const POINT &ptMiddle)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = PointArrayToString(&ptMiddle, 1);

    AddElementUnderCollectTag(KTAG_CURPTMIDDLE, strValue);
}

void CWGRTraceLogXML::AddElectee(const E_WGR_MOVEDIR &dir, const int &votes)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "dir: ";
    switch(dir)
    {
    case E_WGR_MDIR_None:
        strValue += "E_WGR_MDIR_None";
        break;

    case E_WGR_MDIR_Move:
        strValue += "E_WGR_MDIR_Move";
        break;

    case E_WGR_MDIR_LHoriz:
        strValue += "E_WGR_MDIR_LHoriz";
        break;

    case E_WGR_MDIR_RHoriz:
        strValue += "E_WGR_MDIR_RHoriz";
        break;

    case E_WGR_MDIR_DVertic:
        strValue += "E_WGR_MDIR_DVertic";
        break;

    case E_WGR_MDIR_UVertic:
        strValue += "E_WGR_MDIR_UVertic";
        break;

    default:
        break;
    }

    strValue += "; votes:";
    strValue += Int2String(votes);

    AddElementUnderCollectTag(KTAG_ELECTEE, strValue);
}

//*********************************

//////////////////////////////////////////////////////////////////////////
//CWGRTraceLog
CWGRTraceLog::CWGRTraceLog(void)
{   
    m_bHasStarted = false;
}

CWGRTraceLog::~CWGRTraceLog(void)
{
}

void CWGRTraceLog::StartLog()
{
//#ifdef _DEBUG
    if (!m_bHasStarted)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        m_logName = "";
        m_logName += "_IWBTraceLogDir\\";

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

void CWGRTraceLog::StopLog()
{
    if (!m_bHasStarted) return;
    m_LogXml.StopLog(m_logName);

    m_bHasStarted = false;
}

void CWGRTraceLog::AddCollectTag(int curCollect, int curState)
{
    if (!m_bHasStarted) return ;

    m_LogXml.AddCollectTag(curCollect, curState);
}

void CWGRTraceLog::AddSpotCount(int nSpotCount)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotCount(nSpotCount);
}

void CWGRTraceLog::AddSpotPos(const POINT *ptArray, int nCount)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotPos(ptArray, nCount);
}

void CWGRTraceLog::AddGestureShape(EWGR_GESTURE_SHAPE gShape)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddGestureShape(gShape);
}

void CWGRTraceLog::AddCurMissShapeTolerance(int nCMissShapTol)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurMissShapeTolerance(nCMissShapTol);
}

void CWGRTraceLog::AddCurMissTouchCountTolerance(int nCMissTCountTol)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurMissTouchCountTolerance(nCMissTCountTol);
}

void CWGRTraceLog::AddCurMissDisLimitTolerance(int nCMissDisLimitTol)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurMissDisLimitTolerance(nCMissDisLimitTol);
}

void CWGRTraceLog::AddCurMissNonMoveTolerance(int nCMissNonMoveTol)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurMissNonMoveTolerance(nCMissNonMoveTol);
}

void CWGRTraceLog::AddCurUnexpectedKnownDirTolerance(int nTol)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurUnexpectedKnownDirTolerance(nTol);
}

void CWGRTraceLog::AddCurUnexpectedUnknownDirTolerance(int nTol)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurUnexpectedUnknownDirTolerance(nTol);
}

void CWGRTraceLog::AddEnterLess(bool bEnterLess)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddEnterLess(bEnterLess);
}

void CWGRTraceLog::AddMoveDirection(E_WGR_MOVEDIR dir)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddMoveDirection(dir);
}

void CWGRTraceLog::AddSateToCovert(int stateCoverted)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSateToCovert(stateCoverted);
}

void CWGRTraceLog::AddLastDisplacement(int lstXDisplacement, int lstYDisplacement)
{
    if(!m_bHasStarted) return;

    m_LogXml.AddLastDisplacement(lstXDisplacement, lstYDisplacement);
}

void CWGRTraceLog::AddCurrentDisplacement(int curXDisplacement, int curYDisplacement)
{
    if(!m_bHasStarted) return;

    m_LogXml.AddCurrentDisplacement(curXDisplacement, curYDisplacement);
}

void CWGRTraceLog::AddStartMiddlePoint(const POINT &pt)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddStartMiddlePoint(pt);
}

void CWGRTraceLog::AddLastMiddlePoint(const POINT &pt)
{
    if (!m_bHasStarted) return;
    
    m_LogXml.AddLastMiddlePoint(pt);
}

void CWGRTraceLog::AddCurrentMiddlePoint(const POINT &pt)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurrentMiddlePoint(pt);
}

void CWGRTraceLog::AddSpotArea(int nCMoments)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotArea(nCMoments);
}

void CWGRTraceLog::AddStdArea(int nCMoments)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddStdArea(nCMoments);
}

void CWGRTraceLog::AddTimesCMIncreased(int nTimes)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddTimesCMIncreased(nTimes);
}

void CWGRTraceLog::AddTimesCMDecreased(int nTimes)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddTimesCMDecreased(nTimes);
}

void CWGRTraceLog::AddTotCollects(int nTotCollects)
{
    if (!m_bHasStarted) return;
    
    m_LogXml.AddTotCollects(nTotCollects);
}

void CWGRTraceLog::AddLastPoint(const POINT *ptArray, int nCount)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddLastPoint(ptArray, nCount);
}

void CWGRTraceLog::AddStartPoint(const POINT *ptArray, int nCount)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddStartPoint(ptArray, nCount);
}

void CWGRTraceLog::AddXShift(int xShift)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddXShift(xShift);
}

void CWGRTraceLog::AddYShift(int yShift)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddYShift(yShift);
}

void CWGRTraceLog::AddLastSpotMap(const std::map<UINT, ST_MOTIONINFO> &refMap)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddLastSpotMap(refMap);
}

void CWGRTraceLog::AddSpotInfo(const int &type, const ST_TouchInfo *spotInfo, const int &nSize)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotInfo(type, spotInfo, nSize);
}

void CWGRTraceLog::AddPTMiddle(const POINT &ptMiddle)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddPTMiddle(ptMiddle);
}

void CWGRTraceLog::AddElectee(const E_WGR_MOVEDIR &dir, const int &votes)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddElectee(dir, votes);
}