#include "StdAfx.h"
#include "kdw/ComboBox.h"
#include "kdw/ColorComboBox.h"
#include "kdw/Container.h"

#include "kdw/_WidgetWindow.h"

#include <windows.h>
#include <windowsx.h>
#include "kdw/Win32/Rectangle.h"

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"

namespace kdw{

REGISTER_CLASS(Widget, ComboBox, "Выпадающий список")

class ComboBoxImpl : public _WidgetWindow{
public:
	ComboBoxImpl(kdw::ComboBox* owner);
	~ComboBoxImpl();
	const char* className() const{ return "kdw.ComboBoxImpl"; }
	
	BOOL onMessageEraseBkgnd(HDC dc);
	BOOL onMessageEnable(BOOL enabled);
	int onMessageSetFocus(HWND lastFocusedWindow);
	BOOL onMessageSize(UINT type, USHORT width, USHORT height);

	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);

	void showDropDown();
	HWND comboBox(){ return comboBoxHandle_; }
protected:
	HWND comboBoxHandle_;
	kdw::ComboBox* owner_;

	friend class kdw::ComboBox;
	static UINT WM_KDW_FINISH;
};
UINT ComboBoxImpl::WM_KDW_FINISH = RegisterWindowMessage("kdw.Finish");

ComboBoxImpl::ComboBoxImpl(kdw::ComboBox* owner)
: _WidgetWindow(owner)
, owner_(owner)
, comboBoxHandle_(0)
{
	VERIFY(create("", WS_CHILD, Recti(0, 0, 10, 10), *Win32::_globalDummyWindow));
	comboBoxHandle_ = ::CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", "",
							   WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VISIBLE | WS_VSCROLL,
							 0, 0, 42, 42, *this, 0, (HINSTANCE)(GetWindowLong(*this, GWLP_HINSTANCE)), 0);
	
	ASSERT(comboBoxHandle_);
	SetWindowFont(comboBoxHandle_, Win32::defaultFont(), FALSE);
}

ComboBoxImpl::~ComboBoxImpl()
{
}

LRESULT ComboBoxImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	if(message == WM_KDW_FINISH){
		ShareHandle<kdw::Widget> ref(owner_->numRef() == 0 ? 0 : owner_);
		owner_->onEdited();
		return 0;
	}
	switch(message){
	case WM_COMMAND:
		if(IsWindowVisible(comboBoxHandle_)){
			switch(HIWORD(wparam)){
				case CBN_SELCHANGE:
					{
					owner_->selectedIndex_ = SendMessage(comboBoxHandle_, CB_GETCURSEL, wparam, lparam);
					owner_->onSelectionChanged();
					break;
					}
				case CBN_SELENDOK:
				{
					owner_->selectedIndex_ = SendMessage(comboBoxHandle_, CB_GETCURSEL, wparam, lparam);
					PostMessage(*this, WM_KDW_FINISH, WPARAM(HWND(*this)), 0);
					return 0;
					break;
				}
				case CBN_CLOSEUP:
				{
					PostMessage(*this, WM_KDW_FINISH, WPARAM(HWND(*this)), 0);
					return 0;
					break;
				}
				case CBN_SELENDCANCEL:
				{
					break;
				}
				case CBN_KILLFOCUS:
				{
					owner_->selectedIndex_ = SendMessage(comboBoxHandle_, CB_GETCURSEL, 0, 0);
					PostMessage(*this, WM_KDW_FINISH, WPARAM(HWND(*this)), 0);
					return 0;
					break;
				}
				case CBN_SETFOCUS:
				{
					break;
				}
			}
		}
		break;
	case WM_SHOWWINDOW:
		UINT msg = message;
		ShowWindow(comboBoxHandle_, msg);
		break;
	}
	return __super::onMessage(message, wparam, lparam);
}

BOOL ComboBoxImpl::onMessageSize(UINT type, USHORT width, USHORT height)
{
	if(comboBoxHandle_)
		::MoveWindow(comboBoxHandle_, 0, 0, width, height + owner_->_minimalSize().y * owner_->dropDownHeight_, TRUE);
	return TRUE;
}


BOOL ComboBoxImpl::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

BOOL ComboBoxImpl::onMessageEnable(BOOL enabled)
{
	::EnableWindow(comboBoxHandle_, enabled);
	ASSERT(::IsWindowEnabled(comboBoxHandle_) == enabled);
	return FALSE;
}

int ComboBoxImpl::onMessageSetFocus(HWND lastFocusedWindow)
{
	::SetFocus(comboBoxHandle_);
	return __super::onMessageSetFocus(lastFocusedWindow);
}

void ComboBoxImpl::showDropDown()
{
	ComboBox_ShowDropdown(comboBoxHandle_, TRUE);
}

// --------------------------------------------------- 


#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
ComboBox::ComboBox(bool expandByContent, int border)
: expandByContent_(expandByContent)
, dropDownHeight_(10)
, selectedIndex_(0)
, _WidgetWithWindow(new ComboBoxImpl(this), border)
{
	updateMinimalSize();
}

#pragma warning(pop)

ComboBox::ComboBox(ComboBoxImpl* impl, bool expandByContent, int border)
: expandByContent_(expandByContent)
, dropDownHeight_(10)
, selectedIndex_(0)
, _WidgetWithWindow(impl, border)
{
}

void ComboBox::updateMinimalSize()
{
	Vect2i size = Win32::calculateTextSize(impl()->comboBox(), GetWindowFont(*impl()), " ");
	if(expandByContent_){
		Items::iterator it;
		FOR_EACH(items_, it){
			Vect2i anotherSize = Win32::calculateTextSize(*impl(), GetWindowFont(*impl()), it->c_str());
			size.x = max(anotherSize.x, size.x);
			size.y = max(anotherSize.y, size.y);
		}
	}
	_setMinimalSize(size + Vect2i(4, 2) + Vect2i(GetSystemMetrics(SM_CXFOCUSBORDER), GetSystemMetrics(SM_CYFOCUSBORDER)) * 2
		            + Vect2i(border_, border_) * 2 + Vect2i(GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER)));
}


void ComboBox::_setPosition(const Recti& position)
{
	Widget::_setPosition(position);

	Win32::Window32* window = _findWindow(parent());
	ASSERT(window);
	Win32::Window32::PositionDeferer deferer = window->positionDeferer();
	Recti pos(position.left() + border_, position.top() + border_,
		      position.width() - border_ * 2, position.height() - border_ * 2);
	deferer->defer(_window(), pos);
}

void ComboBox::setExpandByContent(bool expand)
{
	expandByContent_ = expand;
	updateMinimalSize();
	_queueRelayout();
}

void ComboBox::showDropDown()
{
	impl()->showDropDown();
}

void ComboBox::set(const char* comboList, const char* value, bool onlyVisible)
{
	clear();
	ComboStrings strings;
	splitComboList(strings, comboList);
	ComboStrings::iterator it;
	int index = 0;
	int selectedIndex = 0;

	FOR_EACH(strings, it){
		if(!onlyVisible || it->c_str()[0] != '\0'){
			add(it->c_str());
			if(*it == value)
				selectedIndex = index;
			++index;
		}
	}
	setSelectedIndex(selectedIndex);
}


void ComboBox::set(const ComboListString& comboList, bool onlyVisible)
{
	set(comboList.comboList(), comboList.value().c_str(), onlyVisible);
}

void ComboBox::get(ComboListString& comboList)
{
	std::string str;
	joinComboList(str, items_);
	comboList.setComboList(str.c_str());
	comboList.value() = items_[selectedIndex_];
}


void ComboBox::setSelectedIndex(int selectedIndex)
{
	selectedIndex_ = selectedIndex;
	::SendMessage(impl()->comboBox(), CB_SETCURSEL, selectedIndex, 0);
}

int ComboBox::selectedIndex() const
{
    return selectedIndex_;
}

std::string ComboBox::selectedString() const
{
	ASSERT(selectedIndex_ >= 0 && selectedIndex_ < int(items_.size()));
	if(selectedIndex_ >= 0 && selectedIndex_ < int(items_.size())){
		Items::const_iterator it = items_.begin();
		std::advance(it, selectedIndex_);
		return *it;
	}
	return "";
}

void ComboBox::clear()
{
	items_.clear();
	::SendMessage(impl()->comboBox(), CB_RESETCONTENT, 0, 0);
	updateMinimalSize();
	_queueRelayout();
}

void ComboBox::onEdited()
{
	ShareHandle<kdw::Widget> ref(numRef() == 0 ? 0 : this);
	signalEdited_.emit();
}

void ComboBox::add(const char* text)
{
	insert(end(), text);
}

void ComboBox::insert(iterator before, const char* text)
{
	int index = before == items_.end() ? -1 : std::distance(items_.begin(), before);
	items_.insert(before, text);
	::SendMessage(impl()->comboBox(), CB_INSERTSTRING, index, (LPARAM)text);
	if(items_.size() == 1)
		::SendMessage(impl()->comboBox(), CB_SETCURSEL, 0, 0);
	updateMinimalSize();
	_queueRelayout();
}

void ComboBox::setDropDownHeight(int lines)
{
	dropDownHeight_ = lines;
	_setPosition(_position());
}

int ComboBox::dropDownHeight() const
{
	return dropDownHeight_;
}

void ComboBox::_setFocus()
{
	::SetFocus(impl()->comboBoxHandle_);
}
void ComboBox::serialize(Archive& ar)
{
	ar.serialize(items_, "items", "Элементы");
	ar.serialize(selectedIndex_, "selectedIndex", "&Выбранный элемент");
	ar.serialize(dropDownHeight_, "dropDownHeight_", "Высота списка");
	ar.serialize(expandByContent_, "expandByContent", "Расширять по содержимому");
	Widget::serialize(ar);
}

// ---------------------------------------------------------------------------
class ColorComboBoxImpl : public ComboBoxImpl{
public:
	ColorComboBoxImpl(ColorComboBox* owner);
	BOOL onMessageDrawItem(UINT id, DRAWITEMSTRUCT* drawItemStruct);
};

ColorComboBoxImpl::ColorComboBoxImpl(ColorComboBox* owner)
: ComboBoxImpl(owner)
{
	VERIFY(create("", WS_CHILD, Recti(0, 0, 10, 10), *Win32::_globalDummyWindow));
	comboBoxHandle_ = ::CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", "",
							   WS_CHILD | WS_TABSTOP  | WS_VISIBLE | WS_VSCROLL
							   | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
							   0, 0, 42, 42, *this, 0, (HINSTANCE)(GetWindowLong(*this, GWLP_HINSTANCE)), 0);
	
	ASSERT(comboBoxHandle_);
	SetWindowFont(comboBoxHandle_, Win32::defaultFont(), FALSE);
}

BOOL ColorComboBoxImpl::onMessageDrawItem(UINT id, DRAWITEMSTRUCT* drawItemStruct)
{
	HDC dcContext = drawItemStruct->hDC;
	if(!dcContext)
		return FALSE;

	Win32::Rect itemRect = drawItemStruct->rcItem;
	Win32::Rect blockRect = itemRect;
	Win32::Rect textRect = blockRect;
	int item = drawItemStruct->itemID;
	int action = drawItemStruct->itemAction;
	int state = drawItemStruct->itemState;
	COLORREF color = 0;
	COLORREF normalColor = GetSysColor(COLOR_WINDOW);
	COLORREF selectedColor = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF textColor = GetSysColor(COLOR_WINDOWTEXT);
	int fourthWidth = blockRect.width();
	HBRUSH frameBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

	if(state & ODS_SELECTED){
		SetTextColor(dcContext, (0x00FFFFFF & ~textColor));
		SetBkColor(dcContext, selectedColor);
		HBRUSH solidBrush = CreateSolidBrush(selectedColor);
		FillRect(dcContext, &blockRect, solidBrush);
		DeleteObject((HGDIOBJ)solidBrush);
	}
	else{
		SetTextColor(dcContext, textColor);
		SetBkColor(dcContext, normalColor);
		HBRUSH solidBrush = CreateSolidBrush(normalColor);
		FillRect(dcContext, &blockRect, solidBrush);
		DeleteObject((HGDIOBJ)solidBrush);
	}

	if(state & ODS_FOCUS){
		DrawFocusRect(dcContext, &itemRect);
	}

	InflateRect(&blockRect, -2, -2);

	if(item != -1){
		if(state & ODS_DISABLED){
			color = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
			SetTextColor(dcContext, color);
		}
		else{
			color = (COLORREF)::SendMessage(comboBox(), CB_GETITEMDATA, item, 0);
			//color = GetItemData(item);
		}

		std::string strColor;
		SetBkMode(dcContext, TRANSPARENT);

		HBRUSH solidBrush = CreateSolidBrush(color);
		FillRect(dcContext, &blockRect, solidBrush);
		DeleteObject((HGDIOBJ)solidBrush);
		FrameRect(dcContext, &blockRect, frameBrush);
	}
	return TRUE;
}

// ---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list

ColorComboBox::ColorComboBox(int border)
: ComboBox(new ColorComboBoxImpl(this), false, border)
{
}

void ColorComboBox::add(Color4c color, const char* text)
{
	int index = ::SendMessage(impl()->comboBox(), CB_INSERTSTRING, -1, (LPARAM)text);
	if(index >= 0)
		::SendMessage(impl()->comboBox(), CB_SETITEMDATA, index, (LPARAM)color.RGBGDI());
}

void ColorComboBox::set(const ComboListColor& value)
{
	clear();
	const ColorContainer& colors = value.comboList();
	ColorContainer::const_iterator it;
	int index = 0;
	int selectedIndex = -1;
	FOR_EACH(colors, it){
		add(Color4c(*it));
		if(Color4c(*it) == Color4c(value.value()))
			selectedIndex = index;
		++index;
	}
	if(selectedIndex >= 0)
        setSelectedIndex(selectedIndex);
}
/*
ComboListColor ColorComboBox::get() const
{

}
*/

}
#pragma warning(pop)
