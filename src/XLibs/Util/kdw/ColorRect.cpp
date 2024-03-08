#include "StdAfx.h"
#include "XTL/SafeCast.h"
#include "kdw/ColorRect.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/MemoryDC.h"

namespace kdw{

class ColorRectImpl : public _WidgetWindow{
public:
	const char* className() const{ return "kdw.ColorRect"; }
	ColorRectImpl(ColorRect* owner);

	BOOL onMessageEraseBkgnd(HDC);
	void onMessagePaint();

	void redraw(HDC dc);
protected:
	ColorRect* owner_;
};

ColorRectImpl::ColorRectImpl(ColorRect* owner)
: _WidgetWindow(owner)
, owner_(owner)
{
	VERIFY(create("", WS_CHILD, Recti(0, 0, 10, 10), *Win32::_globalDummyWindow));
}


void ColorRectImpl::redraw(HDC dc)
{
	RECT rect;
	GetClientRect(*this, &rect);
	::FillRect(dc, &rect, GetSysColorBrush(COLOR_BTNFACE));

	int roundness = 7;
	::SelectObject(dc, GetStockObject(NULL_PEN));
	::SelectObject(dc, GetSysColorBrush(COLOR_3DSHADOW));
	::RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, roundness, roundness);
	
	HRGN region = CreateRoundRectRgn(rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, roundness, roundness - 2);
	::SelectClipRgn(dc, region);

	Color4c c(owner_->color_);
	HBRUSH brush = CreateSolidBrush(RGB(c.r, c.g, c.b));

	RECT filledRect = { rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1};
	
	Color4f black;
	black.interpolate3(Color4f(0.0f, 0.0f, 0.0f, 1.0f), owner_->get(), owner_->get().a);
	Color4f white;
	white.interpolate3(Color4f(1.0f, 1.0f, 1.0f, 1.0f), owner_->get(), owner_->get().a);
	float w = (filledRect.right - filledRect.left) * 0.25f;
	float h = (filledRect.bottom - filledRect.top) * 0.25f;

	HBRUSH blackBrush = CreateSolidBrush(black.RGBGDI());
	HBRUSH whiteBrush = CreateSolidBrush(white.RGBGDI());

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 2; ++j) {
			RECT rect = { round(i * w), round(j * h + h * 2), round((i + 1) * w), round ((j + 1) * h + h * 2) };
			FillRect(dc, &rect, ((i + j) % 2) ? whiteBrush : blackBrush);
		}
	}

	RECT r = { filledRect.left, filledRect.top, filledRect.left + round(w * 4), filledRect.top + round(h * 2) };
	::FillRect(dc, &r, brush);

	::SelectClipRgn(dc, 0);
	DeleteObject(blackBrush);
	DeleteObject(whiteBrush);
	DeleteObject(brush);
	DeleteObject(region);
}

void ColorRectImpl::onMessagePaint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(*this, &ps);
	{
		Win32::MemoryDC memoryDC(dc);
		redraw(memoryDC);
	}
	EndPaint(*this, &ps);
}

BOOL ColorRectImpl::onMessageEraseBkgnd(HDC)
{
	return FALSE;
}

// ---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
ColorRect::ColorRect(Color4f color, int border)
: _WidgetWithWindow(new ColorRectImpl(this), border)
{
	_setMinimalSize(Vect2i(40, 40));
}
#pragma warning(pop)

void ColorRect::set(const Color4f& color)
{
	color_ = color;
	if(::IsWindowVisible(impl()))
		RedrawWindow(impl(), 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
}


ColorRect::~ColorRect()
{

}

ColorRectImpl& ColorRect::impl()
{
	return *safe_cast<ColorRectImpl*>(_window());
}

}
