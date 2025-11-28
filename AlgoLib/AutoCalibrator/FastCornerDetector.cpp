#include "stdafx.h"
#include "CornerDetect.h"
#include "FastCornerDetector.h"
#include "ImageFrame.h"
#include "ImageDib.h"
#include "BlobDetectorImpl.h"
#include "SIMD.h"
#include "ImageDebug.h"
//@功能:快速检测角点算法
//@参数:pSrcImage, 指向灰度图片数据的指针
//      nWidth, 图片宽度
//      nHeight, 图片高度
bool CFastCornerDetector::operator()(
                                     const unsigned char* pSrcImage, 
                                     int nWidth, 
                                     int nHeight, 
                                     UINT uExpectedCornerNumber, 
                                     /*int nDetectRadius, */
                                     ECalibrationDebugLevel eDebugLevel, 
                                     int nDebugTimes )
{
    static CImageFrame corner;
    corner.SetSize(nWidth, nHeight, 1);
    memcpy(corner.GetData(), pSrcImage, corner.Size());
    int nThreshold =  corner.Avg();
    //nThreshold >>=1 ;//门限减半
    nThreshold =5 ;//门限固定

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(corner, _T("FastDetect_PatternRaw.bmp"));
    }



    corner_detect5_sse2(nWidth, nHeight, pSrcImage, corner.GetData());

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        //Debug_SaveImageFrame(corner, _T("FastDetectCorner01.bmp"), nDebugTimes);
    }


    BYTE* pCorner  = corner.GetData();

    //二值化
    for(int row=0; row < nHeight; row++)
    {
        for(int col=0; col < nWidth; col++)
        {
            //注意等号,门限为0时, 若无等号则二值化为全白
            if(*pCorner <= nThreshold)
            {
                *pCorner = 0x00;
            }
            else
            {
                *pCorner = 0xFF;
            }
            pCorner++;
        }
    }

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(corner, _T("FastDetect_CornerCandidate.bmp"));
    }

    CConnectedComponentScanlineAlogrithm<> blobDetect;


    CBitFrame bitFrame;
    bitFrame.SetSize(nWidth, nHeight);


    GrayToBitFrame_SSE2((const BYTE*)corner.GetData(), (BYTE*)bitFrame.GetData(), 0xFF, nWidth*nHeight);

    //if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    //{
    //    SaveBitFrame(bitFrame,  _T("FastDetect_BitFrame.bmp"));
    //}

    blobDetect.ProcessImage((const BYTE*)bitFrame.GetData(), nWidth, nHeight);

    std::vector<TBlobObject> blobObjs;

    const TBlobObject* pObj = blobDetect.GetObjs();

    int nObjCount = blobDetect.GetObjCount();
    int nCandidateNum = nObjCount;

    //剔除形状离谱的目标
    for(int i= 0; i < nObjCount; i++)
    {
        //剔除长宽之比 > 2的目标
        int nWidth  = pObj[i].rcArea.right  - pObj[i].rcArea.left;
        int nHeight = pObj[i].rcArea.bottom - pObj[i].rcArea.top;


        //if(nCandidateNum > uExpectedCornerNumber)
        {
            if(nWidth == 0 || nHeight == 0)
            {
                nCandidateNum -- ;
                continue;
            }
            else if(nWidth > nHeight && nWidth*2 > nHeight*5)//W/L > 5/2
            {
                nCandidateNum -- ;
                continue;
            }
            else if(nWidth < nHeight && nHeight*2 > nWidth*5)//L/W > 5/2
            {
                nCandidateNum -- ;
                continue;
            }
        }

        blobObjs.push_back(pObj[i]);
        nCandidateNum -- ;
    }

    if(blobObjs.size() == 0)
    {
        return FALSE;
    }


    
	CHeapSortAlgorithm<TBlobObject> heapSort;


    //按照目标的质量降序排列各个目标, 以便按照
    //质量大小, 优先选取目标
    heapSort(&blobObjs[0], blobObjs.size(), false);


    //<<调试
    CImageFrame debugFrame;
    debugFrame = corner;
    //调试>>


    if(blobObjs.size() < uExpectedCornerNumber)
    {

        return FALSE;

    }
    else
    {

        m_vecCorners.resize(uExpectedCornerNumber);

        for(UINT i=0; i<uExpectedCornerNumber; i++)
        {

            m_vecCorners[i].x =  (blobObjs[i].my + blobObjs[i].mass/2)/blobObjs[i].mass;
            m_vecCorners[i].y  = (blobObjs[i].mx + blobObjs[i].mass/2)/blobObjs[i].mass;

            //<<调试
            debugFrame.DrawRectange(blobObjs[i].rcArea, 125);
            debugFrame.SetPixel(blobObjs[i].GPos, 125);
            //调试>>
        }

    }

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(debugFrame, _T("FastDetect_CornerCandidateResult.bmp"));
    }
    return TRUE;

}
