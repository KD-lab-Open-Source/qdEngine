#include "StdAfx.h"
#include "Viewport2D.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/MemoryDC.h"
#include "kdw/Win32/Handle.h"
#include <windows.h>

using Win32::AutoSelector;

namespace kdw{

const float ZoomTable[] = { 4.0f, 3.0f, 2.0f, 
		1.5f, 1.0f, 0.75f, 0.5f };

enum { MaxZoom = sizeof (ZoomTable) / sizeof (ZoomTable [0]) };

class Viewport2DImpl : public _WidgetWindow{
public:
	Viewport2DImpl(Viewport2D* owner);
	~Viewport2DImpl();

	const char* className() const{ return "kdw.Viewport2D"; }
	BOOL onMessageEraseBkgnd(HDC dc);

	void onMessagePaint();
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags);
	void onMessageLButtonDblClk(int x, int y);
	void onMessageMouseMove(UINT button, int x, int y);
	void onMessageMouseWheel(SHORT delta);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageRButtonUp(UINT button, int x, int y);
	void onMessageMButtonDown(UINT button, int x, int y);
	void onMessageMButtonUp(UINT button, int x, int y);
	int onMessageKillFocus(HWND focusedWindow);
	BOOL onMessageSize(UINT type, USHORT width, USHORT height);
protected:
	friend class Viewport2D;
	friend class RenderWindow;
	Viewport2D* owner_;
	bool initialized_;
};

Viewport2DImpl::Viewport2DImpl(Viewport2D* owner)
: _WidgetWindow(owner, 0)
, owner_(owner)
, initialized_(false)
{
	VERIFY(create("", WS_CHILD | WS_TABSTOP, Recti(0, 0, 0, 0), *Win32::_globalDummyWindow));
	initialized_ = true;
}


Viewport2DImpl::~Viewport2DImpl()
{
}

BOOL Viewport2DImpl::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

int Viewport2DImpl::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	owner_->onKeyDown(sKey(keyCode, true));
	return __super::onMessageKeyDown(keyCode, count, flags);
}

int Viewport2DImpl::onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	return __super::onMessageKeyUp(keyCode, count, flags);
}

int Viewport2DImpl::onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	return __super::onMessageSysKeyDown(keyCode, count, flags);
}

int Viewport2DImpl::onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	return __super::onMessageSysKeyUp(keyCode, count, flags);
}

void Viewport2DImpl::onMessagePaint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(handle_,&ps);
	{
		Color3c c = owner_->backgroundColor();
		Win32::MemoryDC memoryDC(dc);
		Win32::Handle<HBRUSH> brush(CreateSolidBrush(RGB(c.r, c.g, c.b)));
		::FillRect(memoryDC, &ps.rcPaint, brush);
		owner_->onRedraw(memoryDC);
	}
	EndPaint(handle_, &ps);
}

void Viewport2DImpl::onMessageMouseMove(UINT button, int x, int y)
{
	ASSERT(x > -0xFFFF && x < 0xFFFF);
	ASSERT(y > -0xFFFF && y < 0xFFFF);
	Vect2i delta = Vect2i(x, y) - owner_->mousePosition_;
	owner_->mousePosition_.x = x;
	owner_->mousePosition_.y = y;
	owner_->onMouseMove(delta);
}

void Viewport2DImpl::onMessageLButtonDblClk(int x, int y)
{
	Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();

	owner_->onMouseButtonDown(MOUSE_BUTTON_LEFT_DOUBLE);
}

void Viewport2DImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();

	owner_->onMouseButtonDown(MOUSE_BUTTON_LEFT);
}

void Viewport2DImpl::onMessageMouseWheel(SHORT delta)
{
	owner_->onMouseButtonDown(delta < 0 ? MOUSE_BUTTON_WHEEL_DOWN : MOUSE_BUTTON_WHEEL_UP);
}

void Viewport2DImpl::onMessageLButtonUp(UINT button, int x, int y)
{
	owner_->onMouseButtonUp(MOUSE_BUTTON_LEFT);
}


void Viewport2DImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();
	
	owner_->onMouseButtonDown(MOUSE_BUTTON_RIGHT);
}

void Viewport2DImpl::onMessageRButtonUp(UINT button, int x, int y)
{
	owner_->onMouseButtonUp(MOUSE_BUTTON_RIGHT);
}

void Viewport2DImpl::onMessageMButtonDown(UINT button, int x, int y)
{
	Widget::_setFocusedWidget(owner_);
	owner_->_setFocus();
	owner_->onMouseButtonDown(MOUSE_BUTTON_MIDDLE);
}

void Viewport2DImpl::onMessageMButtonUp(UINT button, int x, int y)
{
	owner_->onMouseButtonUp(MOUSE_BUTTON_MIDDLE);
}

int Viewport2DImpl::onMessageKillFocus(HWND focusedWindow)
{
	::RedrawWindow(*this, 0, 0, RDW_INVALIDATE | RDW_NOERASE);
	return __super::onMessageKillFocus(focusedWindow);
}

BOOL Viewport2DImpl::onMessageSize(UINT type, USHORT width, USHORT height)
{
	if(initialized_){
		owner_->onResize(int(width), int(height));
		owner_->size_.set(width, height);
	}
	return __super::onMessageSize(type, width, height);
}

// ---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4355) //  warning C4355: 'this' : used in base member initializer list

Viewport2D::Viewport2D(int border, int fontHeight)
: _WidgetWithWindow(new Viewport2DImpl(this), border)
, viewOffset_(Vect2f::ZERO)
, viewSize_(Vect2f::ZERO)
, zoomIndex_(4)
, scrolling_(false)
, lmbPressed_(false)
, rmbPressed_(false)
, backgroundColor_(64, 64, 64)
, fontHeight_(fontHeight)
{
	viewScale_ = ZoomTable[zoomIndex_];
	createFont();
}

#pragma warning(pop)

Viewport2D::~Viewport2D()
{
}

void Viewport2D::createFont()
{
	positionFont_ = CreateFont(round(fontHeight_*viewScale()), 0, 0, 0,
		FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
}

void Viewport2D::onRedraw(HDC dc)
{
	viewSize_.set(size().x, size().y);
}

void Viewport2D::onResize(int width, int height)
{
	viewSize_.set(width, height);
}

void Viewport2D::setVisibleArea(const Rectf& rect)
{
	viewOffset_ = -rect.left_top() - rect.size()*0.5f;
	viewScale_ = min(viewSize_.x/rect.size().x, viewSize_.y/rect.size().y);
}

Vect2f Viewport2D::coordsToScreen(const Vect2f& pos) const
{
	return (pos + viewOffset_)*viewScale() + Vect2f(viewSize_)*0.5f;
}

Vect2f Viewport2D::coordsFromScreen(const Vect2f& pos) const
{
	return (pos - Vect2f(viewSize_)*0.5f)/viewScale() - viewOffset_;
}

Rectf Viewport2D::visibleArea() const
{
	return Rectf(coordsFromScreen(Vect2f::ZERO), Vect2f(viewSize_)/viewScale());
}

void Viewport2D::onMouseButtonDown(kdw::MouseButton button)
{
	if(button == kdw::MOUSE_BUTTON_RIGHT){
		rmbPressed_ = true;
	}
	else if(button == kdw::MOUSE_BUTTON_LEFT){
		lmbPressed_ = true;
	}
	if(button == kdw::MOUSE_BUTTON_WHEEL_DOWN){
		zoomIndex_ = max(0, zoomIndex_ - 1);
		viewScale_ = ZoomTable[zoomIndex_];
		createFont();
		redraw();
	}
	else if(button == kdw::MOUSE_BUTTON_WHEEL_UP){
		zoomIndex_ = min (MaxZoom - 1, zoomIndex_ + 1);
		viewScale_ = ZoomTable[zoomIndex_];
		createFont();
		redraw();
	}
}

void Viewport2D::onMouseButtonUp(kdw::MouseButton button)
{
	if(button == kdw::MOUSE_BUTTON_RIGHT){
		rmbPressed_ = false;
	}
	if(button == kdw::MOUSE_BUTTON_LEFT){
		lmbPressed_ = false;
	}
}

void Viewport2D::onMouseMove(const Vect2i& delta)
{
	if(rmbPressed_){
		if(scrolling_)
			SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));
		scrolling_ = true;
		viewOffset_ += delta/viewScale();
		redraw();
	}
	else{
		if(scrolling_)
			SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW)));
		scrolling_ = false;
	}
}

void Viewport2D::redraw(bool updateNow)
{
	RedrawWindow(*impl(), 0, 0, RDW_INVALIDATE | (updateNow ? RDW_UPDATENOW : 0));
}

void Viewport2D::captureMouse()
{
	::SetCapture(*impl());
}

void Viewport2D::releaseMouse()
{
	if(::GetCapture() == *impl())
		::ReleaseCapture();
}

Viewport2DImpl* Viewport2D::impl()
{
	return static_cast<Viewport2DImpl*>(_window());
}

void Viewport2D::drawPixel(HDC dc, const Vect2f& pos, const Color4c& color)
{
	Vect2i posScr = coordsToScreen(pos);
	SetPixel(dc, posScr.x, posScr.y, color.RGBGDI());
}

void Viewport2D::drawCircle(HDC dc, const Vect2f& pos, float radius, const Color4c& color, int outline_width)
{
	Vect2f center = coordsToScreen(pos);
	float rad = radius*viewScale();

	AutoSelector oldPen(dc, CreateSolidBrush(RGB(color.r, color.g, color.b)));
	AutoSelector oldBrush(dc, CreatePen(PS_SOLID, outline_width, RGB(0, 0, 0)));
	Ellipse(dc,
		round(center.x - rad),
		round(center.y - rad),
		round(center.x + rad),
		round(center.y + rad));
}

void Viewport2D::drawLine(HDC dc, const Vect2f& _start, const Vect2f& _end, const Color4c& color, int style, int width)
{
	Vect2i start(coordsToScreen(_start));
	Vect2i end(coordsToScreen(_end));

	AutoSelector pen(dc, CreatePen(style, width, color.RGBGDI()));
	MoveToEx(dc, start.x, start.y, 0);
	LineTo(dc, end.x, end.y);
}


void Viewport2D::fillRectangle(HDC dc, const Rectf& rect, const Color4c& color)
{
	Vect2i left_top (coordsToScreen(Vect2f(rect.left(), rect.top())));
	Vect2i right_bottom (coordsToScreen(Vect2f(rect.right(), rect.bottom())));
	RECT rt = { left_top.x, left_top.y, right_bottom.x, right_bottom.y };
	Win32::Handle<HBRUSH> brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	FillRect(dc, &rt, brush);
}

void Viewport2D::drawRectangle(HDC dc, const Rectf& rect, const Color4c& color)
{
	Vect2i left_top (coordsToScreen(Vect2f(rect.left(), rect.top())));
	Vect2i right_bottom (coordsToScreen(Vect2f(rect.right(), rect.bottom())));
	RECT rt = { left_top.x, left_top.y, right_bottom.x, right_bottom.y };
	Win32::Handle<HBRUSH> brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	FrameRect(dc, &rt, brush);
}

void Viewport2D::drawText(HDC dc, const Rectf& rect, const char* text, TextAlign text_align, bool endEllipsis)
{
	Vect2i p1(coordsToScreen(Vect2f(rect.left(), rect.top())));
	Vect2i p2(coordsToScreen(Vect2f(rect.right(), rect.bottom())));
	RECT rt = { p1.x, p1.y, p2.x, p2.y };
	int flags = DT_VCENTER | DT_SINGLELINE;
	if (text_align == ALIGN_LEFT) {
		flags |= DT_LEFT;
	} else if (text_align == ALIGN_CENTER) {
		flags |= DT_CENTER;
	} else {
		flags |= DT_RIGHT;
	}
	if(endEllipsis)
		flags |= DT_END_ELLIPSIS;
	
	SetBkMode(dc, TRANSPARENT);
	DrawText(dc, text, strlen(text), &rt, flags);
	SetBkMode(dc, OPAQUE);
}

void Viewport2D::drawText(HDC dc, const Vect2f& pos, const char* text, const Color4c& textColor, const Color4c& backColor, TextAlign align)
{
	RECT rt;
	DrawText(dc, text, strlen(text), &rt, DT_CALCRECT);
	Vect2f size(rt.right - rt.left + 2, rt.bottom - rt.top + 2);

	Vect2f sign(0, -0.5f);
	if(align == ALIGN_CENTER)
		sign.x = -0.5f;
	else if(align == ALIGN_RIGHT)
			sign.x = 1;

	if(backColor.a){
		drawRectangle(dc, Rectf(pos - Vect2f::ID*pixelWidth(), (size + Vect2f::ID*2)/viewScale()), Color4c::BLACK);
		fillRectangle(dc, Rectf(pos, size/viewScale()), backColor);
	}

	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, textColor.RGBGDI());

	Vect2f posScr = coordsToScreen(pos) + size*sign;
	TextOut(dc, posScr.xi(), posScr.yi(), text, strlen(text));
}

}
