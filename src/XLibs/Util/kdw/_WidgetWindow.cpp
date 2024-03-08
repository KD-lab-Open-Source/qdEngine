#include "StdAfx.h"
#include "kdw/_WidgetWithWindow.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/HotkeyContext.h"
#include "kdw/Tooltip.h"
#include <commctrl.h>

namespace kdw{

_WidgetWindow::_WidgetWindow(_WidgetWithWindow* owner, HWND handle)
: Window32(handle)
, owner_(owner)
{

}

int _WidgetWindow::onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext){
		if(hotkeyContext->injectPress(sKey(keyCode, true)))
			return 0;
	}
	return __super::onMessageSysKeyDown(keyCode, count, flags);
}

LRESULT _WidgetWindow::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	if(message == WM_MOUSELEAVE && owner_->toolTip())
		owner_->toolTip()->hide();

	if(message == WM_NOTIFY){
		NMHDR* pNMHDR = (NMHDR*)lparam;
		if(pNMHDR->code == TTN_GETDISPINFO && owner_->toolTip()){
			LPNMTTDISPINFO pdi = (LPNMTTDISPINFO)lparam;
			::SendMessage(pdi->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, GetSystemMetrics(SM_CXSCREEN));
			pdi->lpszText = (char*)owner_->toolTip()->text();
			pdi->hinst = 0;
		}
	}
	return __super::onMessage(message, wparam, lparam);
}

int _WidgetWindow::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext && !((flags >> 14) & 1)){
		if(hotkeyContext->injectPress(sKey(keyCode, true)))
			return 0;
	}
	return __super::onMessageKeyDown(keyCode, count, flags);
}

int _WidgetWindow::onMessageSetFocus(HWND lastFocusedWindow)
{
    if(owner_->_focusable())
		Widget::_setFocusedWidget(owner_);
	return __super::onMessageSetFocus(lastFocusedWindow);
}

// ---------------------------------------------------------------------------

_ContainerWindow::_ContainerWindow(_ContainerWithWindow* owner)
: owner_(owner)
{

}

int _ContainerWindow::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext && !((flags >> 14) & 1)){
		hotkeyContext->injectPress(sKey(keyCode, true));
	}
	return __super::onMessageKeyDown(keyCode, count, flags);
}

}
