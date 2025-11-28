#include "stdafx.h"
#include <dshow.h>
#include "libAVI.h"
CAVIFile::CAVIFile()
    :
    m_nFrameNo(0),
    //m_dwFilePos(0x0),
    m_dwFrameLength(0x0)
{
    //initialize bitmap header
    memset(&m_bmpInfoHeader, 0, sizeof(BITMAPINFOHEADER));
    m_bmpInfoHeader.biSize     = 40;
    m_bmpInfoHeader.biWidth    = 1280;
    m_bmpInfoHeader.biHeight   = 720;
    m_bmpInfoHeader.biPlanes   = 1;
    m_bmpInfoHeader.biBitCount = 16;
    m_bmpInfoHeader.biCompression = MAKEFOURCC('M', 'J','P', 'G');

    //<<temp,2020/09/27
    //m_bmpInfoHeader.biWidth = 640;
    //m_bmpInfoHeader.biHeight = 480;
    //temp>>


}

CAVIFile::~CAVIFile()
{
    close();
}

bool CAVIFile::open(const char* szFileName)
{
    //reset file state
    m_file.clear();
    m_nFrameNo = 0;
    //m_dwFilePos = 0;

    m_file.open(szFileName, std::ifstream::in | std::ifstream::binary);
    if (m_file.fail())
    {
        return false;
    }

    if (!seekToFirstFrame())
    {
        m_file.close();
    }
    

    return true;
}

bool CAVIFile::seekToNextFrame()
{
    //The data is always padded to nearest WORD boundary
    std::streampos pos = m_file.tellg();

    if (pos % sizeof(WORD) == 1)
    {//ganranted the file pos to satisfying WORD alignment
        
        int temp = m_file.get();
        temp -= temp;

        //m_dwFilePos++;
    }

    const DWORD _00db_ = MAKEFOURCC('0', '0', 'd', 'b');//uncompressed video frame
    const DWORD _00dc_ = MAKEFOURCC('0', '0', 'd', 'c');//compressed video frame

    unsigned long  dwData;

    bool ret = false;
    for (;;)
    {
        m_file.read((char*)&dwData, sizeof(dwData));
        //m_dwFilePos += sizeof(dwData);

        if (m_file.fail())
        {
            break;
        }

        if (dwData == _00db_ || dwData == _00dc_)
        {
            m_file.read((char*)&dwData, sizeof(dwData));
            //m_dwFilePos += sizeof(dwData);
            if (m_file.fail())
            {
                break;
            }
            m_dwFrameLength = dwData;
            ret = true;
            break;
        }
    }

    return ret;
}

bool CAVIFile::seekToFirstFrame()
{
    //Seek to head
    m_file.seekg(0);
    
    return seekToNextFrame();
}


bool CAVIFile::close()
{
    m_file.close();
    return true;
}

bool CAVIFile::readFrame(unsigned char* pData, long cbBuffer, long* plBytes, long* plSamples)
{
    if (!m_file.is_open()) return false;

    *plBytes = m_dwFrameLength;

    if (DWORD(cbBuffer) < m_dwFrameLength)
    {
        return false;
    }

    //<<debug
    int nFilePos = m_file.tellg();
    //debug>>
    m_file.read((char*)pData, m_dwFrameLength);

    //m_dwFilePos += m_dwFrameLength;

    if (m_file.fail())
    {
        bool bRet = false;
        if (m_file.eof())
        {
            bRet =  true;
        }
        m_file.close();
        return bRet;
    }

    *plSamples = 1;

    m_nFrameNo++;

    if (!seekToNextFrame())
    {
        m_file.close();
    }

    return true;
}

bool CAVIFile::IsOpen()const
{
    return m_file.is_open();
}


const BITMAPINFOHEADER*  CAVIFile::GetAVIFormat()const
{

    return &m_bmpInfoHeader;
}

