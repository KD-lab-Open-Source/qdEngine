#include "StdAfx.h"
#include "kdw/_WidgetWithWindow.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/Container.h"
#include "kdw/Win32/Window.h"

namespace kdw{

_WidgetWithWindow::_WidgetWithWindow(_WidgetWindow* window, int border)
: window_(0)
{
	setWindow(window);
	setBorder(border);
}

_WidgetWithWindow::~_WidgetWithWindow()
{
	Win32::Window32* window = window_;
	window_ = 0;
	delete window;
}


void _WidgetWithWindow::setWindow(Win32::Window32* window)
{
	window_ = window;
}

void _WidgetWithWindow::_setPosition(const Recti& position)
{
	Widget::_setPosition(position);

	Win32::Window32* window = _findWindow(parent());
	ASSERT(window);
	Win32::Window32::PositionDeferer deferer = window->positionDeferer();
	Recti pos(position.left() + border_, position.top() + border_, position.width() - border_ * 2, position.height() - border_ * 2);
	deferer->defer(window_, pos);
}

void _WidgetWithWindow::_setParent(Container* container)
{
	Win32::Window32* window = _findWindow(container);
	ASSERT(window);
	
	Widget::_setParent(container);
	ASSERT(::IsWindow(*window_));
	if(::IsWindow(*window_)){
		window_->setParent(window);
		window_->move(position_);
	}
}

bool _WidgetWithWindow::isVisible() const
{
	return window_->isVisible();
}

void _WidgetWithWindow::setSensitive(bool sensitive)
{
	Widget::setSensitive(sensitive);
	window_->enable(sensitive);
}

void _WidgetWithWindow::_updateVisibility()
{
	Widget::_updateVisibility();
	if(_visible() && _visibleInLayout())
		window_->show(SW_SHOWNOACTIVATE);
	else
		window_->show(SW_HIDE);
}

void _WidgetWithWindow::_setFocus()
{
	::SetFocus(*window_);
	__super::_setFocus();
}

// ---------------------------------------------------------------------------
// FIXME somehow: Copy & Paste!

_ContainerWithWindow::_ContainerWithWindow(_ContainerWindow* window, int border)
: window_(window)
{
	setBorder(border);
}

_ContainerWithWindow::~_ContainerWithWindow()
{
	Win32::Window32* window = window_;
	window_ = 0;
	delete window;
}

void _ContainerWithWindow::_setPosition(const Recti& position)
{
	Container::_setPosition(position);
	window_->move(Recti(position.left() + border(), position.top() + border(), position.width() - border() * 2, position.height() - border() * 2));
	_arrangeChildren();
}

void _ContainerWithWindow::_setParent(Container* container)
{
	Win32::Window32* window = _findWindow(container);
	ASSERT(window);
	
	Widget::_setParent(container);
	ASSERT(window_);
	ASSERT(::IsWindow(*window_));
	if(::IsWindow(*window_)){
		window_->setParent(window);
		window_->move(position_);
	}
}

bool _ContainerWithWindow::isVisible() const
{
	return window_->isVisible();
}

void _ContainerWithWindow::setSensitive(bool sensitive)
{
	Widget::setSensitive(sensitive);
	window_->enable(sensitive);
}

void _ContainerWithWindow::_updateVisibility()
{
	Widget::_updateVisibility();
	if(_visible() && _visibleInLayout())
		window_->show(SW_SHOWNOACTIVATE);
	else
		window_->show(SW_HIDE);
}

void _ContainerWithWindow::_setFocus()
{
	if(::IsWindowVisible(*window_))
		::SetFocus(*window_);	
	__super::_setFocus();
}

}
