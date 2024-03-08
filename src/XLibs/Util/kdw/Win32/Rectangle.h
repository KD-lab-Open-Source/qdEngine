#ifndef __KDW_WIN32_RECT_H_INCLUDED__
#define __KDW_WIN32_RECT_H_INCLUDED__

#include <windows.h>
#include "XTL/Rect.h"
#include "XMath/XMath.h"

namespace Win32{

struct Rect : ::RECT{
	Rect()
	{}
	Rect(POINT leftTop, POINT rightBottom)
	{
		left = leftTop.x;
		top = leftTop.y;
		right = rightBottom.x;
		bottom = rightBottom.y;
	}
	Rect(Vect2i leftTop, Vect2i rightBottom)
	{
		left = leftTop.x;
		top = leftTop.y;
		right = rightBottom.x;
		bottom = rightBottom.y;
	}
	Rect(int left, int top, int right, int bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	explicit Rect(const ::Recti& rect)
	{
		left = rect.left();
		top = rect.top();
		right = rect.right();
		bottom = rect.bottom();
	}
	Rect(const RECT& rect)
	{
		left = rect.left;
		top = rect.top;
		right = rect.right;
		bottom = rect.bottom;
	}

	int width() const{ return right - left; }
	int height() const{ return bottom - top; }

	bool pointIn(Vect2i point) const{
		return point.x >= left && point.x < right &&
			   point.y >= top && point.y < bottom;
	}
	Recti recti(){
		return Recti(left, top, right-left, bottom-top);
	}
};


}

#endif
