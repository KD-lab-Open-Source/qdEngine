#include "StdAfx.h"
#include "kdw/ColorRamp.h"
#include "kdw/ColorChooser.h"
#include "kdw/HBox.h"
#include "kdw/VBox.h"
#include "kdw/Slider.h"
#include "kdw/ColorRect.h"
#include "kdw/Label.h"
#include "kdw/Entry.h"

namespace kdw{

ColorChooser::ColorChooser(int border)
: VBox(5, border)
, showColor_(true)
, showAlpha_(true)
{
	relayout();
}

void ColorChooser::relayout()
{
	clear();

	sliderRed_ = 0;
	sliderGreen_ = 0;
	sliderBlue_ = 0;
	sliderAlpha_ = 0;
	entryRed_ = 0;
	entryGreen_ = 0;
	entryBlue_ = 0;
	entryAlpha_ = 0;
	ramp_ = 0;

	if(showColor_){
		ramp_ = new ColorRamp();
		ramp_->signalChanged().connect(this, &ColorChooser::onRampChanged);
		add(ramp_, false, true, true);
	}
	
	HBox* hbox = new HBox(5);
	add(hbox, false, false, false);
	{
        colorRect_ = new ColorRect();
		colorRect_->setRequestSize(Vect2i(64, 64));
		colorRect_->set(color_);
		hbox->add(colorRect_);

		VBox* vbox = new VBox();
		hbox->add(vbox, false, false, false);
		{
			if(showColor_){
				vbox->add(new Label("R"), false, true, true);
				vbox->add(new Label("G"), false, true, true);
				vbox->add(new Label("B"), false, true, true);
			}
			if(showAlpha_){
				vbox->add(new Label("A"), false, true, true);
			}
		}
		vbox = new VBox();
		hbox->add(vbox, false, true, true);
		{
			if(showColor_){
				sliderRed_ = new Slider();
				sliderRed_->setStepsCount(256);
				sliderRed_->signalChanged().connect(this, &ColorChooser::onSliderChanged);
				vbox->add(sliderRed_, false, true, true);
				sliderGreen_ = new Slider();
				sliderGreen_->setStepsCount(256);
				sliderGreen_->signalChanged().connect(this, &ColorChooser::onSliderChanged);
				vbox->add(sliderGreen_, false, true, true);
				sliderBlue_ = new Slider();
				sliderBlue_->setStepsCount(256);
				sliderBlue_->signalChanged().connect(this, &ColorChooser::onSliderChanged);
				vbox->add(sliderBlue_, false, true, true);
			}
			if(showAlpha_){
				sliderAlpha_ = new Slider();
				sliderAlpha_->setStepsCount(256);
				sliderAlpha_->signalChanged().connect(this, &ColorChooser::onSliderChanged);
				vbox->add(sliderAlpha_, false, true, true);
			}
		}
		vbox = new VBox();
		hbox->add(vbox, false, false, false);
		vbox->setRequestSize(Vect2i(50, 10));
		{
			if(showColor_){
				entryRed_ = new Entry();
				entryRed_->signalChanged().connect(this, &ColorChooser::onEntryChanged);
				entryRed_->setTextAlign(ALIGN_CENTER);
				vbox->add(entryRed_, false, true, true);

				entryGreen_ = new Entry();
				entryGreen_->signalChanged().connect(this, &ColorChooser::onEntryChanged);
				entryGreen_->setTextAlign(ALIGN_CENTER);
				vbox->add(entryGreen_, false, true, true);

				entryBlue_ = new Entry();
				entryBlue_->signalChanged().connect(this, &ColorChooser::onEntryChanged);
				entryBlue_->setTextAlign(ALIGN_CENTER);
				vbox->add(entryBlue_, false, true, true);
			}
			if(showAlpha_){
				entryAlpha_ = new Entry();
				entryAlpha_->signalChanged().connect(this, &ColorChooser::onEntryChanged);
				entryAlpha_->setTextAlign(ALIGN_CENTER);
				vbox->add(entryAlpha_, false, true, true);
			}
		}
	}
}

void ColorChooser::setShowColor(bool showColor)
{
	showColor_ = showColor;
	relayout();
}

void ColorChooser::setShowAlpha(bool showAlpha)
{
	showAlpha_ = showAlpha;
	relayout();
}

void ColorChooser::set(const Color4f& color)
{
	color_ = color;
	if(ramp_)
		ramp_->set(color);
	colorRect_->set(color);
	updateEntries(color);
	updateSliders(color);
}

static void setEntryValue(Entry* entry, float value)
{
	XBuffer buf;
	buf <= round(value * 255);
	entry->setText(buf);
}

void ColorChooser::updateEntries(const Color4f& color)
{
	if(showColor_){
		setEntryValue(entryRed_, color.r);
		setEntryValue(entryGreen_, color.g);
		setEntryValue(entryBlue_, color.b);
	}
	if(showAlpha_){
		setEntryValue(entryAlpha_, color.a);
	}
}

void ColorChooser::updateSliders(const Color4f& color)
{
	if(showColor_){
		sliderRed_->setValue(color.r);
		sliderGreen_->setValue(color.g);
		sliderBlue_->setValue(color.b);
	}
	if(showAlpha_){
		sliderAlpha_->setValue(color.a);
	}
}

void ColorChooser::onRampChanged()
{
	xassert(ramp_);
	color_ = Color4f(ramp_->get().r, ramp_->get().g, ramp_->get().b, color_.a);
	updateSliders(color_);
	updateEntries(color_);
	colorRect_->set(color_);
	signalChanged_.emit();
}

void ColorChooser::onSliderChanged()
{
	if(showColor_){
		color_.r = sliderRed_->value();
		color_.g = sliderGreen_->value();
		color_.b = sliderBlue_->value();
	}
	if(showAlpha_){
		color_.a = sliderAlpha_->value();
	}
	if(ramp_)
		ramp_->set(color_);
	colorRect_->set(color_);
	updateEntries(color_);
	signalChanged_.emit();
}

void ColorChooser::onEntryChanged()
{
	if(showColor_){
		color_.r = clamp(atof(entryRed_->text()) / 255.0f, 0.0f, 1.0f);
		color_.g = clamp(atof(entryGreen_->text()) / 255.0f, 0.0f, 1.0f);
		color_.b = clamp(atof(entryBlue_->text()) / 255.0f, 0.0f, 1.0f);
	}
	if(showAlpha_){
		color_.a = clamp(atof(entryAlpha_->text()) / 255.0f, 0.0f, 1.0f);
	}
	if(ramp_)
		ramp_->set(color_);
	colorRect_->set(color_);
	updateSliders(color_);
	signalChanged_.emit();
}

}
