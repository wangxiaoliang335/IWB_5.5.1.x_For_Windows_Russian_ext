// Dib.cpp: implementation of the CDib class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <fstream>
#include "Dib.h"
#include "shlwapi.h"
#include "atlstr.h"
#pragma comment(lib, "shlwapi.lib")
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "../MyAPI.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDib::CDib()
{
	m_hDrawDib=NULL;
	m_pDib=NULL;
}

CDib::~CDib()
{
	Close();
}

void CDib::Draw(HDC hDC,int nWidth, int nHeight)
{
	if(m_pDib!=NULL)
	{
		//ASSERT(IsValid());
		DrawDibRealize(m_hDrawDib, hDC ,TRUE);
		DrawDibDraw(
			    m_hDrawDib,
				hDC,
				0,  //dest left
				0,  //dest top
				nWidth,
				nHeight,
				(BITMAPINFOHEADER *)m_pDib,
				(LPVOID) GetBits(),
				0,  //source left
				0,  //source top
				((BITMAPINFOHEADER *)m_pDib)->biWidth,
				abs(((BITMAPINFOHEADER *)m_pDib)->biHeight),
				DDF_BACKGROUNDPAL);
	}
}

SIZE CDib::GetSize() const
{
	SIZE sz;
	sz.cx = ((BITMAPINFOHEADER *)m_pDib)->biWidth;
	sz.cy = ((BITMAPINFOHEADER *)m_pDib)->biHeight;
	return sz;	
}


LONG CDib::GetWidth() const
{
	
	if(m_pDib == NULL) return 0;
	return ((BITMAPINFOHEADER *)m_pDib)->biWidth;
}

LONG CDib::GetHeight() const
{

	if(m_pDib == NULL) return 0;
	LONG height = ((BITMAPINFOHEADER *)m_pDib)->biHeight;
	if(height < 0 ) height = abs(height);
	return	height;
}

void CDib::Close()
{
	if(m_hDrawDib!=NULL)
	{
		DrawDibClose(m_hDrawDib);
		m_hDrawDib=NULL;
	}

	if(m_pDib!=NULL)
	{
		delete[] m_pDib;
		m_pDib=NULL;
	}
}

BOOL CDib::Open(const TCHAR * pzFileName)
{
	BITMAPFILEHEADER bmpFileHeader;
	std::ifstream file;
	
	int nBmpFileHeaderSize;

	Close();

	//drawdibopen initialize the diradib library and 
	//returns a handle for all drawdib operations
	if(!(m_hDrawDib=DrawDibOpen()))
		goto exit;

 	//open and read the DIB file header
	nBmpFileHeaderSize=sizeof(BITMAPFILEHEADER);

	file.open(CT2W(pzFileName), std::ios_base::binary | std::ios_base::in);
	if(file.fail())
	{
		goto exit;
	}

	file.read((char *)&bmpFileHeader,nBmpFileHeaderSize);

	if(file.gcount()!=(UINT)nBmpFileHeaderSize)
		goto failure;

	//validate the DIB file header by checking the first
	//two characters for the signature "BM"
	if(bmpFileHeader.bfType!=*((WORD *)"BM"))
		goto failure;

	//allocate a big chuck of global memory to store the DIB
	m_pDib=(BYTE *)new char [bmpFileHeader.bfSize-nBmpFileHeaderSize];

	//allocate memory fail
	if(!m_pDib)
		goto failure;

	//read the dib into the buffer at a time using ReadHuge
	file.read((char*)m_pDib,bmpFileHeader.bfSize-nBmpFileHeaderSize);

	BITMAPINFOHEADER *pDib=(BITMAPINFOHEADER *)m_pDib;
	if(((BITMAPINFOHEADER *)m_pDib)->biSizeImage==0)
	{
		//the application that create this bitmap didn't fill
		//in the biSizeImage field. Let's fill it
		//in even though the DrawDib * functions don't need it.
		
		//scan lines must be DWord aligned, hence the strange bit stuff
		pDib->biSizeImage=((((pDib->biWidth*pDib->biBitCount)+31)&~31)>>3)*pDib->biHeight;
		
	}
	m_nLineStride = (((pDib->biWidth*pDib->biBitCount)+31)&~31)>>3;



	file.close();
	return TRUE;

failure:
	file.close();
exit:
	Close();
	return FALSE;
}

BOOL CDib::Save(const TCHAR * pzFileName)
{
	std::ofstream file;

	CAtlString absPath = CheckAndCreatePath(pzFileName);

	file.open(CT2W(absPath),std::ios_base::binary | std::ios_base::out);

	if(file.fail())
		goto exit;


	BITMAPINFOHEADER *pBmpInfoHeader =(BITMAPINFOHEADER *)m_pDib;


	BITMAPFILEHEADER bmpFileHeader;
	memset(&bmpFileHeader, 0 ,sizeof(BITMAPFILEHEADER));
	bmpFileHeader.bfType = *((WORD *)"BM");
	bmpFileHeader.bfSize = pBmpInfoHeader->biSizeImage + sizeof(BITMAPINFOHEADER);
	//The offset, in bytes, from the beginning of the BITMAPFILEHEADER structure to the 
	//bitmap bits
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	file.write((const char*)&bmpFileHeader, sizeof(bmpFileHeader)); 

	//read the dib into the buffer at a time using ReadHuge
	file.write((const char*)m_pDib,bmpFileHeader.bfSize);

	file.close();
	return TRUE;

exit:
	return FALSE;
}

const BYTE* CDib::GetBits()const
{
	return ((CDib*)this)->GetBits();
}


BYTE * CDib::GetBits()
{
	//the size of the color map is determined by the number
	//of RGBQUAD structures presend.
	//it also depends on the bit_depth of the Dib
	DWORD dwNumColors,dwColorTableSize;
	BITMAPINFOHEADER *lpDib=(BITMAPINFOHEADER *)m_pDib;

	WORD wBitCount=lpDib->biBitCount;

	if(lpDib->biSize>=36)
		dwNumColors=lpDib->biClrUsed;
	else
		dwNumColors=0;

	if(dwNumColors==0)
	{
		if(wBitCount!=24)
			dwNumColors=1L<<wBitCount;
		else 
			dwNumColors=0;
	}

	dwColorTableSize=dwNumColors*sizeof(RGBQUAD);

	return m_pDib+lpDib->biSize+dwColorTableSize;
}

int CDib::GetBiBitCount()const
{
	if(m_pDib!=NULL)
		return ((BITMAPINFOHEADER *)m_pDib)->biBitCount; 
	return 0;
}


BOOL  CDib::Create(int nWidth, int nHeight)
{
	Close();
	if(!(m_hDrawDib=DrawDibOpen()))
		goto exit;

	int absHeight = abs(nHeight);

	
	int nPaddingLength =  ((nWidth*3) % 4 != 0)? 4 - (nWidth*3) % 4:0;
	
	m_nLineStride = nPaddingLength + nWidth*3;

	int memSize = sizeof(BITMAPINFOHEADER) + m_nLineStride*absHeight;

	m_pDib = new  BYTE[memSize];

	memset(m_pDib, 0, memSize);


	

	BITMAPINFOHEADER* pBmpInfoHeader = reinterpret_cast<BITMAPINFOHEADER*>(m_pDib);


    pBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);

     pBmpInfoHeader->biWidth        = nWidth;
     pBmpInfoHeader->biHeight       = nHeight;
     pBmpInfoHeader->biPlanes       = 1;
	 pBmpInfoHeader->biBitCount     = 24;
     pBmpInfoHeader->biCompression  = 0;
     pBmpInfoHeader->biSizeImage    = m_nLineStride*absHeight;
     pBmpInfoHeader->biXPelsPerMeter = 0;
     pBmpInfoHeader->biYPelsPerMeter = 0;
     pBmpInfoHeader->biClrUsed       = 0;
     pBmpInfoHeader->biClrImportant  = 0;


	 return TRUE;

exit:
	Close();
	return FALSE;

}


void CDib::SetPixel(int x, int y, COLORREF color)
{
	if(m_pDib  == NULL) return;

	if(x<0 || x >= GetWidth()) return ;
	if(y<0 || y >= GetHeight()) return ;

	BYTE* pPixel  = GetBits() + (GetHeight() - 1 - y)*m_nLineStride + x*3;

	pPixel[0] = GetBValue(color);
	pPixel[1] = GetGValue(color);
	pPixel[2] = GetRValue(color);


}