#include "StdAfx.h"

#include "kdw/Win32/MemoryDC.h"
#include <windows.h>
#include <windowsx.h>

namespace Win32{

MemoryDC::MemoryDC(HDC dc)
: destinationDC_(dc)
, dc_(0)
{
	VERIFY(GetClipBox(destinationDC_, &rect_) != ERROR);
	VERIFY(dc_ = ::CreateCompatibleDC(destinationDC_));
	//VERIFY(bitmap_ = ::CreateCompatibleBitmap(dc, rect_.right - rect_.left, rect_.bottom - rect_.top));
	VERIFY(bitmap_ = ::CreateCompatibleBitmap(dc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
	oldBitmap_ = (HBITMAP)::SelectObject(dc_, (HGDIOBJ)bitmap_);

/*

	GetBoundsRect(destinationDC_, &rect_, 0);
	VERIFY(dc_ = ::CreateCompatibleDC(destinationDC_));
	VERIFY(bitmap_ = ::CreateCompatibleBitmap(dc, rect_.right - rect_.left, rect_.bottom - rect_.top));
	oldBitmap_ = (HBITMAP)::SelectObject(dc_, (HGDIOBJ)bitmap_);
*/
	//SetViewportOrgEx(dc_, -rect_.left, -rect_.top, 0); 
}

MemoryDC::~MemoryDC()
{
	VERIFY(::BitBlt(destinationDC_, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			 dc_, 0, 0, SRCCOPY));
	::SelectObject(dc_, (HGDIOBJ)oldBitmap_);
	::DeleteDC(dc_);
	::DeleteObject(bitmap_);
}

}