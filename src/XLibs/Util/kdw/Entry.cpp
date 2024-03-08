#include "StdAfx.h"
#include "kdw/Entry.h"
#include "kdw/Container.h"
#include "kdw/_WidgetWindow.h"
#include <windows.h>
#include <windowsx.h>

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"

namespace kdw{
REGISTER_CLASS(Widget, Entry, "Поле ввода");

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

class EntryImpl : public _WidgetWindow{
public:
	EntryImpl(kdw::Entry* owner, bool multiline);
	~EntryImpl();
	const char* className() const{ return "EDIT"; }
	void setText(const char* text);

	int onMessageChar(UINT code, USHORT count, USHORT flags);
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);
	
	BOOL onMessageEraseBkgnd(HDC dc);
	int onMessageCommand(USHORT command, USHORT id, HWND wnd);
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam);

	void commit();
	HWND edit() { return *this; }
protected:
	kdw::Entry* owner_;
	bool setting_;
	static WNDPROC controlWindowProc_;
};

WNDPROC EntryImpl::controlWindowProc_ = 0;

#pragma warning(disable: 4312) // 'type cast' : conversion from 'LONG' to 'HINSTANCE' of greater size
EntryImpl::EntryImpl(kdw::Entry* owner, bool multiline)
: _WidgetWindow(owner)
, owner_(owner)
, setting_(false)
{
	VERIFY(create("", WS_CHILD | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL 
		| (multiline ? ES_MULTILINE : 0),
		Recti(0, 0, 800, 60), *Win32::_globalDummyWindow, WS_EX_CLIENTEDGE));

	controlWindowProc_ = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(handle_, GWLP_WNDPROC));
	::SetWindowLongPtr(handle_, GWLP_WNDPROC, reinterpret_cast<LONG>(&Win32::universalWindowProcedure));
	LONG userData = ::GetWindowLong(handle_, GWL_USERDATA);
	::SetWindowLong(handle_, GWL_USERDATA, reinterpret_cast<LONG>(this));

	HFONT font = Win32::defaultFont();
	SetWindowFont(*this, font, FALSE);

	Vect2i textSize = Win32::calculateTextSize(*this, font, " ");
	owner_->_setMinimalSize(textSize + Vect2i(GetSystemMetrics(SM_CXBORDER) * 2 + 6 + 1, GetSystemMetrics(SM_CYBORDER) * 2 + 6 + 1));
}

EntryImpl::~EntryImpl()
{

}

void EntryImpl::commit()
{
	char buffer[1024];
	GetWindowText(*this, buffer, sizeof(buffer) - 1);

	ASSERT(owner_);
	owner_->text_ = buffer;
	owner_->onEdited();
	//}
}

void EntryImpl::setText(const char* text)
{
	setting_ = true;
	VERIFY(::SetWindowText(*this, text));
	setting_ = false;
}


LRESULT EntryImpl::defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
	return ::CallWindowProc(controlWindowProc_, handle_, message, wparam, lparam);
}

int EntryImpl::onMessageChar(UINT code, USHORT count, USHORT flags)
{
	if(code == VK_RETURN || code == VK_ESCAPE || code == VK_TAB){
		commit();
		if(code != VK_TAB)
			return 0;
	}
	return __super::onMessageChar(code, count, flags);
}

int EntryImpl::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	if((keyCode == VK_RETURN && owner_->swallowReturn_) || keyCode == VK_ESCAPE/* || VK_TAB*/)
		return 0;
	return __super::onMessageKeyDown(keyCode, count, flags);
}

LRESULT EntryImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	ShareHandle<kdw::Widget> ref((owner_->numRef() == 0) ? 0 : owner_);

	return __super::onMessage(message, wparam, lparam);
}

int EntryImpl::onMessageCommand(USHORT command, USHORT code, HWND wnd)
{
	switch(command){
	case EN_CHANGE:
		if(!setting_){
			char buffer[1024];
			GetWindowText(*this, buffer, sizeof(buffer) - 1);
			owner_->text_ = buffer;
			owner_->onChanged();
		}
		return TRUE;
	case EN_SETFOCUS:
		kdw::Widget::_setFocusedWidget(owner_);
		Edit_SetSel(*this, 0, -1);
		return TRUE;
	case EN_KILLFOCUS:
		if(GetFocus() != *this)
			commit();
		return TRUE;
	}
	return __super::onMessageCommand(command, code, wnd);
}

BOOL EntryImpl::onMessageEraseBkgnd(HDC dc)
{
	return __super::onMessageEraseBkgnd(dc);
	//return FALSE;
}


Entry::Entry(const char* text, bool multiline, int border)
: _WidgetWithWindow(0, border)
, swallowReturn_(false)
{
	setWindow(new EntryImpl(this, multiline)); 
	setBorder(border);
	setText(text);
}

Entry::~Entry()
{
}

EntryImpl* Entry::impl() const
{
	return static_cast<EntryImpl*>(_window());
}

void Entry::setSelection(EntrySelection selection)
{
	Edit_SetSel(impl()->edit(), selection.start(), selection.end());
}

EntrySelection Entry::selection() const
{
	DWORD range = SendMessage(*impl(), EM_GETSEL, 0, 0);
	WORD start = LOWORD(range);
	WORD end = HIWORD(range);
	return EntrySelection(start, end);
}

void Entry::replace(EntrySelection selection, const char* text)
{
	setSelection(selection);
	VERIFY(SendMessage(*impl(), EM_REPLACESEL, 0, (LPARAM)text));
}

void Entry::setText(const char* text)
{
	impl()->setText(text);
}

void Entry::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_DESIGN)){
		ar.serialize(text_, "text", "&Текст");
	}
	Widget::serialize(ar);
}

void Entry::commit()
{
	impl()->commit();
}

};

#pragma warning(pop)
