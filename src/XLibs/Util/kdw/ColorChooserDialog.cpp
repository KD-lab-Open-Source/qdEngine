#include "StdAfx.h"
#include "kdw/ColorChooserDialog.h"
#include "kdw/ColorChooser.h"


namespace kdw{

ColorChooserDialog::ColorChooserDialog(kdw::Widget* parent, const Color4f& color, bool showColor, bool showAlpha, int border)
: Dialog(parent, border)
{
	xassert(showColor || showAlpha);

	if(showColor)
		setTitle(TRANSLATE("Âûáîğ öâåòà"));
	else
		setTitle(TRANSLATE("Âûáîğ àëüôû"));

	if(showColor)
		setDefaultSize(Vect2i(350, 400));
	else
		setDefaultSize(Vect2i(400, 0));
	setResizeable(true);
	
	chooser_ = new ColorChooser();
	chooser_->signalChanged().connect(this, &ColorChooserDialog::onChooserChanged);
	chooser_->setShowColor(showColor);
	chooser_->setShowAlpha(showAlpha);
	add(chooser_);

	addButton(TRANSLATE("ÎÊ"),     RESPONSE_OK);
	addButton(TRANSLATE("Îòìåíà"), RESPONSE_CANCEL);

	set(color);
}

void ColorChooserDialog::set(const Color4f& color)
{
	color_ = color;
	chooser_->set(color);
}

void ColorChooserDialog::onChooserChanged()
{
	color_ = chooser_->get();
}

}
