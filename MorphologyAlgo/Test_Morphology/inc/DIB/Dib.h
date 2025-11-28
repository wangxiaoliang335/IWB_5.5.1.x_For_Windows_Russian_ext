// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vfw.h>
#pragma comment(lib,"vfw32.lib")

class CDib 
{
public:
	CDib();
	virtual ~CDib();

//Attributes
public:
	BYTE    *GetBits();
	const BYTE    *GetBits()const;
	SIZE	 GetSize() const;
	LONG	 GetWidth() const;
	LONG	 GetHeight() const;
	int      GetBiBitCount()const;
	BOOL     IsValid()const  { return(m_hDrawDib!=NULL); }
	operator HDRAWDIB() { return m_hDrawDib; }
	BITMAPINFO* GetBitmapInfo() { return (BITMAPINFO *)m_pDib;}

	BOOL     Create(int nWidth, int nHeight);

	int      GetLineStride() const{ return m_nLineStride;}

	void SetPixel(int x, int y, COLORREF color);
//operations
public:
	BOOL Open(const TCHAR *pzFileName);
	BOOL Save(const TCHAR *pzFileName);
	void Draw(HDC hDC,int nWidth, int nHeight);
	void Close();

protected:
	//BYTE		*m_pDibBits;
	BYTE		*m_pDib;
	HDRAWDIB	m_hDrawDib;
	int         m_nLineStride;

	//BITMAPFILEHEADER bmpFileHeader;

};

