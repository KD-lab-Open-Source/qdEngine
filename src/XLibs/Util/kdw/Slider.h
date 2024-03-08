#ifndef __KDW_SLIDER_H_INCLUDED__
#define __KDW_SLIDER_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"

namespace kdw{
class SliderImpl;

class KDW_API Slider : public _WidgetWithWindow{
public:
	Slider(int border = 0);

	void setValue(float value);
	float value() const{ return value_; }
	sigslot::signal0& signalChanged(){ return signalChanged_; }
	void setStepsCount(int stepsCount);
protected:
	sigslot::signal0 signalChanged_;
	float value_;
	int stepsCount_;

	SliderImpl& impl();
	friend class SliderImpl;
};
}

#endif
