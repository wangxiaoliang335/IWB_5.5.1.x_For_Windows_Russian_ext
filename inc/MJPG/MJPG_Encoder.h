#pragma once
#ifdef MJPG_ENCODER_EXPORTS
    #define MJPG_ENCODER_API _declspec(dllexport)
#else
    #define MJPG_ENCODER_API _declspec(dllimport)
#endif
//@功  能:将DIB保存为,JPEG文件
//@参  数:pBIH, 指向DIB头的数据,后接调色板和像素数据
//@返回值:成功, TRUE
//        失败, FALSE
MJPG_ENCODER_API BOOL Dib2JPEG(const BITMAPINFOHEADER* pBIH, LPCTSTR szFileName);