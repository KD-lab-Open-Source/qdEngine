#ifndef __KDW_COLOR_RAMP_H_INCLUDED__
#define __KDW_COLOR_RAMP_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include "XMath/Colors.h"
namespace kdw{

class ColorRampImpl;
class ColorRamp : public _WidgetWithWindow{
public:
	ColorRamp(int border = 0);

	sigslot::signal0& signalChanged(){ return signalChanged_; }

	void set(const Color4f& color);
	const Color4f& get() const{ return color_; }
protected:
	ColorRampImpl& impl();
	sigslot::signal0 signalChanged_;

	Color4f color_;
	friend class ColorRampImpl;
};

}

#endif
