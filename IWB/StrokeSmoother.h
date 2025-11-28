#pragma once

//@Author  :toxuke@gmail.com
//@Date    :April/1st/2013
//@Revision:April/1st/2013
// 
//模板参数K为平滑滤波级数, K越大则越平滑, 但过大则会使线条的棱角平滑成圆弧。

//算法描述:假设SP(i-1)为上一点的平滑位置， 当前点P(i)对应的对应的平滑位置SP(i)
//    SP(i) = W0*SP(i-1) + W1*P(i);
//    W0, W1,为平滑系数， 满足关系W0 + W1 = 1;
//    为了避免浮点元算，将取重系数放大2^W_SCALE_SHIFT_SIZE
//
template<int K>
class CStrokeSmoother
{
public:
    CStrokeSmoother(void)
        :
      m_nTimes(0)
    {

        //初始化权重数组

        //新点在平滑计算中的权重
        int WEIGHT_DENOMINATOR = 1 << W_SCALE_SHIFT_SIZE;

        int  Wmin = WEIGHT_DENOMINATOR*1/10;
        int  Wmax = WEIGHT_DENOMINATOR;
        int  Xmax = WEIGHT_NUM;
        int  Xmin = 0;


        //假设距离平方X与平滑滤波系数W1的关系满足
        //X=a*W1^2 + b, W1为Y轴。
        //W1 = sqrt((X-b)/a)
        //W1的范围在[0, WEIGHT_DENOMINATOR]之间
        double a = (double)(Xmax - Xmin)/(double)(Wmax*Wmax - Wmin*Wmin);
        double b = double(Xmin - a*Wmin*Wmin);
        for(int X=Xmin; X <= Xmax; X++)
        {
            m_Weights[X][1]  =(int)sqrt((X-b)/a);
            m_Weights[X][0 ] = WEIGHT_DENOMINATOR - m_Weights[X][1];

            assert(m_Weights[X][1]  >= 0);
            assert(m_Weights[X][0]  >= 0);
        }

    }

    ~CStrokeSmoother(void)
    {


    }

    POINT Reset()
    {
        m_nTimes = 0;

        POINT ptResult;
         ptResult.x = m_aryPt[0].x >> SCALE_SHIFT_SIZE;
         ptResult.y = m_aryPt[0].y >> SCALE_SHIFT_SIZE;
         return ptResult;
    }

    BOOL IsReset()
    {
        return 0 == m_nTimes;
    }
    


    //说明:输入的是计算机屏幕坐标, 为了减少取整造成的误差，
    //     首先将屏幕坐标进行尺寸放大，将计算误差减少到原来的
    //     1/(2^SCALE_SHIFT_SIZE)
    //
    POINT Smooth(const POINT& ptInput)
    {
        //return ptInput;
        
        if( 0 == m_nTimes )
        {
            for(int i=0; i <= K; i++)
            {
                m_aryPt[i].x = ptInput.x << SCALE_SHIFT_SIZE;
                m_aryPt[i].y = ptInput.y << SCALE_SHIFT_SIZE;
            }
        }
        else
        {
             int nDx = ptInput.x - (m_aryPt[K].x >> SCALE_SHIFT_SIZE);
             int nDy = ptInput.y - (m_aryPt[K].y >> SCALE_SHIFT_SIZE);
             int weightIndex =( nDx*nDx + nDy*nDy);

             if(weightIndex >= WEIGHT_NUM) weightIndex = WEIGHT_NUM - 1;
             if(weightIndex < 0) weightIndex = 0;
            
             //以距离的平方作为滤波系统的索引号
             int W0 = m_Weights[weightIndex][0];
             int W1 = m_Weights[weightIndex][1];


            m_aryPt[0].x = ptInput.x << SCALE_SHIFT_SIZE;
            m_aryPt[0].y = ptInput.y << SCALE_SHIFT_SIZE;

            for(int i=1; i <= K; i++)
            {
                
                m_aryPt[i].x = (m_aryPt[i-1].x * W1 + m_aryPt[i].x *  W0) >> W_SCALE_SHIFT_SIZE;
                m_aryPt[i].y = (m_aryPt[i-1].y * W1 + m_aryPt[i].y *  W0) >> W_SCALE_SHIFT_SIZE;
                
            }
        }


        m_nTimes ++;

         POINT ptResult;
         ptResult.x = m_aryPt[K].x >> SCALE_SHIFT_SIZE;
         ptResult.y = m_aryPt[K].y >> SCALE_SHIFT_SIZE;

         return ptResult;

    }

protected:

    int m_nTimes       ;//滤波次数
    POINT m_aryPt[K+1] ;//K级滤波中间点数组。
                        //即m_aryPt[0]，为上一个输入点; m_aryPt[1]为上一个1级平滑位置; m_aryPt[2]为上一个2级平滑位置
                    
    POINT m_ptLastPos;//上一次位置





    static const int MAX_DIFF_RANGE  = 7;//前后点小于该距离 则进行平滑; 否则

    static const int WEIGHT_NUM = MAX_DIFF_RANGE*MAX_DIFF_RANGE;
    int m_Weights[WEIGHT_NUM+1][2];


    //尺寸放大因子,减小滤波过程中由于计算取整导致的误差
    static const int SCALE_SHIFT_SIZE  = 8;

    static const int SCALE_FACTOR = 2<<SCALE_SHIFT_SIZE;//尺寸放大因子

	static const int W_SCALE_SHIFT_SIZE = 8;
	
    

};
