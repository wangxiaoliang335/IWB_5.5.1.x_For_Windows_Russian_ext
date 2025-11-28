#ifndef LIB_AVI_HEADER
#define LIB_AVI_HEADER
#ifdef LIBAVI_EXPORTS
#define LIBAVI_API _declspec(dllexport)
#else
#define LIBAVI_API _declspec(dllimport)
#endif


#include <fstream>
#pragma warning(disable:4251)
class  LIBAVI_API CAVIFile
{
public:
    CAVIFile();
    ~CAVIFile();

    bool open(const char* szFileName);

    bool close();

    bool readFrame(unsigned char* pData, long cbBuffer, long* plBytes, long* plSamples);

    bool IsOpen()const;
    
    const BITMAPINFOHEADER*  GetAVIFormat()const;

    //const BITMAPINFOHEADER* GetAVIFormat();
    bool seekToFirstFrame();
protected:
    bool seekToNextFrame();
protected:
    std::ifstream m_file;
    long          m_nFrameNo;
    //DWORD         m_dwFilePos;
    DWORD         m_dwFrameLength;
    BITMAPINFOHEADER m_bmpInfoHeader;
};
#pragma warning(default:4251)



#endif
