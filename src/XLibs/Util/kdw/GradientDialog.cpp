#include "StdAfx.h"
#include "kdw/ColorRect.h"
#include "kdw/GradientDialog.h"
#include "kdw/GradientBar.h"
#include "kdw/HBox.h"
#include "kdw/VBox.h"
#include "kdw/Button.h"
#include "kdw/Label.h"
#include "kdw/Entry.h"
#include "kdw/Slider.h"
#include "Render/src/NParticle.h"

namespace kdw{

GradientDialog::GradientDialog(Widget* parent, const KeysColor& gradient, PositionStyle positionMode, int gradientStyle)
: Dialog(parent)
, gradientBar_(0)
, gradient_(*new KeysColor())
{
	for(int i = 0; i < 4; ++i){
		entries_[i] = 0;
		sliders_[i] = 0;
	}

	setTitle("Gradient");
	setResizeable(true);
	setDefaultSize(Vect2i(720, 150));

	addButton(TRANSLATE("ОК"), RESPONSE_OK);
	addButton(TRANSLATE("Отмена"), RESPONSE_CANCEL);

	VBox* vbox = new VBox(4);
	add(vbox); {

		gradientBar_ = new GradientBar;
		gradientBar_->setRequestSize(Vect2i(0, 60));
		gradientBar_->setAllowAddPoints((gradientStyle & GRADIENT_STYLE_FIXED_POINTS_COUNT) == 0);
		gradientBar_->setHasAlpha((gradientStyle & GRADIENT_STYLE_NO_ALPHA) == 0);
		gradientBar_->setHasColor((gradientStyle & GRADIENT_STYLE_NO_COLOR) == 0);
		gradientBar_->setCycled((gradientStyle & GRADIENT_STYLE_CYCLED) != 0);
		gradientBar_->signalChanged().connect(this, &GradientDialog::onGradientChanged);
		vbox->add(gradientBar_);
		
		HBox* hbox = new HBox(4);
		vbox->add(hbox); {

			/*
			ColorRect* colorRect = new ColorRect;
			hbox->add(colorRect);

			VBox* channelsBox = new VBox();
			hbox->add(channelsBox, false, true, true); {
				const char* labels[] = { "Red", "Green", "Blue", "Alpha" };
				for(int i = 0; i < 4; ++i){
					HBox* box = new HBox(4);
					channelsBox->add(box);{

						Label* label = new Label();
						label->setRequestSize(Vect2i(50, 0));
						label->setText(labels[i]);
						box->add(label);

						entries_[i] = new Entry;
						entries_[i]->setRequestSize(Vect2i(60, 0));
						box->add(entries_[i]);

						sliders_[i] = new Slider;
						box->add(sliders_[i], false, true, true);
					}
				}

				VBox* positionBox = new VBox();
				hbox->add(positionBox); {
					HBox* box = new HBox();
					positionBox->add(box, false, true, true);{
						Button* prevButton = new Button();
						box->add(prevButton, false, false, false);

						Label* label = new Label();
						box->add(label, false, true, true);

						Button* nextButton = new Button();
						box->add(nextButton, false, false, false);
					}
				}
			}
			*/

		}


	}

	set(gradient);
}


GradientDialog::~GradientDialog()
{
	delete &gradient_;
}

void GradientDialog::onGradientChanged()
{
	gradient_ = gradientBar_->get();
}

void GradientDialog::onSliderChange()
{
	
}

void GradientDialog::onEntryChange()
{

}

void GradientDialog::onPositionChange()
{

}

void GradientDialog::set(const KeysColor& gradient)
{
	gradient_ = gradient;
	gradientBar_->set(gradient);
}

const KeysColor& GradientDialog::get() const
{
	return gradient_;
}

}
