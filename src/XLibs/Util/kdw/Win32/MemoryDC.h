#ifndef __KDW_WIN32_MEMORYDC_H_INCLUDED__
#define __KDW_WIN32_MEMORYDC_H_INCLUDED__

#include <windows.h>

namespace Win32{

class MemoryDC{
public:
	MemoryDC(HDC dc);
	~MemoryDC();

	operator HDC() const{ return dc_; }
protected:
	RECT rect_;
	HDC destinationDC_;
	HDC dc_;
	HBITMAP bitmap_;
	HBITMAP oldBitmap_;
};

}

#endif
