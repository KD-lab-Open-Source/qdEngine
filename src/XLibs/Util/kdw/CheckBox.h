#ifndef __KDW_CHECKBOX_H_INCLUDED__
#define __KDW_CHECKBOX_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include <string>


namespace kdw{

class CheckBoxImpl;
class KDW_API CheckBox : public _WidgetWithWindow{
public:
	CheckBox(const char* text = "CheckBox", int border = 0);

	const char* text() const { return text_.c_str(); }
	void setText(const char* text);
	void setStatus(bool status);

	void setButtonLike(bool buttonLike);
	bool buttonLike() const{ return buttonLike_; }

	bool status() const { return status_; }

	virtual void onChanged();
	sigslot::signal0& signalChanged() { return signalChanged_; }

	void serialize(Archive& ar);
protected:
	// внутренние функции
	CheckBoxImpl* window() const{ return reinterpret_cast<CheckBoxImpl*>(_window()); }

	sigslot::signal0 signalChanged_;
	std::string text_;
	bool status_;
	bool buttonLike_;

	friend class CheckBoxImpl;
};

}

#endif
