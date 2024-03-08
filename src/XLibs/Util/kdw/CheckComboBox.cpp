#include "StdAfx.h"
#include <windows.h>
#include <windowsx.h>

#include "kdw/CheckComboBox.h"

#include "kdw/Container.h"

#include "kdw/_WidgetWindow.h"


#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"

namespace kdw{
REGISTER_CLASS(Widget, CheckComboBox, "Выпадающий список галочек")

class CheckComboBoxImpl : public _WidgetWindow{
public:
	CheckComboBoxImpl(kdw::CheckComboBox* owner);
	const char* className() const{ return "COMBOBOX"; }

	BOOL onMessageMeasureItem(UINT id, MEASUREITEMSTRUCT* drawItemStruct);
	BOOL onMessageDrawItem(UINT id, DRAWITEMSTRUCT* drawItemStruct);
	BOOL onMessageEnable(BOOL enabled);
	int onMessageSetFocus(HWND lastFocusedWindow);
	int onMessageKillFocus(HWND focusedWindow);
	int onMessageCommand(USHORT command, USHORT code, HWND wnd);
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);

	LRESULT defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam);
	void onEdited();
	void selectAll() {}
	void showDropDown();
	HWND comboBox(){ return handle_; }
	bool getCheck(int index) const;
	void setCheck(int index, bool checked);
	void updateValue();
protected:
	friend class CheckComboListBoxImpl;
	kdw::CheckComboBox* owner_;
	ShareHandle<CheckComboListBoxImpl> listBox_;
	bool itemHeightSet_;
	static WNDPROC controlWindowProc_;
};


// ---------------------------------------------------------------------------

WNDPROC CheckComboBoxImpl::controlWindowProc_ = 0;

class CheckComboListBoxImpl : public Win32::Window32{
public:
	CheckComboListBoxImpl(HWND handle, CheckComboBoxImpl* owner);
	~CheckComboListBoxImpl();

	void onMessageRButtonDown(UINT button, int x, int y);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	int onMessageDestroy();
	int onMessageChar(UINT code, USHORT count, USHORT flags);
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam);
protected:
	CheckComboBoxImpl* owner_;
	static WNDPROC controlWindowProc_;
};

WNDPROC CheckComboListBoxImpl::controlWindowProc_ = 0;

CheckComboListBoxImpl::CheckComboListBoxImpl(HWND handle, CheckComboBoxImpl* owner)
: Window32(handle)
, owner_(owner)
{
	controlWindowProc_ = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(handle, GWLP_WNDPROC));
	::SetWindowLongPtr(handle, GWLP_WNDPROC, reinterpret_cast<LONG>(&Win32::universalWindowProcedure));
	LONG userData = ::GetWindowLong(handle, GWL_USERDATA);
	::SetWindowLong(handle, GWL_USERDATA, reinterpret_cast<LONG>(this));

}

LRESULT CheckComboListBoxImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	case LB_GETCURSEL: {
		return -1;
	default:
		return Win32::Window32::onMessage(message, wparam, lparam);
	}
}

void CheckComboListBoxImpl::onMessageLButtonUp(UINT button, int x, int y)
{
	__super::onMessageLButtonUp(button, x, y);
}

CheckComboListBoxImpl::~CheckComboListBoxImpl()
{
	::SetWindowLong(handle_, GWL_USERDATA, 0);
}

int CheckComboListBoxImpl::onMessageDestroy()
{
	ASSERT(::IsWindow(*owner_));
	owner_->listBox_ = 0;
	return 0;
}

LRESULT CheckComboListBoxImpl::defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
	return CallWindowProc(controlWindowProc_, handle_, message, wparam, lparam);
}

void CheckComboListBoxImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	int count = owner_->owner_->items_.size();
	int selectedCount = 0;

	for(int i = 0; i < count; ++i) {
		if(owner_->getCheck(i))
			selectedCount++;
	}
	owner_->owner_->selectAll(count != selectedCount);
	SendMessage(*owner_, WM_COMMAND, MAKELONG(GetWindowLong(handle_, GWL_ID), CBN_SELCHANGE), LPARAM(handle_));
	InvalidateRect(handle_, 0, FALSE);
}

void CheckComboListBoxImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	RECT clientRect;
	GetClientRect(handle_, &clientRect);

	POINT pt = { x, y };

	if(PtInRect(&clientRect, pt)) {
		INT nItemHeight = SendMessage(handle_, LB_GETITEMHEIGHT, 0, 0);
		INT nTopIndex   = SendMessage(handle_, LB_GETTOPINDEX, 0, 0);

		INT nIndex = nItemHeight == 0 ? -1 : nTopIndex + pt.y / nItemHeight;

		RECT rcItem;
		SendMessage(handle_, LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem);

		if(PtInRect(&rcItem, pt)) {
			owner_->setCheck(nIndex, !owner_->getCheck(nIndex));
			SendMessage(*owner_, WM_COMMAND, MAKELONG(GetWindowLong(handle_, GWL_ID), CBN_SELCHANGE), (LPARAM)handle_);
			owner_->updateValue();
			InvalidateRect(handle_, &rcItem, FALSE);
		}
	}
	__super::onMessageLButtonDown(button, x, y);
}

int CheckComboListBoxImpl::onMessageChar(UINT code, USHORT count, USHORT flags)
{
	if(code == VK_SPACE){
		int index = ::CallWindowProc(controlWindowProc_, handle_, LB_GETCURSEL, WPARAM(code), MAKELPARAM(count, flags));
		RECT itemRect;
		SendMessage(handle_, LB_GETITEMRECT, index, (LONG)(VOID*)&itemRect);
		InvalidateRect(handle_, &itemRect, FALSE);

		owner_->setCheck(index, !owner_->getCheck(index));
		owner_->updateValue();
		SendMessage(*owner_, WM_COMMAND, MAKELONG(GetWindowLong(handle_, GWL_ID), CBN_SELCHANGE), LPARAM(handle_));
	}
	return 0;
}

// ---------------------------------------------------------------------------


CheckComboBoxImpl::CheckComboBoxImpl(kdw::CheckComboBox* owner)
: _WidgetWindow(owner)
, owner_(owner)
, itemHeightSet_(false)
{
	
	//VERIFY(create("", WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST, Recti(0, 0, 24, 24), Win32::_globalDummyWindow));

	bool created = false;
	VERIFY(create("", WS_CHILD | WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS, Recti(0, 0, 30, 30), *Win32::_globalDummyWindow));

	if(!created){
		DWORD error = GetLastError();
		unsigned int c = error;
	}

	controlWindowProc_ = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(handle_, GWLP_WNDPROC));
	::SetWindowLongPtr(handle_, GWLP_WNDPROC, reinterpret_cast<LONG>(&Win32::universalWindowProcedure));
	::SetWindowLongPtr(handle_, GWLP_USERDATA, reinterpret_cast<LONG>(this));
	SetWindowFont(handle_, Win32::defaultFont(), FALSE);
}

void CheckComboBoxImpl::onEdited()
{
	updateValue();
	owner_->onEdited();
}

LRESULT CheckComboBoxImpl::defaultWindowProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
	return ::CallWindowProc(controlWindowProc_, handle_, message, wparam, lparam);
}

void CheckComboBoxImpl::setCheck(int index, bool flag)
{
	VERIFY(::SendMessage(handle_, CB_SETITEMDATA, index, flag ? TRUE : FALSE) != -1);
	ASSERT(::SendMessage(handle_, CB_GETITEMDATA, index, 0) != -1);
	ASSERT((flag ? TRUE : FALSE) == ::SendMessage(handle_, CB_GETITEMDATA, index, 0));

	InvalidateRect(handle_, 0, FALSE);
}

void CheckComboBoxImpl::updateValue()
{
	std::string& value = owner_->value_;
	kdw::CheckComboBox::Items& items = owner_->items_;
	kdw::CheckComboBox::Items::iterator it;
	int index = 0;
	value = "";
	bool first = true;
	for(it = items.begin(); it != items.end(); ++it){
		if(getCheck(index)){
			if(!first)
				value += "|";
			first = false;
			value += *it;
		}
		++index;
	}
}

bool CheckComboBoxImpl::getCheck(int index) const
{
	return bool(::SendMessage(handle_, CB_GETITEMDATA, index, 0) == FALSE ? false : true);
}



LRESULT CheckComboBoxImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	ShareHandle<kdw::Widget> ownerReference(owner_->numRef() == 0 ? 0 : owner_);
	switch(message){
	case WM_CTLCOLORLISTBOX:
		HWND wnd = HWND(lparam);
		if(!listBox_ && wnd != 0 && wnd != handle_ ){
			listBox_ = new CheckComboListBoxImpl(wnd, this);
		}
		return TRUE;
	}
	return Window32::onMessage(message, wparam, lparam);
}


int CheckComboBoxImpl::onMessageCommand(USHORT command, USHORT code, HWND wnd)
{
	ShareHandle<kdw::Widget> ownerReference(owner_->numRef() == 0 ? 0 : owner_);
	switch(command){
		case CBN_SELCHANGE:
		{
			//owner_->selectedIndex_ = SendMessage(comboBox_, CB_GETCURSEL, wparam, lparam);
			//owner_->onSelectionChanged();
			return 0;
		}
		case CBN_SELENDOK:
		{
			//owner_->selectedIndex_ = SendMessage(comboBox_, CB_GETCURSEL, wparam, lparam);
			onEdited();
			return 0;
		}
		case CBN_CLOSEUP:
		{
			onEdited();
			return 0;
		}
		case CBN_SELENDCANCEL:
		{
			//PostMessage(comboBox_, WM_KEYDOWN, VK_ESCAPE, 1);
			onEdited();
			return 0;
		}
		case CBN_KILLFOCUS:
		{
			// owner_->selectedIndex_ = SendMessage(comboBox_, CB_GETCURSEL, 0, 0);
			onEdited();
			return 0;
		}
		case CBN_SETFOCUS:
		{
			return 0;
		}
		case CBN_DROPDOWN:
		{
			return 0;
		}
		default:
			break;
	}
	return Window32::onMessageCommand(command, code, wnd);
}

BOOL CheckComboBoxImpl::onMessageMeasureItem(UINT id, MEASUREITEMSTRUCT* measureItemStruct)
{
	HDC dc = GetDC(*this);
	HFONT oldFont = (HFONT)::SelectObject(dc, GetWindowFont(handle_));
	if(oldFont != 0){
		TEXTMETRIC metrics;
		GetTextMetrics(dc, &metrics);

		measureItemStruct->itemHeight = metrics.tmHeight + metrics.tmExternalLeading;

		measureItemStruct->itemHeight += 2;

		if(!itemHeightSet_){
			itemHeightSet_ = TRUE;
			//SetItemHeight(-1, measureItemStruct->itemHeight);
			SendMessage(handle_, LB_SETITEMHEIGHT, -1, measureItemStruct->itemHeight);
		}
		SelectObject(dc, oldFont);
	}
	ReleaseDC(handle_, dc);
	return TRUE;
}

BOOL CheckComboBoxImpl::onMessageDrawItem(UINT id, DRAWITEMSTRUCT* drawItemStruct)
{
	HDC dc = drawItemStruct->hDC;

	RECT rect = drawItemStruct->rcItem;
	RECT text   = drawItemStruct->rcItem;

	// 0 - No check, 1 - Empty check, 2 - Checked
	INT nCheck = 0;

	std::string str ;

	if((LONG)(drawItemStruct->itemID) < 0){
		updateValue();
		str = owner_->value_;
		nCheck = 0;
	}
	else{
		int len = CallWindowProc(controlWindowProc_, handle_, CB_GETLBTEXTLEN, (WPARAM)drawItemStruct->itemID, 0);
		if(len > 0){
			char* buffer = new char[len + 1];
			memset((void*)buffer, 0, len + 1);
			ASSERT(buffer);
			VERIFY(CallWindowProc(controlWindowProc_, handle_, CB_GETLBTEXT, (WPARAM)drawItemStruct->itemID, (LPARAM)buffer) != CB_ERR);
			str = buffer;
			delete[] buffer;
		}

		nCheck = 1 + (CallWindowProc(controlWindowProc_, handle_, CB_GETITEMDATA, drawItemStruct->itemID, 0) != 0);

		TEXTMETRIC metrics;
		GetTextMetrics(dc, &metrics);

		rect.left    = 0;
		rect.right   = rect.left + metrics.tmHeight + metrics.tmExternalLeading + 6;
		rect.top    += 1;
		rect.bottom -= 1;

		text.left = rect.right;
	}
	
	if (nCheck > 0) {
		SetBkColor(dc, GetSysColor(COLOR_WINDOW));
		SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));

		UINT nState = DFCS_BUTTONCHECK;

		if (nCheck > 1)
			nState |= DFCS_CHECKED;

		// Draw the checkmark using DrawFrameControl
		DrawFrameControl(dc, &rect, DFC_BUTTON, nState);
	}

	if(drawItemStruct->itemState & ODS_SELECTED) {
		SetBkColor(dc, GetSysColor(COLOR_HIGHLIGHT));
		SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else{
		SetBkColor(dc, GetSysColor(COLOR_WINDOW));
		SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
	}

	// Erase and draw
	ExtTextOut(dc, 0, 0, ETO_OPAQUE, &text, 0, 0, 0);
	DrawText(dc, str.c_str(), strlen(str.c_str()), &text, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	if((drawItemStruct->itemState & (ODS_FOCUS|ODS_SELECTED)) == (ODS_FOCUS|ODS_SELECTED))
		DrawFocusRect(dc, &text);

	return TRUE;
}

BOOL CheckComboBoxImpl::onMessageEnable(BOOL enabled)
{
	return FALSE;
}

int CheckComboBoxImpl::onMessageSetFocus(HWND lastFocusedWindow)
{
	return Window32::onMessageSetFocus(lastFocusedWindow);
}

int CheckComboBoxImpl::onMessageKillFocus(HWND focusedWindow)
{
	return Window32::onMessageKillFocus(focusedWindow);
}

void CheckComboBoxImpl::showDropDown()
{
	ComboBox_ShowDropdown(handle_, TRUE);
}

// --------------------------------------------------- 


#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
CheckComboBox::CheckComboBox(bool expandByContent, int border)
: expandByContent_(expandByContent)
, dropDownHeight_(10)
, _WidgetWithWindow(new CheckComboBoxImpl(this), border)
{
	updateMinimalSize();
}
#pragma warning(pop)

void CheckComboBox::updateMinimalSize()
{
	Vect2i size = Win32::calculateTextSize(window()->comboBox(), GetWindowFont(*window()), " ");
	if(expandByContent_){
		Items::iterator it;
		FOR_EACH(items_, it){
			Vect2i anotherSize = Win32::calculateTextSize(*window(), GetWindowFont(*window()), it->c_str());
			size.x = max(anotherSize.x, size.x);
			size.y = max(anotherSize.y, size.y);
		}
	}
	_setMinimalSize(size + Vect2i(4, 4) + Vect2i(GetSystemMetrics(SM_CXFOCUSBORDER), GetSystemMetrics(SM_CYFOCUSBORDER)) * 2
		            + Vect2i(border_, border_) * 2 + Vect2i(GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER)));
}


void CheckComboBox::_setPosition(const Recti& position)
{
	Widget::_setPosition(position);

	Win32::Window32* window = _findWindow(parent());
	ASSERT(window);
	Win32::Window32::PositionDeferer deferer = window->positionDeferer();
	Recti pos(position.left() + border_, position.top() + border_,
		      position.width() - border_ * 2, position.height() - border_ * 2 + _minimalSize().y * dropDownHeight_);
	deferer->defer(_window(), pos);
}

void CheckComboBox::setExpandByContent(bool expand)
{
	expandByContent_ = expand;
	updateMinimalSize();
	_queueRelayout();
}

void CheckComboBox::showDropDown()
{
	window()->showDropDown();
}

void CheckComboBox::set(const char* comboList, const char* value)
{
	clear();
	value_ = "";

	ComboStrings values;
	splitComboList(values, value);

	ComboStrings strings;
	splitComboList(strings, comboList);
	ComboStrings::iterator it;
	int index = 0;
	FOR_EACH(strings, it){
		ComboStrings::iterator vit;
		add(it->c_str());
		FOR_EACH(values, vit){
			if(*vit == *it){
				if(vit != values.begin())
					value_ += "|";
				value_ += *it;
				window()->setCheck(index, true);
				break;
			}
		}
		++index;
	}
}

void CheckComboBox::clear()
{
	items_.clear();
	::SendMessage(window()->comboBox(), CB_RESETCONTENT, 0, 0);
	updateMinimalSize();
	_queueRelayout();
}


void CheckComboBox::add(const char* text)
{
	insert(end(), text);
}

void CheckComboBox::insert(iterator before, const char* text)
{
	int index = before == items_.end() ? -1 : std::distance(items_.begin(), before);
	items_.insert(before, text);
	::SendMessage(window()->comboBox(), CB_INSERTSTRING, index, (LPARAM)text);
	if(items_.size() == 1)
		::SendMessage(window()->comboBox(), CB_SETCURSEL, 0, 0);
	updateMinimalSize();
	_queueRelayout();
}

void CheckComboBox::setDropDownHeight(int lines)
{
	dropDownHeight_ = lines;
	_setPosition(_position());
}

void CheckComboBox::selectAll(bool select)
{
	for(int i = 0; i < items_.size(); ++i)
		window()->setCheck(i, select);
}

int CheckComboBox::dropDownHeight() const
{
	return dropDownHeight_;
}

void CheckComboBox::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_DESIGN)){
		ar.serialize(items_, "items", "Элементы");
		ar.serialize(value_, "value", "&Выбранные элементы");
		ar.serialize(dropDownHeight_, "dropDownHeight_", "Высота списка");
		ar.serialize(expandByContent_, "expandByContent", "Расширять по содержимому");
	}
	Widget::serialize(ar);
}

}
