#include "StdAfx.h"

#include "kdw/RadioButton.h"
#include "kdw/_WidgetWindow.h"
#include <windowsx.h>

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"


namespace kdw{

REGISTER_CLASS(Widget, RadioButton, "Радио галочка");

class RadioButtonGroup: public ShareHandleBase{
public:
    void add(kdw::RadioButton* rBtn);
	void remove(kdw::RadioButton* rBtn);
	typedef std::vector<kdw::RadioButton*> Group;
	Group group;
};

void RadioButtonGroup::add(kdw::RadioButton* rBtn)
{
	group.push_back(rBtn);
}

void RadioButtonGroup::remove(kdw::RadioButton* rBtn)
{
	Group::iterator i = std::find(group.begin(), group.end(), rBtn);
	if(i != group.end())
		group.erase(i);
}

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

class RadioButtonImpl: public _WidgetWindow{
public:
	RadioButtonImpl(RadioButton* owner, RadioButton* groupWith);
	~RadioButtonImpl();
	const char* className() const{ return "BUTTON"; }
	
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags);
	int onMessageSetFocus(HWND lastFocusedWindow);
	//BOOL onMessageEraseBkgnd(HDC dc);
	LRESULT defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam);

	void setRadioButtonText(const char* text);
	void setRadioButtonStatus(bool status);
	RadioButtonGroup* group() const { return group_; }
protected:
	void setFocusNext();
	void setFocusPrev();
	kdw::RadioButton* owner_;
	ShareHandle<RadioButtonGroup> group_;
	static WNDPROC controlWindowProc_;
};

WNDPROC RadioButtonImpl::controlWindowProc_ = 0;

#pragma warning(push)
#pragma warning(disable: 4312) // 'type cast' : conversion from 'LONG' to 'HINSTANCE' of greater size

RadioButtonImpl::RadioButtonImpl(RadioButton* owner, RadioButton* groupWith)
: _WidgetWindow(owner)
, owner_(owner)
{
	if(!groupWith)
		group_ = new RadioButtonGroup();
	else
		group_ = groupWith->group();
	group_->add(owner);
	VERIFY(create("", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_NOTIFY, Recti(0, 0, 42, 42), *Win32::_globalDummyWindow));

	controlWindowProc_ = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(handle_, GWLP_WNDPROC));
	::SetWindowLongPtr(handle_, GWLP_WNDPROC, reinterpret_cast<LONG>(&Win32::universalWindowProcedure));
	::SetWindowLongPtr(handle_, GWLP_USERDATA, reinterpret_cast<LONG>(this));

	SetWindowFont(handle_, Win32::defaultFont(), FALSE);
}

#pragma warning(pop)

RadioButtonImpl::~RadioButtonImpl()
{
	group_->remove(owner_);
}

void RadioButtonImpl::setFocusPrev()
{
	RadioButtonGroup::Group::iterator i;
	RadioButtonGroup::Group::iterator j;
	FOR_EACH(group()->group, i){
		if((*i) == owner_){
			if(i != group()->group.begin())
				::SetFocus((*j)->window()->get());
			break;
		}
		j = i;
	}
}

void RadioButtonImpl::setFocusNext()
{
	RadioButtonGroup::Group::iterator i;
	bool mustBreak = false;
	FOR_EACH(group()->group, i){
		if(mustBreak){
			::SetFocus((*i)->window()->get());
			break;
		}
		if((*i) == owner_)
			mustBreak = true;
	}
}

void RadioButtonImpl::setRadioButtonStatus(bool status)
{
	if(status){
		RadioButtonGroup::Group::iterator it; 
		FOR_EACH(group_->group, it)
			if(*it != owner_)
				(*it)->setStatus(false);
	}
	ASSERT(::IsWindow(handle_));
	::SendMessage(handle_, BM_SETCHECK, status ? BST_CHECKED : BST_UNCHECKED, 0);
}
void RadioButtonImpl::setRadioButtonText(const char* text)
{
	ASSERT(::IsWindow(handle_));
	VERIFY(::SetWindowText(handle_, text));

	HFONT font = GetWindowFont(handle_);
	Vect2i textSize = Win32::calculateTextSize(handle_, font, text);

	owner_->_setMinimalSize(textSize + Vect2i(GetSystemMetrics(SM_CXBORDER) * 2 + 6 + 1,
		GetSystemMetrics(SM_CYBORDER) * 2 + 6 + 1) + Vect2i(owner_->border(), owner_->border()));
	owner_->_queueRelayout();
}

int RadioButtonImpl::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	if(keyCode == VK_UP || keyCode == VK_LEFT)
		setFocusPrev();
	else if(keyCode == VK_DOWN || keyCode == VK_RIGHT)
		setFocusNext();
	return __super::onMessageKeyDown(keyCode, count, flags);
}

int RadioButtonImpl::onMessageSysKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	if(keyCode == VK_UP || keyCode == VK_LEFT)
		setFocusPrev();
	else if(keyCode == VK_DOWN || keyCode == VK_RIGHT)
		setFocusNext();
	return __super::onMessageSysKeyDown(keyCode, count, flags);
}

LRESULT RadioButtonImpl::defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
	return ::CallWindowProc(controlWindowProc_, handle_, message, wparam, lparam);
}

int RadioButtonImpl::onMessageSetFocus(HWND lastFocusedWindow)
{
	Widget::_setFocusedWidget(owner_);
	owner_->onChanged();
	setRadioButtonStatus(owner_->status());
	return 0;
}


/*
BOOL RadioButtonImpl::onMessageEraseBkgnd(HDC dc)
{
	return __super::onMessageEraseBkgnd(dc);
}
*/

// --------------------------------------------------------------------------------------------

RadioButton::RadioButton(RadioButton* groupWith, const char* text, int border)
: _WidgetWithWindow(new RadioButtonImpl(this, groupWith), border), status_(groupWith == 0)
{
	_setMinimalSize(Vect2i(24, 24));
	setText(text);
	window()->setRadioButtonStatus(status_);
}

RadioButtonImpl* RadioButton::window() const
{
	return static_cast<RadioButtonImpl*>(_window());
}

RadioButtonGroup* RadioButton::group() const
{ 
	return window()->group(); 
}

bool RadioButton::_focusable() const
{
	return Widget::_focusable() && status();
}

void RadioButton::setText(const char* text)
{
	text_ = text;
	window()->setRadioButtonText(text);
}

void RadioButton::onChanged()
{
	if(!status_){
		status_ = !status_; 
		signalChanged_.emit();
	}	
}

void RadioButton::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_DESIGN)){
		ar.serialize(text_, "text", "&Текст");
		ar.serialize(status_, "status", "&Статус");
	}
	Widget::serialize(ar);
}

void RadioButton::setStatus(bool status)
{
	status_ = status;
	window()->setRadioButtonStatus(status_);
}

};


#pragma warning(pop)

