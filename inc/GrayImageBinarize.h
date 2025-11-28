//GrayImageBinarize.h
//目标: 查找合适的阈值来准确地分割黑白图片的目标和背景。
//参考文档<<直接从双峰直方图确定二值化阈值 2001>>
//
//一幅物体与背景对比明显的图像一般具有包含双峰的灰度直方图。
//找出两个双峰位置x1,x2, 域值可以设为Xt= (X1 + X2)/2
//实践表明这样做比寻找两峰之间的谷底更加可靠。
//问题的关键是如何寻找出双峰
//两种情形
//1.如果图片的标准差较大, 则物体和目标在图像中所占的面积大致相当。
//  灰度均值处于两峰之间，可以先用灰度均值将两峰大致分开，而后
//  分别在较高灰度部分和较低灰度部分查找双峰。最后以两峰位置的中间
//  值作为阈值。



//2.物体和背景在图像中所占的面积相差悬殊，那么在直方图上所反映出的将是一大一小
//  两个峰。像素灰度平均值处于较大的那个山峰的范围内， 并且灰度的平均值处于较大
//  的那个山峰的范围内。并且灰度的标准偏差较小。在这样情形下， 灰度平均值不能分开
//  两个山峰。找到的两个山峰中最高峰是我们要找的大峰，现在的问题是如何查找小峰。
//  通过分析知道灰度值中点和灰度平均值的关系，知道灰度平均值靠近大峰远离小峰，而
//  灰度值中点mid比灰度平均值avg更加接近小峰。以灰度值中点为界搜索小峰的位置。
//  若mid > avg, 则以灰度之中点为界在高灰度区域搜索小峰
//  若mid < avg, 则以灰度之中点为界在低灰度区域搜索小峰
//
//
//算法计算步骤如下
//1.计算像素灰度平均值(abg),标准差std.
//2.以像素平均值为分界点，分别求出左、右部分的最大值的位置
//3.若两峰位置相距较近(在标准差范围内), 说明值方图的双峰中有个峰很低,
//  因此需要另寻低峰的位置，否则至第(7)步。
//4.求出像素灰度中值点位置(midpos);
//5.如果midpos > avg, 表明小峰在大峰左侧，否则表明小峰在大峰右侧.
//6.以灰度中值点为界，找出低峰的位置。
//7.以两峰位置的中点作为所要求取得阈值。

//
//@功能:根据直方图查找二值化门限
//@参数:hist[N], 包含N个bin的直方图
//

//template<typename T, int N>
//inline int GetBinarizeThreshold(T hist[N])
//could not deduce template argument for 'T [N]' from 'int [256]'

//http://stackoverflow.com/questions/5173494/pass-reference-to-array-in-c
//C++ standard
//14.8.2.1 Deducing template arguments from a function call [temp.deduct.call]
//Template argument deduction is done by comparing each function template parameter type (call it P) with the type of the corresponding argument of the call (call it A) as described below.
//If P is not a reference type:
//-- If A is an array type, the pointer type produced by the array-to-pointer standard conversion (4.2) is used in place of A for type deduction; otherwise,
//-- If A is a function type, the pointer type produced by the function-to-pointer standard conversion (4.3) is used in place of A for type deduction; otherwise,
//-- If A is a cv-qualified type, the top level cv-qualifiers of A's type are ignored for type deduction.
//-- If P is a cv-qualified type, the top level cv-qualifiers of P's type are ignored for type deduction. If P is a reference type, the type referred to by P is used for type deduction


//http://www.mombu.com/programming/c/t-whats-is-cv-qualified-type-801613.html
//"cv-qualified" is an abbreviation for "const- and/or volatile-
//qualified". A cv-qualified type is a type that has const and/or
//volatile at its top level, e.g. const int.
//
//确定二进制门限的方法
enum EThresholdMethod
{
    TM_MIDDLE_POINT           ,//以直方图的左右峰的中点作为门限值
    TM_VALLEY_FROM_RIGHT      ,//从直方图右峰开始搜索谷底，作为门限值, 保证在谷底平坦的情形下, 选取较大门限
    TM_VALLEY_FROM_LEFT       ,//从直方图左峰开始搜索谷底，作为门限值, 保证在谷底平坦的情形下, 选取较小门限
};

template<typename T, int N >
inline int GetBinarizeThreshold(T (&hist)[N], EThresholdMethod eMethod=TM_MIDDLE_POINT)

{
    BYTE nThreshold  = 0;
    int i           = 0;
    int nGraySum    = 0;
    int nPixelCount = 0;
    int nAvgVal     = 0;

    static const int MINIMUM_GRAY_VALUE = 5;//最小灰度值

    //1.计算像素灰度平均值
    for(i = MINIMUM_GRAY_VALUE; i < N ; i++)
    {
        nPixelCount  += hist[i];
        nGraySum     += i*hist[i];
    }

    if(nPixelCount  == 0 ) return  MINIMUM_GRAY_VALUE;

    nAvgVal = nGraySum/nPixelCount;


    //2.计算像素灰度标准差
    int nSumOfSquaredError = 0;


    for(i = MINIMUM_GRAY_VALUE; i < N ; i++)
    {
        nSumOfSquaredError += hist[i]*(i - nAvgVal) * (i - nAvgVal);
    }

    int std_square = nSumOfSquaredError/nPixelCount;


    const int NOISY_PIXEL_COUNT_THRESHOLD = 10;//像素个数的噪声门限

    //这样将 像素个数 < MIMINUM_PIXEL_COUNT的灰度值跳过, 以便
    //确定正确的最大灰度值

    //3.搜索像素分布的灰度最大值
    int nMaxGrayValue = MINIMUM_GRAY_VALUE;//最大灰度值
    for(i=N-1; i> MINIMUM_GRAY_VALUE; i--)
    {
        if(hist[i] > NOISY_PIXEL_COUNT_THRESHOLD)
        {
            nMaxGrayValue = i;
            break;
        }
    }



    //4.以平均值为界, 在最小灰度值和最大灰度值之间搜索左，右部分的峰值的位置
    int nPeakLeft = 0, nPeakRight = nAvgVal;

    //查找左部分峰值(低灰度值部分)
    for(i = MINIMUM_GRAY_VALUE; i<= nAvgVal; i++)
    {
        if(hist[i] > hist[nPeakLeft])
        {
            nPeakLeft = i;
        }
    }

    //查找右部分峰值(高灰度值部分)
    for(i= nAvgVal + 1; i < N; i++)
    {
        if(hist[i] > hist[nPeakRight])
        {
            nPeakRight = i;
        }
    }

    //5.两峰的距离小于标准差, 证明两峰异常靠近, 需要寻找小峰
    int nPeakError = (nPeakLeft - nPeakRight)*(nPeakLeft - nPeakRight);
    if(nPeakError < std_square)
    {

        int nMidValue = (nMaxGrayValue + MINIMUM_GRAY_VALUE) >> 1;//像素值中点

        if(nMidValue > nAvgVal)
        {
            //像素值中间值大于灰度平均值，则小峰在大峰的右侧。
            //从中点开始向右寻找小峰
            nPeakRight = nMidValue;
            for(i= nMidValue + 1; i<nMaxGrayValue; i++)
            {
                if(hist[i] > hist[nPeakRight])
                {
                    nPeakRight = i;
                }

            }


        }
        else
        {
            //像素值中间值小于于灰度平均值，则小峰在大峰的右侧。
            nPeakLeft = nMidValue;
            for(i= MINIMUM_GRAY_VALUE ; i< nMidValue - 1; i++)
            {
                if(hist[i] > hist[nPeakLeft])
                {
                    nPeakLeft = i;
                }

            }
        }
    }

    //6.左右两峰的中间值作为门限
    switch(eMethod)
    {
    case TM_VALLEY_FROM_RIGHT://从右峰开始搜索谷底，作为门限值, 
                                             
            nThreshold = nPeakRight;
            for(i=nPeakRight; i>=nPeakLeft; i--)
            {
                if(hist[i]  + NOISY_PIXEL_COUNT_THRESHOLD < hist[nThreshold])
                {
                    nThreshold = i;
                }
            }
            break;
    
        case TM_VALLEY_FROM_LEFT://从左峰开始搜索谷底，作为门限值
            nThreshold = nPeakLeft;
            for(i=nPeakLeft; i <= nPeakRight; i++)
            {
                if(hist[i] + NOISY_PIXEL_COUNT_THRESHOLD < hist[nThreshold])
                {
                    nThreshold = i;
                }
            }

            break;
        
        default:nThreshold = (nPeakLeft + nPeakRight) >> 1;

    }

    return nThreshold;
}



//@功 能:根据Otsu（大津展之おうつのぶゆき）算法,计算前景和背景的门限值
//@参 数:pSrcData，指向黑白像素数据的首地址
//      nDataLength,数据的长度
//@返回值:返回计算得出的门限值
//@说 明:根据类间方差最大的原理来计算
//       between-class variance:
//      σB = ω0× ω1×(μ1-μ0)^2    ---(1)
//       ω0 + ω1 = 1
//             
//       ω0 = (ΣNi)/N,  i∈[1,k]
//       μ1, 前景的均值
//       μ0, 背景的均值
//算  法:
//		公式(1)两端都乘以N^2后得到新的公式为
//      N^2 × σB  = N0 × N1 ×(μ1-μ0)^2 --(2)
//      其中 
//      N0 = (ΣNi), i∈[1,k]
//      N1= (ΣNj) = N - N0;j∈[k+1,L)
//
//      N为所有像素的个数, L代表L级灰度
//
//                  
//                    
//       μ0 = 1/N0 ×Σ(i*Ni), i∈[1,k]
//                    
//
//                         
//      令U0 = N*μ0   =  Σ(i*Ni), i∈[1,k]
//                        
//                     L
//        μ1 = 1/N1* Σ(j*Nj), j∈[k+1,L]
//                    j= k+1
//
//                        L
//      令U1 = N1*μ1  = Σ(j*Nj),j∈[k+1,L]
//                       j=k+1
//
//
//       U0 + U1 = U
//
//则(2)两端再乘以(N1×N0)^2得到
//
//   Sigma = N^2 × σB × (N0×N1)^2 =  N0 × N1 ×(μ1*N0*N1-μ0*N0*N1)^2 = N0×N1 ×(U1*N0 - U0*N1)^2 --(3)
//
//  只需找出(3)右端最大时对应的k值即为所求的门限值???
//
inline BYTE FindThreshold_Ostu(const INT (&histogram)[256])
{
    int L = 256;

    BYTE cThreshold = 0;

    int N = 0;
    for(int i=0; i<L; i++)
    {
        N += histogram[i];
    }
    int N0 = 0;
    int N1 = 0;

    //  k
    int U0 = 0;//U=Σ(i*Ni)
    //  i=1


    //  L
    int U1 = 0;//U=Σ(i*Ni)
    //  i=k+1


    //  L
    int U = 0; //U=Σ(i*Ni)
    //  i=1
    //__int64 SigmaMax = 0;
    //__int64 Sigma = 0;


    for(int i=0; i<L; i++)
    {
        U += i*histogram[i];
    }

    __int64  varMax = 0;

    for(int i=0; i< L; i++)
    {

        int gray_count = histogram[i];
        if(gray_count)
        {
            N0 += gray_count;//Weighted Backgound
        }
        else
        {
            continue;
        }


        N1  = N - N0;//Weighted Foreground
        if(N1 == 0)
        {
            break;
        }


        U0 += i*histogram[i];

        U1  = U - U0;


        int mB = (U0 + (N0>>2)) /N0;//Mean Background;
        int mF = (U1 + (N1>>2)) /N1;//Mean Foreground

        // Calculate Between Class Variance
        __int64 varBetween = (__int64)N0 * (__int64)N1 * (mB - mF) * (mB - mF);

        // Check if new maximum found
        if (varBetween > varMax) {
            varMax = varBetween;
            cThreshold = i;
        }

    }//for


    return cThreshold;
}
