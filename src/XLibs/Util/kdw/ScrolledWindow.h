#ifndef __KDW_SCROLLED_WINDOW_H_INCLUDED__
#define __KDW_SCROLLED_WINDOW_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"

namespace Win32{ class Window; }

namespace kdw{
class ScrolledWindowImpl;

enum ScrollPolicy{
	SCROLL_AUTOMATIC,
	SCROLL_ALWAYS,
    SCROLL_NEVER
};

class ScrolledWindow: public _ContainerWithWindow{
public:
	ScrolledWindow(int border = 0);
	~ScrolledWindow();

	void add(Widget* child);
	void remove();
	void visitChildren(WidgetVisitor& visitor) const;

	void setPolicy(ScrollPolicy horizontalPolicy = SCROLL_AUTOMATIC, ScrollPolicy verticalPolicy = SCROLL_AUTOMATIC);
	ScrollPolicy policyHorizontal() const{ return policyHorizontal_; }
	ScrollPolicy policyVertical() const{ return policyVertical_; }

	void serialize(Archive& ar);

	void _arrangeChildren();
	void _relayoutParents();
	Widget* _nextWidget(Widget* last, FocusDirection direction);
protected:
	ScrolledWindowImpl* impl();	

	Vect2i clientAreaSize() const;
	bool verticalScrollVisible() const;
	bool horizontalScrollVisible() const;

	ShareHandle<Widget> child_;

	ScrollPolicy policyHorizontal_;
	ScrollPolicy policyVertical_;

	Vect2i offset_;

	friend class ScrolledWindowImpl;
};

}

#endif
// vim:noet: