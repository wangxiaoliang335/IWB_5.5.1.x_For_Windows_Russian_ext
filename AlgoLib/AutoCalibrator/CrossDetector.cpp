#include "stdafx.h"
//#include "../inc/DispMonitorFinder.h"
//#include "CornerDetect.h"
//#include "CrossDetector.h"
//#include "ImageFrame.h"
//#include "ImageDib.h"
//#include "BlobDetectorImpl.h"
//#include "SIMD.h"
//#include "ImageDebug.h"
//#include "../MorphologyAlgo/inc/MorphologyAlgo.h"
#include "../headers.h"

struct RatioFilterElement
{
    UINT nObjIndex;
    double dbRatio;

	bool operator < (RatioFilterElement& right)
	{
		return (dbRatio < right.dbRatio);
	}

	bool operator <= (RatioFilterElement& right)
	{
		return (dbRatio <= right.dbRatio);
	}


	bool operator > (RatioFilterElement& right)
	{
		return (dbRatio > right.dbRatio);
	}

	bool operator >= (RatioFilterElement& right)
	{
		return (dbRatio >= right.dbRatio);
	}

};

//@功能:按照一定的准则过滤目标
//@参数:目标之间的最小距离
//      pSrcObjs, 原始目标数据
//      n,原始目标的个数
//      pOutObjs, 保存输出的目标的缓冲区
//      nOutSize, 用作输入参数时指定缓冲区尺寸, 用作输出参数时指定过滤后得目标个数
void FilterObject(int nMinDist, const TBlobObject* pSrcObjs, unsigned int n, TBlobObject* pOutObjs,unsigned int &nOutObjs)
{

    std::vector<bool> vecClutterFlags;//目标为噪声标志

    unsigned int nMaxObjs = nOutObjs;
        nOutObjs = 0;

    vecClutterFlags.resize(n,false);
 

    int nMinDist2 = nMinDist * nMinDist;
    //过滤准则
    //1.计算每对目标重心之间的距离, 如果距离小于设定的门限，则质量小的目标被剔除
    for(UINT i=0; i < n; i++)
    {
        if(vecClutterFlags[i])continue;//噪声目标
        const POINT& pt1   = pSrcObjs[i].GPos;
        const int&   mass1 = pSrcObjs[i].mass;

        for(UINT j = i+1; j < n; j++)
        {
            const POINT& pt2   = pSrcObjs[j].GPos;
            const int&   mass2 = pSrcObjs[j].mass;

            int nDx   = (pt2.x  - pt1.x);
            int nDy   = (pt2.y  - pt1.y);
            int nDist2 = nDx*nDx + nDy*nDy;
            if(nDist2 < nMinDist2)
            {
                if(mass1 <= mass2)
                {
                    vecClutterFlags[i] = true;
                    break;
                }
                else
                {
                     vecClutterFlags[j] = true;
                }
                
            }

        }//for(j)

        if(!vecClutterFlags[i])
        {
            if(nOutObjs >= nMaxObjs) break;
            pOutObjs[nOutObjs] =  pSrcObjs[i];
            nOutObjs++;
        }


    }//for(i)

}


//@功能:返回矩形区域内的目标的长轴,短轴
//@参数:image  ,灰度图片
//      rcBound,矩形区域  
//@说明:利用PCA(Principal Component Analyis)分析来获取目标的长轴和短轴长度
//      http://en.wikipedia.org/wiki/Principal_component_analysis
struct DoublePrecisionSize
{
    double cx;
    double cy;
};

DoublePrecisionSize GetObjectAxisLength(const CImageFrame& image, const RECT& rcBound)
{
    DoublePrecisionSize sz;
    sz.cx  = 0.0;
    sz.cy  = 0.0;

    int W = image.Width();
    int H = image.Height();
    const BYTE* pImageData = image.GetData();
    //1.计算重心
    POINT ptG;
    long lMx = 0, lMy = 0, lMass = 0;

    for(int y = rcBound.top ; y < rcBound.bottom; y ++)
    {
        const BYTE* pData = pImageData + y * W + rcBound.left;
       
        for(int x = rcBound.left; x < rcBound.right; x ++)
        {
            int val = *pData;
            if(val)
            {
                lMass += val;
                lMx   += val * x;
                lMy   += val * y;
            }

            pData ++;
        }
        
    }

    if(lMass == 0) return sz;

    ptG.x = lMx / lMass;
    ptG.y = lMy / lMass;


    long Dxx = 0, Dyy = 0, Dxy = 0;

    //计算协方差矩阵
    //          ┌Dx^2   DxDy ┐
    //cov(x,y) =│            │
    //          │DxDy   Dy^2 │
    //          └            ┘
    //
    long n = 0;
    for(int y = rcBound.top ; y < rcBound.bottom; y ++)
    {
        const BYTE* pData = pImageData + y * W + rcBound.left;
       
        for(int x = rcBound.left; x < rcBound.right; x ++)
        {
            int val = *pData;
            if(val)
            {
              int dx = x - ptG.x;
              int dy = y - ptG.y;

              Dxx += dx*dx;
              Dyy += dy*dy;
              Dxy += dx*dy;
              n ++ ;
            }
            pData ++;
        }
        
    }

    

    Dxx = Dxx/n;
    Dyy = Dyy/n;
    Dxy = Dxy/n;

    //求解协方差矩阵的连个特征根
    //这两个特征值的平方根即对应目标的长半径长度和短半径长度
    //特征方程为
    //λ^2 - (Dxx + Dyy) + (DxxDyy - Dxy^2) = 0;

    double eigen_value1, eigen_value2;

    double b = -(Dxx + Dyy);
    double c = Dxx * Dyy - Dxy*Dxy;

    double d = sqrt(b*b - 4*c);//注意b*b - 4*c恒大于0

    eigen_value1 = (-b  + d)/2.0;//大值
    eigen_value2 = (-b  - d)/2.0;//小值


    sz.cx = sqrt(eigen_value1) * 2.0;//乘以2,是因为特征值为半径长度,我们返回的是直径
    sz.cy = sqrt(eigen_value2) * 2.0;



    //如果只有一个点,则计算得到的特征值为0

    if(0.0 == sz.cx)sz.cx = 1.0;
    
    if(0.0 == sz.cy)sz.cy = 1.0;

    return sz;
}
           


//@参数:pSrcImage, 指向灰度图片数据的指针
//      nWidth, 图片宽度
//      nHeight, 图片高度
//      uExpectedCornerNumber, 期望的校正点个数
//      nGridSize, 棋盘网格平均大小
//      pMaskImage, 指向屏蔽图的指针，屏蔽图的指针和图片的高度和宽度一致
//      ECalibDebugLevel, 调试级别
//      nDebugTimes,调试参数
bool CCrossDetector::operator()(
    const unsigned char* pSrcImage, 
    int nWidth, 
    int nHeight, 
    UINT uExpectedCornerNumber, 
    int nGridSize, 
    const unsigned char* pMaskImage,
    ECalibDebugLevel eDebugLevel, 
    int nDebugTimes )
{
    //模式图片
    static CImageFrame s_src;
    s_src.SetSize(nWidth, nHeight, 1);
    memcpy(s_src.GetData(), pSrcImage, s_src.Size());


    //模式图片
    static CImageFrame s_pattern;
    s_pattern.SetSize(nWidth, nHeight, 1);
    //memcpy(s_pattern.GetData(), pSrcImage, s_pattern.Size());

    //屏蔽图片
    static CImageFrame s_mask;
    s_mask.SetSize(nWidth, nHeight, 1);
    if(pMaskImage)
    {
        //外部调用指定了屏蔽图则使用该屏蔽图
        memcpy(s_mask.GetData(), pMaskImage, s_mask.Size());
    }
    else
    {
        //外部未指定了屏蔽图，则取这个图片
        s_mask.Set(0xFF);
    }


    static CImageFrame s_debug;
    s_debug.SetSize(nWidth, nHeight, 1);


    if(nGridSize > 15)
    {
        //5*5的检测窗口
        //cross_detect5_sse2(nWidth, nHeight, pSrcImage, s_pattern.GetData());
        cross_detect5_improve(nWidth, nHeight, pSrcImage, s_pattern.GetData());
        
        //屏幕检测区腐蚀5个像素
        for(int i=0;i<5; i++)
        {
            Morph_Erode8(s_mask.GetData(),s_mask.GetData(),s_mask.Width(), s_mask.Height());
        }
        
        //Debug_SaveImageFrame(s_mask, _T("FastCrossDetect_mask.bmp"));

    }
    else
    {
        //3*3的检测窗口
        cross_detect3_sse2(nWidth, nHeight, pSrcImage, s_pattern.GetData());
        
        
        //屏幕检测区腐蚀3个像素
        for(int i=0;i<3; i++)
        {
            Morph_Erode8(s_mask.GetData(),s_mask.GetData(),s_mask.Width(), s_mask.Height());
        }
        
    }
    
    //用腐蚀后的屏蔽图,屏蔽边界处的干扰点
    s_pattern &= s_mask;


    CConnectedComponentScanlineAlogrithm<> blobDetect;


    CBitFrame bitFrame;
    bitFrame.SetSize(nWidth, nHeight);

    BYTE cThreshold = 0x08;

    GrayToBitFrame_SSE2((const BYTE*)s_pattern.GetData(), (BYTE*)bitFrame.GetData(), cThreshold, nWidth*nHeight);


    //if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
    {

        //二值化的1位位图转为8位灰度位图
        BitToGrayFrame_MMX(
            (const BYTE*)bitFrame.GetData(),
            s_debug.GetData(),
            bitFrame.GetPixelCount());

        s_debug.Merge(s_src, 50);
        
    }

  

    blobDetect.ProcessImage((const BYTE*)bitFrame.GetData(), nWidth, nHeight);


    const TBlobObject* pObj = blobDetect.GetObjs();

    int nObjCount = blobDetect.GetObjCount();
    
    
    const int MAX_OBJS_COUNT  = 256;
    TBlobObject objs[MAX_OBJS_COUNT];
    unsigned int nObjNumber = _countof(objs);

    //1.校正图案形变较小时的目标最小距离
    //int nMinDist   =  nGridSize/2 + 2;

    //2.校正图案桶形变较大时的目标最小距离
    int nMinDist   =  nGridSize/3;
    

    //根据距离目标之间的距离关系, 过滤噪声目标
    FilterObject(nMinDist, pObj, nObjCount, objs, nObjNumber);
    

    
    if(nObjNumber < uExpectedCornerNumber)
    {
        //无条件输出错误图片,以便找出失败原因
        Debug_SaveImageFrame(s_debug, _T("Missing_Corner.jpg"));
        return FALSE;
    }

    if(nObjNumber > uExpectedCornerNumber)
    {

         CImageFrame binaryImage;
         binaryImage.SetSize(nWidth, nHeight,1);

        //二值化的1位位图转为8位灰度位图
        BitToGrayFrame_MMX(
            (const BYTE*)bitFrame.GetData(),
            binaryImage.GetData(),
            bitFrame.GetPixelCount());


        std::vector<RatioFilterElement> vecCandidates;
        
        double ratio_upper_limit = 4.0;
        //剔除长宽比和宽长比大于4中比值最大的目标        
        for(UINT i=0; i< nObjNumber; i++)
        {
      
          //计算目标的长轴和短轴
          DoublePrecisionSize size = GetObjectAxisLength(binaryImage, objs[i].rcArea);

          double ratio = max(size.cx/size.cy, size.cy/size.cx);

          if(ratio > ratio_upper_limit)
          {
              RatioFilterElement e;
              e.nObjIndex = i;
              e.dbRatio   = ratio;
              vecCandidates.push_back(e);
          }
        }

        if(vecCandidates.size())
        {

            CHeapSortAlgorithm<RatioFilterElement> heapSort;
            heapSort(&vecCandidates[0], vecCandidates.size(),FALSE);


            UINT nDelNumber = nObjNumber - uExpectedCornerNumber;
            if(vecCandidates.size() < nDelNumber)
            {
                nDelNumber = vecCandidates.size();
            }

            for(UINT i=0; i < nDelNumber; i++)
            {
                objs[vecCandidates[i].nObjIndex].bIsValid = FALSE;
            }

            //移除数组中bIsValid被标记为false的对象
            int  j = 0;
            for(UINT i=0; i < nObjNumber; i++)
            {
                if(objs[i].bIsValid)
                {
                    objs[j] = objs[i];
                    j ++ ;
                }
            }

        

            nObjNumber -= nDelNumber;
        }
        

    

    }
   
    
	CHeapSortAlgorithm<TBlobObject> heapSort;


    //按照目标的质量降序排列各个目标, 
    //以便按照质量大小, 优先选取目标
    heapSort(objs, nObjNumber, false);


    //<<调试
    CDib dibDebug;
        
    //if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        //屏蔽图与校正图案合并, 屏蔽图透明度为10%
        //s_debug.Merge(s_mask, 5);
        LoadToDib(s_debug, dibDebug);
     }

    //调试>>
    m_vecCrosses.resize  (uExpectedCornerNumber);

    m_vecHDCrosses.resize(uExpectedCornerNumber);

    const BYTE*  pPattern = s_pattern.GetData();

    for(UINT i=0; i<uExpectedCornerNumber; i++)
    {

        m_vecCrosses[i].x   = (objs[i].my + (objs[i].mass>>1))/objs[i].mass;
        m_vecCrosses[i].y   = (objs[i].mx + (objs[i].mass>>1))/objs[i].mass;

        //高精度数值,
        m_vecHDCrosses[i].x = ((objs[i].my << INT_SCALE_SHIFT_SIZE) + (objs[i].mass>>1))/objs[i].mass;
        m_vecHDCrosses[i].y = ((objs[i].mx << INT_SCALE_SHIFT_SIZE) + (objs[i].mass>>1))/objs[i].mass;


        //根据目标的外接矩形,在响应为图中计算精确计算目标响应的质心的位置
        //2014/03/25,由于十字符号的形变和图像的噪声, 用外接矩形来计算十字符号的精确中心,是不合适的。
        //RECT& rcObj = objs[i].rcArea;
        //long t = rcObj.top;
        //long b = rcObj.bottom;
        //long l = rcObj.left;
        //long r = rcObj.right;

        //unsigned int nMx   = 0;//力矩
        //unsigned int nMy   = 0;//力矩
        //unsigned int nMass = 0;//质量

        ////计算重心
        //for(long y = t; y < b; y++)
        //{
        //    for(int x = l; x < r; x++)
        //    {
        //        BYTE cResponse = pPattern[y * nWidth + x];
        //        nMx   += cResponse * x;
        //        nMy   += cResponse * y;
        //        nMass += cResponse;
        //    }
        //}

        //if(nMass == 0 )
        //{
        //    return FALSE;//出错了   
        //}

         //m_vecHDCrosses[i].x = ((nMx << INT_SCALE_SHIFT_SIZE) + (nMass>>1))/nMass;
         //m_vecHDCrosses[i].y = ((nMy << INT_SCALE_SHIFT_SIZE) + (nMass>>1))/nMass;
                


        if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
        {
            //<<调试
            //外接矩形
            dibDebug.Rectangle(objs[i].rcArea, RGB(128,0,0));


            //十字符号的重心点
            dibDebug.SetPixel(m_vecCrosses[i].x, m_vecCrosses[i].y, RGB(255,0,0));

            //十字符号的高精度位置点
            dibDebug.SetPixel(
                (m_vecHDCrosses[i].x +  2^(INT_SCALE_SHIFT_SIZE -1)) >> INT_SCALE_SHIFT_SIZE, 
                (m_vecHDCrosses[i].y +  2^(INT_SCALE_SHIFT_SIZE -1)) >> INT_SCALE_SHIFT_SIZE, 
                RGB(0,255,0));
            //调试>>
        }
    }


     if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
     {
        //剩下的当做噪声目标
        for(UINT i = uExpectedCornerNumber; i < nObjNumber; i++)
        {
            //<<调试
            //dibDebug.Rectangle(objs[i].rcArea, RGB(0,255,255));
             dibDebug.SetPixel(objs[i].GPos.x, objs[i].GPos.y, RGB(255,0,255));
             //调试>>
        }




        Debug_SaveDibToJPEG(dibDebug, _T("FastDetectCross_Result.jpg"));
    }
    return TRUE;

}
