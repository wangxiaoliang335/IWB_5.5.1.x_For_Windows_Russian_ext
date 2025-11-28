#pragma once

class CDibCanvas
{
public:
    CDibCanvas(UINT uWidth = 0 , UINT uHeight= 0);

    ~CDibCanvas();
    
    void* GetBuffer();


    void SetSize(UINT uWidth, UINT uHeight);

    HDC GetDC() const{return m_hMemDC;}

    UINT GetWidth () const { return m_uWidth;}
    UINT GetHeight() const { return m_uHeight;}
protected:

    BOOL CreateDIBSection();
    void CloseDIBSetction();

protected:
    UINT m_uWidth ;
    UINT m_uHeight;
    HDC  m_hMemDC ;

    HBITMAP m_hbmDIB;
    HBITMAP m_hbmOld;
    void*   m_buffer;

};


