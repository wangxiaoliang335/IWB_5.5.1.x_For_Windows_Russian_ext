#pragma once
#include "ContactInfo.h"

//
//@说明:由于笔迹插值
//
template<int CONTACT_COUNT>
class CInterpolateDispatcher
{
public:
    CInterpolateDispatcher()
    {
        Reset();
    }

    void Reset()
    {
        for (int i = 0; i < _countof(m_touchPoints); i++)
        {
            m_touchPoints[i].uId       = 0xFFFFFFFF;
            m_touchPoints[i].pt.x      = 0xFFFFFFFF;
            m_touchPoints[i].pt.y      = 0xFFFFFFFF;
            m_touchPoints[i].ePenState = E_PEN_STATE_UP;
        }

        m_nActiveCount = 0;
    }

    //@功能:用新的触控点状态，更新触控点状态
    void PreProcess(const TContactInfo* pPenInfos, int nPenCount)
    {
        
        for (int i = 0; i < nPenCount; i++)
        {
            bool bFound = false;
            UINT uId = pPenInfos[i].uId;
            for (int j = 0; j < m_nActiveCount; j++)
            {
                if (m_touchPoints[j].uId == uId)
                {
                    m_touchPoints[j] = pPenInfos[i];
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                if (m_nActiveCount < _countof(m_touchPoints))
                {
                    m_touchPoints[m_nActiveCount] = pPenInfos[i];
                    m_nActiveCount++;
                }
            }
        }
        
    }

    //@功能:根据触控点的是否抬离屏幕，来更新活动笔状态
    void PostProcess()
    {
        int nLoopCount = m_nActiveCount;

        int idx = 0;
        while (nLoopCount)
        {
            if (m_touchPoints[idx].ePenState == E_PEN_STATE_UP)
            {//笔弹起了, 后面的元素往前挪动一个单元
                int nRemainedCount = m_nActiveCount - idx - 1;

                for (int i = 0; i < nRemainedCount; i++)
                {
                    m_touchPoints[idx + i] = m_touchPoints[idx + i + 1];
                }

                m_nActiveCount--;
            }
            else
            {
                idx++;
            }

            nLoopCount--;
        }//while;
    }

    const TContactInfo* GetAllContactData(int* pActiveCount)const
    {
        if (pActiveCount)
        {
            *pActiveCount = m_nActiveCount;
        }

        return &m_touchPoints[0];

    }


protected:
    TContactInfo m_touchPoints[CONTACT_COUNT];
    int m_nActiveCount;//活动笔的个数
};
