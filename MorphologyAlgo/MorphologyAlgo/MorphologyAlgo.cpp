// MorphologyAlgo.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../inc/MorphologyAlgo.h"
#include "../inc/MorphImage.h"
#include "../inc/RLEBinaryImage.h"
#include <assert.h>

//@功能:对Marker图进行迭代膨胀，并使用屏蔽图做屏蔽来完成重构操作。
//@参数:pMarker     , 输入参数, 指向
//      pMask       , 输入参数, 指向屏蔽图的首地址
//      nImageWidth , 输入参数, 图片帧宽度
//      nImageHeight, 输入参数, 图片帧高度
//      pKernel      , 输入参数, 指向结构元素(structure element, or kernel)的首地址
//      nKernelWidth,  输入参数, 结构元素(structure element, or kernel)的宽度
//      nKernelHeight, 输入参数, 结构元素(structure element, or kernel)的高度
void  MI_Reconstruct(
                     CMorphImage& marker,
               const CMorphImage& mask,
               const BYTE* pKernel,
               int nKernelWidth,
               int nKernelHeight)
{


    //CMorphImage lastMarker = marker;

    //int nImageWidth  = marker.GetWidth ();
    //int nImageHeight = marker.GetHeight();

    //do
    //{
    //    //使用屏蔽图屏蔽
    //    marker = marker & mask;

    //    if(lastMarker == marker)
    //    {
    //        break;
    //    }

    //    lastMarker = marker;
    //}while(1);
    ////
    ////

}



void  MI_Reconstruct(
                     CRLEBinaryImage& marker,
               const CRLEBinaryImage& mask)
{

    CRLEBinaryImage lastMarker = marker;

    int nDebugCount = 0;
    do
    {
        marker.Dilate_4();

        //使用屏蔽图屏蔽
        marker &=  mask;

        if(lastMarker == marker)
        {
            break;
        }

        lastMarker = marker;

        nDebugCount ++;
    }while(1);

}




extern "C"
{

//@功能:腐蚀操作, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
//      pKernel      , 输入参数, 指向结构元素(structure element, or kernel)的首地址
//      nKernelWidth,  输入参数, 结构元素(structure element, or kernel)的宽度
//      nKernelHeight, 输入参数, 结构元素(structure element, or kernel)的高度
//void MORPHOLOGY_API Morph_Erose(
//           const BYTE* pImageSrcData, 
//           BYTE* pImageDestBuf,
//           int nImageWidth, 
//           int nImageHeight,
//           const BYTE* pKernel,
//           int nKernelWidth,
//           int nKernelHeight)
//{
//
//
//
//}

//@功能:四联通腐蚀操作, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值当作白色。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度

void MORPHOLOGY_API Morph_Erode4(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight)
{
     //Step1:转为RLE编码图
     CRLEBinaryImage src(nImageWidth, nImageHeight, pImageSrcData);

     //Step2:对RLE图做四联通腐蚀
     src.Erode_4();
     
    //Step3:从RLE输出灰度图
    src.RLE2Gray(pImageDestBuf, nImageWidth*nImageHeight);
}



//@功能:八联通腐蚀操作, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值当作白色。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度

void MORPHOLOGY_API Morph_Erode8(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight)
{
     //Step1:转为RLE编码图
     CRLEBinaryImage src(nImageWidth, nImageHeight, pImageSrcData);

     //Step2:对RLE图做四联通腐蚀
     src.Erode_8();
     
    //Step3:从RLE输出灰度图
    src.RLE2Gray(pImageDestBuf, nImageWidth*nImageHeight);

}



//@功能:膨胀, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
//      pKernel      , 输入参数, 指向结构元素(structure element, or kernel)的首地址
//      nKernelWidth,  输入参数, 结构元素(structure element, or kernel)的宽度
//      nKernelHeight, 输入参数, 结构元素(structure element, or kernel)的高度
//void MORPHOLOGY_API Morph_Dilate(
//           const BYTE* pImageSrcData, 
//           BYTE* pImageDestBuf,
//           int nImageWidth, 
//           int nImageHeight,
//           const BYTE* pKernel,
//           int nKernelWidth,
//           int nKernelHeight)
//{
//
//
//}


//@功能:四连通膨胀, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Dilate4(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight)
{
     //Step1:转为RLE编码图
     CRLEBinaryImage src(nImageWidth, nImageHeight, pImageSrcData);
     
     //Step2:RLE图做四连通膨胀
     src.Dilate_4();


     //Step3:从RLE输出灰度图
      src.RLE2Gray(pImageDestBuf, nImageWidth*nImageHeight);
}



//@功能:八连通膨胀, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Dilate8(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight)
{
     //Step1:转为RLE编码图
     CRLEBinaryImage src(nImageWidth, nImageHeight, pImageSrcData);
     
     //Step2:RLE图做四连通膨胀
     src.Dilate_8();


     //Step3:从RLE图输出为灰度图
      src.RLE2Gray(pImageDestBuf, nImageWidth*nImageHeight);
     

}



//@功能:八连通开操作,即先腐蚀后膨胀 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Open(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight)
{
     //Step1: 转为RLE编码图
     CRLEBinaryImage src(nImageWidth, nImageHeight, pImageSrcData);

     //Step2: 8联通腐蚀
     src.Erode_8();

     //Step3: 8联通膨胀
     src.Dilate_8();

     //Step4.从RLE图输出为灰度图
      src.RLE2Gray(pImageDestBuf, nImageWidth*nImageHeight);
}


//@功能:八连通闭操作,即先膨胀后腐蚀 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Close(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight)
{
     //Step1: 转为RLE编码图
     CRLEBinaryImage src(nImageWidth, nImageHeight, pImageSrcData);


     //Step2: 8联通膨胀
     src.Dilate_8();

     //Step3: 8联通腐蚀
     src.Erode_8();


     //Step4.从RLE图输出为灰度图
      src.RLE2Gray(pImageDestBuf, nImageWidth*nImageHeight);

}

//@功能:对Marker图进行迭代重构，并使用屏蔽图做屏蔽来完成重构操作。
//@参数:pMarker     , 输入参数, 指向
//      pMask       , 输入参数, 指向屏蔽图的首地址
//      nImageWidth , 输入参数, 图片帧宽度
//      nImageHeight, 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Reconstruct(
                    BYTE* pMarker,
                    const BYTE* pMask,
                    int nImageWidth,
                    int nImageHeight
                    )
{
    //Step1: 转为RLE编码图
    CRLEBinaryImage marker(nImageWidth, nImageHeight, pMarker);
    CRLEBinaryImage mask(nImageWidth, nImageHeight, pMask);
    
    //Step2:重构
     MI_Reconstruct(marker,mask);

    //Step3.从RLE图得到灰度图
    marker.RLE2Gray(pMarker, nImageWidth*nImageHeight);

}



    //@功能:填补二值化图像中空洞, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
    //@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
    //      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
    //      nImageWidth  , 输入参数, 图片帧宽度
    //      nImageHieght , 输入参数, 图片帧高度
    //      pKernel      , 输入参数, 指向结构元素(structure element, or kernel)的首地址
    //      nKernelWidth,  输入参数, 结构元素(structure element, or kernel)的宽度
    //      nKernelHeight, 输入参数, 结构元素(structure element, or kernel)的高度
    void MORPHOLOGY_API Morph_FillHole(
               const BYTE* pImageSrcData, 
               BYTE* pImageDestBuf,
               int nImageWidth, 
               int nImageHeight)
    {
        //Step1:转为RLE编码图
        CRLEBinaryImage src(nImageWidth, nImageHeight, pImageSrcData);

        //Step2:构建源数据图的互补图
        CRLEBinaryImage mask = src;
        mask.Complement();
        
       //Step3:构建初Marker图, 将Mask图中四周边界加入Marker图
        CRLEBinaryImage marker(nImageWidth, nImageHeight);

        marker[0] = mask[0];

        for(int r = 1; r < nImageHeight - 1; r++)
        {

            CRLERow& maskRow = mask[r];
            CRLERow& dstRow = marker[r];

            if(maskRow.m_nSegmentCount  == 1)//源图只有一条线段
            {
                if(maskRow.m_pSegments[0].nXl == 0)
                {
                    dstRow.m_pSegments[0] = maskRow.m_pSegments[0];
                    dstRow.m_nSegmentCount = 1;
                }
                else if(maskRow.m_pSegments[0].nXr == nImageWidth - 1)
                {
                    dstRow.m_pSegments[0] = maskRow.m_pSegments[0];
                    dstRow.m_nSegmentCount = 1;
                }
            }
            else//源图有两条以上线段
            {
                if(maskRow.m_pSegments[0].nXl == 0)
                {
                    dstRow.m_pSegments[0] = maskRow.m_pSegments[0];
                    dstRow.m_nSegmentCount = 1;
                }
                
                if(maskRow.m_pSegments[maskRow.m_nSegmentCount - 1].nXr == nImageWidth - 1)
                {
                    dstRow.m_pSegments[ dstRow.m_nSegmentCount] = maskRow.m_pSegments[maskRow.m_nSegmentCount - 1];
                    dstRow.m_nSegmentCount ++;
                }
    
            }


        }//for


        marker[nImageHeight - 1] =  mask[nImageHeight - 1];

        //Step4, 膨胀重构
        MI_Reconstruct(marker, mask);


        //Step5. Marker取补集
        marker.Complement();

        //Step 6.输出灰度结果
        marker.RLE2Gray(pImageDestBuf, nImageWidth*nImageHeight);
    }


}