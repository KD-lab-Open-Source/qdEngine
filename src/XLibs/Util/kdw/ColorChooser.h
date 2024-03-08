#ifndef __KDW_COLOR_CHOOSER_H_INCLUDED__
#define __KDW_COLOR_CHOOSER_H_INCLUDED__

#include "kdw/VBox.h"
#include "XMath/Colors.h"

namespace kdw{

class ColorRamp;
class ColorRect;
class Entry;
class Slider;

class ColorChooser : public VBox{
public:
	ColorChooser(int border = 0);

	void set(const Color4f& color);
	const Color4f& get() const{ return color_; }

	void setShowColor(bool showColor);
	bool showColor()const{ return showColor_; }
	void setShowAlpha(bool showAlpha);
	bool showAlpha()const{ return showAlpha_; }

	sigslot::signal0& signalChanged() { return signalChanged_; }
protected:
	sigslot::signal0 signalChanged_;
	void relayout();
	void onRampChanged();
	void onSliderChanged();
	void onEntryChanged();
	void updateEntries(const Color4f& color);
	void updateSliders(const Color4f& color);
	
	bool showColor_;
	bool showAlpha_;

	Entry* entryRed_;
	Entry* entryGreen_;
	Entry* entryBlue_;
	Entry* entryAlpha_;

	Slider* sliderRed_;
	Slider* sliderGreen_;
	Slider* sliderBlue_;
	Slider* sliderAlpha_;

	ColorRamp* ramp_;
	ColorRect* colorRect_;
	Color4f color_;
};

}

#endif
