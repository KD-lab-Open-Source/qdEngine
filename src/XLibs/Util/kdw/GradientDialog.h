#ifndef __GRADIENT_DIALOG_H_INCLUDED__
#define __GRADIENT_DIALOG_H_INCLUDED__

#include "kdw/Dialog.h"

class KeysColor;

namespace kdw{

class Entry;
class Slider;
class GradientBar;
class Widget;

enum PositionStyle{
	GRADIENT_POSITION_TIME,
	GRADIENT_POSITION_BYTE,
	GRADIENT_POSITION_FLOAT
};

enum{
	GRADIENT_STYLE_NO_ALPHA = 1 << 0,
	GRADIENT_STYLE_NO_COLOR = 1 << 1,
	GRADIENT_STYLE_FIXED_POINTS_COUNT = 1 << 2,
	GRADIENT_STYLE_CYCLED = 1 << 3
};

class GradientDialog : public Dialog{
public:
	GradientDialog(Widget* parent, const KeysColor& gradient, PositionStyle positionMode = GRADIENT_POSITION_FLOAT, int gradientStyle = 0);
	~GradientDialog();

	void onGradientChanged();
	void onSliderChange();
	void onEntryChange();
	void onPositionChange();

	void set(const KeysColor& gradient);
	const KeysColor& get() const;
protected:
	KeysColor& gradient_;
	GradientBar* gradientBar_;
	Entry* entries_[4];
	Slider* sliders_[4];
};

}

#endif 
