#include "StdAfx.h"
#include "kdw/Toolbar.h"
#include "kdw/_WidgetWindow.h"
#include "kdw/CommandManager.h"
#include "kdw/ImageStore.h"
#include "kdw/Tooltip.h"
#include "Serialization/Serialization.h"
#include "XTL/SafeCast.h"
#include "kdw/Win32/Handle.h"

namespace kdw{

class ToolbarItem{
public:
	ToolbarItem(const char* commandName, int imageIndex)
	: commandName_(commandName)
	, imageIndex_(imageIndex)
	, checked_(false)
	, enabled_(false)
	, separated_(false)
	{
	}
	const char* commandName() const{ return commandName_.c_str(); }
	int imageIndex() const{ return imageIndex_; }
	void setImageIndex(int index){
		imageIndex_ = index;
	}
	void setChecked(bool checked){ checked_ = checked; }
	bool checked() const{ return checked_; }
	bool enabled() const{ return enabled_; }
	void setEnabled(bool enabled){ enabled_ = enabled; }
	bool separated() const{ return separated_; }
	void setSeparated(bool separated = true){ separated_ = separated; }
protected:
	std::string commandName_;
	int imageIndex_;
	bool separated_;
	bool checked_;
	bool enabled_;
};

// ---------------------------------------------------------------------------

class ToolbarImpl : public _WidgetWindow{
public:
	static const int SEPARATOR_WIDTH = 7;
	const char* className() const{ return "kdw.Toolbar"; }
	ToolbarImpl(Toolbar* owner);
	LRESULT onMessage(UINT message, WPARAM wparam, LPARAM lparam);
	void onMessagePaint();
	void onMessageLButtonDown(UINT button, int x, int y);
	void onMessageLButtonUp(UINT button, int x, int y);
	void onMessageMouseMove(UINT button, int x, int y);
	BOOL onMessageEraseBkgnd(HDC dc);
	void redraw(HDC dc);

	void setImageStore(ImageStore* imageStore);
	void setCommands(CommandManager* commands);
protected:
	Recti calculateItemRect(ToolbarItem& item, int previousRight);
	int calculateItemOffset(int itemIndex);
	int hitTestItem(int x, int y);
	bool updateHoveredItem();

	Tooltip tooltip_;

	typedef std::vector<ToolbarItem> ToolbarItems;
	ToolbarItems items_;

	ShareHandle<ImageStore> imageStore_;
	CommandManagerRef commands_;
	Vect2i buttonSize_;
	int hoveredIndex_;
	int pressedIndex_;

	friend class Toolbar;
};

ToolbarImpl::ToolbarImpl(Toolbar* owner)
: _WidgetWindow(owner)
, buttonSize_(16, 16)
, hoveredIndex_(-1)
, pressedIndex_(-1)
{
	create("", WS_CHILD, Recti(0, 0, 16, 16), *Win32::_globalDummyWindow);
}

void ToolbarImpl::onMessagePaint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(*this, &ps);
	redraw(dc);
	EndPaint(*this, &ps);
}

void ToolbarImpl::onMessageLButtonDown(UINT button, int x, int y)
{
	pressedIndex_ = hoveredIndex_;
	SetCapture(*this);
	RedrawWindow(*this, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
}

void ToolbarImpl::onMessageLButtonUp(UINT button, int x, int y)
{
	if(GetCapture() == *this)
		ReleaseCapture();

	if(pressedIndex_ == hoveredIndex_ && hoveredIndex_ != -1){
		xassert(hoveredIndex_ >= 0 && hoveredIndex_ < items_.size());
		if(commands_){
			ToolbarItem& item = items_[hoveredIndex_];
			CommandTemplate* cmd = CommandTemplateManager::instance().get(item.commandName());
			xassert(cmd);
			if(!cmd)
				return;
			CommandInstance* inst = commands_->commandInstanceByIndex(cmd->index());
			xassert(inst);
			if(inst)
				commands_->invokeCommandInstance(inst);
			pressedIndex_ = -1;
			hoveredIndex_ = -1;
		}
	}

	pressedIndex_ = -1;
	RedrawWindow(*this, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
}

bool ToolbarImpl::updateHoveredItem()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(*this, &pt);
	int oldHoveredIndex = hoveredIndex_;
	int index = hitTestItem(pt.x, pt.y);
	if(index >= 0 && items_[index].enabled())
		hoveredIndex_ = index;
	else
		hoveredIndex_ = -1;
	return (hoveredIndex_ != oldHoveredIndex);
}

void ToolbarImpl::onMessageMouseMove(UINT button, int x, int y)
{
	if(updateHoveredItem())
		RedrawWindow(*this, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	if(hoveredIndex_ != -1){
		ToolbarItem& item =  items_[hoveredIndex_];
		tooltip_.setText(CommandTemplateManager::instance().get(item.commandName())->label());
	}
	else
		tooltip_.setText("");
	tooltip_.show();
}

LRESULT ToolbarImpl::onMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	if(message == WM_MOUSELEAVE){
		if(updateHoveredItem())
			RedrawWindow(*this, 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
	}
	return __super::onMessage(message, wparam, lparam);
}


void ToolbarImpl::redraw(HDC dc)
{
	RECT rect;
	GetClientRect(*this, &rect);
	::FillRect(dc, &rect, GetSysColorBrush(COLOR_BTNFACE));
	int count = items_.size();
	int offset = 0;
	Recti itemRect(0, 0, 0, 0);
	for(int i = 0; i < count; ++i){
		itemRect = calculateItemRect(items_[i], itemRect.right());
		if(itemRect.right() < rect.right){
			ToolbarItem& item = items_[i];
			RECT rect = { itemRect.left() - 1, itemRect.top() - 1, itemRect.right() + 1, itemRect.bottom() + 1 };
			if(pressedIndex_ == i || item.checked()){
				//if(pressedIndex_ == i)
				FillRect(dc, &rect, GetSysColorBrush(COLOR_3DHIGHLIGHT));
				DrawEdge(dc, &rect, BDR_SUNKENOUTER, BF_RECT);
			}
			else{
				if(hoveredIndex_ == i && pressedIndex_ == -1)
					DrawEdge(dc, &rect, BDR_RAISEDINNER, BF_RECT);
			}
			if(imageStore_){
				if(pressedIndex_ == i && hoveredIndex_ == i || item.checked())
					imageStore_->_draw(item.imageIndex(), dc, itemRect.left() + 1, itemRect.top() + 1, !item.enabled());
				else
					imageStore_->_draw(item.imageIndex(), dc, itemRect.left(), itemRect.top(), !item.enabled());
			}
			else{
				//Win32::StockSelector brush(dc, GetStockObject(DC_BRUSH));
				Rectangle(dc, itemRect.left(), itemRect.top(), itemRect.right(), itemRect.bottom());
			}
		}
	}
}

BOOL ToolbarImpl::onMessageEraseBkgnd(HDC dc)
{
	return FALSE;
}

void ToolbarImpl::setImageStore(ImageStore* imageStore)
{
	imageStore_ = imageStore;
	if(imageStore_)
		buttonSize_ = imageStore_->slideSize();
}

void ToolbarImpl::setCommands(CommandManager* commands)
{
	commands_ = commands;
}

int ToolbarImpl::calculateItemOffset(int itemIndex)
{
	xassert(itemIndex >= 0 && itemIndex <= items_.size());
	int offset = 1;
	for(int i = 0; i < itemIndex; ++i){
		offset += buttonSize_.x;
		offset += 4;
		if(items_[i].separated())
			offset += SEPARATOR_WIDTH;
	}
	return offset;
}

int ToolbarImpl::hitTestItem(int x, int y)
{
	int count = items_.size();
	Recti itemRect(0, 0, 0, 0);
	for(int i = 0; i < count; ++i){
		itemRect = calculateItemRect(items_[i], itemRect.right());
		if(itemRect.point_inside(Vect2i(x, y)))
			return i;
	}	
	return -1;
}

Recti ToolbarImpl::calculateItemRect(ToolbarItem& item, int previousRight)
{
	Recti result;
	result.top(1);
	result.left(previousRight + 2 + (item.separated() ? SEPARATOR_WIDTH : 0));
	result.width(buttonSize_.x + 2);
	result.height(buttonSize_.y + 2);
	return result;
}

// ---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

Toolbar::Toolbar(CommandManager* commands, int border)
: _WidgetWithWindow(new ToolbarImpl(this), border)
, separateNext_(false)
{
	updateMinimalSize();
	setCommands(commands);

}

#pragma warning(pop)


void Toolbar::updateMinimalSize()
{
	Vect2i size = impl().buttonSize_;
	size.y = max(size.y, 16);
	size.y += 4;
	size.x = impl().calculateItemOffset(impl().items_.size()) + 2;
	size.x = max(size.x, 16);
	
	_setMinimalSize(size);
	
	impl().tooltip_.attach(this);
}

void Toolbar::setImageStore(ImageStore* imageStore)
{
	impl().setImageStore(imageStore);
	updateMinimalSize();
	_queueRelayout();
}


void Toolbar::setCommands(CommandManager* commands)
{
	impl().setCommands(commands);
}

void Toolbar::onCommandInstanceStateChanged(CommandInstance* instance)
{
	if(!impl().commands_)
		return;
	CommandManager* commands = impl().commands_;
	int count = impl().items_.size();
	for(int i = 0; i < count; ++i){
		ToolbarItem& item = impl().items_[i];
		int index = CommandTemplateManager::instance().get(item.commandName())->index();
		CommandInstance* inst = commands->commandInstanceByIndex(index);
		item.setChecked(inst->checked());
		item.setEnabled(inst->enabled());
		//item.setChe
	}
	RedrawWindow(impl(), 0, 0, RDW_UPDATENOW | RDW_INVALIDATE);
}


void Toolbar::addButton(const char* commandName, int imageIndex)
{
	CommandManager* commands = impl().commands_;
	xassert(commands);

	ToolbarItem item(commandName, imageIndex);

	CommandTemplate* templ = CommandTemplateManager::instance().get(commandName);
	xassert(templ && "No such command");
	
	CommandInstance* inst = commands->commandInstanceByIndex(templ->index());
	xassert(inst);
	inst->signalStateChanged().connect(this, Toolbar::onCommandInstanceStateChanged);
	item.setEnabled(inst->enabled());
	item.setChecked(inst->checked());
	if(separateNext_){
		item.setSeparated(true);
		separateNext_ = false;
	}
	impl().items_.push_back(item);
	updateMinimalSize();
	_queueRelayout();
}

void Toolbar::addSeparator()
{
	separateNext_ = true;
}

ToolbarImpl& Toolbar::impl()
{
	return *safe_cast<ToolbarImpl*>(_window());
}

}
