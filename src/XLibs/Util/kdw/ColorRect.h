#ifndef __KDW_COLOR_RECT_H_INCLUDED__
#define __KDW_COLOR_RECT_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include "XMath/Colors.h"

namespace kdw{

class ColorRectImpl;
class ColorRect : public _WidgetWithWindow{
public:
	ColorRect(Color4f color = Color4f(1.0f, 1.0f, 1.0f, 1.0f), int border = 0);
	~ColorRect();

	void set(const Color4f& color);
	const Color4f& get() const{ return color_; }
	sigslot::signal0& signalActivate(){ return signalActivate_; }
protected:
	ColorRectImpl& impl();
	sigslot::signal0 signalActivate_;
	Color4f color_;
	friend class ColorRectImpl;
};

}

#endif
