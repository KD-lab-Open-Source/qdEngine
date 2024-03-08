#ifndef __LAYOUT_M_F_C_H_INCLUDED__
#define __LAYOUT_M_F_C_H_INCLUDED__

#include "Layout.h"

class CWnd;

class LayoutWindow : public LayoutElement{
public:
	LayoutWindow(CWnd* window);

	void add(LayoutElement* children);
	void relayout(bool redraw = true);

	bool empty() const { return children_ == 0; }
private:
	CWnd* window_;

	ShareHandle<LayoutElement> children_;
};

class LayoutControl : public LayoutElement{
public:
	LayoutControl(CWnd* control, int border = 0);
	LayoutControl(UINT id, CDialog* dialog, int border = 0);

	void setPosition(const Recti& rect);
private:
	CWnd* window_;
	CDialog* dialog_;
	UINT id_;
};

#endif
