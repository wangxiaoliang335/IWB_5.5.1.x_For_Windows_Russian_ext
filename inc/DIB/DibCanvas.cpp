#include <windows.h>
#include "DibCanvas.h"

CDibCanvas::CDibCanvas(UINT uWidth, UINT uHeight)
:
m_hMemDC(NULL),
m_hbmDIB(NULL),
m_hbmOld(NULL),
m_uWidth(uWidth),
m_uHeight(uHeight),
m_buffer(NULL)
{

    if(m_uWidth && m_uHeight)
    {
        CreateDIBSection();
    }
    

}


CDibCanvas::~CDibCanvas()
{
    CloseDIBSetction();
}


struct BMI
{
    BITMAPINFOHEADER bmiHeader;
    UINT32 red;
    UINT32 green;
    UINT32 blue;
};

BOOL CDibCanvas::CreateDIBSection()
{

    HDC hDCDesktop = ::GetDC(GetDesktopWindow());
    
    m_hMemDC = CreateCompatibleDC(hDCDesktop);

    ReleaseDC(GetDesktopWindow(), hDCDesktop);

    BMI bmi;
    //BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));


    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biCompression = BI_BITFIELDS;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biWidth       = m_uWidth;
    bmi.bmiHeader.biHeight      = - (int)m_uHeight;//取负值，意味第一行在缓冲区开头
    bmi.red   = 0x00ff0000;
    bmi.green = 0x0000ff00;
    bmi.blue  = 0x000000ff;

    m_hbmDIB = ::CreateDIBSection(m_hMemDC, (BITMAPINFO *)&bmi, DIB_RGB_COLORS, &m_buffer, NULL, NULL);

    if(m_hbmDIB == NULL) return FALSE;


    m_hbmOld = (HBITMAP)SelectObject(m_hMemDC, m_hbmDIB);


    return TRUE;
}


void  CDibCanvas::CloseDIBSetction()
{
    if (m_hbmOld != 0) {
        SelectObject(m_hMemDC, m_hbmOld);
        m_hbmOld = 0;
    }

    if (m_hbmDIB != 0) {
        DeleteObject(m_hbmDIB);
        m_hbmDIB = 0;
    }

    if (m_hMemDC != 0) {
        DeleteDC(m_hMemDC);
        m_hMemDC = 0;
    }

}


void* CDibCanvas::GetBuffer()
{
    return m_buffer;
}


void CDibCanvas::SetSize(UINT uWidth, UINT uHeight)
{
    if(m_uWidth != uWidth || m_uHeight != uHeight)
    {

        CloseDIBSetction();
        m_uWidth  = uWidth;
        m_uHeight = uHeight;
        CreateDIBSection();
    }
}
