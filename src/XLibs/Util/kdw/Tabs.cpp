#include "StdAfx.h"
#include "Tabs.h"
#include "XTL/SafeCast.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/Window.h"
#include "kdw/Win32/MemoryDC.h"
#include "kdw/Win32/Rectangle.h"
#include "kdw/Win32/Handle.h"
#include "kdw/Win32/Drawing.h"
#include "XMath/Colors.h"

namespace kdw{

struct TabsItem{
	std::string text;
	Win32::Rect rect;
};

class TabsImpl : public _WidgetWindow{
public:
	TabsImpl(Tabs* tabs);
	const char* className() const{ return "kdw.Tabs"; }

	void redraw(HDC dc);
	void onMessagePaint();
	BOOL onMessageSize(UINT type, USHORT width, USHORT height);
	BOOL onMessageEraseBkgnd(HDC dc);
	int onMessageSetFocus(HWND lastFocusedWindow);
	int onMessageKillFocus(HWND focusedWindow);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageMButtonDown(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);
	int onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags);

	void recalculateRects();
protected:
	typedef std::vector<TabsItem> Items;
	Items items_;
	Tabs* owner_;
	int selectedTab_;
	int focusedTab_;
	friend class Tabs;
};

TabsImpl::TabsImpl(Tabs* tabs)
: _WidgetWindow(tabs)
, selectedTab_(0)
, focusedTab_(-1)
, owner_(0)
{
	VERIFY(create("", WS_CHILD | WS_TABSTOP, Recti(0, 0, 20, 20), *Win32::_globalDummyWindow));
	owner_ = tabs;
}

int TabsImpl::onMessageKeyDown(UINT keyCode, USHORT count, USHORT flags)
{
	if(keyCode == VK_LEFT){
		if(selectedTab_ > 0){
			owner_->setSelectedTab(selectedTab_ - 1, owner_);
			focusedTab_ = selectedTab_;
		}
	}
	if(keyCode == VK_RIGHT){
		if(selectedTab_ < items_.size() - 1){
			owner_->setSelectedTab(selectedTab_ + 1, owner_);
			focusedTab_ = selectedTab_;
		}
	}
	return __super::onMessageKeyDown(keyCode, count, flags);
}

void TabsImpl::onMessagePaint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(handle_, &ps);
	{
		Win32::MemoryDC memoryDC(dc);
		redraw(memoryDC);
	}
	EndPaint(handle_, &ps);
}

BOOL TabsImpl::onMessageSize(UINT type, USHORT width, USHORT height)
{
	if(owner_)
		recalculateRects();
	return __super::onMessageSize(type, width, height);
}


int TabsImpl::onMessageSetFocus(HWND lastFocusedWindow)
{
	if(owner_){
		kdw::Widget::_setFocusedWidget(owner_);
		RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	}
	return Window32::onMessageSetFocus(lastFocusedWindow);
}

int TabsImpl::onMessageKillFocus(HWND focusedWindow)
{
	if(owner_)
		RedrawWindow(handle_, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	return Window32::onMessageKillFocus(focusedWindow);
}

BOOL TabsImpl::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

void TabsImpl::recalculateRects()
{
	Win32::Rect rect;
	GetClientRect(*this, &rect);
	int offset = 0;
	Items::iterator it;

	const int PADDING_X = 8;
	const int PADDING_Y = 4;

	int height = 20;
	int index = 0;

	FOR_EACH(items_, it){
		TabsItem& item = *it;

		HFONT font = selectedTab_ == index ? Win32::defaultBoldFont() : Win32::defaultFont();
		Vect2i size = Win32::calculateTextSize(*this, font, item.text.c_str());

		size.x += PADDING_X * 2;
		size.y += PADDING_Y * 2;
		height = max(size.y, height);

		item.rect = Win32::Rect(offset, 0, size.x + offset, size.y);
		offset += size.x;
		++index;
	}
	FOR_EACH(items_, it){
		TabsItem& item = *it;
		item.rect.bottom = rect.height();//item.rect.top + height;
		if(offset > rect.width()){
			float scale =  rect.width() / float(offset);
			item.rect.left = round(item.rect.left * scale);
			item.rect.right = round(item.rect.right * scale);
		}
	}
	owner_->_setMinimalSize(Vect2i(20, height));
	::RedrawWindow(*this, 0, 0, RDW_INVALIDATE);
}


void TabsImpl::redraw(HDC dc)
{
	Win32::Rect rect;
	GetClientRect(*this, &rect);


	FillRect(dc, &rect, GetSysColorBrush(COLOR_BTNFACE));
	SetBkMode(dc, TRANSPARENT);

	int index = 0;
	Items::iterator it;
	Win32::AutoSelector pen(dc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHIGHLIGHT)));
	FOR_EACH(items_, it){
		TabsItem& item = *it;
		bool selected = selectedTab_ == index;
		bool focused = index == focusedTab_ && GetFocus() == *this;

		const int gradientHeight = 3;
		const int roundness = 9;
		if(selected){
			Win32::StockSelector nullPen(dc, GetStockObject(NULL_PEN));
			{
				Win32::AutoSelector halfShadowBrush(dc,
					CreateSolidBrush(Win32::blendColor(GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_3DSHADOW), 0.4f)));
				RoundRect(dc, item.rect.left, item.rect.top, item.rect.right, item.rect.bottom + roundness, roundness + 1, roundness + 1);
			}

			Win32::StockSelector shadowBrush(dc, GetSysColorBrush(COLOR_3DSHADOW));

			RoundRect(dc, item.rect.left, item.rect.top + 1, item.rect.right, item.rect.bottom + roundness, roundness, roundness);
			{
				Win32::StockSelector whiteBrush(dc, GetSysColorBrush(COLOR_WINDOW));
				RoundRect(dc, item.rect.left + 2, item.rect.top + 2,
							item.rect.right - 2, item.rect.bottom - 1 + roundness, roundness - 1, roundness - 1);
				
				Win32::drawVerticalBlend(dc, Win32::Rect(item.rect.left + 2, item.rect.bottom - gradientHeight,
														 item.rect.right - 3, item.rect.bottom),
										 GetSysColor(COLOR_WINDOW),
										 GetSysColor(COLOR_BTNFACE));

			}
			MoveToEx(dc, item.rect.right, item.rect.bottom - 1, 0);
		}
		else{
			Win32::StockSelector nullPen(dc, GetStockObject(NULL_PEN));
			Win32::StockSelector shadowBrush(dc, GetSysColorBrush(COLOR_3DSHADOW));

			RoundRect(dc, item.rect.left + 1, item.rect.top + 2,
					  item.rect.right - 1, item.rect.bottom + roundness, roundness, roundness);
			Win32::StockSelector whiteBrush(dc, GetSysColorBrush(COLOR_BTNFACE));
			RoundRect(dc, item.rect.left + 2, item.rect.top + 3,
						item.rect.right - 2, item.rect.bottom - 1 + roundness, roundness - 1, roundness - 1);

			Win32::AutoSelector shadowPen(dc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHIGHLIGHT)));
			MoveToEx(dc, item.rect.left, item.rect.bottom - 1, 0);
			LineTo(dc, item.rect.right, item.rect.bottom - 1);
		}		

		HFONT font = selected ? Win32::defaultBoldFont() : Win32::defaultFont();
		const char* text = item.text.c_str();
		Win32::StockSelector fontSel(dc, font);
		
		Win32::Rect textRect = item.rect;
		textRect.left += 3;
		textRect.right -= 3;
		if(!selected){
			textRect.top += 1;
			textRect.bottom += 1;
		}
		::DrawText(dc, text, int(strlen(text)), &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

		if(focused){
			Win32::Rect focusRect = item.rect;
			focusRect.left += 3;
			focusRect.top += 4;
			focusRect.right -= 4;
			focusRect.bottom -= 2;
			::DrawFocusRect(dc, &focusRect);
		}

		++index;
	}
	if(!items_.empty())
		LineTo(dc, rect.right, rect.bottom - 1);
}

void TabsImpl::onMessageMButtonDown(UINT button, int x, int y)
{
	int index = 0;
	Items::iterator it;
	FOR_EACH(items_, it){
		TabsItem& item = *it;
		if(item.rect.pointIn(Vect2i(x, y))){
			owner_->signalMouseButtonDown().emit(MOUSE_BUTTON_MIDDLE, index);
			break;
		}
		++index;
	}
	__super::onMessageMButtonDown(button, x, y);
}

void TabsImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	int index = 0;
	Items::iterator it;
	FOR_EACH(items_, it){
		TabsItem& item = *it;
		if(item.rect.pointIn(Vect2i(x, y))){
			owner_->signalMouseButtonDown().emit(MOUSE_BUTTON_RIGHT, index);
			break;
		}
		++index;
	}

	__super::onMessageRButtonDown(button, x, y);
}

void TabsImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	::SetFocus(*this);
	int index = 0;
	Items::iterator it;
	FOR_EACH(items_, it){
		TabsItem& item = *it;
		if(item.rect.pointIn(Vect2i(x, y))){
			focusedTab_ = index;
			if(index != selectedTab_){
				owner_->setSelectedTab(index);
			}
		}

		HFONT font = selectedTab_ == index ? Win32::defaultBoldFont() : Win32::defaultFont();
		Vect2i size = Win32::calculateTextSize(*this, font, item.text.c_str());
		++index;
	}
	RedrawWindow(*this, 0, 0, RDW_INVALIDATE);
	__super::onMessageLButtonDown(button, x, y);
}


// ---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) // warning C4355: 'this' : used in base member initializer list
Tabs::Tabs(int border)
: _WidgetWithWindow(new TabsImpl(this), border)
{
	_setMinimalSize(Vect2i(20, 20));
	impl().recalculateRects();
}
#pragma warning(pop)

TabsImpl& Tabs::impl()
{
	return *safe_cast<TabsImpl*>(_window());
}

int Tabs::selectedTab()
{
	return impl().selectedTab_;
}

void Tabs::setSelectedTab(int index, const TabChanger* changer)
{
	int oldTab = impl().selectedTab_;
	impl().selectedTab_ = index;
	if(oldTab != index){
		impl().recalculateRects();
		signalChanged_.emit(changer);
	}
	RedrawWindow(impl(), 0, 0, RDW_INVALIDATE);
}

int Tabs::add(const char* tabTitle, int before)
{
	TabsItem item;
	item.text = tabTitle;
	item.rect = Win32::Rect(0, 0, 0, 0);
	impl().items_.push_back(item);
	impl().recalculateRects(); 
	return impl().items_.size() - 1;
}

void Tabs::remove(int index)
{
	xassert(index >= 0 && index < impl().items_.size());
	if(index < 0 || index >= impl().items_.size())
		return;

	impl().items_.erase(impl().items_.begin());
	impl().recalculateRects();
}

void Tabs::clear()
{
	impl().focusedTab_ = -1;
	impl().selectedTab_ = -1;
	impl().items_.clear();
}

// ---------------------------------------------------------------------------

TabPages::TabPages(int border)
: VBox(2, border)
{
	tabs_ = new Tabs();
	tabs_->signalChanged().connect(this, &TabPages::onTabChange);
	VBox::add(tabs_);
}


int TabPages::add(const char* title, Widget* widget, int before)
{
	xassert(widget);
	tabs_->add(title, before);

	if(before > -1)
		widgets_.insert(widgets_.begin() + before, widget);
	else
		widgets_.push_back(widget);

	if(widgets_.size() == 1){
		VBox::add(widget, true, true, true);
	}
	return widgets_.size() - 1;
}

void TabPages::onTabChange(const TabChanger* changer)
{
	if(changer == this)
		return;
	setSelectedTab(tabs_->selectedTab());
}

void TabPages::remove(int index)
{
	if(index == tabs_->selectedTab() && VBox::size() > 1){
		VBox::remove(&*widgets_[index]);
	}
	tabs_->remove(index);
	widgets_.erase(widgets_.begin() + index);
	setSelectedTab(index);
}

Widget* TabPages::widgetByIndex(int index)
{
	return widgets_.at(index);
}

int TabPages::size() const
{
	return int(widgets_.size());
}

void TabPages::setSelectedTab(int index)
{
	if(VBox::size() > 1)
		VBox::remove(1);
	Widget* widget = widgets_.at(index);
	VBox::add(widget, true, true, true);
	widget->showAll();
}

}
