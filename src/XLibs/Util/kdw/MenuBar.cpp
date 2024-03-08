#include "StdAfx.h"
#include "kdw/MenuBar.h"
#include "kdw/PopupMenu.h"
#include "kdw/CommandManager.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/Win32/Rectangle.h"
#include "kdw/Win32/Drawing.h"

namespace kdw{

class KDW_API MenuBarItem{
public:
	MenuBarItem()
	: commandIndex_(-1)
	{
	}

	MenuBarItem(int commandIndex, const char* text, const Recti& rect)
	: commandIndex_(commandIndex)
	, rect_(rect)
	, text_(text)
	{

	}

	void setRect(Recti& rect){
		rect_ = rect;
	}
	const char* text() const{ return text_.c_str(); }
	const Recti& rect() const{ return rect_; }
	int commandIndex() const{ return commandIndex_; }
protected:
	std::string text_;
	//Command* command_;
	int commandIndex_;
	Recti rect_;
};

class MenuBarImpl : public _WidgetWindow, public sigslot::has_slots{
public:
	MenuBarImpl(kdw::MenuBar* owner);
	virtual ~MenuBarImpl();
	const char* className() const{ return "kdw.MenuBarImpl"; }
	void redraw(HDC dc);

	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	void onMessagePaint();
	void onMessageMouseMove(UINT button, int x, int y);
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageRButtonDown(UINT button, int x, int y);


	void onCommandsChanged();
	
	void updateItems();
	typedef std::vector<MenuBarItem> Items;

protected:
	bool updateActiveItem();
	CommandManagerRef commands_;
	Items items_;
	MenuBar* owner_;
	int activeItem_;
	bool showMenu_;
	bool inMenu_;
	friend class MenuBar;
};

MenuBarImpl::MenuBarImpl(kdw::MenuBar* owner)
: _WidgetWindow(owner)
, owner_(owner)
, showMenu_(false)
, inMenu_(false)
, activeItem_(-1)
{
	create("", WS_CHILD, Recti(0, 0, 24, 24), *Win32::_globalDummyWindow);
}

MenuBarImpl::~MenuBarImpl()
{

}

void MenuBarImpl::redraw(HDC dc)
{
	RECT rect;
	GetClientRect(*this, &rect);
	FillRect(dc, &rect, GetSysColorBrush(COLOR_MENU));

	//RECT lineRect = rect;
	//lineRect.top = lineRect.bottom - 1;
	//::FillRect(dc, &lineRect, GetSysColorBrush(COLOR_3DSHADOW));

	Items::iterator it;
	int index = 0;
	for(it = items_.begin(); it != items_.end(); ++it){
		MenuBarItem& item = *it;
		HFONT font = Win32::defaultFont();
		HFONT oldFont = (HFONT)::SelectObject(dc, font);
		int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
		Win32::Rect rect(item.rect());
		const char* text = item.text();
		if(activeItem_ == index){
			::FillRect(dc, &rect, GetSysColorBrush(COLOR_HIGHLIGHT));
			COLORREF oldTextColor = ::SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			::DrawText(dc, text, strlen(text), &rect, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
			::SetTextColor(dc, oldTextColor);
		}
		else{
			COLORREF oldTextColor = ::SetTextColor(dc, GetSysColor(COLOR_MENUTEXT));
			::DrawText(dc, text, strlen(text), &rect, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
			::SetTextColor(dc, oldTextColor);
		}
		::SetBkMode(dc, oldBkMode);
		::SelectObject(dc, oldFont);
		++index;
	}
    //Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
}

LRESULT MenuBarImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = __super::onMessage(message, wparam, lparam);
	if(message == WM_MOUSELEAVE){
		if(updateActiveItem())
			RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
	}
	else if(message == WM_ENTERIDLE){
		UINT reason = (UINT)wparam;
		HANDLE handle = (HANDLE)lparam;
		if(reason == MSGF_MENU){
			if(!showMenu_)
				return result;
			if(updateActiveItem()){
				RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
				inMenu_ = true;
				SendMessage(*this, WM_CANCELMODE, 0, 0);
			}
			return 0;
		}
	}
	return result;
}

void MenuBarImpl::onMessagePaint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(*this, &ps);
	redraw(dc);
	EndPaint(*this, &ps);
}

void MenuBarImpl::onMessageMouseMove(UINT button, int x, int y)
{
	if(updateActiveItem())
		RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
	HWND captureWindow = GetCapture();
	ASSERT(captureWindow == 0);
	if(activeItem_ != -1){
		TRACKMOUSEEVENT event;
		ZeroMemory((void*)(&event), sizeof(event));
		event.cbSize = sizeof(event);
		event.dwFlags = TME_LEAVE;
		event.hwndTrack = *this;
		VERIFY(TrackMouseEvent(&event));
	}

	__super::onMessageMouseMove(button, x, y);
}

void MenuBarImpl::onMessageLButtonUp(UINT button, int x, int y)
{
}

void MenuBarImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	updateActiveItem();
	showMenu_ = true;
	do{
		inMenu_ = false;
		if(activeItem_ != -1){
			MenuBarItem& item = items_.at(activeItem_);
			PopupMenu menu(100);
			if(commands_){
				commands_->generateMenu(menu.root(), item.commandIndex());
				POINT pt = { item.rect().left(), item.rect().bottom() };
				ClientToScreen(*this, &pt);
				RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
				menu.spawn(Vect2i(pt.x, pt.y), owner_);
				activeItem_ = -1;
				updateActiveItem();
				RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
			}
		}
	} while(inMenu_);
	showMenu_ = false;
}

void MenuBarImpl::onMessageRButtonDown(UINT button, int x, int y)
{
	__super::onMessageRButtonDown(button, x, y);
}

void MenuBarImpl::onCommandsChanged()
{
	updateItems();
	::RedrawWindow(*this, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
}

void MenuBarImpl::updateItems()
{
	//RECT clientRect;
	//GetClientRect(*this, &clientRect);
	Recti pos(owner_->_position());
	Vect2i size = calculateTextSize(*this, Win32::defaultFont(), " ");
	size.y +=  + GetSystemMetrics(SM_CYEDGE) * 4;
	int height = max(size.y, owner_->_position().height());
	items_.clear();
	if(owner_->path_ != ""){
		kdw::CommandTemplate* command = CommandTemplateManager::instance().get(owner_->path_.c_str());
		CommandTemplate::iterator it;
		int offset = 0;
		FOR_EACH(*command, it){
			CommandTemplate& child = *it;
			const char* text = child.label();
			Vect2i textSize = Win32::calculateTextSize(*Win32::_globalDummyWindow, Win32::defaultFont(), text);
			textSize.x += 10;
			Recti rect(offset, (pos.height() - height) / 2, textSize.x, height);
			ASSERT(child.index() >= 0);
			items_.push_back(MenuBarItem(child.index(), child.label(), rect));

			offset += textSize.x;
		}
		size.x = offset + GetSystemMetrics(SM_CXEDGE) * 2;
	}
	owner_->_setMinimalSize(size);
}

bool MenuBarImpl::updateActiveItem()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(*this, &cursorPos);

	Items::iterator it;
	int index = 0;
	for(it = items_.begin(); it != items_.end(); ++it){
		MenuBarItem& item = *it;
		Win32::Rect rect(item.rect());
		if(::PtInRect(&rect, cursorPos)){
			bool result = activeItem_ != index;
			activeItem_ = index;
			return result;
		}
		++index;
	}
	if(showMenu_)
		return false;
	else{
		bool result = activeItem_ != -1;
		activeItem_ = -1;
		return result;
	}
}

// -----------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4355) //  'this' : used in base member initializer list
MenuBar::MenuBar(CommandManager* commandManager, const char* path, int border)
: path_(path)
, _WidgetWithWindow(new MenuBarImpl(this), border)
{
	CommandTemplateManager::instance().signalChanged().connect(impl(), &MenuBarImpl::onCommandsChanged);
	set(path);
	impl()->commands_ = commandManager;
}
#pragma warning(pop)

MenuBar::~MenuBar()
{
//	if(commandManager_)
//		commandManager_->setHotkeyContext(0);
}

void MenuBar::set(const char* path)
{
	path_ = path;
	impl()->updateItems();
}


void MenuBar::_setPosition(const Recti& position)
{
	__super::_setPosition(position);
	impl()->updateItems();
}

void MenuBar::_setParent(Container* container)
{
	__super::_setParent(container);
	impl()->updateItems();
}

void MenuBar::_updateVisibility()
{
	__super::_updateVisibility();
	impl()->updateItems();
}

MenuBarImpl* MenuBar::impl()
{
	return static_cast<MenuBarImpl*>(_window());
}

};
