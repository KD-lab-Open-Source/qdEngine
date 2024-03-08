#include "StdAfx.h"
#include <windows.h>
#include <commctrl.h>

#include "kdw/Win32/Window.h"
#include "kdw/Application.h"

namespace Win32{

#ifdef _MSC_VER
EXTERN_C IMAGE_DOS_HEADER __ImageBase; // mega-hint!
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#else
# error Something broken horribly...
#endif

static HFONT initializeDefaultFont()
{
	NONCLIENTMETRICS nonClientMetrics;
	nonClientMetrics.cbSize = sizeof(nonClientMetrics);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(nonClientMetrics), (PVOID)&nonClientMetrics, 0);
	return CreateFontIndirect(&nonClientMetrics.lfMessageFont);
}

static Win32::Window32* initializeGlobalDummyWindow()
{
	Win32::Window32* window = new Win32::Window32();
	VERIFY(window->create("dummy", 0, Recti(0, 0, 100, 100), 0));
	return window; 
}


HINSTANCE globalApplicationInstance_ = HINST_THISCOMPONENT;
Win32::Window32* _globalDummyWindow = initializeGlobalDummyWindow();
HFONT _globalDefaultFont = initializeDefaultFont();

void KDW_API _setGlobalInstance(HINSTANCE instance)
{
	globalApplicationInstance_ = instance;
}

HINSTANCE KDW_API _globalInstance(){
	return globalApplicationInstance_;
}

HFONT defaultFont()
{
	return _globalDefaultFont;
}

struct DefaultBoldFont{
	HFONT font_;
	DefaultBoldFont()
	: font_(0)
	{
		NONCLIENTMETRICS nonClientMetrics;
		nonClientMetrics.cbSize = sizeof(nonClientMetrics);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(nonClientMetrics), (PVOID)&nonClientMetrics, 0);
		LOGFONT font = nonClientMetrics.lfMessageFont;
		font.lfWeight = FW_BOLD;
		font_ = CreateFontIndirect(&font);
	}
	~DefaultBoldFont(){
		DeleteObject(font_);
	}
};

DefaultBoldFont globalDefaultBoldFont;

HFONT defaultBoldFont()
{
	return globalDefaultBoldFont.font_;
}

Vect2i calculateTextSize(HWND window, HFONT font, const char* text)
{
	HDC dc = ::GetDC(window);
	xassert(dc);
	SIZE size = { 0, 0 };
	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	VERIFY(::GetTextExtentPoint32(dc, text, int(strlen(text)), &size));
	VERIFY(::SelectObject(dc, oldFont) == font);
	VERIFY(ReleaseDC(window, dc));
	return Vect2i(size.cx, size.cy);
}

LRESULT CALLBACK universalWindowProcedure(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	Window32 tempWindow(handle);
	if(message == WM_NCCREATE){
		CREATESTRUCT* createStruct = (CREATESTRUCT*)(lparam);
		tempWindow.setUserDataLongPtr((LONG_PTR)(createStruct->lpCreateParams));
	}

	Window32* window = (Window32*)tempWindow.getUserDataLongPtr();
	if(window && message == WM_NCCREATE)
		window->attach(handle);
    
	if(window == 0)
		return ::DefWindowProc(handle, message, wparam, lparam);

	return window->onMessage(message, wparam, lparam);
}

bool KDW_API isKeyPressed(UINT keyCode)
{
	return GetAsyncKeyState(keyCode) & 0x8000;
}

bool KDW_API isClassRegistered(const char* className)
{
	WNDCLASS classInfo;
	return GetClassInfo(globalApplicationInstance_, className, &classInfo) ? true : false;
}

int KDW_API basicMessageLoop(HACCEL acceleratorTable)
{
	MSG msg;
	
	while(GetMessage(&msg, 0, 0, 0)){ // бегает в цикле пока не получит WM_QUIT
		if(!TranslateAccelerator(msg.hwnd, acceleratorTable, &msg)){ 
			TranslateMessage(&msg); // генерит WM_CHAR из WM_KEYDOWN и т.п.
			DispatchMessage(&msg);
		}
	}
	return int(msg.wParam);
}

// ---------------------------------------------------------------------------

bool Window32::registerClass(const char* className)
{
	WNDCLASSEX windowClass;

	windowClass.cbSize = sizeof(WNDCLASSEX); 
	windowClass.style			= CS_DBLCLKS;
	windowClass.lpfnWndProc	    = &universalWindowProcedure;
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= globalApplicationInstance_ ;
	windowClass.hIcon			= 0; //LoadIcon(0, (LPCTSTR)IDI_KDWTEST);
	windowClass.hCursor		    = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
	windowClass.lpszMenuName	= 0;
	windowClass.lpszClassName	= className;
	windowClass.hIconSm		    = 0; //LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	
	if(::RegisterClassEx(&windowClass) != 0)
		return true;
	else
		return false;
}

Window32::Window32(HWND handle)
: handle_(0)
, positionDeferer_(0)
, handleOwner_(false)
, creating_(false)
{
	attach(handle);
}

Window32::~Window32()
{
	if(::IsWindow(handle_)){
		if(handleOwner_){
			ShowWindow(handle_, SW_HIDE);
			VERIFY(DestroyWindow(handle_) != 0); 

			MSG msg;
			while(PeekMessage(&msg, handle_, 0, 0, PM_REMOVE)){
				UINT message = msg.message;
			}

			handle_ = 0;
		}
		else{
			xassert(getUserDataLongPtr() != reinterpret_cast<LONG_PTR>(this));
		}
	}

	//xassert(positionDeferer_ = 0);
	if(positionDeferer_){
		xassert(positionDeferer_->numRef() == 0);
		delete positionDeferer_;
	}
}

HWND Window32::attach(HWND handle)
{
	HWND result = handle_;
	handle_ = handle;
	return result;
}

HWND Window32::detach()
{
	return attach(0);
}



bool Window32::create(const char* windowName, UINT style, const Recti& position, HWND parentWnd, UINT exStyle)
{
	if(!Win32::_globalInstance())
		_setGlobalInstance(HINST_THISCOMPONENT);
	//xassert(Win32::_globalInstance() && " is not set!");
	creating_ = true;
	if(!isClassRegistered(className()))
		VERIFY(registerClass(className()));

	xassert(isClassRegistered(className()));
	if(parentWnd)
		xassert(::IsWindow(parentWnd));
	handle_ = ::CreateWindowEx(exStyle, className(), windowName, style,
		position.left(), position.top(), position.width(), position.height(), parentWnd, 0, 0, (LPVOID)(this));
	
	handleOwner_ = true;
	creating_ = false;
	if(handle_)
		return true;
	else
		return false;
}

LRESULT Window32::defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
	return ::DefWindowProc(handle_, message, wparam, lparam);
}

BOOL Window32::onMessageEraseBkgnd(HDC dc)
{
	return defaultWindowProcedure(WM_ERASEBKGND, WPARAM(dc), 0);
}

void Window32::onMessageShowWindow(BOOL show, int status)
{
	defaultWindowProcedure(WM_SHOWWINDOW, WPARAM(show), LPARAM(status));
}

BOOL Window32::onMessageSize(UINT type, USHORT width, USHORT height)
{
	return defaultWindowProcedure(WM_SIZE, WPARAM(type), MAKELPARAM(width, height));
}

int Window32::onMessageSetFocus(HWND lastFocusedWindow)
{
	return defaultWindowProcedure(WM_SETFOCUS, WPARAM(lastFocusedWindow), 0);
}

int Window32::onMessageCommand(USHORT command, USHORT id, HWND wnd)
{
	return defaultWindowProcedure(WM_COMMAND, MAKEWPARAM(id, command), LPARAM(wnd));
}

int Window32::onMessageKillFocus(HWND focusedWindow)
{
	return defaultWindowProcedure(WM_KILLFOCUS, WPARAM(focusedWindow), 0);
}

BOOL Window32::onMessageDrawItem(UINT id, DRAWITEMSTRUCT* drawItemStruct)
{
	return defaultWindowProcedure(WM_DRAWITEM, WPARAM(id), LPARAM(drawItemStruct));
}

BOOL Window32::onMessageMeasureItem(UINT id, MEASUREITEMSTRUCT* drawItemStruct)
{
	return defaultWindowProcedure(WM_MEASUREITEM, WPARAM(id), LPARAM(drawItemStruct));
}

int Window32::onMessageChar(UINT code, USHORT count, USHORT flags)
{
	return defaultWindowProcedure(WM_CHAR, WPARAM(code), MAKELPARAM(count, flags));
}

int Window32::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	return defaultWindowProcedure(WM_KEYDOWN, WPARAM(keyCode), MAKELPARAM(count, flags));
}

int Window32::onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	return defaultWindowProcedure(WM_KEYUP, WPARAM(keyCode), MAKELPARAM(count, flags));
}

int Window32::onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	return defaultWindowProcedure(WM_SYSKEYDOWN, WPARAM(keyCode), MAKELPARAM(count, flags));
}

int Window32::onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags)
{
	return defaultWindowProcedure(WM_SYSKEYUP, WPARAM(keyCode), MAKELPARAM(count, flags));
}

BOOL Window32::onMessageSetCursor(HWND handle, USHORT hitTest, USHORT message)
{
	return defaultWindowProcedure(WM_SETCURSOR, WPARAM(handle), MAKELPARAM(hitTest, message));
}

void Window32::onMessageMouseWheel(SHORT delta)
{
	defaultWindowProcedure(WM_MOUSEWHEEL, MAKEWPARAM(0, delta), 0);
}

void Window32::onMessageMouseMove(UINT button, int x, int y)
{
	defaultWindowProcedure(WM_MOUSEMOVE, button, MAKELPARAM(SHORT(x), SHORT(y)));
}

void Window32::onMessageLButtonDblClk(int x, int y)
{
	defaultWindowProcedure(WM_LBUTTONDBLCLK, 0, MAKELPARAM(SHORT(x), SHORT(y)));
}

void Window32::onMessageLButtonDown(UINT button, int x, int y)
{
	defaultWindowProcedure(WM_LBUTTONDOWN, WPARAM(button), MAKELPARAM(SHORT(x), SHORT(y)));
}

void Window32::onMessageLButtonUp(UINT button, int x, int y)
{
	defaultWindowProcedure(WM_LBUTTONUP, WPARAM(button), MAKELPARAM(SHORT(x), SHORT(y)));
}

void Window32::onMessageMButtonDown(UINT button, int x, int y)
{
	defaultWindowProcedure(WM_MBUTTONDOWN, WPARAM(button), MAKELPARAM(SHORT(x), SHORT(y)));
}

void Window32::onMessageMButtonUp(UINT button, int x, int y)
{
	defaultWindowProcedure(WM_MBUTTONUP, WPARAM(button), MAKELPARAM(SHORT(x), SHORT(y)));
}

void Window32::onMessageRButtonDown(UINT button, int x, int y)
{
	defaultWindowProcedure(WM_RBUTTONDOWN, WPARAM(button), MAKELPARAM(SHORT(x), SHORT(y)));
}

void Window32::onMessageRButtonUp(UINT button, int x, int y)
{
	defaultWindowProcedure(WM_RBUTTONUP, WPARAM(button), MAKELPARAM(SHORT(x), SHORT(y)));
}

int Window32::onMessageDestroy()
{
	while(!timers_.empty()){
		if(timers_.back())
			detachTimer(timers_.back());
		else
			timers_.pop_back();
	}
	return defaultWindowProcedure(WM_DESTROY, 0, 0);
}

void Window32::onMessagePaint()
{
	defaultWindowProcedure(WM_PAINT, 0, 0);
}

void Window32::onMessageTimer(int id)
{
	int index = id - 100;
	if(index >= 0 && index < timers_.size()){
		TimerInterface* timer = timers_[index];
		if(timer)
			timer->onTimer();
	}
	
	defaultWindowProcedure(WM_TIMER, id, 0); // упускаем callback
}

LRESULT Window32::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	xassert(::IsWindow(handle_));
	switch(message){
		case WM_CREATE:
		{
			CREATESTRUCT* createStruct = (CREATESTRUCT*)lparam;
			return TRUE;
		}
		case WM_DESTROY:
		{
			setUserDataLongPtr(0);
			return onMessageDestroy();
		}
		case WM_NCDESTROY:
		{
			break;
		}
		case WM_KILLFOCUS:
		{
			HWND wnd = HWND(wparam);
			return onMessageKillFocus(wnd);
		}
		case WM_SHOWWINDOW:
		{
			BOOL show = BOOL(wparam);
			int status = int(lparam);
			onMessageShowWindow(show, status);
			return 0;
		}
		case WM_SIZE:
		{
			UINT type = UINT(wparam);
			USHORT width = LOWORD(lparam);
			USHORT height = HIWORD(lparam);
			return onMessageSize(type, width, height);
		}
		case WM_SETFOCUS:
		{
			HWND wnd = HWND(wparam);
			return onMessageSetFocus(wnd);
		}
		case WM_COMMAND:
		{
			USHORT command = HIWORD(wparam);
			USHORT id = LOWORD(wparam);
			HWND wnd = HWND(lparam);
			// возвращаем WM_COMMAND обратно
			if(wnd != handle_)
				::SendMessage(wnd, message, wparam, lparam);
			return onMessageCommand(command, id, wnd);
		}
		case WM_MEASUREITEM:
		{
			UINT id = UINT(wparam);
			::MEASUREITEMSTRUCT* measureItemStruct = (::MEASUREITEMSTRUCT*)(lparam);
			return onMessageMeasureItem(id, measureItemStruct);
		}
		case WM_DRAWITEM:
		{
			UINT id = UINT(wparam);
			::DRAWITEMSTRUCT* drawItemStruct = (::DRAWITEMSTRUCT*)(lparam);
			//if(drawItemStruct->hwndItem != handle_)
			//	::SendMessage(drawItemStruct->hwndItem, message, wparam, lparam);
			return onMessageDrawItem(id, drawItemStruct);
		}
		case WM_ERASEBKGND:
		{
			HDC dc = HDC(wparam);
			return onMessageEraseBkgnd(dc);
		}
		case WM_KEYDOWN:
		{
			UINT code = (UINT)wparam;
			USHORT count = LOWORD(lparam);
			USHORT flags = HIWORD(lparam);
			return onMessageKeyDown(code, count, flags);
		}
		case WM_KEYUP:
		{
			UINT code = (UINT)wparam;
			USHORT count = LOWORD(lparam);
			USHORT flags = HIWORD(lparam);
			return onMessageKeyUp(code, count, flags);
		}
		case WM_SYSKEYDOWN:
		{
			UINT code = (UINT)wparam;
			USHORT count = LOWORD(lparam);
			USHORT flags = HIWORD(lparam);
			return onMessageSysKeyDown(code, count, flags);
		}
		case WM_SYSKEYUP:
		{
			UINT code = (UINT)wparam;
			USHORT count = LOWORD(lparam);
			USHORT flags = HIWORD(lparam);
			return onMessageSysKeyUp(code, count, flags);
		}
		case WM_CHAR:
		{
			UINT code = (UINT)wparam;
			USHORT count = LOWORD(lparam);
			USHORT flags = HIWORD(lparam);
			return onMessageChar(code, count, flags);
		}
		case WM_SETCURSOR:
		{
			HWND handle = (HWND)wparam;
			USHORT hitTest = LOWORD(lparam);
			USHORT message = HIWORD(lparam);
			return onMessageSetCursor(handle, hitTest, message);
		}
		case WM_MOUSEMOVE:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageMouseMove(button, x, y);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			SHORT delta = HIWORD(wparam);
            onMessageMouseWheel(delta);
			return 0;
		}
		case WM_LBUTTONDBLCLK:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageLButtonDblClk(x, y);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageLButtonDown(button, x, y);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageLButtonUp(button, x, y);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageMButtonDown(button, x, y);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageMButtonUp(button, x, y);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageRButtonDown(button, x, y);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			UINT button = (UINT)wparam;
			SHORT x = LOWORD(lparam);
			SHORT y = HIWORD(lparam);
			onMessageRButtonUp(button, x, y);
			return 0;
		}
		case WM_PAINT:
		{
			onMessagePaint();
			return 0;
		}
		case WM_TIMER:
		{
			int id = int(wparam);
			onMessageTimer(id);
			return 0;
		}
	}
	return defaultWindowProcedure(message, wparam, lparam);
}

Recti Window32::getRect() const
{
	RECT rect;
	VERIFY(GetWindowRect(handle_, &rect));
	return Recti(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

void Window32::move(const Recti& rect, bool redraw)
{
	xassert(::IsWindow(handle_));
	if(::IsWindow(handle_))
	VERIFY(::MoveWindow(handle_, rect.left(), rect.top(), rect.width(), rect.height(), redraw ? TRUE : FALSE));
}

Window32* Window32::parent() const
{
	HWND parent = GetParent(handle_);
	if(parent)
		return (Window32*)(Window32(parent).getUserDataLongPtr());
	else
		return 0;
}

LONG_PTR Window32::getLongPtr(int index)
{
	return ::GetWindowLongPtr(handle_, index);
}

LONG_PTR Window32::setLongPtr(int index, LONG_PTR newLongPtr)
{
	xassert(::IsWindow(handle_));
	return ::SetWindowLongPtr(handle_, index, LONG(newLongPtr));
}

LONG_PTR Window32::setUserDataLongPtr(LONG_PTR newLongPtr)
{
	return ::SetWindowLongPtr(handle_, GWL_USERDATA, LONG(newLongPtr));
}

LONG_PTR Window32::getUserDataLongPtr()
{
	return ::GetWindowLongPtr(handle_, GWL_USERDATA);
}

void Window32::setWindowText(const char* windowText)
{
	VERIFY(::SetWindowText(handle_, windowText));
}


void Window32::setStyle(UINT style)
{
	VERIFY(::SetWindowLong(handle_, GWL_STYLE, style));
	VERIFY(::SetWindowPos(handle_, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED));
	if(::IsWindowVisible(handle_))
		::RedrawWindow(handle_, 0, 0, RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
}

UINT Window32::getStyle()
{
	return ::GetWindowLong(handle_, GWL_STYLE);
}

void Window32::setParent(Window32* newParent)
{
	xassert(::IsWindow(handle_));
	::SetParent(handle_, newParent->get());
	xassert(::GetParent(handle_) == newParent->get());
	xassert(::IsWindow(handle_));
}

void Window32::enable(bool enabled)
{
	::EnableWindow(handle_, enabled);
	xassert(::IsWindowEnabled(handle_) == (enabled ? TRUE : FALSE));
}

void Window32::show(int showCommand)
{
	xassert(::IsWindow(handle_));
	::ShowWindow(handle_, showCommand);
}

void Window32::update()
{
	VERIFY(::UpdateWindow(handle_));
}

bool Window32::isVisible() const
{
	return ::IsWindowVisible(handle_) ? true : false;
}

Window32::PositionDeferer Window32::positionDeferer(int numWindows)
{
	if(positionDeferer_)
		return positionDeferer_;
	else{
		positionDeferer_ = new WindowPositionDeferer(this, numWindows);
		return positionDeferer_;
	}		
}

void Window32::clientToScreen(RECT& rect)
{
	::ClientToScreen(*this, (POINT*)(&rect));
	::ClientToScreen(*this, (POINT*)(&rect) + 1);
}

void Window32::attachTimer(TimerInterface* timer, int interval)
{
	xassert(std::find(timers_.begin(), timers_.end(), timer) == timers_.end());
	if(timer->window())
		timer->window()->detachTimer(timer);
	Timers::iterator it = std::find(timers_.begin(), timers_.end(), ShareHandle<TimerInterface>(0));
	int index = 0;
	if(it != timers_.end()){
		*it = timer;
		int index = std::distance(timers_.begin(), it);
	}
	else{
		timers_.push_back(timer);
		index = timers_.size() - 1;
	}
	timer->setWindow(this);
	SetTimer(*this, 100 + index, interval, 0);
}

void Window32::detachTimer(TimerInterface* timer)
{
	xassert(timer);
	xassert(timer->window() == this);
	Timers::iterator it = std::find(timers_.begin(), timers_.end(), timer);
	xassert(it != timers_.end());
	if(it != timers_.end()){
		int index = std::distance(timers_.begin(), it);
		KillTimer(*this, 100 + index);
		timer->setWindow(0);
		*it = 0;
		while(!timers_.empty() && timers_.back() == 0)
			timers_.pop_back();
	}
}

void Window32::screenToClient(RECT& rect)
{
	::ScreenToClient(*this, (POINT*)(&rect));
	::ScreenToClient(*this, (POINT*)(&rect) + 1);
}

WindowPositionDeferer::WindowPositionDeferer(Window32* parent, int numWindows)
: parent_(parent)
{
	handle_ = BeginDeferWindowPos(numWindows);
	xassert(handle_);
}

WindowPositionDeferer::~WindowPositionDeferer()
{
	xassert(handle_);
	VERIFY(::EndDeferWindowPos(handle_));
	parent_->positionDeferer_ = 0;
}

void WindowPositionDeferer::defer(Window32* window, Recti position)
{
	xassert(handle_);
	xassert(::IsWindow(*window));
	VERIFY(::DeferWindowPos(handle_, *window, 0, position.left(), position.top(), position.width(), position.height(), SWP_NOZORDER));
}

void initializeCommonControls()
{
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_WIN95_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	VERIFY(InitCommonControlsEx(&iccex));
}
	
}
