#ifndef __KDW_WIN32_WINDOW_H_INCLUDED__
#define __KDW_WIN32_WINDOW_H_INCLUDED__

#include "kdw/API.h"
#include "kdw/Win32/Types.h"

#include "XTL/Rect.h"


struct tagDRAWITEMSTRUCT;
typedef tagDRAWITEMSTRUCT DRAWITEMSTRUCT;

struct tagMEASUREITEMSTRUCT;
typedef tagMEASUREITEMSTRUCT MEASUREITEMSTRUCT;

namespace Win32{

void KDW_API _setGlobalInstance(HINSTANCE instance);
HINSTANCE KDW_API _globalInstance();
/// проверяет зарегиистрирован ли класс кна
bool KDW_API isClassRegistered(const char* className);
bool KDW_API isKeyPressed(UINT keyCode);

LRESULT CALLBACK universalWindowProcedure(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

/// регистрирует класс окна использую обработчик сообщений библиотеки (вызывает метод onMessage)
//bool KDW_API registerClass(const char* className);

/// приступить к обработки сообщений вплоть до получения WM_QUIT 
int KDW_API basicMessageLoop(HACCEL acceleratorTable = 0);

Vect2i calculateTextSize(HWND window, HFONT font, const char* text);
HFONT defaultFont();
HFONT defaultBoldFont();
void initializeCommonControls();

class Window32;

/// инкапсуляция интерфейса BeginDeferWindowPos, DeferWindowPos, EndDeferWindowPos
class WindowPositionDeferer : public ShareHandleBase{
public:
	WindowPositionDeferer(Window32* parent, int numWindows = 1);
	~WindowPositionDeferer();

    void defer(Window32* window, Recti position);
protected:
	HDWP handle_;
	Window32* parent_;
};

class TimerInterface : public ShareHandleBase{
public:
	TimerInterface()
	: window_(0)
	{}
	virtual ~TimerInterface();

	virtual void onTimer(){}
	void setWindow(Win32::Window32* window){ 
		xassert(window_ == 0 || window == 0);
		window_ = window;
	}
	Window32* window(){ return window_; }
protected:
	Window32* window_;
};

/// инкапсулирует Windows-окно
class KDW_API Window32 : public ShareHandleBase{
public:
	Window32(HWND handle = 0);
	virtual ~Window32();
	HWND attach(HWND handle);
	HWND detach();

	virtual const char* className() const{ return "kdw.Win32.Window"; }
	virtual bool registerClass(const char* className);
		
	virtual LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	
	virtual LRESULT defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam);

	virtual BOOL onMessageMeasureItem(UINT id, MEASUREITEMSTRUCT* drawItemStruct);
	virtual BOOL onMessageDrawItem(UINT id, DRAWITEMSTRUCT* drawItemStruct);

	virtual int onMessageDestroy();
	virtual int onMessageChar(UINT code, USHORT count, USHORT flags);
	virtual int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	
	virtual int onMessageKeyUp(UINT keyCode, USHORT count, USHORT flags);
	virtual int onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags);
	virtual int onMessageSysKeyUp(UINT keyCode, USHORT count, USHORT flags);

	virtual BOOL onMessageEraseBkgnd(HDC dc);
	virtual void onMessageShowWindow(BOOL show, int status);
	virtual BOOL onMessageSize(UINT type, USHORT width, USHORT height);
	virtual int onMessageCommand(USHORT command, USHORT id, HWND wnd);
	virtual int onMessageKillFocus(HWND focusedWindow);
	virtual int onMessageSetFocus(HWND lastFocusedWindow);

	virtual BOOL onMessageSetCursor(HWND window, USHORT hitTest, USHORT message);
	virtual void onMessageMouseMove(UINT button, int x, int y);
	virtual void onMessageMouseWheel(SHORT delta);
	
	virtual void onMessageLButtonDblClk(int x, int y);

	virtual void onMessageLButtonDown(UINT button, int x, int y);
	virtual void onMessageLButtonUp(UINT button, int x, int y);

	virtual void onMessageMButtonDown(UINT button, int x, int y);
	virtual void onMessageMButtonUp(UINT button, int x, int y);

	virtual void onMessageRButtonDown(UINT button, int x, int y);
	virtual void onMessageRButtonUp(UINT button, int x, int y);

	virtual void onMessagePaint();
	virtual void onMessageTimer(int id);

	bool isWindow() const;

	Window32* parent() const;

	// api functions
    bool create(const char* windowName, UINT style, const Recti& position, HWND parentWnd, UINT exStyle = 0);
	bool creating() const{ return creating_; }
	void destroy();

	void setWindowText(const char* windowText);
	void enable(bool enabled);
	void show(int showCommand);
	Recti getRect() const;
	void move(const Recti& position, bool redraw = false);
	void update();
	bool isVisible() const;

	void setStyle(UINT style);
	UINT getStyle();
	void setParent(Window32* newParent);

	LONG_PTR setUserDataLongPtr(LONG_PTR newLongPtr);
	LONG_PTR getUserDataLongPtr();

	LONG_PTR getLongPtr(int index);
	LONG_PTR setLongPtr(int index, LONG_PTR newLongPtr);

	typedef ShareHandle<WindowPositionDeferer> PositionDeferer;
	PositionDeferer positionDeferer(int numWindows = 1);	

	void clientToScreen(RECT& rect);
	void screenToClient(RECT& rect);
	//

	HWND get() const{ return handle_; }
	operator HWND() const{ return get(); }

	void attachTimer(TimerInterface* timer, int interval);
	void detachTimer(TimerInterface* timer);
	typedef std::vector<ShareHandle< TimerInterface> > Timers;
protected:
	Timers timers_;
	friend class WindowPositionDeferer;
	WindowPositionDeferer* positionDeferer_;
	bool handleOwner_;
	bool creating_;
	HWND handle_;
};

extern Win32::Window32* _globalDummyWindow;

inline TimerInterface::~TimerInterface()
{
	if(window_)
		window_->detachTimer(this);
}

}

#endif
