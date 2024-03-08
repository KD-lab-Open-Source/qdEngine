#ifndef __KDW_IMAGESTORE_H_INCLUDED__
#define __KDW_IMAGESTORE_H_INCLUDED__

#include "kdw/API.h"
#include "kdw/Win32/Types.h"
#include "xMath/xmath.h"

namespace kdw{

class KDW_API ImageStore : public ShareHandleBase{
public:
	ImageStore(int cx, int cy, const char* resourceName = 0, unsigned int maskColor = 0);
	~ImageStore();
	void addFromResource(const char* bitmapID, unsigned int color);
	void addFromFile(const char* fileName, unsigned int color);
	Vect2i slideSize() const { return size_; }

	void _draw(int index, HDC destDC, int x, int y, bool disabled = false);
protected:
	void _createFromBitmap(HBITMAP bitmap, unsigned int color);

	HBITMAP bitmap_;
	HBITMAP bitmapGray_;
	HIMAGELIST imageList_;
	HIMAGELIST imageListGray_;
	Vect2i size_;
};

}

#endif
