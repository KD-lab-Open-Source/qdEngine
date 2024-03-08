#ifndef __KDW_WINDOW_H_INCLUDED__
#define __KDW_WINDOW_H_INCLUDED__

#include <string>
#include "kdw/Container.h"
#include "kdw/HotkeyContext.h"
#include "Win32/Types.h"

namespace Win32{
    class Window;
};

namespace kdw{

enum WindowPosition{
	POSITION_DEFAULT,
	POSITION_CENTER,
	//POSITION_CENTER_PARENT,
	POSITION_LEFT_TOP,
	POSITION_LEFT,
	POSITION_LEFT_BOTTOM,
	POSITION_RIGHT_TOP,
	POSITION_RIGHT,
	POSITION_RIGHT_BOTTOM,
	POSITION_TOP,
	POSITION_BOTTOM,
	POSITION_CURSOR
};

class WindowImpl;
class KDW_API Window : public Container{
public:
    Window(Widget* owner, int border = 4);
    Window(int border = 4, int style = 0);

    // virtuals:
    ~Window();

	bool isVisible() const;
	void showAll();

    void visitChildren(WidgetVisitor& visitor) const;
    // ^^^


	/// добавить контрол в окно
	void add(Widget* widget);
    /// убирает единственный дочерний виджет
    void remove();

	/// устанавливает заголок окна (тот, что в TitleBar-е)
    //void setTitle(const char* title);
	void setTitle(std::string str);
	void setShowTitleBar(bool showTitleBar);
	void setIconFromResource(const char* resourceName);
	void setIconFromFile(const char* resourceName);

	void setDefaultPosition(WindowPosition position);
	void setDefaultSize(Vect2i size);

    /// разрешает/запрещает изменение размеров окна
	void setResizeable(bool allowResize);
	bool resizeable() const{ return resizeable_; }
    /// разрешает/запрещает сворачивание окна
	void setMinimizeable(bool allowMinimize);
	bool minimizeable() const{ return minimizeable_; }

	Recti restoredPosition() const;
	void setRestoredPosition(const Recti& position);

	void setMaximized(bool maximized);
	bool maximized() const;

	// signals
	sigslot::signal0& signalClose(){ return signalClose_; }

	// virtual events:
	virtual void onClose();
	virtual void onResize(int width, int height) {}
	virtual void onSetFocus() {}

	void serialize(Archive& ar);

	Win32::Window32* _window() const{ return window_; }

	void setDefaultWidget(Widget* widget);
	Widget* defaultWidget() const { return defWidget_; } 

	HotkeyContext* hotkeyContext(){ return hotkeyContext_; }

	void onHotkeyFocusNext();
	void onHotkeyFocusPrev();
protected:
    Window(HWND parent, int border);
	void init(HWND parent, int border);

	friend WindowImpl;
	void _updateVisibility();
	void _arrangeChildren();
	void _relayoutParents();

	void _setFocus();
	unsigned int generateWin32Style();
	void reposition();

	sigslot::signal0 signalClose_;

	bool resizeable_;
	bool minimizeable_;
	bool showTitleBar_;
	bool positioned_;
	WindowPosition windowPosition_;
	Vect2i defaultSize_;
	std::string title_;
	int style_;

	ShareHandle<Widget> child_;
	ShareHandle<HotkeyContext> hotkeyContext_;

    Win32::Window32* window_;

	Widget* defWidget_;
};

};

#endif
