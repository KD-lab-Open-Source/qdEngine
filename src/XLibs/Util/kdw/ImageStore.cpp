#include "stdafx.h"
#include "kdw/ImageStore.h"
#include <windows.h>
#include <CommCtrl.h>

#include "XMath/Colors.h"
#include "kdw/Win32/Window.h"

#pragma message("Automatically linking with comctl32.lib") 
#pragma comment(lib, "comctl32.lib") 

namespace kdw{

ImageStore::ImageStore(int cx, int cy, const char* resourceName, unsigned int maskColor)
{
	InitCommonControls();
	
	imageList_ = ImageList_Create(cx, cy, ILC_COLOR32 | ILC_MASK, 0, 16);
	ASSERT(imageList_ != NULL);
	imageListGray_ = ImageList_Create(cx, cy, ILC_COLOR32 | ILC_MASK, 0, 16);
	ASSERT(imageListGray_ != NULL);

	size_ = Vect2i(cx, cy);
	if(resourceName)
		addFromResource(resourceName, maskColor);
}

void ImageStore::_createFromBitmap(HBITMAP bitmap, unsigned int color)
{
	BITMAP bitmapStruct;
	ZeroMemory(&bitmapStruct, sizeof(bitmapStruct));

	VERIFY(GetObject(bitmap, sizeof(bitmapStruct), &bitmapStruct));
	xassert(bitmapStruct.bmBitsPixel == 32);

	typedef Color4c ColorType;
	int count = bitmapStruct.bmWidth * bitmapStruct.bmHeight;
	ColorType* source = new ColorType[count];
	ColorType* dest = new ColorType[count];
	GetBitmapBits(bitmap, sizeof(ColorType) * count, source);

	int width = bitmapStruct.bmWidth;
	int height = bitmapStruct.bmHeight;
	for(int y = 0; y < height; ++y)
		for(int x = 0; x < width; ++x){
			ColorType c = source[y * width + x];
			if(color != RGB(c.r, c.g, c.b)){
				unsigned char lum = round((c.r * 0.30f + c.g * 0.59f + c.b * 0.11f) * 0.5f + 96);
				dest[y * width + x].set(lum, lum, lum);
			}
			else{
				dest[y * width + x] = c;
			}
		}
	bitmapStruct.bmBits = dest;

	bitmapGray_ = CreateBitmapIndirect(&bitmapStruct);
	xassert(bitmapGray_);
	VERIFY(ImageList_AddMasked(imageList_, bitmap, color) != -1);
	VERIFY(ImageList_AddMasked(imageListGray_, bitmapGray_, color) != -1);
	delete[] dest;
	delete[] source;
}

void ImageStore::addFromResource(const char* bitmapID, unsigned int color)
{
	HBITMAP bitmap = LoadBitmap(Win32::_globalInstance(), bitmapID);
	xassert(bitmap);
	_createFromBitmap(bitmap, color);
}

void ImageStore::addFromFile(const char* fileName, unsigned int color)
{
	HBITMAP bitmap = (HBITMAP)LoadImage(Win32::_globalInstance(), fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	xassert(bitmap);
	_createFromBitmap(bitmap, color);
}

ImageStore::~ImageStore()
{
	ImageList_Destroy(imageList_);
}

void ImageStore::_draw(int i, HDC destDC, int x, int y, bool disabled)
{
	VERIFY(ImageList_Draw(disabled ? imageListGray_ : imageList_, i, destDC, x, y, ILD_NORMAL));
}

}
