#include "StdAfx.h"

#include "kdw/Widget.h"
#include "kdw/Container.h"
#include "kdw/Window.h"

#include "kdw/Win32/Window.h"

#include "kdw/Serialization.h"

namespace kdw{

Widget::Widget()
: border_(0)
, minimalSize_(0, 0)
, requestSize_(0, 0)
, position_(0, 0, 1, 1)
, parent_(0)
, visible_(false)
, sensitive_(true)
, visibleInLayout_(true)
, relayoutQueued_(true)
, toolTip_(0)
{
}

Widget::~Widget()
{
	signalDelete_.emit();
	if(this == _focusedWidget())
		_setFocusedWidget(0);
		//_setFocusedWidget(parent());
}

void Widget::show()
{
	bool wasVisible = visible_;
	visible_ = true;
	if(!wasVisible)
		_updateVisibility();
}

void Widget::hide()
{
	visible_ = false;
	_updateVisibility();
}

void Widget::setVisibility(bool visible)
{
	if(visible)
		show();
	else
		hide();
}

void Widget::_setVisibleInLayout(bool visibleInLayout)
{
	bool wasVisibleInLayout = visibleInLayout_;
	visibleInLayout_ = visibleInLayout;
	if(wasVisibleInLayout != visibleInLayout_)
		_updateVisibility();
}

void Widget::_setPosition(const Recti& position)
{
	position_ = position;
	//if(!(position_.width() >= minimalSize_.x && position_.height() >= minimalSize_.y))
		//Beep(10, 10);
}

void Widget::setRequestSize(const Vect2i size)
{
	requestSize_ = size;
}

void Widget::setBorder(int border)
{
	border_ = border;
}

void Widget::_setMinimalSize(const Vect2i& size)
{
	minimalSize_ = size;
}

void Widget::_setParent(Container* parent)
{
	parent_ = parent;
}

void Widget::_queueRelayout()
{
	if(!isVisible()){
		relayoutQueued_ = true;
		if(parent())
			parent()->_queueRelayout();
	}
	else
		_relayoutParents();
}

void Widget::_relayoutParents(bool propagate)
{
	relayoutQueued_ = false;
	if(propagate && parent()){
		if(parent()->isVisible())
			parent()->_relayoutParents();
		else
			parent()->_queueRelayout();
	}
}

void Widget::_relayoutParents()
{
	_relayoutParents(true);
}

void Widget::_updateVisibility()
{
	if(visible_ && relayoutQueued_){
		_relayoutParents();
	}
}

void Widget::setFocus()
{
	_setFocus();
}


Window* Widget::mainWindow()
{
	Container* cont = dynamic_cast<Container*>(this);
	if(!cont)
		cont = parent();
	while(cont && cont->parent())
		cont = cont->parent();
	return dynamic_cast<Window*>(cont);
}

Widget* Widget::_nextWidget(Widget* last, FocusDirection direction)
{
	return 0;
}

Widget* Widget::_nextFocusableWidget(Widget* last, FocusDirection direction)
{
	switch(direction){
	case FOCUS_FIRST:
		if(kdw::Widget* widget = _nextWidget(last, direction)){
			if(!widget->_focusable())
				return _nextFocusableWidget(widget, FOCUS_NEXT);
		}
		else
			return 0;
	case FOCUS_LAST:
		if(kdw::Widget* widget = _nextWidget(last, direction)){
			if(!widget->_focusable())
				return _nextFocusableWidget(widget, FOCUS_PREVIOUS);
		}
		else
			return 0;
	case FOCUS_NEXT:
	case FOCUS_PREVIOUS:
	{
		kdw::Widget* widget = last;
		while(widget = _nextWidget(widget, direction))
			if(widget->_focusable())
				return widget;
        return 0;
	}
	}
	return 0;
}


void Widget::passFocus(FocusDirection direction)
{
	Widget* focused = _focusedWidget() ? _focusedWidget() : this;
	Widget* next = 0;
	Widget* container = focused->parent();

	if(container){
		do{
			if(!container)
				break;

			next = container->_nextFocusableWidget(focused, direction);
			if(next){
				xassert(next->_focusable());
				if(next->_focusable()){
					Widget::_setFocusedWidget(next);
					next->_setFocus();
					break;
				}
				else{
					focused = 0;
					container = next;
					next = 0;
				}
			}
			else{
				focused = container;
				if(container->parent()){
					container = container->parent();
				}
				else{
					container = container->parent();
				}
			}
		} while(next == 0);
	}
}


Widget* Widget::focusedWidget_ = 0;

void Widget::_setFocus()
{
	xassert(_focusable());
	_setFocusedWidget(this);
}

void Widget::_setFocusedWidget(Widget* widget)
{
	if(focusedWidget_ && focusedWidget_->canBeDefault())
		focusedWidget_->setDefaultFrame(false);
	if(widget){
		Window* wnd = widget->mainWindow();
		Widget* defaultWidget = wnd ? wnd->defaultWidget() : 0;
		if(widget->canBeDefault()){
			widget->setDefaultFrame(true);
			if(defaultWidget && defaultWidget != widget)
				defaultWidget->setDefaultFrame(false);
		}
		else
			if(defaultWidget)
				defaultWidget->setDefaultFrame(true);
	}
    focusedWidget_ = widget;
}

Widget* Widget::_focusedWidget()
{
    return focusedWidget_;
}


struct ShowAllVisitor : public WidgetVisitor{
	void operator()(Widget& widget){
		widget.showAll();
	}
};

void Widget::showAll()
{
	visitChildren(ShowAllVisitor());
	show();
}

void Widget::serialize(Archive& ar)
{
	if(ar.filter(kdw::SERIALIZE_DESIGN)){
		ar.serialize(_property(border_, this, &Widget::setBorder), "border", "Отступы");
		ar.serialize(requestSize_, "requestSize", "Размер");
		ar.serialize(_property(sensitive_, this, &Widget::setSensitive), "sensitive", "Активность");
		ar.serialize(_property(visible_, this, &Widget::setVisibility), "visible", "Видимость");
	}
}

HotkeyContext* findHotkeyContext(Widget* widget)
{
	while(widget){
		if(HotkeyContext* context = widget->hotkeyContext())
			return context;
		widget = widget->parent();
	}
	return 0;
}

// --------------------------------------------------------------------------------

void Container::setBorder(int border)
{
	Widget::setBorder(border);
	_arrangeChildren();
}

void Container::_setFocus()
{
	xassert(_focusable());
	kdw::Widget* widget = _nextFocusableWidget(0, FOCUS_FIRST);
	xassert(widget != 0);
	if(widget)
		widget->_setFocus();
}

struct FocusMover : public WidgetVisitor{
	FocusMover(Widget* current, FocusDirection direction)
	: direction_(direction)
	, current_(current)
	, previous_(0)
	, result_(0)
	{}

	Widget* result(){ return result_; }
	void operator()(Widget& widget){
		//if(widget._focusable()){
		switch(direction_){
		case FOCUS_FIRST:
			if(!result_)
				result_ = &widget;
			break;
		case FOCUS_LAST:
			result_ = &widget;
			break;
		case FOCUS_NEXT:
			if(previous_ == current_)
				result_ = &widget;
			break;
		case FOCUS_PREVIOUS:
			if(&widget == current_)
				result_ = previous_;
			break;
		}
		previous_ = &widget;
		// }
	}
protected:
	FocusDirection direction_;
	Widget* result_;
	Widget* previous_;
	Widget* current_;
};

Widget* Container::_nextWidget(Widget* last, FocusDirection direction)
{
	switch(direction){
	case FOCUS_DOWN:
	case FOCUS_RIGHT:
		direction = FOCUS_NEXT;
		break;
	case FOCUS_UP:
	case FOCUS_LEFT:
		direction = FOCUS_PREVIOUS;
		break;
	default:
		break;
	}
	FocusMover mover(last, direction);
	visitChildren(mover);
	return mover.result();
}

bool Container::isActive() const
{
	Win32::Window32* window = _findWindow(this);
	if(window){
		// получаем родительское окно и смотрим активно ли оно
		while(window->parent())
			window = window->parent();
		return (::GetActiveWindow() == *window);
	}
	else{
		ASSERT(0);
		return false;
	}
}

bool Container::isVisible() const
{
	return parent() ? parent()->isVisible() : false;
	/*
	if(Win32::Window32* window = _findWindow())
		return ::IsWindowVisible(*window) ? TRUE : FALSE;
	return false;
	*/
}


struct HaveFocusable : public WidgetVisitor{
	bool result;
	HaveFocusable()
	: result(false)
	{
	}
	void operator()(Widget& widget){
		if(widget._focusable())
			result = true;
	}
};

bool Container::_focusable() const
{
	//return false;
	HaveFocusable haveFocusable;
	visitChildren(haveFocusable);
	return haveFocusable.result;
}

Win32::Window32* _findWindow(const Widget* widget)
{
	while(widget){
		if(Win32::Window32* window = widget->_window())
			return window;
		else
			widget = widget->parent();
	}
	return Win32::_globalDummyWindow;
}

void _ensureChildren(kdw::Container* container, kdw::Widget* widget)
{
	if(widget && container && widget->parent() != container){
		container->_arrangeChildren();
		widget->_setParent(container);
		container->_queueRelayout();
	}
}

}
