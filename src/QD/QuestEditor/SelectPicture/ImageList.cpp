// ImageList.cpp: implementation of the ImageList class.
//
//////////////////////////////////////////////////////////////////////
#pragma  warning(disable : 4786)
#include "stdafx.h"
#include "ImageList.h"
#include <xutil.h>
#include <additional//auto_object.h>
#include <additional//object_eraser.h>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VOID ImageList::DrawImage(HDC dc, RECT* rc, int idx, COLORREF dest_color)
{
	const IMAGE_INFO& ii = m_pictures[idx];
	if (ii.bUseAlpha)
		DrawPicWithAlpha(dc, rc, ii,dest_color);
	else
		DrawPic(dc, rc, ii);
}

inline COLORREF get_alpha(COLORREF c){
	return (c>>24);
}

inline COLORREF apply_alpha(COLORREF src, COLORREF dest)
{
	COLORREF alpha = get_alpha(src);
	if (alpha == 255ul) return src;
	COLORREF r = (((255 - alpha)*(dest&0xFF))>>8)+((alpha*(src&0xff))>>8);
	COLORREF g = (((255 - alpha)*((dest>>8)&0xFF))>>8)+((alpha*((src>>8)&0xff))>>8);
	COLORREF b = (((255 - alpha)*((dest>>16)&0xFF))>>8)+((alpha*((src>>16)&0xff))>>8);
	
	return RGB(r,g,b);
}

VOID ImageList::DrawPicWithAlpha(HDC dc, RECT* rc, const IMAGE_INFO& ii, COLORREF dest_color)
{
	HBITMAP hbmp = ii.hbmp;
	ASSERT(hbmp);
	
	axl::auto_object<HDC> hdc(CreateCompatibleDC(dc));
	if (!hdc.get())
		return;
	
	BITMAP src_data;
	if (!GetObject(hbmp, sizeof(src_data), &src_data))
		return;
	
	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biBitCount = src_data.bmBitsPixel;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biWidth = src_data.bmWidth;
	bi.bmiHeader.biHeight = -src_data.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biSizeImage = src_data.bmHeight * src_data.bmWidth;
	bi.bmiHeader.biXPelsPerMeter = src_data.bmWidth;
	bi.bmiHeader.biYPelsPerMeter = src_data.bmHeight;
	bi.bmiHeader.biClrImportant = 0;
	
	COLORREF* dst_clrs = NULL;
	axl::auto_object<HBITMAP> hDestBmp
		(CreateDIBSection(hdc.get(), &bi, DIB_RGB_COLORS,(void**)&dst_clrs, NULL, 0));
	if (!hDestBmp.get())
		return;
	
	COLORREF* src_clrs = reinterpret_cast<COLORREF*>(src_data.bmBits);
	int clrs_size = bi.bmiHeader.biSizeImage;
	
	for(int i = clrs_size; --i>=0; )
		dst_clrs[i] = apply_alpha(src_clrs[i], dest_color);
	
	HGDIOBJ oldbmp = ::SelectObject(hdc.get(), hDestBmp.get());
	
	int left = rc->left;
	if (rc->right - rc->left > ii.dims.cx)
		left = ((rc->right + rc->left)>>1) - (ii.dims.cx>>1);
	int top = rc->top;
	if (rc->bottom - rc->top > ii.dims.cy)
		top = ((rc->bottom + rc->top)>>1) - (ii.dims.cy>>1);
	
	BitBlt(dc, left, top, ii.dims.cx, ii.dims.cy, hdc.get(), 0, 0, SRCCOPY);
	::SelectObject(hdc.get(), oldbmp);
}

VOID ImageList::DrawPic(HDC dc, RECT *rc, const IMAGE_INFO& ii)
{
	axl::auto_object<HDC> hdc(CreateCompatibleDC(dc));
	HGDIOBJ oldbmp = ::SelectObject(hdc.get(), ii.hbmp);
	
	int left = rc->left;
	if (rc->right - rc->left > ii.dims.cx)
		left = ((rc->right + rc->left)>>1) - (ii.dims.cx>>1);

	int top = rc->top;
	if (rc->bottom - rc->top > ii.dims.cy)
		top = ((rc->bottom + rc->top)>>1) - (ii.dims.cy>>1);
	
	BitBlt(dc, left, top, ii.dims.cx, ii.dims.cy, hdc.get(), 0, 0, SRCCOPY);
	::SelectObject(hdc.get(), oldbmp);
}

int ImageList::remove_image(int iImage){

	IMAGE_INFO& ii = m_pictures[iImage];
	bool bNeedSizeUpdate = (ii.dims.cx == m_max_pic_size.cx || 
							ii.dims.cy == m_max_pic_size.cy);
	DeleteObject(ii.hbmp);
	bitmap_vec_t::iterator res = m_pictures.erase(m_pictures.begin() + iImage);
//	if (bNeedSizeUpdate) 
		update_pic_size();

	if (res == m_pictures.end()) 
		return -1;
	return res - m_pictures.begin();
}

void ImageList::update_pic_size(){
	bitmap_vec_t::iterator i = m_pictures.begin(), e = m_pictures.end();
	m_max_pic_size.cx = m_max_pic_size.cy = 0;
	for(; i != e; ++i)
	{
		IMAGE_INFO& ii = *i;
		if (m_max_pic_size.cx < ii.dims.cx) m_max_pic_size.cx = ii.dims.cx; 
		if (m_max_pic_size.cy < ii.dims.cy) m_max_pic_size.cy = ii.dims.cy; 
	}
}