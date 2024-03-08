#include "StdAfx.h"
#include "kdw/Window.h"
#include "kdw/Widget.h"

#include "kdw/Win32/Window.h"
#include "kdw/Win32/Rectangle.h"
#include "kdw/Win32/Handle.h"
#include "ExcludeClipRectVisitor.h"
#include "kdw/HotkeyContext.h"

#include <windows.h>

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"

namespace kdw{
REGISTER_CLASS(Container, Window, "ќкно приложени€")

class WindowImpl : public Win32::Window32{
public:
	WindowImpl(kdw::Window* window);
	~WindowImpl();
	// virtuals:
	const char* className() const{ return "kdw.WindowImpl"; }
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	BOOL onMessageEraseBkgnd(HDC dc);
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	// ^^^
protected:
	kdw::Window* owner_;
};

WindowImpl::WindowImpl(kdw::Window* owner)
: Win32::Window32()
, owner_(owner)
{
}


WindowImpl::~WindowImpl()
{
}




BOOL WindowImpl::onMessageEraseBkgnd(HDC dc)
{
	ExcludeClipRectVisitor visitor(dc);
	owner_->visitChildren(visitor);
	return Win32::Window32::onMessageEraseBkgnd(dc);
}



LRESULT WindowImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message){
		/*
		UINT keyCode = UINT(wparam);
		if(keyCode == VK_TAB){
			if(Win32::isKeyPressed(VK_SHIFT))
				owner_->passFocus(kdw::FOCUS_PREVIOUS);
			else
				owner_->passFocus(kdw::FOCUS_NEXT);
		}
		*/

	case WM_CLOSE:
		owner_->onClose();
		return 0;
	case WM_SETFOCUS:
		owner_->onSetFocus();
		break;
	case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS* windowPos = (WINDOWPOS*)(lparam);
			if(!(windowPos->flags & SWP_NOSIZE)){
				UINT mode = UINT(wparam);
				UINT width = LOWORD(lparam);
				UINT height = HIWORD(lparam);
				if(mode != SIZE_MINIMIZED){
					owner_->_arrangeChildren();

					RedrawWindow(*this, 0, 0, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE);
				}
			}
			return TRUE;
		}
	case WM_SIZING: 
		{
			// здесь будем провер€ть чтобы окно не сделали меньше размера
			UINT edge = (UINT)wparam;   // угол/ребро за которое т€нет пользователь
			RECT* rect = (RECT*)lparam; // пр€моугольник содержащий координаты окна
										// (не клиентской области - а внешней рамки), мен€ем ниже
			RECT windowRect;
			RECT clientRect;
			::GetWindowRect(*this, &windowRect);
			::GetClientRect(*this, &clientRect);

			POINT borderSize; // размер рамок и TitleBar-а с обоих сторон
							  // (точнее разница в полном размере окна и клиетнской области)
			borderSize.x = windowRect.right - windowRect.left - (clientRect.right - clientRect.left);
			borderSize.y = windowRect.bottom - windowRect.top - (clientRect.bottom - clientRect.top);

			if(rect->right - rect->left < owner_->_minimalSize().x + borderSize.x){
				if(edge == WMSZ_BOTTOMLEFT || edge == WMSZ_TOPLEFT || edge == WMSZ_LEFT)
					rect->left = rect->right - owner_->_minimalSize().x - borderSize.x;
				else if(edge == WMSZ_BOTTOMRIGHT || edge == WMSZ_TOPRIGHT || edge == WMSZ_RIGHT)
					rect->right = rect->left + owner_->_minimalSize().x + borderSize.x;
			}
			if(rect->bottom - rect->top < owner_->_minimalSize().y + borderSize.y){
				if(edge == WMSZ_BOTTOMLEFT || edge == WMSZ_BOTTOMRIGHT || edge == WMSZ_BOTTOM)
					rect->bottom = rect->top + owner_->_minimalSize().y + borderSize.y;
				else if(edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT || edge == WMSZ_TOP)
					rect->top = rect->bottom - owner_->_minimalSize().y - borderSize.y;
			}
			return TRUE;
		}
	}
	return Win32::Window32::onMessage(message, wparam, lparam);
}

int WindowImpl::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext && !((flags >> 14) & 1))
		hotkeyContext->injectPress(sKey(keyCode, true));
	return __super::onMessageKeyDown(keyCode, count, flags);
}


// -------------------------------------------------------------------------------------
Window::Window(int border, int style)
{
	style_ = style;
	init(0, border);
}

Window::Window(HWND parent, int border)
{
	style_ = 0;
	init(parent, border);
}

void Window::init(HWND owner, int border)
{
	resizeable_ = true;
	minimizeable_ = true;
	showTitleBar_ = true;
	windowPosition_ = POSITION_DEFAULT;
	positioned_ = false;
	defaultSize_.set(0, 0);
	defWidget_ = 0;
	hotkeyContext_ = new HotkeyContext;

	window_ = new WindowImpl(this);
	VERIFY(window_->create(title_.c_str(), generateWin32Style(), Recti(0, 0, 400, 400), owner));
	setBorder(border);
	
	hotkeyContext_->signalPressed(sKey(VK_TAB)).connect(this, &Window::onHotkeyFocusNext);
	hotkeyContext_->signalPressed(sKey(VK_TAB | sKey::SHIFT)).connect(this, &Window::onHotkeyFocusPrev);
}

unsigned int Window::generateWin32Style()
{
	if(style_)
		return style_;

	int style = WS_CLIPCHILDREN | WS_SYSMENU;
	if(showTitleBar_)
		//style |= WS_DLGFRAME; // WS_CAPTION
		style |= WS_CAPTION;
	else
		if(resizeable_)
			style |= WS_DLGFRAME;

	if(resizeable_)
		style |= WS_MAXIMIZEBOX | WS_THICKFRAME; 
	else{
		style |= WS_POPUP | WS_BORDER/*  | WS_DLGFRAME| WS_BORDER*/;
	}
	if(minimizeable_)
		style |= WS_MINIMIZEBOX;

	return style;
}

void Window::setResizeable(bool allowResize)
{
	resizeable_ = allowResize;
	window_->setStyle(generateWin32Style()); 
}

void Window::setRestoredPosition(const Recti& position)
{
	WINDOWPLACEMENT placement;
	placement.length = sizeof(placement);
	VERIFY(GetWindowPlacement(*_window(), &placement));
	placement.rcNormalPosition = Win32::Rect(position);
	VERIFY(SetWindowPlacement(*_window(), &placement));
}

Recti Window::restoredPosition() const
{
	WINDOWPLACEMENT placement;
	placement.length = sizeof(placement);
	VERIFY(GetWindowPlacement(*_window(), &placement));
	return Win32::Rect(placement.rcNormalPosition).recti();
}

void Window::setMinimizeable(bool allowMinimize)
{
	minimizeable_ = allowMinimize;
	window_->setStyle(generateWin32Style()); 
}

void Window::setMaximized(bool maximized)
{
	if(this->maximized() != maximized){
		ShowWindow(*_window(), maximized ? SW_MAXIMIZE : SW_RESTORE);
		RedrawWindow(*_window(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE);
	}	
}

bool Window::maximized() const
{
	return (_window()->getStyle() & WS_MAXIMIZE) != 0;
}

Window::~Window()
{
	defWidget_ = 0;
	if(child_)
		child_->_setParent(0);
	child_ = 0;
	delete window_;
	window_ = 0;
}

bool Window::isVisible() const
{
	return window_->isVisible();
}

void Window::_updateVisibility()
{
	Widget::_updateVisibility();
	if(_visible()){
		if(!positioned_){
			reposition();
		}
		_arrangeChildren(); // FIXME: hack
		window_->show(SW_SHOW);
	}
	else
		window_->show(SW_HIDE);
}

void Window::setDefaultWidget(Widget* widget)
{
	if(widget->canBeDefault())
		defWidget_ = widget;
}

void Window::showAll()
{
	Widget::showAll();	
	_setFocus();
}


void Window::setTitle(std::string title)
{
	title_ = title;
	window_->setWindowText(title.c_str());
}

void Window::setShowTitleBar(bool showTitleBar)
{
	showTitleBar_ = showTitleBar;
	window_->setStyle(generateWin32Style()); 
}


void Window::setIconFromResource(const char* resourceName)
{
	HICON icon = LoadIcon(Win32::_globalInstance(), resourceName);
	xassert(icon);
 
	SetClassLong(*window_, GCL_HICON, (LONG)icon);
}

void Window::setIconFromFile(const char* resourceName)
{
	HANDLE icon = LoadImage(0, resourceName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	xassert(icon);

	SetClassLong(*window_, GCL_HICON, (LONG)icon);
}

void Window::add(Widget* widget)
{
	child_ = widget;
	widget->_setParent(this);
	_arrangeChildren();
}

void Window::remove()
{
	if(child_)
		child_->_setParent(0);
	child_ = 0;
}

void Window::reposition()
{
	int screenWidth = int(GetSystemMetrics(SM_CXSCREEN));
	int screenHeight = int(GetSystemMetrics(SM_CYSCREEN));
	
	int width = max(max(GetSystemMetrics(SM_CXMINSPACING), _minimalSize().x), defaultSize_.x);
	int height = max(_minimalSize().y, defaultSize_.y);
	if(resizeable_){
		width += GetSystemMetrics(SM_CXBORDER);
		height += GetSystemMetrics(SM_CYBORDER);
	}
	if(showTitleBar_){
		height += GetSystemMetrics(SM_CYCAPTION);
	}

	Recti rect;
	switch(windowPosition_){
		case POSITION_LEFT_TOP:
			rect.set(0, 0, width, height);
			break;
		case POSITION_LEFT:
			rect.set(0, (screenHeight - height) / 2, width, height);
			break;
		case POSITION_LEFT_BOTTOM:
			rect.set(0, screenHeight - height, width, height);
			break;
		case POSITION_RIGHT_TOP:
			rect.set(screenWidth - width, 0, width, height);
			break;
		case POSITION_RIGHT:
			rect.set(screenWidth - width, (screenHeight - height) / 2, width, height);
			break;
		case POSITION_RIGHT_BOTTOM:
			rect.set(screenWidth - width, screenHeight - height, width, height);
			break;
		case POSITION_TOP:
			rect.set((screenWidth - width) / 2, 0, width, height);
			break;
		case POSITION_BOTTOM:
			rect.set((screenWidth - width) / 2, screenHeight - height, width, height);
			break;
		case POSITION_CENTER:
			rect.set((screenWidth - width) / 2, (screenHeight - height) / 2, width, height);
			break;
		case POSITION_DEFAULT:
			rect.set(0, 0, width, height);
			break;
		case POSITION_CURSOR: {
			POINT point;
			GetCursorPos(&point);
			rect.set(point.x, point.y, width, height);
			} break;
	}

	//if(windowPosition_ != POSITION_DEFAULT)
	::SetWindowPos(*window_, 0, rect.left(), rect.top(), rect.width(), rect.height(), SWP_NOZORDER | SWP_NOACTIVATE | (visible_ ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	positioned_ = true;
}

void Window::setDefaultPosition(WindowPosition position)
{
	windowPosition_ = position;
	positioned_ = false;
	reposition();
}

void Window::setDefaultSize(Vect2i size)
{
	defaultSize_ = size;
	positioned_ = false;
	reposition();
}

void Window::visitChildren(WidgetVisitor& visitor) const
{
	if(child_){
		visitor(*child_);
	}
}

void Window::_arrangeChildren()
{
	if(child_){
		RECT rect;
		VERIFY(::GetClientRect(*window_, &rect));
		VERIFY(::InflateRect(&rect, -border_, -border_));
		child_->_setPosition(Recti(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top));
	}
}

void Window::_relayoutParents()
{
	if(child_)
		_setMinimalSize(child_->_minimalSize() + Vect2i(border_ * 2, border_ * 2));
	else
		_setMinimalSize(Vect2i(border_ * 2, border_ * 2));

	RECT clientRect;
	::GetClientRect(*window_, &clientRect);
	bool move = false;
	RECT windowRect;
	::GetWindowRect(*window_, &windowRect);
	SIZE borderSize = { windowRect.right - windowRect.left - (clientRect.right - clientRect.left),
						windowRect.bottom - windowRect.top - (clientRect.bottom - clientRect.top) };

	if(clientRect.right - clientRect.left < _minimalSize().x){
		windowRect.right = windowRect.left + _minimalSize().x + borderSize.cx;
		move = true;
	}
	if(clientRect.bottom - clientRect.top < _minimalSize().y){
		windowRect.bottom = windowRect.top + _minimalSize().y + borderSize.cy;
        move = true;
	}
	if(move){
		window_->move(Recti(windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top));
		window_->update();
	}
	else
		::RedrawWindow(*window_, 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);

	__super::_relayoutParents();
}

void Window::_setFocus()
{
	__super::_setFocus();
}

void Window::onClose()
{
	hide();
	signalClose().emit();
}


void Window::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_STATE)){
		Recti position = restoredPosition();
		bool maximized = this->maximized();

		ar.serialize(position, "restoredPosition", 0);
		ar.serialize(maximized, "maximized", 0);

		if(ar.isInput()){
			setRestoredPosition(position);
			setMaximized(maximized);
		}
	}
	if(ar.filter(SERIALIZE_DESIGN)){
		ar.serialize(_property(title_, this, &Window::setTitle), "title", "«аголовок");
		ar.serialize(_property(defaultSize_, this, &Window::setDefaultSize), "defaultSize", "Ќачальный размер");
		ar.serialize(resizeable_, "resizeable", "–азрешить масштабирование");
		if(resizeable_){
			ar.serialize(minimizeable_, "resizeable", "–азрешить сворачивание");
			ar.serialize(showTitleBar_, "showTitleBar", "ѕоказывать строку заголовка");
		}
		ar.serialize(child_, "widget", " онтрол");
	}
}

void Window::onHotkeyFocusNext()
{
	passFocus(kdw::FOCUS_NEXT);
}
void Window::onHotkeyFocusPrev()
{
	passFocus(kdw::FOCUS_PREVIOUS);
}

}
