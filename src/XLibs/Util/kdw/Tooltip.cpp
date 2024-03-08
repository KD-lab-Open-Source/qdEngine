#include "StdAfx.h"
#include "kdw/Win32/Window.h"
#include "kdw/Widget.h"
#include "kdw/Tooltip.h"
#include <commctrl.h>
#include "kdw/Win32/Window.h"

namespace kdw{
	

// ---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

Tooltip::Tooltip(const char* text, bool baloon)
: text_(text)
, widget_(0)
, baloon_(baloon)
, toolTipWindow_(0)
, offset_(15, 15)
{
	Win32::initializeCommonControls();
}

#pragma warning(pop)

void Tooltip::setBaloon(bool baloon)
{
	baloon_ = baloon;
}

void Tooltip::attach(Widget* widget)
{
	widget_ = widget;
	widget_->setToolTip(this);

	Win32::Window32* window = _findWindow(widget);
	xassert(window);
	HWND ownerHandle = *window;
	xassert(::IsWindow(*window));

	if(!toolTipWindow_){
		toolTipWindow_ = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, 0,
			WS_POPUP | TTS_ALWAYSTIP | (baloon_ ? TTS_BALLOON : 0),
			0, 0, 0, 0,
			ownerHandle, 0, Win32::_globalInstance(), 0);
	}

	TOOLINFO    ti;
	ti.cbSize = 44;//sizeof(TOOLINFO);
	ti.uFlags = TTF_TRACK;
	ti.hwnd   = ownerHandle;
	ti.uId    = 12345;
	ti.hinst  = Win32::_globalInstance();
	ti.lpszText  = LPSTR_TEXTCALLBACK;
	ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

	xassert(::IsWindow(toolTipWindow_));
	VERIFY(SendMessage(toolTipWindow_, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti));
	VERIFY(SendMessage(toolTipWindow_, TTM_TRACKACTIVATE,(WPARAM)TRUE, (LPARAM)&ti));
}

void Tooltip::setText(const char* text)
{
	text_ = text;
	
	if(!widget_)
		return;
	
	attach(widget_);

	if(!text_.empty()){
		TRACKMOUSEEVENT event;
		ZeroMemory((void*)(&event), sizeof(event));
		event.cbSize = sizeof(event);
		event.dwFlags = TME_LEAVE;
		event.hwndTrack = *_findWindow(widget_);
		VERIFY(TrackMouseEvent(&event));
	}
}

void Tooltip::show()
{
	POINT point;
	GetCursorPos(&point);

	SendMessage(toolTipWindow_, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(point.x + offset_.x, point.y + offset_.y));
}

void Tooltip::hide()
{
	setText("");

	TOOLINFO    ti;
	ti.cbSize = 44;//sizeof(TOOLINFO);
	ti.uFlags = TTF_TRACK;
	ti.hwnd   = *_findWindow(widget_);
	ti.uId    = 12345;
	ti.hinst  = Win32::_globalInstance();
	ti.lpszText  = LPSTR_TEXTCALLBACK;
	ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

	VERIFY(SendMessage(toolTipWindow_, TTM_TRACKACTIVATE,(WPARAM)FALSE, (LPARAM)&ti));
}


}
