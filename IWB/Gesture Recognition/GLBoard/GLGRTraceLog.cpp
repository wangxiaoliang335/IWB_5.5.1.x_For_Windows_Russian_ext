#include "StdAfx.h"
#include "GLGRTraceLog.h"

//////////////////////////////////////////////////////////////////////////
//常量定义
const char *KTAG_SINGLEPEN                  = "SINGLEPEN";
const char *KTAG_MULTIPEN                   = "MULTIPEN";
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

//
const char *KTAG_EWBS_PEN                   = "EWBS_PEN";
const char *KTAG_EWBS_SELECTION             = "EWBS_SELECTION";
const char *KTAG_EWBS_ROATE                 = "EWBS_ROATE";
const char *KTAG_EWBS_ROAM                  = "EWBS_ROAM";

const char *KTAG_EWBS_ERASER                = "EWBS_ERASER";
const char *KTAG_EWBS_SPOTLIGHT             = "EWBS_SPOTLIGHT";
const char *KTAG_EWBS_MASK                  = "EWBS_MASK";
const char *KTAG_EWBS_RETURNDESKTOP         = "EWBS_RETURNDESKTOP";

const char *KTAG_MATCHINFO                  = "MATCHINFO";

const char *KTAG_PREMATINFO                 = "PREMATINFO";
const char *KTAG_AFTMATINFO                 = "AFTMATINFO";

const char *KTAG_RAWSPOTINFO                = "RAWSPOTINFO";
const char *KTAG_ALTERERASERPOS             = "ALTERERASERPOS";
const char *KTAG_SPLITERASRINFO             = "SPLITERASERINFO";
const char *KTAG_SPLITPENINFO               = "SPLITPENINFO";

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CGLGRTraceLogXML

CGLGRTraceLogXML::CGLGRTraceLogXML()
{
    m_pXmlDoc = NULL;    
    m_pEle_CurCollects = NULL;   //用于记录采集次数的元素
    
    m_pEle_CurCollectTag = NULL; //用于记录采集数据的元素，不要与m_pEle_CurCollects混淆
    
    m_pEle_EWBS_Eraser = NULL;  
    m_pEle_EWBS_Spotlight = NULL;    
    m_pEle_EWBS_Mask = NULL;  
    m_pEle_EWBS_RETURNTODISKTOP = NULL;    

    m_pEle_EWBS_Pen = NULL;    
    m_pEle_EWBS_Selection = NULL;    
    m_pEle_EWBS_Roate = NULL;    
    m_pEle_EWBS_Roam = NULL;    

    m_pEle_MultiPen = NULL;
}    

CGLGRTraceLogXML::~CGLGRTraceLogXML()
{
    if (m_pXmlDoc)
    {
        StopLog("UnKnown.xml");
    }
}

void CGLGRTraceLogXML::StartLog()
{
    StopLog("UnKnown.xml");

    m_pXmlDoc = new TiXmlDocument();
    TiXmlElement * element = new TiXmlElement("GLBOARDGR");  
    m_pXmlDoc->LinkEndChild(element);

    m_pEle_CurCollects = new TiXmlElement("CURRENTCOLLET");
    element->LinkEndChild(m_pEle_CurCollects);

    TiXmlElement *eleSingle = new TiXmlElement(KTAG_SINGLEPEN);
    element->LinkEndChild(eleSingle);

    m_pEle_MultiPen = new TiXmlElement(KTAG_MULTIPEN);
    element->LinkEndChild(m_pEle_MultiPen);

    TiXmlElement *eleFinger = new TiXmlElement(KTAG_FINGERSHAPE);
    TiXmlElement *elePalm   = new TiXmlElement(KTAG_PALMSHAPE);

    eleSingle->LinkEndChild(eleFinger);
    eleSingle->LinkEndChild(elePalm);    

    m_pEle_EWBS_Pen = new TiXmlElement(KTAG_EWBS_PEN);
    eleFinger->LinkEndChild(m_pEle_EWBS_Pen);

    m_pEle_EWBS_Selection = new TiXmlElement(KTAG_EWBS_SELECTION);
    eleFinger->LinkEndChild(m_pEle_EWBS_Selection);

    m_pEle_EWBS_Roate = new TiXmlElement(KTAG_EWBS_ROATE);
    eleFinger->LinkEndChild(m_pEle_EWBS_Roate);

    m_pEle_EWBS_Roam = new TiXmlElement(KTAG_EWBS_ROAM);
    eleFinger->LinkEndChild(m_pEle_EWBS_Roam);

    m_pEle_EWBS_Eraser = new TiXmlElement(KTAG_EWBS_ERASER);
    elePalm->LinkEndChild(m_pEle_EWBS_Eraser);

    m_pEle_EWBS_Spotlight = new TiXmlElement(KTAG_EWBS_SPOTLIGHT);
    elePalm->LinkEndChild(m_pEle_EWBS_Spotlight);

    m_pEle_EWBS_Mask = new TiXmlElement(KTAG_EWBS_MASK);
    elePalm->LinkEndChild(m_pEle_EWBS_Mask);

    m_pEle_EWBS_RETURNTODISKTOP = new TiXmlElement(KTAG_EWBS_RETURNDESKTOP);
    elePalm->LinkEndChild(m_pEle_EWBS_RETURNTODISKTOP);
}

void CGLGRTraceLogXML::StopLog(const std::string &xmlFileName)
{
    if (m_pXmlDoc)
    {
        m_pXmlDoc->SaveFile(xmlFileName.c_str());
        delete m_pXmlDoc;        
    }

    m_pXmlDoc = NULL;    
    m_pEle_CurCollects = NULL;   //用于记录采集次数的元素
    m_pEle_EWBS_Eraser = NULL;  
    m_pEle_EWBS_Spotlight = NULL;    
    m_pEle_EWBS_Mask = NULL;  
    m_pEle_EWBS_RETURNTODISKTOP = NULL;    

    m_pEle_EWBS_Pen = NULL;    
    m_pEle_EWBS_Selection = NULL;    
    m_pEle_EWBS_Roate = NULL;    
    m_pEle_EWBS_Roam = NULL;    

    m_pEle_CurCollectTag = NULL; //用于记录采集数据的元素，不要与m_pEle_CurCollects混淆
}

void CGLGRTraceLogXML::AddCollectTag(int curCollect, E_WBS curState)
{
    if (!m_pXmlDoc) return ;

    std::string tagName = KTAG_COLLECT;    

    tagName += Int2String(curCollect);

    TiXmlElement *pTmpEle = NULL;
    switch(curState)
    {
    case E_WBS_IDLE:
        /*strValue = "E_WBS_IDLE";*/
        break;

    case E_WBS_ERASER:
        pTmpEle = m_pEle_EWBS_Eraser;
        break;

    case E_WBS_PEN:
        pTmpEle = m_pEle_EWBS_Pen;
        break;

    case E_WBS_ROAM:
        pTmpEle = m_pEle_EWBS_Roam;
        break;

    case E_WBS_SPOTLIGHT:
        pTmpEle = m_pEle_EWBS_Spotlight;
        break;

    case E_WBS_RETURNTODISKTOP:
        pTmpEle = m_pEle_EWBS_RETURNTODISKTOP;
        break;

    case E_WBS_NEXTPAGE:
        //pTmpEle = m_pEle_EWBS_Eraser;
        break;

    case E_WBS_PROPAGE:
        //strValue = "E_WBS_PROPAGE";
        break;

    case E_WBS_ZOOM:
        pTmpEle = m_pEle_EWBS_Roate;
        break;

    case E_WBS_RORATE:
        pTmpEle = m_pEle_EWBS_Roate;
        break;

    case E_WBS_MASK:
        pTmpEle = m_pEle_EWBS_Mask;
        break;

    case E_WBS_SELECTION:
        pTmpEle = m_pEle_EWBS_Selection;
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

void CGLGRTraceLogXML::AddCollectTag(int curCollect)
{
    if (!m_pXmlDoc) return ;

    std::string tagName = KTAG_COLLECT;    

    tagName += Int2String(curCollect);

    m_pEle_CurCollectTag = new TiXmlElement(tagName.c_str());
    m_pEle_MultiPen->LinkEndChild(m_pEle_CurCollectTag);
}

void CGLGRTraceLogXML::AddRawSpotInfo(const TLightSpot *aryLightSpots,  const int &nCount)
{
    if (!m_pEle_CurCollectTag) return;

    const int KMAXPOINTS = 128;
    POINT pt[KMAXPOINTS];
    //memset(pt, 0, KMAXPOINTS * sizeof(POINT));
    int nActualCount = nCount > KMAXPOINTS ? KMAXPOINTS : nCount;

    for(int i = 0; i < nActualCount; i++)
    {
        pt[i].x = aryLightSpots[i].ptPosInScreen.x;
        pt[i].y = aryLightSpots[i].ptPosInScreen.y;
    }

    std::string strValue = PointArrayToString(pt, nActualCount);

    AddElementUnderCollectTag(KTAG_RAWSPOTINFO, strValue);
}

void CGLGRTraceLogXML::AddAlterErasrPos(const int *posAlterEraserArry, const int &nPosEraserCount, const TLightSpot *aryLightSpots,  const int &nCount)
{
    if (!m_pEle_CurCollectTag) return;

    const int KMAXPOINTS = 128;
    POINT pt[KMAXPOINTS];
    //memset(pt, 0, KMAXPOINTS * sizeof(POINT));
    int nActualCount = nPosEraserCount > KMAXPOINTS ? KMAXPOINTS : nPosEraserCount;

    for(int i = 0; i < nActualCount; i++)
    {
        const int &refPos = posAlterEraserArry[i];

        if (refPos >= nCount || refPos < 0) continue;

        pt[i].x = aryLightSpots[refPos].ptPosInScreen.x;
        pt[i].y = aryLightSpots[refPos].ptPosInScreen.y;
    }

    std::string strValue = PointArrayToString(pt, nActualCount);

    AddElementUnderCollectTag(KTAG_ALTERERASERPOS, strValue);

}

void CGLGRTraceLogXML::AddSplitEraserInfo(const POINT *ptArray, int nCount)
{
    if (!m_pEle_CurCollectTag) return;

    //TiXmlElement *ele = new TiXmlElement(KTAG_SpotCount);    

    std::string strValue = PointArrayToString(ptArray, nCount);

    AddElementUnderCollectTag(KTAG_SPLITERASRINFO, strValue);
}

void CGLGRTraceLogXML::AddSplitPenInfo(const POINT *ptArray, int nCount)
{
    if (!m_pEle_CurCollectTag) return;

    //TiXmlElement *ele = new TiXmlElement(KTAG_SpotCount);    

    std::string strValue = PointArrayToString(ptArray, nCount);

    AddElementUnderCollectTag(KTAG_SPLITPENINFO, strValue);
}

void CGLGRTraceLogXML::AddSpotCount(int nSpotCount)
{
    if (!m_pEle_CurCollectTag) return;

    //TiXmlElement *ele = new TiXmlElement(KTAG_SpotCount);    

    std::string strValue = Int2String(nSpotCount);

    AddElementUnderCollectTag(KTAG_SpotCount, strValue);
}

std::string CGLGRTraceLogXML::PointArrayToString(const POINT *ptArray, int nCount)
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

void CGLGRTraceLogXML::AddSpotPos(const POINT *ptArray, int nCount)
{
    if (!m_pEle_CurCollectTag) return;

    //TiXmlElement *ele = new TiXmlElement(KTAG_SpotPos);

    std::string strPoints = PointArrayToString(ptArray, nCount);
    //TiXmlText *text = new TiXmlText(strPoints.c_str());

    AddElementUnderCollectTag(KTAG_SpotPos, strPoints);
}

void CGLGRTraceLogXML::AddGestureShape(EGLGR_GESTURE_SHAPE gShape)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "Unknown Shape";
    switch(gShape)
    {
    case E_GLGR_GShape_Palm:
        strValue = "Palm Shape";
        break;

    case E_GLGR_GShape_Finger:
        strValue = "Finger Shape";
        break;

    default:        
        break;
    }

    AddElementUnderCollectTag(KTAG_GestureShape, strValue);
}

void CGLGRTraceLogXML::AddMoveDirection(E_2TOUCHS_MOVES dir)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "E_WGR_MDIR_Unknown!";
    switch(dir)
    {
    case E_2TOUCHS_MOVES_NONE:
        strValue = "E_2TOUCHS_MOVES_NONE!";
        break;

    case E_2TOUCHS_MOVES_SAME:
        strValue = "E_2TOUCHS_MOVES_SAME!";
        break;

    case E_2TOUCHS_MOVES_ROAM:
        strValue = "E_2TOUCHS_MOVES_ROAM!";
        break;

    case E_2TOUCHS_MOVES_SELECTION:
        strValue = "E_2TOUCHS_MOVES_SELECTION!";
        break;

    case E_2TOUCHS_MOVES_ZOOM:
        strValue = "E_2TOUCHS_MOVES_ZOOM!";
        break;

    case E_2TOUCHS_MOVES_RORATE:
        strValue = "E_2TOUCHS_MOVES_RORATE!";
        break;

    //case E_WGR_MDIR_DVertic:
    //    strValue = "E_WGR_MDIR_DVertic!";
    //    break;

    //case E_WGR_MDIR_Complex:
    //    strValue = "E_WGR_MDIR_Complex!";
    //    break;

    //case E_WGR_MDIR_Grab:
    //    strValue = "E_WGR_MDIR_Grab!";
    //    break;

    //case E_WGR_MDIR_Open:
    //    strValue = "E_WGR_MDIR_Open!";
    //    break;

    //case E_WGR_MDIR_Move:
    //    strValue = "E_WGR_MDIR_Move";
    //    break;

    default:
        break;
    }

    AddElementUnderCollectTag(KTAG_MDIR, strValue);
}

void CGLGRTraceLogXML::AddSateToCovert(E_WBS stateCoverted)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "WGR_IDLE_STATE";

    switch(stateCoverted)
    {
    case E_WBS_IDLE:
        strValue = "E_WBS_IDLE";
        break;

    case E_WBS_ERASER:
        strValue = "E_WBS_ERASER";
        break;

    case E_WBS_PEN:
        strValue = "E_WBS_PEN";
        break;

    case E_WBS_ROAM:
        strValue = "E_WBS_ROAM";
        break;

    case E_WBS_SPOTLIGHT:
        strValue = "E_WBS_SPOTLIGHT";
        break;

    case E_WBS_RETURNTODISKTOP:
        strValue = "E_WBS_RETURNTODISKTOP";
        break;

    case E_WBS_NEXTPAGE:
        strValue = "E_WBS_NEXTPAGE";
        break;

    case E_WBS_PROPAGE:
        strValue = "E_WBS_PROPAGE";
        break;

    case E_WBS_ZOOM:
        strValue = "E_WBS_ZOOM";
        break;

    case E_WBS_RORATE:
        strValue = "E_WBS_RORATE";
        break;

    case E_WBS_MASK:
        strValue = "E_WBS_MASK";
        break;

    case E_WBS_SELECTION:
        strValue = "E_WBS_SELECTION";
        break; 

    default:
        break;
    }

    AddElementUnderCollectTag(KTAG_SateCovert, strValue);
}

void CGLGRTraceLogXML::AddLastDisplacement(int lstXDisplacement, int lstYDisplacement)
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

void CGLGRTraceLogXML::AddCurrentDisplacement(int curXDisplacement, int curYDisplacement)
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

void CGLGRTraceLogXML::AddStartMiddlePoint(const POINT &pt)
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

void CGLGRTraceLogXML::AddLastMiddlePoint(const POINT &pt)
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

void CGLGRTraceLogXML::AddSpotArea(const int &nActualArea)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "";
    strValue += Int2String(nActualArea);

    AddElementUnderCollectTag(KTAG_SpotArea, strValue);
}

void CGLGRTraceLogXML::AddStdArea(const int &nStdAra)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = "";
    strValue += Int2String(nStdAra);

    AddElementUnderCollectTag(KTAG_StdArea, strValue);
}

//void CGLGRTraceLogXML::AddCurrentMiddlePoint(const POINT &pt)
//{
//    if (!m_pEle_CurCollectTag) return;
//
//    std::string strValue = "";
//    strValue += "(";
//    strValue += Int2String(pt.x);
//    strValue += ", ";
//    strValue += Int2String(pt.y);;
//    strValue += ")";
//
//    AddElementUnderCollectTag(KTAG_ptCurMid, strValue);
//
//}
//
//void CGLGRTraceLogXML::AddSpotArea(int nCMoments)
//{
//    if (!m_pEle_CurCollectTag) return;
//
//    std::string strValue = Int2String(nCMoments);
//
//    AddElementUnderCollectTag(KTAG_SpotArea, strValue);
//}
//
//void CGLGRTraceLogXML::AddStdArea(int nCMoments)
//{
//    if (!m_pEle_CurCollectTag) return;
//
//    std::string strValue = Int2String(nCMoments);
//
//    AddElementUnderCollectTag(KTAG_StdArea, strValue);
//}

//void CGLGRTraceLogXML::AddTotCollects(int nTotCollects)
//{
//    if (!m_pEle_CurCollectTag) return;
//
//    std::string strValue = Int2String(nTotCollects);
//
//    AddElementUnderCollectTag(KTAG_TotCollects, strValue);
//}

//void CGLGRTraceLogXML::AddLastPoint(const POINT *ptArray, int nCount)
//{
//    if (!m_pEle_CurCollectTag) return;
//
//    std::string strValue = PointArrayToString(ptArray, nCount);
//
//    AddElementUnderCollectTag(KTAG_ptLast, strValue);
//}

//void CGLGRTraceLogXML::AddStartPoint(const POINT *ptArray, int nCount)
//{
//    if (!m_pEle_CurCollectTag) return;
//
//    std::string strValue = PointArrayToString(ptArray, nCount);
//
//    AddElementUnderCollectTag(KTAG_ptStart, strValue);
//}

void CGLGRTraceLogXML::AddXShift(int xShift)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue = Int2String(xShift);

    AddElementUnderCollectTag(KTAG_xShift, strValue);
}

void CGLGRTraceLogXML::AddYShift(int yShift)
{
    if (!m_pEle_CurCollectTag) return;

    std::string strValue =Int2String(yShift);

    AddElementUnderCollectTag(KTAG_yShift, strValue);
}

void CGLGRTraceLogXML::AddSpotInfo(const GLMatchInfo *matchInfo, const int &nSize)
{
    if (!m_pEle_CurCollectTag) return;    

    std::string strValue = "";

    for (int i = 0; i < nSize; i++)
    {
        strValue += "{(uId:";
        strValue += Int2String(matchInfo[i].uId);

        strValue += "), ";

        strValue += "(matchState: ";

        switch(matchInfo[i].eMatchState)
        {
        case E_GLMISMATCHED:
            strValue += "Mismatch), ";
            break;

        case E_GLMATCHED:
            strValue += "Match), ";
            break;

        case E_GLNEW_ELEMENT:
            strValue += "NewElement), ";
            break;
        }

        strValue += "[(Pos:|";
        strValue += Int2String(matchInfo[i].ptPos.x);
        strValue += ", ";
        strValue += Int2String(matchInfo[i].ptPos.y);
        strValue += "|), (Disp:|";

        if (i != nSize - 1) strValue += "; ";       
    }

    AddElementUnderCollectTag(KTAG_MATCHINFO, strValue);
}

//*********************************

CGLGRTraceLog::CGLGRTraceLog()
{
    m_bHasStarted = false;
}

CGLGRTraceLog::~CGLGRTraceLog()
{

}

void CGLGRTraceLog::StartLog()
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

        m_logName += "GLBoardGR";

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

void CGLGRTraceLog::StopLog()
{
    if (!m_bHasStarted) return;
    m_LogXml.StopLog(m_logName);

    m_bHasStarted = false;
}

void CGLGRTraceLog::AddCollectTag(int curCollect, E_WBS curState)
{
    if (!m_bHasStarted) return;
    
    m_LogXml.AddCollectTag(curCollect, curState);
}

void CGLGRTraceLog::AddSpotCount(int nSpotCount)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotCount(nSpotCount);
}

void CGLGRTraceLog::AddSpotPos(const POINT *ptArray, int nCount)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotPos(ptArray, nCount);
}

void CGLGRTraceLog::AddGestureShape(EGLGR_GESTURE_SHAPE gShape)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddGestureShape(gShape);
}

void CGLGRTraceLog::AddMoveDirection(E_2TOUCHS_MOVES dir)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddMoveDirection(dir);
}

void CGLGRTraceLog::AddSateToCovert(E_WBS stateCoverted)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSateToCovert(stateCoverted);
}

void CGLGRTraceLog::AddLastDisplacement(int lstXDisplacement, int lstYDisplacement)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddLastDisplacement(lstXDisplacement, lstYDisplacement);
}

void CGLGRTraceLog::AddCurrentDisplacement(int curXDisplacement, int curYDisplacement)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddCurrentDisplacement(curXDisplacement, curYDisplacement);
}

void CGLGRTraceLog::AddStartMiddlePoint(const POINT &pt)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddStartMiddlePoint(pt);
}

void CGLGRTraceLog::AddLastMiddlePoint(const POINT &pt)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddLastMiddlePoint(pt);
}

void CGLGRTraceLog::AddXShift(int xShift)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddXShift(xShift);
}

void CGLGRTraceLog::AddYShift(int yShift)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddYShift(yShift);
}

//void AddLastSpotMap(const std::map<UINT, ST_MOTIONINFO> &refMap);
void CGLGRTraceLog::AddSpotInfo(const GLMatchInfo *matchInfo, const int &nSize)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotInfo(matchInfo, nSize);
}

void CGLGRTraceLog::AddSpotArea(const int &nActualArea)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddSpotArea(nActualArea);
}

void CGLGRTraceLog::AddStdArea(const int &nStdAra)
{
    if (!m_bHasStarted) return;

    m_LogXml.AddStdArea(nStdAra);
}