#pragma once
//功能:自动亮度调节器
//原理:通过计算灰度图片的平均亮度值和期望值之间的差值来调用反馈控制函数,来实现对亮度的自动控制

template<class T>
class CAutoBrightnessRegulator
{
public:

    CAutoBrightnessRegulator(int nExpectedBrightness = 100,  bool (*fpControl)(bool inc,T absDiff, T nIndex, LPVOID lpCtxData) = NULL, LPVOID lpCtxData = NULL)
        :
    m_ExptectedBrightness(nExpectedBrightness),
		m_nIndex(0),
        m_fpControl(fpControl),
        m_lpCtxData(lpCtxData),
		m_nFrameCount(0)
    {

    }

    //@功能:设置期望的亮度值
    void SetExpectedBrightness(T nExpectedBrightness)
    {
        m_ExptectedBrightness = nExpectedBrightness;
    }

	void SetAutoCalibrateParamsIndex(T nIndex)
	{
		m_nIndex = nIndex;
	}

    //@功能:设置期望的亮度值
    T GetExpectedBrightness()const
    {
        return m_ExptectedBrightness;
    }

    void SetFeedbackCtrlFunction(bool (*fpControl)(bool inc, T absDiff ,T nIndex, LPVOID lpCtxData), LPVOID lpCtxData = NULL)
    {
        m_fpControl = fpControl;
		m_lpCtxData = lpCtxData;
    }

    //@功能:根据当前图片的亮度信息做出调节决定
    //@参数:pImage, 图片像素首地址
    //      nWidth, 图片宽度
    //      nHeight,图片高度
    //@返回值:图片亮度与期望值得差值的绝对值
    T ProcessImage(const T* pImage, int nWidth, int nHeight)
    {

        T avg_brightness = 0;
        T absDiff        = 0;//亮度差值
        __int64 brightness_sum = 0;
        for(int y = 0; y < nHeight; y++)
        {
            for(int x = 0; x < nWidth; x++)
            {
                brightness_sum += *pImage ++;
            }
        }
        int nPixelCount = nWidth*nHeight;
        if(nPixelCount)
        {
            avg_brightness = BYTE(brightness_sum/(nWidth*nHeight));
        }


        bool bInc = true;
        if(avg_brightness > m_ExptectedBrightness)
        {
            absDiff = avg_brightness - m_ExptectedBrightness;
            bInc = false;
        }
        else
        {
            absDiff = m_ExptectedBrightness - avg_brightness;

        }

        if((m_nFrameCount % 10) == 0)//每10帧改变一次亮度参数,摄像头对参数的变化需要一定时间响应
        {
            if(m_fpControl)
            {
                m_fpControl(bInc, absDiff,m_nIndex,m_lpCtxData);
            }
        }

        m_nFrameCount ++;

        return absDiff;
    }

    //@功能:状态复位
    void Reset()
    {
        m_nFrameCount = 0;
    }

protected:
    T m_ExptectedBrightness;//期望的亮度
	T m_nIndex;             //第几个摄像头参数

    //反馈控制函数指针
    //@参 数:bInc, true:增加亮度; false:减少亮度
    //       db, 亮度绝对差值
    //       lpCtxData,上下文数据
    //@返回值:
    bool (*m_fpControl)(bool bInc, T absDiff, T nIndex, LPVOID lpCtxData);//

    //反馈控制上下文数据
    LPVOID m_lpCtxData;

    int m_nFrameCount;

};