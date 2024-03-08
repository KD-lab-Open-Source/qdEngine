#ifndef __KDW_TOOLTIP_H_INCLUDED__
#define __KDW_TOOLTIP_H_INCLUDED__

namespace kdw{

class Widget;

class KDW_API Tooltip{
public:
	Tooltip(const char* text = "", bool baloon = false);
	void attach(Widget* widget);

	void setText(const char* text);
	const char* text() const { return text_.c_str(); }
	void show();
	void hide();

	void setBaloon(bool baloon);
	void setOffset(const Vect2i& offset) { offset_ = offset; }

protected:
	string text_;
	bool baloon_;
	Widget* widget_;
	HWND toolTipWindow_;
	Vect2i offset_;
};

}
#endif
