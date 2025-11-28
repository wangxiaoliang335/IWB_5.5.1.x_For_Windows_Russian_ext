//Expectation-Maximization Algorithm
template <class T>
double sum(const T* pData, int length)
{
    double sum = 0.0;
    for(int i = 0; i < length; i++)
    {
        sum += (double)*pData;
        pData ++;
    }
    return sum;
}

template<class T>
double mean(const T* pData, int n)
{
    double sum_value = sum(pData, n);
    double mean_value = 0.0;
    if(n > 0)
        mean_value = sum_value /n;
    return mean_value;
}


template<class T>
double stdErr(const T* pData, int n)
{
    double mean = mean(pData, n);
    
    double variance2 = 0.0;
    for(int i=0; i < n; i++)
    {
        double offset = (double)pData[i] - mean;
        variance2 += offset * offset;
    }

    double stdDeviation = 0.0;
    
    if(n > 1)
    {
        stdDeviation = sqrt(variance2/(n-1));
    }

    return stdDeviation;
}

//log of maximum likelyhood 
template <class T>
double lnLikelihood(double u1, double sigma1, double probability_C1, double u2, double sigma2, double probability_C2, const T* pData, int length)
{
    double S1 = 0.0, S2 = 0.0;
    for(int i = 0; i < length; i++)
    {
        S1 += (double(pData[i]) - u1) * (double(pData[i]) - u1);
        S2 += (double(pData[i]) - u2) * (double(pData[i]) - u2);
    }


    double value = log(probability_C1) - 0.5*log(sigma1) - 0.5 * S1/sigma1 + \
        log(probability_C2) - 0.5*log(sigma2) - 0.5*S2/sigma2;

    return value;
}



//@功能:利用Expectation-Maximization来查找图片的二值化门限
//@参数:frameSrc, 灰度图片
//      mask, 尺寸与frameSrc相同的屏蔽图，屏蔽中值为0xFF的像素，对应位置的灰度图像素才需要考虑二值化操作。
BYTE EM_Threshold(const CImageFrame& frameSrc, const CImageFrame mask)
{
    int n = 0;
    const unsigned char* pMaskData = mask.GetData();
    int nImagePixelCount = mask.GetPixelCount();
    int i = 0;
    while(i < nImagePixelCount)
    {
        if(*pMaskData == 0xFF) n ++;
        pMaskData ++;
        i++;
    };

    std::vector<unsigned char> data;
    data.resize(n);

    i = 0;
    n = 0;
     
    pMaskData = mask.GetData();
    const unsigned char* pSrcData = frameSrc.GetData();
    while(i < nImagePixelCount)
    {
        if(*pMaskData == 0xFF) 
        {
            data[n] = *pSrcData;
            n++;
        }
        pMaskData ++ ;
        pSrcData ++;

        i++;
    };

    if(n == 0 ) return (unsigned char)0u;

    double avg = mean(&data[0], n);

    double est_mean_background = 0.0, est_variance_background = 0.0;//估计的前景均值和方差
    double est_mean_foreground = 0.0, est_variance_foreground = 0.0;//估计的前景均值和方差
    double probability_background = 0.0;//任意像素为背景的概率
    double probability_foreground = 0.0;//任意像素为前景的概率

    //以avg为门限计算切割前景和背景，计算前景和背景的像素个数和均值
    //我们规定背景暗, 前景亮。
    double sum_background = 0, sum_foreground = 0;
    int pixel_count_background = 0, pixel_count_foreground = 0;
    for(int i = 0; i < n; i++)
    {
        if(data[i] < avg)
        {
            sum_background += data[i];
            pixel_count_background ++;
        }
        else
        {
            sum_foreground += data[i];
            pixel_count_foreground ++;
        }
    }

    if(pixel_count_background > 0 )
    {
       est_mean_background = sum_background / double(pixel_count_background);
       probability_background = double(pixel_count_background) / double(n);
    }
    if(pixel_count_foreground > 0 )
    {
        est_mean_foreground = sum_foreground /double(pixel_count_foreground);
        probability_foreground = double(pixel_count_foreground) / double(n);
    }

    double sum_variance2_background = 0.0, sum_variance2_foreground = 0.0;
    for(i = 0; i < n; i++)
    {
         if(data[i] < avg)
         {
            sum_variance2_background += (data[i] - est_mean_background) * (data[i] - est_mean_background);
         }
         else
         {
            sum_variance2_foreground += (data[i] - est_mean_foreground) * (data[i] - est_mean_foreground);
         }
    }

    if(pixel_count_background > 1) est_variance_background = sum_variance2_background/(pixel_count_background - 1);
    if(pixel_count_foreground > 1) est_variance_foreground = sum_variance2_foreground/(pixel_count_foreground - 1);


    if(est_variance_background < 1e-10 || est_variance_foreground < 1e-10)//方差趋于0, 门限已确定。
        return (unsigned char) avg;

    double lastlnLikelihood = lnLikelihood (
        est_mean_background, 
        est_variance_background, 
        probability_background, 
        est_mean_foreground,
        est_variance_foreground,
        probability_foreground,
        &data[0],
        n);


    //在指定的前景和背景的正态分布参数的情形下,每个像素的条件概率
    std::vector<double> conditional_probability_background;
    std::vector<double> conditional_probability_foreground;
    std::vector<double> membership_probability_background;
    std::vector<double> membership_probability_foreground;

    conditional_probability_background.resize(n);
    conditional_probability_foreground.resize(n);
    membership_probability_background.resize(n);
    membership_probability_foreground.resize(n);

    int max_iter = 100;//最大迭代次数
    int t = 0;
    while(t < max_iter)
    {
        for(i = 0;  i < n; i++)
        {
            double  e = - (data[i] - est_mean_background) * (data[i] - est_mean_background) /(2.0*est_variance_background);
            conditional_probability_background[i] = \
                 1.0/sqrt(2*M_PI*est_variance_background) * exp(e);

            e = - (data[i] - est_mean_foreground) * (data[i] - est_mean_foreground) /(2.0*est_variance_foreground);
            conditional_probability_foreground[i] = \
                1.0/sqrt(2*M_PI*est_variance_foreground) * exp(e);


            double denominitor = probability_background * conditional_probability_background[i] + probability_foreground * conditional_probability_foreground[i];

            membership_probability_background[i] = probability_background * conditional_probability_background[i] / denominitor;
            membership_probability_foreground[i] = probability_foreground * conditional_probability_foreground[i] / denominitor; 

        }


       probability_background = sum(&membership_probability_background[0], n) / double(n);
       probability_foreground = sum(&membership_probability_foreground[0], n) / double(n);



       //更新前景和背景的均值和方差。
       double mean_foreground_nominator = 0.0;
       double mean_background_nominator = 0.0;
       double variance_foreground_nominator = 0.0;
       double variance_background_nominator = 0.0;

       for(i = 0; i < n; i++)
       {
            mean_background_nominator += membership_probability_background[i] *  data[i];
            mean_foreground_nominator += membership_probability_foreground[i] *  data[i];
       }
       
       double sum_membership_background = sum(&membership_probability_background[0], n);
       double sum_membership_foreground = sum(&membership_probability_foreground[0], n);

       est_mean_background = mean_background_nominator / sum_membership_background;
       est_mean_foreground = mean_foreground_nominator / sum_membership_foreground;


       for(i = 0; i < n; i++)
       {
            variance_background_nominator += membership_probability_background[i] * (data[i] - est_mean_background) * (data[i] -  est_mean_background);
            variance_foreground_nominator += membership_probability_foreground[i] * (data[i] - est_mean_foreground) * (data[i] -  est_mean_foreground);
       }
       
       est_variance_background = variance_background_nominator / sum_membership_background;
       est_variance_foreground = variance_foreground_nominator / sum_membership_foreground;



        double lnLikelihood_value = lnLikelihood (
        est_mean_background, 
        est_variance_background, 
        probability_background, 
        est_mean_foreground,
        est_variance_foreground,
        probability_foreground,
        &data[0],
        n);

        double likelihoodIncrement = fabs(lnLikelihood_value - lastlnLikelihood);

        //if( likelihoodIncrement < 1e-2)
        if( likelihoodIncrement < 1e-1)
        {
            break;
        }

        lastlnLikelihood = lnLikelihood_value;

        t ++;
    }


    //两个正态分布, x若满足
    //p(x|u_1,σ_1) =p(x|u_2,σ_2)
    //则x为门限。
    double a,b,c, th1, th2, th;
    a = 0.5 * ( 1/est_variance_foreground - 1/est_variance_background);
    b = est_mean_background / est_variance_background - est_mean_foreground / est_variance_foreground;
    c = 0.5 * ((est_mean_foreground * est_mean_foreground) / est_variance_foreground - (est_mean_background * est_mean_background) / est_variance_background) + \
        log(probability_background / probability_foreground) + 0.5 * log(est_variance_foreground / est_variance_background);

    th1 = (-b - sqrt(b*b - 4*a*c))/(2*a);
    th2 = (-b + sqrt(b*b - 4*a*c))/(2*a);


    //th1距离u1,u2的距离的平方和
    double distance1  = (th1 - est_mean_foreground)*(th1 - est_mean_foreground) + (th1 - est_mean_background)*(th1 - est_mean_background);

    //th1距离u1,u2的距离的平方和
    double distance2  = (th2 - est_mean_foreground)*(th2 - est_mean_foreground) + (th2 - est_mean_background)*(th2 - est_mean_background);

    if(distance1 < distance2)
    {
        th = th1;
    }
    else
    {
        th = th2;
    }


    return (unsigned char)th;
}
