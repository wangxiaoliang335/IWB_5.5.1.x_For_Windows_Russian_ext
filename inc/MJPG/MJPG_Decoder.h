#pragma once
//@Author:toxuke@gmail.com
//@Date  :2013/03/27

#ifdef MJPG_DECODER_EXPORTS
	#define MJPG_DECODER_API _declspec(dllexport)
#else
	#define  MJPG_DECODER_API _declspec(dllimport)
#endif


//定义解码结束后的回调函数
typedef void  (_stdcall *DECODE_DONE_CALLBACK)(void* lpCtx, int nErroCode, const BYTE* lpYUY2, int nLength);


struct jpeg_decompress_struct;
struct my_error_mgr;
class MJPG_DECODER_API CMJPG_Decoder
{
public:
	CMJPG_Decoder()
		:
	m_pfCallback(NULL),
    m_YBuf(NULL),
    m_UBuf(NULL),
    m_VBuf(NULL),
    m_GrayBuf(NULL),
	m_nImageWidth(640),
     m_nImageHeight(480)
	{
		Init();
	}

	~CMJPG_Decoder()
	{
		Uninit();
	}

	void Init();

	void Uninit();
	
	//@功能:输入MJPG数据,并进行解码处理
	//参数 :lpMJPGData,指向MJPG数据的指针
	//      nLength, MJPG数据长度
    //      pGrayData, 输出参数, 保存灰度数据的缓冲区
    //      pEffectiveDataLength, 输出参数, 保存实际有效数据长度的变量地址
	int ProcessData(const BYTE* lpMJPGData,  int nLength,BYTE* pGrayData, int* pEffectiveDataLength = NULL);

	//@功能:设置解码结束后的回调函数
	void SetCallBackFunc(DECODE_DONE_CALLBACK callback);

	//const BYTE* GetYUVData() const;
    const BYTE* GetGrayData() const;

	int GetImageWidth()  const;
	int GetImageHeight() const;

private:
    DECODE_DONE_CALLBACK m_pfCallback;

    struct jpeg_decompress_struct* m_cinfo;
    struct my_error_mgr*           m_jerr;

    static  const int RAW_LINES = 8;
    unsigned char**  m_raw_data[3];/* Output row buffer */
    unsigned char*   m_YRowPtr[RAW_LINES];
    unsigned char*   m_URowPtr[RAW_LINES];
    unsigned char*   m_VRowPtr[RAW_LINES];

    int  m_nImageWidth;
    int  m_nImageHeight;

    unsigned char* m_YBuf;
    unsigned char* m_UBuf;
    unsigned char* m_VBuf;

    //unsigned char*  m_YUY2Buf;
    unsigned char*  m_GrayBuf;//保存灰度图片的缓冲区。

	int m_nRowSize;


};