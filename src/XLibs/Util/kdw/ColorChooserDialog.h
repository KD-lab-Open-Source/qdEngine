#ifndef __COLOR_CHOOSER_DIALOG_H_INCLUDED__
#define __COLOR_CHOOSER_DIALOG_H_INCLUDED__

#include "kdw/Dialog.h"
#include "XMath/Colors.h"

namespace kdw{

class ColorChooser;

class ColorChooserDialog : public kdw::Dialog{
public:
	ColorChooserDialog(kdw::Widget* parent, const Color4f& color = Color4f(0.0f, 0.8f, 0.0f, 1.0f), bool showColor = true, bool showAlpha = true, int border = 12);

	void set(const Color4f& color);
	const Color4f& get() const{ return color_; }
protected:
	void onChooserChanged();

	ColorChooser* chooser_;
	Color4f color_;
};

}

#endif
