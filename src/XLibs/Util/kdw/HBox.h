#ifndef __KDW_HBOX_H_INCLUDED__
#define __KDW_HBOX_H_INCLUDED__

#include "kdw/Box.h"

namespace kdw{

class KDW_API HBox : public Box{
public:
	HBox(int spacing = 0, int border = 0);
protected:
	Widget* _nextWidget(Widget* last, FocusDirection direction);
	void setElementPosition(Element& element, float offset, float length);
	float elementLength(const Element& element) const;
	float elementWidth(const Element& element) const;
	float boxLength() const;
	void setBoxSize(const Vect2i& size);
};

}

#endif
