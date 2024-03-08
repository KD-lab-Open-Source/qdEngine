#ifndef __KDW_CONTAINER_H_INCLUDED__
#define __KDW_CONTAINER_H_INCLUDED__

#include "kdw/Widget.h"

namespace kdw{

class KDW_API Container : public Widget{
public:
	// virtuals:
	bool isVisible() const;
	virtual bool isActive() const;
	void setBorder(int border);
	// ^^^

	// методы для внутреннего пользования:
	virtual void _arrangeChildren() {}
	void _setFocus();
	Widget* _nextWidget(Widget* last, FocusDirection direction);
	bool _focusable() const;
protected:
};

}

#endif
