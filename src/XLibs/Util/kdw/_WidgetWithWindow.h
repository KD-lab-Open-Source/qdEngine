#ifndef __KDW_WIDGET_WITH_WINDOW_H_INCLUDED__
#define __KDW_WIDGET_WITH_WINDOW_H_INCLUDED__

#include "kdw/Widget.h"
#include "kdw/Container.h"

namespace kdw{
class _WidgetWindow;
class KDW_API _WidgetWithWindow : public Widget{
public:
	void setSensitive(bool sensitive);
	bool isVisible() const;

	_WidgetWithWindow(_WidgetWindow* window, int border);
	~_WidgetWithWindow();

	void setWindow(Win32::Window32* window);

	void _setPosition(const Recti& position);
	void _setParent(Container* container);
	void _setFocus();
	void _updateVisibility();

	Win32::Window32* _window() const{ return window_; }
private:
	Win32::Window32* window_;
};

// ---------------------------------------------------------------------------
class _ContainerWindow;
class KDW_API _ContainerWithWindow : public Container{
public:
	void setSensitive(bool sensitive);
	bool isVisible() const;

	_ContainerWithWindow(_ContainerWindow* window, int border);
	~_ContainerWithWindow();

	void _setPosition(const Recti& position);
	void _setParent(Container* container);
	void _setFocus();
	void _updateVisibility();

	Win32::Window32* _window() const{ return window_; }
private:
	Win32::Window32* window_;
};

}

#endif
