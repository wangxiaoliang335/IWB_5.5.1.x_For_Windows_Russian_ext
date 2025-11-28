#pragma once
//循环队列FIFO
//先进先出
template <class T, int size>
class CRoundRobinQueue
{
public:
    CRoundRobinQueue()
    {
        Reset();
    }

    //@功能:往FIFO中写入数据
    //@说明:写入数据后， 清除"FIFO空"标志, 
    //      如果下一个写位置和读位置相同，则置位"FIFO满"标志
    void Write(const T& t)
    {
        m_data[m_nWritePos] = t;

        m_nWritePos++;

        if (m_nWritePos == _countof(m_data))
        {
            m_nWritePos = 0;
        }

        m_bIsEmpty = FALSE;

        if (m_nWritePos == m_nReadPos)
        {
            m_bIsFull = TRUE;
        }


    }

    //@功能:从FIFO中读取数据
    //@说明:读取数据后， 清除"FIFO满"标志, 
    //      如果下一个读位置和写位置相同，则置位"FIFO空"标志
    const T* Read()
    {
        const T* pT =  &m_data[m_nReadPos];

        m_nReadPos++;
        if (m_nReadPos == _countof(m_data))
        {
            m_nReadPos = 0;
        }

        m_bIsFull = FALSE;

        if(m_nReadPos == m_nWritePos)
        {
            m_bIsEmpty = TRUE;
        }


        return pT;
    }



    BOOL IsEmpty()
    {
        return m_bIsEmpty;
    }

    BOOL IsFull()
    {
        return m_bIsFull;
    }


    void Reset()
    {
        m_nReadPos = m_nWritePos = 0;
        m_bIsEmpty = TRUE;
        m_bIsFull  = FALSE;

    }
protected:

    int m_nReadPos;
    int m_nWritePos;
    T m_data[size];

    //说明:读空和写满时, 均满足m_nReadPos == m_nWritePos
    //     因此需要由读写操作实时跟踪空/满状态
    BOOL m_bIsEmpty;//读空标志
    BOOL m_bIsFull ;//写满标志

};