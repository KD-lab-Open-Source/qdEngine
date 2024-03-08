#ifndef __KDW_WIDGET_WINDOW_H_INCLUDED__
#define __KDW_WIDGET_WINDOW_H_INCLUDED__

#include "kdw/Win32/Window.h"

namespace kdw{

class _WidgetWithWindow;
class KDW_API _WidgetWindow : public Win32::Window32{
public:
	_WidgetWindow(_WidgetWithWindow* owner, HWND handle = 0);

	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSetFocus(HWND lastFocusedWindow);
	kdw::_WidgetWithWindow* owner(){ return owner_; }
protected:
	kdw::_WidgetWithWindow* owner_;
};

class _ContainerWithWindow;
class KDW_API _ContainerWindow : public Win32::Window32{
public:
	_ContainerWindow(_ContainerWithWindow* owner);

	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	kdw::_ContainerWithWindow* owner(){ return owner_; }
protected:
	kdw::_ContainerWithWindow* owner_;
};

}

#endif
