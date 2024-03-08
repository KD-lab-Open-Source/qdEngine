#ifndef __KDW_GRADIENT_BAR_H_INCLUDED__
#define __KDW_GRADIENT_BAR_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include "Render/src/NParticle.h"

namespace kdw{

class GradientBarImpl;
class GradientBar : public _WidgetWithWindow{
public:
	GradientBar(int border = 0);
	~GradientBar();

	void set(const KeysColor& gradient);
	const KeysColor& get() const{ return gradient_; }

	sigslot::signal0& signalChanged(){ return signalChanged_; }
	void setAllowAddPoints(bool allow){ allowAddPoints_ = allow; }
	void setHasColor(bool hasColor){ hasColor_ = hasColor; }
	void setHasAlpha(bool hasAlpha){ hasAlpha_ = hasAlpha; }
	void setCycled(bool cycled){ cycled_ = cycled; }
protected:
	sigslot::signal0 signalChanged_;
	friend class GradientBarImpl;
	KeysColor gradient_;

	bool cycled_;
	bool hasColor_;
	bool hasAlpha_;
	bool allowAddPoints_;

	GradientBarImpl& impl();
	friend class GradientBarImpl;
};

}

#endif
