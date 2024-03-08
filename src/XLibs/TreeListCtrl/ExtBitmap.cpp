#include "StdAfx.h"
#include ".\ExtBitmap.h"

CExtBitmap::CExtBitmap(void)
{
}

CExtBitmap::~CExtBitmap(void)
{
}							

bool CExtBitmap::LoadFromMemory(void* buff)
{
	ASSERT(buff);

	BITMAPFILEHEADER* const bfheader = reinterpret_cast<BITMAPFILEHEADER*>(buff);
	BITMAPINFO* const binfo = reinterpret_cast<BITMAPINFO*>(bfheader + 1);

	void* b = NULL;
	HBITMAP const h = CreateDIBSection(NULL, binfo, DIB_RGB_COLORS, &b, NULL, 0);
	if (!Attach(h))
	{
		::DeleteObject(h);
		return false;
	}
	memcpy(b, &binfo->bmiColors, 
		(binfo->bmiHeader.biWidth
		*binfo->bmiHeader.biHeight
		*binfo->bmiHeader.biBitCount)/8);
	return true;
}