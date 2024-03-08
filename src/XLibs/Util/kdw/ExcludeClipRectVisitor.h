#ifndef __KDW_EXCLUDE_CLIP_RECT_VISITOR_H_INCLUDED__
#define __KDW_EXCLUDE_CLIP_RECT_VISITOR_H_INCLUDED__

#include "kdw/Widget.h"
#include "kdw/Win32/Window.h"
#include "kdw/Win32/Rectangle.h"

struct ExcludeClipRectVisitor : public kdw::WidgetVisitor{
	HDC dc_;
	ExcludeClipRectVisitor(HDC dc)
	: dc_(dc)
	{
	}

	void operator()(kdw::Widget& widget){
		if(Win32::Window32* window = widget._window()){
			if(widget._visible() && widget._visibleInLayout() && ::IsWindowVisible(*window)){
				Win32::Rect rect;
				::GetClientRect(*window, &rect);
				window->clientToScreen(rect);
				window->parent()->screenToClient(rect);
				::ExcludeClipRect(dc_, rect.left, rect.top, rect.right, rect.bottom);
			}
		}
		else
			widget.visitChildren(*this);
	}
};

#endif
