#include "StdAfx.h"
#include "kdw/HotkeyButton.h"
#include "kdw/_WidgetWindow.h"

static int addModifiers(int keyCode){
	sKey key(keyCode);
	key.ctrl |= (GetAsyncKeyState(VK_CONTROL) >> 15) ? 1 : 0; 
	key.shift |= (GetAsyncKeyState(VK_SHIFT) >> 15) ? 1 : 0; 
	key.menu |= (GetAsyncKeyState(VK_MENU) >> 15) ? 1 : 0; 
	return key.fullkey;
}

namespace kdw{

class HotkeyButtonImpl : public _WidgetWindow{
public:
	HotkeyButtonImpl(HotkeyButton* owner);

	void redraw(HDC dc);
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);

	void onMessagePaint();
	void onMessageTimer(int id);
	int onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags);
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags);
	void onMessageLButtonDblClk(int x, int y);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageMButtonDown(UINT button, int x, int y);
	void onMessageMButtonUp(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageRButtonUp(UINT button, int x, int y);
	int onMessageSetFocus(HWND lastFocusedWindow);
	int onMessageKillFocus(HWND focusedWindow);
	bool _focusable() const{ return true; }
protected:
	HotkeyButton* owner_;
	bool pressed_;
	int ticksLeft_;
};

static const int TIMER_ID = 100;

HotkeyButtonImpl::HotkeyButtonImpl(HotkeyButton* owner)
: _WidgetWindow(owner)
, owner_(owner)
, pressed_(false)
, ticksLeft_(-1)
{
	VERIFY(create("", WS_CHILD | WS_TABSTOP, Recti(0, 0, 42, 42), *Win32::_globalDummyWindow));
	::SetTimer(*this, TIMER_ID, 100, 0);
}

void HotkeyButtonImpl::redraw(HDC dc)
{
	HGDIOBJ oldFont = ::SelectObject(dc, Win32::defaultFont());
	int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
	
	RECT rt;
	GetClientRect(*this, &rt);
	const char* str = TRANSLATE("Пожалуйста, нажмите кнопку...");
	::DrawText(dc, str, strlen(str), &rt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	::SelectObject(dc, oldFont);
}

LRESULT HotkeyButtonImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	return __super::onMessage(message, wparam, lparam);
}

void HotkeyButtonImpl::onMessagePaint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(*this, &ps);
	redraw(dc);
	EndPaint(*this, &ps);
}

void HotkeyButtonImpl::onMessageTimer(int id)
{
	if(id == TIMER_ID){
		if(ticksLeft_ > 0){
			--ticksLeft_;
			if(ticksLeft_ == 0 &&
			   (owner_->key_.key == VK_LBUTTON ||
				owner_->key_.key == VK_RBUTTON ||
				owner_->key_.key == VK_MBUTTON))
				owner_->signalChanged_.emit();
		}
	}
	__super::onMessageTimer(id);
}

void HotkeyButtonImpl::onMessageLButtonDblClk(int x, int y)
{
//	owner_->key_ = sKey(addModifiers(VK_LDBL));
//	owner_->signalChanged_.emit();

	__super::onMessageLButtonDblClk(x, y);
}


int HotkeyButtonImpl::onMessageSetFocus(HWND lastFocusedWindow)
{
    if(owner_->_focusable())
		Widget::_setFocusedWidget(owner_);
	return 0;
}

int HotkeyButtonImpl::onMessageKillFocus(HWND focusedWindow)
{
	return 0;
}

int HotkeyButtonImpl::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	if(((flags >> 14) & 1) == 0){ // кнопка не была нажата
		pressed_ = true;
		owner_->key_ = sKey(addModifiers(keyCode));
		if(keyCode != VK_SHIFT &&
		   keyCode != VK_CONTROL &&
		   keyCode != VK_MENU)
			owner_->signalChanged_.emit();
	}
	return __super::onMessageKeyDown(keyCode, count, flags);
}

int HotkeyButtonImpl::onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	if(pressed_){
		owner_->key_ = sKey(addModifiers(keyCode));
		owner_->signalChanged_.emit();
	}
	return __super::onMessageKeyUp(keyCode, count, flags);
}

int HotkeyButtonImpl::onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	owner_->key_ = sKey(addModifiers(keyCode));
	pressed_ = true;

	if(keyCode == VK_CANCEL)
		owner_->signalChanged_.emit();

	if(keyCode != VK_SHIFT &&
	   keyCode != VK_CONTROL &&
	   keyCode != VK_MENU)
		owner_->signalChanged_.emit();
	return __super::onMessageSysKeyDown(keyCode, count, flags);
}

int HotkeyButtonImpl::onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	if(pressed_){
		owner_->key_ = sKey(addModifiers(keyCode));
		owner_->signalChanged_.emit();
	}
	return __super::onMessageSysKeyUp(keyCode, count, flags);
}

void HotkeyButtonImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	::SetFocus(*this);
	pressed_ = true;
}

void HotkeyButtonImpl::onMessageLButtonUp(UINT button, int x, int y)
{
	if(pressed_){
		owner_->key_ = sKey(addModifiers(VK_LBUTTON));
		ticksLeft_ = 30;
	}
}

void HotkeyButtonImpl::onMessageMButtonDown(UINT button, int x, int y)
{
	pressed_ = true;
}

void HotkeyButtonImpl::onMessageMButtonUp(UINT button, int x, int y)
{
	if(pressed_){
		owner_->key_ = sKey(addModifiers(VK_MBUTTON));
		ticksLeft_ = 30;
	}
}

void HotkeyButtonImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	pressed_ = true;
}

void HotkeyButtonImpl::onMessageRButtonUp(UINT button, int x, int y)
{
	if(pressed_){
		owner_->key_ = sKey(addModifiers(VK_RBUTTON));
		ticksLeft_ = 30;
	}
}

// --------------------------------------------------------------------------- 
#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

HotkeyButton::HotkeyButton(int border)
: _WidgetWithWindow(new HotkeyButtonImpl(this), border)
{
    _setMinimalSize(Vect2i(200, 26));
}

#pragma warning(pop)
HotkeyButtonImpl& HotkeyButton::impl()
{
	return static_cast<HotkeyButtonImpl&>(*__super::_window());
}

}
