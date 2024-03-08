#include "StdAfx.h"
#include "kdw/Dialog.h"
#include "kdw/Win32/Window.h"

#include "kdw/Serialization.h"


class ButtonResponse : public kdw::Button
{
public:
	ButtonResponse(const char* text, int response, int border = 0)
	: kdw::Button(text, border)
	, response_(response)
	{
	}

	void onPressed(){
		signalPressed_.emit(response_);
	}
	sigslot::signal1<int>& signalPressed() { return signalPressed_; }

protected:
	sigslot::signal1<int> signalPressed_;
	int response_;

};

namespace kdw
{

typedef std::vector<Widget*> SpawnedDialogs;
static SpawnedDialogs spawnedDialogs;

static kdw::Widget* lastDialog()
{
	if(spawnedDialogs.empty())
		return 0;
	kdw::Widget* last = spawnedDialogs.back();
	xassert(last);
	if(!IsWindowEnabled(*_findWindow(last)))
		return 0;
	return last;
}

static HWND findOwner(HWND wnd){
	if(!wnd){
		Widget* last = lastDialog();
		if(last)
			return *_findWindow(last);
		else
			return 0;
	}
	while(HWND parent = GetParent(wnd))
		wnd = parent;
	return wnd;
}

static HWND findOwner(Widget* widget){
	if(!widget){
		Widget* last = lastDialog();
		if(last)
			return *_findWindow(last);
		else
			return 0;
	}
	HWND parentWnd = 0;
	Win32::Window32* window = _findWindow(widget);
	ASSERT(window);
	if(window)
		return findOwner(*window);
	else
		return 0;
}

Dialog::Dialog(HWND parent, int border)
: Window(parentWnd_ = findOwner(parent), border)
{
	init(border);
}

Dialog::Dialog(kdw::Widget* widget, int border)
: Window(parentWnd_ = findOwner(widget), border)
{
	init(border);
}

void Dialog::onKeyDefault()
{
	onResponse(defaultResponse_);
}

void Dialog::onKeyCancel()
{
	onResponse(cancelResponse_);
}


void Dialog::init(int border)
{
	setDefaultPosition(POSITION_CENTER);
	setResizeable(false);
	setMinimizeable(false);
	setBorder(10);
	
	vbox_ = new kdw::VBox(3, 0);
	Window::add(vbox_);
	{
		hboxButtons_ = new kdw::HBox(4, 0);
		vbox_->add(new kdw::HLine(), true);
		vbox_->add(hboxButtons_, true);
	}

	response_ = defaultResponse_ = cancelResponse_ = RESPONSE_CANCEL;

	hotkeyContext()->signalPressed(sKey(VK_RETURN)).connect(this, &Dialog::onKeyDefault);
	hotkeyContext()->signalPressed(sKey(VK_ESCAPE)).connect(this, &Dialog::onKeyCancel);

	hotkeyContext()->signalPressed(sKey(VK_RIGHT)).connect((Window*)this, &Window::onHotkeyFocusNext);
	hotkeyContext()->signalPressed(sKey(VK_LEFT)).connect((Window*)this, &Window::onHotkeyFocusPrev);
}

void Dialog::add(Widget* widget, bool placeInEnd, bool expand, bool fill)
{
	vbox_->add(widget, placeInEnd, expand, fill);
}

void Dialog::addButton(const char* label, int response, bool atRight)
{
	ButtonResponse* button = new ButtonResponse(label, response, 0);
	button->setRequestSize(Vect2i(68, 24));
	button->signalPressed().connect(this, &Dialog::onResponse);
	hboxButtons_->add(button, atRight);
	if(response == RESPONSE_OK || response == RESPONSE_YES){
		defaultResponse_ = response;
		setDefaultWidget(button);
	}
	if(response == RESPONSE_CANCEL || response == RESPONSE_NO)
		cancelResponse_ = response;
}


void Dialog::onResponse(int response)
{
	response_ = response;
	interruptModalLoop();
}

void Dialog::onClose()
{
	onResponse(cancelResponse_);
	signalClose_.emit();
}

void Dialog::setCancelResponse(int response)
{
	cancelResponse_ = response; 
}

void Dialog::setDefaultResponse(int response)
{
	defaultResponse_ = response; 
}

void Dialog::interruptModalLoop()
{
	signalClose_.emit();

	if(parentWnd_)
		::EnableWindow(parentWnd_, TRUE);

	hide();
}

int Dialog::showModal()
{
	spawnedDialogs.push_back(this);

	showAll();

	if(parentWnd_ && parentWnd_ != ::GetDesktopWindow()){
		ASSERT(::IsWindow(parentWnd_));
		if(::IsWindowEnabled(parentWnd_)){
			::SetWindowPos(*_window(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			::EnableWindow(parentWnd_, FALSE);
		}
	}
	vbox_->setFocus();
	
	Win32::MessageLoop loop;
	signalClose().connect(&loop, &Win32::MessageLoop::quit);
	loop.run();
	if(parentWnd_ && parentWnd_ != ::GetDesktopWindow()){
		xassert(::IsWindowEnabled(parentWnd_));
		if(::SetActiveWindow(parentWnd_) == 0){
			/*
			::SetWindowPos(parentWnd_, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			char windowText[501];
			GetWindowText(parentWnd_, windowText, 500);
			OutputDebugString(windowText);
			OutputDebugString("\n");
			*/
		}
	}

	xassert(!spawnedDialogs.empty() && spawnedDialogs.back() == this);
	spawnedDialogs.pop_back();
	return response_;
}

Dialog::~Dialog()
{
}

void Dialog::serialize(Archive& ar)
{
	Window::serialize(ar);
}

}
