#ifndef __KDW_WIN32_DRAWING_H_INCLUDED__
#define __KDW_WIN32_DRAWING_H_INCLUDED__

#include "kdw/Win32/Types.h"
#include "XMath/Colors.h"
#include "XTL/Rect.h"

class KeysColor;


namespace Win32{
	inline COLORREF toColorRef(const Color4f& color){
		return COLORREF(((BYTE)(round(color.r * 255.0f))|
			     ((WORD)((BYTE)(round(color.g * 255.0f)))<<8))|
				(((DWORD)(BYTE)(round(color.b * 255.0f)))<<16));
	}
	inline Color4c toColor4c(COLORREF c){
		return Color4c((c & 0x000000FF), (c & 0x0000FF00) >> 8, (c & 0x00FF0000) >> 16);
	}
	inline COLORREF blendColor(COLORREF from, COLORREF to, float factor)
	{
		Color4c c;
		c.interpolate(toColor4c(from), toColor4c(to), factor);
		return c.RGBGDI();
	}
	// void drawGradient(HDC dc, const RECT& rect, KeysColor& gradient, bool alphaOnly = false);
	void drawColorBlend(HDC dc, const Recti& rect, const Color4f& color1, const Color4f& color2, bool alphaOnly = false);

	void drawVerticalBlend(HDC dc, const RECT& rect, COLORREF color1, COLORREF color2);

	void drawGrayedCheck(HDC dc, const RECT& iconRect);
	void drawNotCheck(HDC dc, const RECT& rect, bool checked);
	void drawCheck(HDC dc, const RECT& rect, bool checked);
	void drawEdit(HDC dc, const RECT& rect, const char* text, HFONT font, bool pathEllipsis = false, bool grayBackground = false);
	void drawButton(HDC dc, const RECT& rect, const char* text, HFONT font);
	void drawComboBox(HDC dc, const RECT& rect, const char* text, HFONT font);
	void drawSlider(HDC dc, const RECT& rect, float value, bool focused);
}

#endif
