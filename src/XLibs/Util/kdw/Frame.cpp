#include "StdAfx.h"

#include "kdw/Frame.h"
#include "kdw/_WidgetWindow.h"
#include <windowsx.h>

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"
#include "ExcludeClipRectVisitor.h"


namespace kdw{

REGISTER_CLASS(Container, Frame, "Кнопка");

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

class FrameImpl: public _ContainerWindow{
public:
	FrameImpl(Frame* owner);
	const char* className() const{ return "kdw.FrameImpl"; }
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);

	HFONT font() { return owner_->emphasis() ? Win32::defaultBoldFont() : Win32::defaultFont(); }
	void setFrameText(const char* text);
protected:
	kdw::Frame* owner_;
//	static WNDPROC controlWindowProc_;
};
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4312) // 'type cast' : onversion from 'LONG' to 'HINSTANCE' of greater size

FrameImpl::FrameImpl(Frame* owner)
: _ContainerWindow(owner)
, owner_(owner)
{
	VERIFY(create("", WS_CHILD | WS_VISIBLE, Recti(0, 0, 42, 42), *Win32::_globalDummyWindow));
}

#pragma warning(pop)

void FrameImpl::setFrameText(const char* text)
{
	ASSERT(::IsWindow(handle_));
	VERIFY(::SetWindowText(handle_, text));

	HFONT font = GetWindowFont(handle_);
	Vect2i textSize = Win32::calculateTextSize(handle_, font, text);

	owner_->_setMinimalSize(textSize + Vect2i(GetSystemMetrics(SM_CXBORDER) * 2 + 6 + 1,
		GetSystemMetrics(SM_CYBORDER) * 2 + 6 + 1) + Vect2i(owner_->border(), owner_->border()));
	owner_->_queueRelayout();

}

LRESULT FrameImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message){
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			UINT keyCode = UINT(wparam);
			if(keyCode == VK_TAB){
				if(Win32::isKeyPressed(VK_SHIFT))
					owner_->passFocus(kdw::FOCUS_PREVIOUS);
				else
					owner_->passFocus(kdw::FOCUS_NEXT);
			}
			else if(keyCode == VK_LEFT)
				owner_->passFocus(kdw::FOCUS_LEFT);
			else if(keyCode == VK_RIGHT)
				owner_->passFocus(kdw::FOCUS_RIGHT);
			else if(keyCode == VK_UP)
				owner_->passFocus(kdw::FOCUS_UP);
			else if(keyCode == VK_DOWN)
				owner_->passFocus(kdw::FOCUS_DOWN);
            
			break;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT paintStruct;
			HDC paintDC = ::BeginPaint(handle_, &paintStruct);
			{
				HDC dc = paintDC;
				RECT clientRect;
				::GetClientRect(*this, &clientRect);
				::FillRect(dc, &clientRect, ::GetSysColorBrush(COLOR_BTNFACE));

				Recti rect(clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
				rect.top(clientRect.top);
				rect.left(clientRect.left);
				rect.bottom(clientRect.bottom);
				rect.right(clientRect.right);
				int spacing = 3;
				int hspacing = 3;
				int vspacing = 3;
				RECT windowRect = { rect.left() + hspacing,
					rect.top() + vspacing*2 + 1,
					rect.right() - hspacing,
					rect.bottom() - vspacing };
				::DrawEdge(dc, &windowRect, EDGE_ETCHED, BF_RECT);
				HFONT oldFont = HFONT(::SelectObject(dc, (HGDIOBJ)font()));
				HBRUSH oldBrush = HBRUSH(::SelectObject(dc, (HGDIOBJ)(::GetStockObject(GRAY_BRUSH))));
				int oldBackMode = ::SetBkMode(dc, OPAQUE);
				COLORREF oldBkColor = ::SetBkColor(dc, ::GetSysColor(COLOR_BTNFACE));
				const char* text = owner_->text();
				HFONT fontInt = font();
				Vect2i textSize = Win32::calculateTextSize(handle_, fontInt, text);
				RECT fillRect = { rect.left() + hspacing*2 - 1,
					rect.top(),
					rect.left() + hspacing*2 + textSize.x + 2,
					rect.top() + textSize.y };
				::FillRect(dc, &fillRect, ::GetSysColorBrush(COLOR_BTNFACE));
				RECT textRect = { rect.left() + hspacing*2,
					rect.top(),
					rect.right() - hspacing,
					rect.bottom() - vspacing };
				DrawText(dc, text, int(strlen(text)), &textRect, FS_CYRILLIC);
				::SelectObject(dc, oldFont);
				::SelectObject(dc, oldBrush);
				::SetBkMode(dc, oldBackMode);
				::SetBkColor(dc, oldBkColor);
			}
			EndPaint(handle_, &paintStruct);
			
			return __super::onMessage(message, wparam, lparam);
		}
	case WM_ERASEBKGND:
		{
			HDC dc = HDC(wparam);
			ExcludeClipRectVisitor visitor(dc);
			owner_->visitChildren(visitor);
			return __super::onMessage(message, wparam, lparam);
		}
	}
	return __super::onMessage(message, wparam, lparam);
}



// --------------------------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

Frame::Frame(const char* text, bool emphasis, int border)
: _ContainerWithWindow(new FrameImpl(this), border)
, space_(7)
, emphasis_(emphasis)
{
	_setMinimalSize(Vect2i(24, 24));
	setText(text);
}

#pragma warning(pop)

Frame::~Frame()
{
	if(child_)
		child_->_setParent(0);
}

void Frame::setEmphasis(bool emphasis)
{
	emphasis_ = emphasis;
}

void Frame::setText(const char* text)
{
	text_ = text;
	safe_cast<FrameImpl*>(_window())->setFrameText(text);
}

void Frame::serialize(Archive& ar)
{
	ar.serialize(text_, "text", "&Текст");
	Widget::serialize(ar);
}

void Frame::add(Widget* widget)
{
	child_ = widget;
	widget->_setParent(this);
	_arrangeChildren();
}

void Frame::_arrangeChildren()
{
	if(child_){
		RECT rect;
		VERIFY(::GetClientRect(*_window(), &rect));
		VERIFY(::InflateRect(&rect, -border_, -border_));
		child_->_setPosition(Recti(rect.left + space_, rect.top + 2*space_, rect.right - rect.left - 2*space_, rect.bottom - rect.top - 3*space_));
	}
}

void Frame::_relayoutParents()
{
	if(child_)
		_setMinimalSize(child_->_minimalSize() + Vect2i(border_ * 2 + space_ * 2, border_ * 2 + space_ * 3));
	else
		_setMinimalSize(Vect2i(border_ * 2, border_ * 2));
	
	RECT clientRect;
	::GetClientRect(*_window(), &clientRect);
	bool move = false;
	RECT windowRect;
	::GetWindowRect(*_window(), &windowRect);
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
		_window()->move(Recti(windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top));
		_window()->update();
	}
	else
		::RedrawWindow(*_window(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);

	Widget::_relayoutParents();
}

Widget* Frame::_nextWidget(Widget* last, FocusDirection focusDirection)
{
	switch(focusDirection)
	{
	case FOCUS_NEXT:
	case FOCUS_PREVIOUS:
		if(last == child_)
			return 0;
		else
			return child_;
	case FOCUS_FIRST:
	case FOCUS_LAST:
		return child_;
	default:
		return 0;
	}
}

void Frame::visitChildren(WidgetVisitor& visitor) const
{
	if(child_){
		visitor(*child_);
	}
}

void Frame::remove()
{
	child_ = 0;
}

};



