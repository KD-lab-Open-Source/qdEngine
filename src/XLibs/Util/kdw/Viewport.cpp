#include "StdAfx.h"
#include "kdw/Serialization.h"
#include "kdw/Viewport.h"
#include "kdw/Container.h"
#include "kdw/HotkeyContext.h"

#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/MessageLoop.h"

#include <windows.h>
#include <windowsx.h>

# include "Render\inc\IRenderDevice.h"

namespace kdw{

// ---------------------------------------------------------------

class RenderWindow: public Win32::Window32, public sigslot::has_slots
{
public:
	RenderWindow(Win32::Window32* parent, kdw::Viewport* owner, cInterfaceRenderDevice* renderDevice);
	~RenderWindow();

	const char* className() const{ return "kdw.Viewport.RenderWindow"; }

	void initializeRenderDevice();

	void redraw();

	bool isMessageListener() const{ return isMessageListener_; }
	void setListenForMessage(bool listen);
	cRenderWindow* renderWindow() { return renderWindow_; }

	void onIdle();
	void onMessageMouseWheel(SHORT delta);
	BOOL onMessageEraseBkgnd(HDC dc);
	void onMessagePaint();
	void onMessageMouseMove(UINT button, int x, int y);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageMButtonDown(UINT button, int x, int y);
	void onMessageMButtonUp(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageRButtonUp(UINT button, int x, int y);
	int onMessageKillFocus(HWND focusedWindow);
	BOOL onMessageSize(UINT type, USHORT width, USHORT height);
	void onMessageShowWindow(BOOL show, int status);
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags);
	void onMessageLButtonDblClk(int x, int y);
protected:
	bool initialized_;
	bool isMessageListener_;
	kdw::Viewport* owner_;
	bool lockRedraw_;
	cInterfaceRenderDevice* renderDevice_;
	cRenderWindow* renderWindow_;
};

RenderWindow::RenderWindow(Win32::Window32* parent, kdw::Viewport* owner, cInterfaceRenderDevice* renderDevice)
: owner_(owner)
, renderWindow_(0)
, renderDevice_(renderDevice)
, initialized_(false)
, isMessageListener_(false)
, lockRedraw_(false)
{
	xassert(renderDevice_);
	VERIFY(create("renderWindow", WS_CHILD | WS_TABSTOP, Recti(0, 0, 200, 200), *parent));
}

RenderWindow::~RenderWindow()
{
	if(owner_->continuousUpdate_)
		Win32::MessageLoop::instance().signalIdle().disconnect(this);
	
	xassert(renderWindow_);

	renderDevice_->selectRenderWindow(0);
	renderDevice_->DeleteRenderWindow(renderWindow_);
	renderWindow_ = 0;
}

void RenderWindow::initializeRenderDevice()
{
	RECT rect;
	GetClientRect(handle_, &rect);
	::InflateRect(&rect, -owner_->border_, -owner_->border_);

	if(!renderDevice_->inited())
		renderDevice_->Initialize(rect.right - rect.left, rect.bottom - rect.top, RENDERDEVICE_MODE_WINDOW, 0, 0, *Win32::_globalDummyWindow);

	renderWindow_ = renderDevice_->createRenderWindow(handle_);
	ASSERT(renderWindow_ != 0);

	int width = max(16, rect.right - rect.left);
	int height = max(16, rect.bottom - rect.top);
	
	initialized_ = true;
	owner_->onInitialize();
}

void RenderWindow::redraw()
{
	if(!renderWindow_)
		return;
	renderDevice_->selectRenderWindow(renderWindow_);
	renderDevice_->Fill(owner_->fillColor_.r, owner_->fillColor_.g, owner_->fillColor_.b, 0);
	renderDevice_->BeginScene();
	owner_->onRedraw();
	renderDevice_->EndScene();
	renderDevice_->Flush();
	renderDevice_->selectRenderWindow(0);
}


void RenderWindow::setListenForMessage(bool listen)
{
	if(listen != isMessageListener_){
		if(listen)
			Win32::MessageLoop::instance().signalIdle().connect(this, &RenderWindow::onIdle);
		else
			Win32::MessageLoop::instance().signalIdle().disconnect(this);
		isMessageListener_ = listen;
	}
}

void RenderWindow::onIdle()
{
	// вызывается, когда приложение не получает сообщений
	if(owner_->parent() && owner_->parent()->isActive() && renderWindow_ && ::IsWindowVisible(*this)){
		redraw();
	}
}

int RenderWindow::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext && !((flags >> 14) & 1))
		hotkeyContext->injectPress(sKey(keyCode, true));
	return __super::onMessageKeyDown(keyCode, count, flags);
}

int RenderWindow::onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext && !((flags >> 14) & 1))
		if(hotkeyContext->injectPress(sKey(keyCode, true)))
			return 0;
	return __super::onMessageSysKeyDown(keyCode, count, flags);
}

int RenderWindow::onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext && ((flags >> 14) & 1))
		hotkeyContext->injectRelease(sKey(keyCode, true));
	return __super::onMessageKeyUp(keyCode, count, flags);
}

int RenderWindow::onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	HotkeyContext* hotkeyContext = findHotkeyContext(owner_);
	if(hotkeyContext && ((flags >> 14) & 1))
		hotkeyContext->injectRelease(sKey(keyCode, true));
	return __super::onMessageSysKeyUp(keyCode, count, flags);
}



BOOL RenderWindow::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

void RenderWindow::onMessagePaint()
{
	if(renderWindow_ && !lockRedraw_){
		lockRedraw_ = true;
		redraw();
		lockRedraw_ = false;
	}
	else{
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(handle_,&ps);
		::FillRect(dc, &ps.rcPaint, GetStockBrush(BLACK_BRUSH));
		EndPaint(handle_, &ps);
	}
	__super::onMessagePaint();
}

void RenderWindow::onMessageMouseMove(UINT button, int x, int y)
{
	ASSERT(x > -0xFFFF && x < 0xFFFF);
	ASSERT(y > -0xFFFF && y < 0xFFFF);
	owner_->mousePosition_.x = x;
	owner_->mousePosition_.y = y;
	if(button & MK_LBUTTON || button & MK_RBUTTON || button & MK_MBUTTON){
		if(HotkeyContext* context = findHotkeyContext(owner_))
			context->lockSticky();
	}
	owner_->onMouseMove();
}

void RenderWindow::onMessageLButtonDblClk(int x, int y)
{
	kdw::Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();

	owner_->onMouseButtonDown(kdw::MOUSE_BUTTON_LEFT_DOUBLE);
}

void RenderWindow::onMessageLButtonDown(UINT button, int x, int y)
{
	kdw::Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();

	owner_->onMouseButtonDown(kdw::MOUSE_BUTTON_LEFT);
}

void RenderWindow::onMessageMouseWheel(SHORT delta)
{
	owner_->onMouseButtonDown(delta < 0 ? kdw::MOUSE_BUTTON_WHEEL_DOWN : kdw::MOUSE_BUTTON_WHEEL_UP);
	owner_->onMouseWheel(delta);
}

void RenderWindow::onMessageLButtonUp(UINT button, int x, int y)
{
	owner_->onMouseButtonUp(kdw::MOUSE_BUTTON_LEFT);
}


void RenderWindow::onMessageRButtonDown(UINT button, int x, int y)
{
	kdw::Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();
	
	owner_->onMouseButtonDown(kdw::MOUSE_BUTTON_RIGHT);
}

void RenderWindow::onMessageRButtonUp(UINT button, int x, int y)
{
	owner_->onMouseButtonUp(kdw::MOUSE_BUTTON_RIGHT);
}

void RenderWindow::onMessageMButtonDown(UINT button, int x, int y)
{
	kdw::Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();
	owner_->onMouseButtonDown(kdw::MOUSE_BUTTON_MIDDLE);
}

void RenderWindow::onMessageMButtonUp(UINT button, int x, int y)
{
	owner_->onMouseButtonUp(kdw::MOUSE_BUTTON_MIDDLE);
}

int RenderWindow::onMessageKillFocus(HWND focusedWindow)
{
	::RedrawWindow(GetParent(handle_), 0, 0, RDW_INVALIDATE | RDW_NOERASE);
	return __super::onMessageKillFocus(focusedWindow);
}

BOOL RenderWindow::onMessageSize(UINT type, USHORT width, USHORT height)
{
	if(initialized_ && !lockRedraw_){
		lockRedraw_ = true;
		renderDevice_->selectRenderWindow(renderWindow_);
		renderDevice_->ChangeSize(width, height, RENDERDEVICE_MODE_WINDOW);
		owner_->onResize(int(width), int(height));
		renderDevice_->selectRenderWindow(0);
		lockRedraw_ = false;
	}
	return __super::onMessageSize(type, width, height);
}

void RenderWindow::onMessageShowWindow(BOOL show, int status)
{
	if(show && !initialized_)
		initializeRenderDevice();
	__super::onMessageShowWindow(show, status);
}

// ---------------------------------------------------------------

class ViewportImpl : public _WidgetWindow{
public:
	ViewportImpl(kdw::Viewport* owner, cInterfaceRenderDevice* renderDevice);
	~ViewportImpl();

	const char* className() const{ return "kdw.ViewportImpl"; }
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
protected:
	friend class kdw::Viewport;
	friend class RenderWindow;
	kdw::Viewport* owner_;
	RenderWindow* window_;

	HPEN activeCaptionPen_;
	HPEN inactiveCaptionPen_;
	HPEN blackPen_;
};

ViewportImpl::ViewportImpl(kdw::Viewport* owner, cInterfaceRenderDevice* renderDevice)
: _WidgetWindow(owner)
, owner_(owner)
, window_(0)
, activeCaptionPen_(0)
, inactiveCaptionPen_(0)
, blackPen_(0)
{
	VERIFY(create("test", WS_CHILD | WS_TABSTOP, Recti(0, 0, 32, 32), *Win32::_globalDummyWindow));
	VERIFY(activeCaptionPen_ = CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_ACTIVECAPTION)));
	VERIFY(inactiveCaptionPen_ = CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_INACTIVECAPTION)));
	VERIFY(blackPen_ = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));

	ASSERT(::IsWindow(handle_));

	window_ = new RenderWindow(this, owner, renderDevice);
	ASSERT(window_ != 0);
	ASSERT(::IsWindow(*window_));
}

ViewportImpl::~ViewportImpl()
{
	delete window_;
	::DeleteObject(activeCaptionPen_);
	::DeleteObject(inactiveCaptionPen_);
	::DeleteObject(blackPen_);
}





LRESULT ViewportImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message){
	case WM_DESTROY:
		break;
	case WM_ERASEBKGND:
		return FALSE;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_SETFOCUS:
		::SetFocus(*window_);
	case WM_KILLFOCUS:
		::RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_NOERASE);
		break;
	case WM_SHOWWINDOW:
		{
			BOOL visible = BOOL(wparam);
			if(visible)
				::ShowWindow(*window_, SW_SHOWNOACTIVATE);
			else
				::ShowWindow(*window_, SW_HIDE);
		}
	case WM_SIZE:
		{
			if(window_){
				RECT rect;
				::GetClientRect(handle_, &rect);

				Recti pos(4, 4, rect.right - rect.left - 8, rect.bottom - rect.top - 8);
				window_->move(pos);
			}
		}
		break;
	case WM_PAINT:
		{
			RECT rect;
			::GetClientRect(handle_, &rect);
			rect.left += owner_->border_;
			rect.top += owner_->border_;
			rect.bottom -= owner_->border_;
			rect.right -= owner_->border_;

			PAINTSTRUCT paintStruct;
			HDC dc = BeginPaint(handle_, &paintStruct);
			bool focused = ::GetFocus() == *window_;

			HBRUSH brush = GetStockBrush(HOLLOW_BRUSH);
			HPEN pen = focused ? activeCaptionPen_ : inactiveCaptionPen_;
			HBRUSH oldBrush = HBRUSH(::SelectObject(dc, brush));
			HPEN oldPen = HPEN(::SelectObject(dc, blackPen_));
			::Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
			::SelectObject(dc, pen);
			::InflateRect(&rect, -1, -1);
			::Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
			::InflateRect(&rect, -1, -1);
			::Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
			::SelectObject(dc, blackPen_);
			::InflateRect(&rect, -1, -1);
			::Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
			::SelectObject(dc, oldPen);
			::SelectObject(dc, oldBrush);

			EndPaint(handle_, &paintStruct);
			return TRUE;
		}
	}
	return __super::onMessage(message, wparam, lparam);
}


#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
Viewport::Viewport(cInterfaceRenderDevice* renderDevice, bool continuousUpdate, int border)
: _WidgetWithWindow(new ViewportImpl(this, renderDevice), border)
, showFocusFrame_(false)
, fillColor_(0, 0, 0)
, size_(32 + border_, 32 + border_)
, continuousUpdate_(false)
, renderDevice_(renderDevice)
{
	xassert(renderDevice_);
	setBorder(border);
	_setMinimalSize(Vect2i(border_, border_));
	setContinuousUpdate(continuousUpdate);
}
#pragma warning(pop)

Viewport::~Viewport()
{
}

void Viewport::onMouseMove()
{

}

void Viewport::redraw(bool updateNow)
{
	if(updateNow)
		RedrawWindow(*impl()->window_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	else
		RedrawWindow(*impl()->window_, 0, 0, RDW_INVALIDATE);
}

void Viewport::setFillColor(unsigned char red, unsigned char green, unsigned char blue)
{
	fillColor_.r = red;
	fillColor_.g = green;
	fillColor_.b = blue;
}


cRenderWindow* Viewport::renderWindow()
{
	return impl()->window_->renderWindow();
}

void Viewport::captureMouse()
{
	::SetCapture(*impl()->window_);
}

void Viewport::releaseMouse()
{
	if(::GetCapture() == *impl()->window_)
		::ReleaseCapture();
}

void Viewport::setContinuousUpdate(bool continuousUpdate)
{
	bool listener = impl()->window_->isMessageListener();
	if(listener != continuousUpdate)
		impl()->window_->setListenForMessage(continuousUpdate);
	continuousUpdate_ = continuousUpdate;
}

void Viewport::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_DESIGN)){
		ar.serialize(_property(continuousUpdate_, this, &Viewport::setContinuousUpdate),
							"continuousUpdate", "Непрерывно обновлять");
		ar.serialize(fillColor_, "fillColor", "Цвет фона");
		ar.serialize(showFocusFrame_, "showFocusFrame", "Показывать рамку фокуса");
	}
	Widget::serialize(ar);
}

ViewportImpl* Viewport::impl()
{
	return static_cast<ViewportImpl*>(_window());
}

}
