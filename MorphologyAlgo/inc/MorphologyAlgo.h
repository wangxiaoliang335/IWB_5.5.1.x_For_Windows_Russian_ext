#pragma once
#ifdef MORPHOLOGYALGO_EXPORTS 
    #define MORPHOLOGY_API _declspec(dllexport)
#else if 
    #define MORPHOLOGY_API _declspec(dllimport)
#endif

extern "C"
{

//形态学处理函数
//

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
//           int nKernelHeight);


//@功能:四联通腐蚀操作, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值当作白色。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度

void MORPHOLOGY_API Morph_Erode4(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight);



//@功能:八联通腐蚀操作, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值当作白色。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度

void MORPHOLOGY_API Morph_Erode8(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight);


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
//           int nKernelHeight);


//@功能:四连通膨胀, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Dilate4(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight);



//@功能:八连通膨胀, 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Dilate8(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight);


//@功能:八连通开操作,即先腐蚀后膨胀 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Open(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight);


//@功能:八连通闭操作,即先膨胀后腐蚀 处理对象为二值化图像, 黑色的亮度值为0, 白色的亮度值为0xFF, 其他值无意义。
//@参数:pImageSrcData, 输入参数, 原始图片帧像素缓冲区的首地址
//      pImageDestBuf, 输出参数, 存放处理后的图片帧像素的缓冲区首地址
//      nImageWidth  , 输入参数, 图片帧宽度
//      nImageHieght , 输入参数, 图片帧高度
void MORPHOLOGY_API Morph_Close(
           const BYTE* pImageSrcData, 
           BYTE* pImageDestBuf,
           int nImageWidth, 
           int nImageHeight);



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
                    );




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
           int nImageHeight);


}
