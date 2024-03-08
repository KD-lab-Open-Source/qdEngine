#include "StdAfx.h"
#include "kdw/Win32Proxy.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/Rectangle.h"

namespace kdw{

class Win32ProxyImpl : public _ContainerWindow{
public:
	Win32ProxyImpl(Win32Proxy* owner, HWND parent);
	const char* className() const{ return "kdw.Win32Proxy"; }

protected:
	Win32Proxy* owner_;
};

Win32ProxyImpl::Win32ProxyImpl(Win32Proxy* owner, HWND _parent)
: _ContainerWindow(owner)
, owner_(owner)
{
	Win32::Window32 parent(_parent);
	Win32::Rect rect;
	GetClientRect(_parent, &rect);
	VERIFY(create("", WS_CHILD | WS_TABSTOP | WS_VISIBLE, rect.recti(), parent));
}

// ---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
Win32Proxy::Win32Proxy(HWND parent, int border)
: _ContainerWithWindow(new Win32ProxyImpl(this, parent), border)
{

}
#pragma warning(pop)

Win32ProxyImpl& Win32Proxy::impl()
{
	return static_cast<Win32ProxyImpl&>(*_window());
}


void Win32Proxy::_arrangeChildren()
{
	if(child_){
		RECT rect;
		VERIFY(::GetClientRect(*_window(), &rect));
		VERIFY(::InflateRect(&rect, -border_, -border_));
		int space_ = 0;
		child_->_setPosition(Recti(rect.left + space_, rect.top + 2*space_, rect.right - rect.left - 2*space_, rect.bottom - rect.top - 3*space_));
	}
}

void Win32Proxy::add(Widget* widget)
{
	if(child_)
		child_->_setParent(0);
	child_ = widget;
	child_->_setParent(this);
	_arrangeChildren();
}

void Win32Proxy::visitChildren(WidgetVisitor& visitor) const
{
	if(child_)
		visitor(*child_);
}

}