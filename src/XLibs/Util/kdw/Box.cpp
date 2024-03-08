#include "StdAfx.h"
#include "kdw/Box.h"
#include "kdw/HBox.h"
#include "kdw/VBox.h"

#include "kdw/Win32/Window.h"

#include "kdw/Serialization.h"
#include "Serialization/SerializationFactory.h"


namespace kdw{

REGISTER_CLASS(Widget, _DummyWidget, "Разметка\\Заглушка");
REGISTER_CLASS(Widget, VBox, "Разметка\\Бокс, вертикальный");
REGISTER_CLASS(Widget, HBox, "Разметка\\Бокс, горизонтальный");
REGISTER_CLASS(Container, VBox, "Бокс, вертикальный");
REGISTER_CLASS(Container, HBox, "Бокс, горизонтальный");

//////////////////////////////////////////////////////////////////////////////

Box::Box(int spacing, int border)
: spacing_(spacing)
, focusedElementIndex_(0)
, dummy_(new _DummyWidget)
, clipChildren_(false)
{
	border_ = border;
	add(dummy_, false, true, true, 0);
	ASSERT(parent_ == 0);
}

Box::~Box()
{
	clear();
}

void Box::remove(Widget* widget)
{
	int index = 0;
	Elements::iterator it;
	FOR_EACH(elements_, it){
		if(it->widget == widget){
			Box::remove(index);
			return;
		}
		++index;
	}
 	xassert(0);
}

int Box::size() const
{
	return elements_.size() - 1;
}


void Box::remove(int index)
{
	xassert(index >= 0 && index < elements_.size());

	Elements::iterator it = elements_.begin();
	int i = 0;
	while(it != elements_.end()){
		if(i == index)
			break;
		if(it->widget != dummy_)
			++i;
		++it;
	}
	xassert(it != elements_.end());

	Widget* widget = it->widget;
	widget->_setParent(0);
	elements_.erase(it);
	_arrangeChildren();
	_queueRelayout();
}

void Box::add(Widget* widget, bool placeInEnd, bool expand, bool fill, int padding)
{
	xassert(widget);
	Element element;

	element.widget = widget;
	element.packing = BoxPacking(expand, fill, padding);

	Elements::iterator it = elements_.begin();
	if(widget == dummy_)
		elements_.push_back(element);
	else{
		while(it->widget != dummy_)
			++it;
		ASSERT(it != elements_.end());
		if(placeInEnd)
			it = elements_.end();
			//++it;
	
		elements_.insert(it, element);

		_arrangeChildren();
		widget->_setParent(this);
		_queueRelayout();
	}
}

void Box::_relayoutParents()
{
	Vect2i oldMinimalSize = _minimalSize();
	Elements::iterator it;

	float length = 0;
	float width = 0;
	FOR_EACH(elements_, it){
		Widget* widget = it->widget;
		const BoxPacking& packing = it->packing;

		length += elementLength(*it);
		width = max(elementWidth(*it), width);
		length += packing.padding * 2.0f;
	}
	length += float(spacing_) * float(max(0, int(elements_.size()))) + border_ * 2.0f;
	width += border_ * 2.0f;
	if(clipChildren_)
		setBoxSize(Vect2i(0, round(width)));
	else
		setBoxSize(Vect2i(round(length), round(width)));
	::RedrawWindow(*_findWindow(this), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
	
	Widget::_relayoutParents(oldMinimalSize != _minimalSize());
}

void Box::visitChildren(WidgetVisitor& visitor) const
{
	Elements::const_iterator it;
	FOR_EACH(elements_, it)
		if(it->widget && it->widget != dummy_)
			visitor(*it->widget);
}

void Box::_updateVisibility()
{
	Widget::_updateVisibility();
	Elements::iterator it;
	FOR_EACH(elements_, it)
		if(it->widget)
			it->widget->_setVisibleInLayout(_visibleInLayout());
}

void Box::_arrangeChildren()
{
	float length = boxLength();

	float fixed_length = 0.0f;
	Elements::iterator it;

	int sizeable_elements = int(elements_.size()) - 1; // без dummy
	Elements::iterator dummyIt = elements_.end();
	FOR_EACH(elements_, it){
		if(it->widget){
			fixed_length += it->packing.padding * 2.0f;
			fixed_length += elementLength(*it);

			if(it->widget == dummy_)
				dummyIt = it;
			else{
				if(it->packing.expand == false)
					--sizeable_elements;
			}
		}
	}
	ASSERT(dummyIt != elements_.end());
	if(sizeable_elements > 0){
		dummyIt->packing.expand = false;
		dummyIt->packing.fill = false;
	}
	else{
		sizeable_elements = 1;
		dummyIt->packing.expand = true;
		dummyIt->packing.fill = true;

		fixed_length += dummyIt->packing.padding * 2.0f;
		fixed_length += elementLength(*dummyIt);
	}


	fixed_length += float(max(0, int(elements_.size()) - 2)) * spacing_;

	float sizeable_length = length - fixed_length;
	
	float element_length = 0.0f;

	if(sizeable_elements > 0 && sizeable_length > FLT_EPS)
		element_length = sizeable_length / float(sizeable_elements);

	float offset = 0.0f;
	if(!elements_.empty()){
		Win32::Window32* window = _findWindow(this);
		ASSERT(window);
		Win32::Window32::PositionDeferer deferer = window->positionDeferer(int(elements_.size()));
		FOR_EACH(elements_, it){
			if(it->widget){
				float len = it->packing.expand ? element_length + elementLength(*it): elementLength(*it);

				offset += it->packing.padding;
				setElementPosition(*it, offset, min(length - offset, len));

				offset += it->packing.padding + len;
				offset += float(spacing_);
				offset = min(offset, length);
			}
		}
	}
	::RedrawWindow(*_findWindow(this), 0, 0, RDW_ALLCHILDREN);
}

void Box::_setParent(Container* container)
{
	Container::_setParent(container);
	Elements::iterator it;
	FOR_EACH(elements_, it){
		it->widget->_setParent(this);
	}	
}

void Box::_setPosition(const Recti& position)
{
	Container::_setPosition(position);
	_arrangeChildren();
}

void Box::clear()
{
	Elements::iterator it;
	FOR_EACH(elements_, it){
		Element& element = *it;
		if(Widget* widget = element.widget)
			widget->_setParent(0);
	}

	// просто так вызвать clear у списков нельзя, т.к. при удаление контролов вызывется код апдейта раскладки
	// и кто-нибудь обязательно проитерируется по удаленным children-ам этого объекта

	dummy_->addRef();
	Elements temp;
	temp.swap(elements_);
	add(dummy_);
	dummy_->decrRef();
}	

void Box::_setFocus()
{
	__super::_setFocus();
}

void BoxPacking::serialize(Archive& ar)
{
	ar.serialize(expand, "expand", "Расширять");
	ar.serialize(fill, "fill", "Заполнять");
	ar.serialize(padding, "padding", "Интервал");
}

void Box::Element::serialize(Archive& ar)
{
	packing.serialize(ar);
	ar.serialize(widget, "widget", "&Контрол");
}

void Box::serialize(Archive& ar)
{
	ar.serialize(_property(spacing_, this, &Box::setSpacing), "spacing", "Интервал");
	Container::serialize(ar);

	ar.serialize(elements_, "elementsStart", "Элементы");
	Elements::iterator it;
	FOR_EACH(elements_, it)
		_ensureChildren(this, it->widget);
}

void Box::setSpacing(int spacing)
{
	spacing_ = spacing;
	_arrangeChildren();
	_relayoutParents();
}

void Box::setClipChildren(bool clipChildren)
{
	clipChildren_ = clipChildren;
}

//////////////////////////////////////////////////////////////////////////////
VBox::VBox(int spacing, int border)
: Box(spacing, border)
{
}

float VBox::elementLength(const Element& element) const
{
	return float(element.widget->_minimalSize().y);
}

float VBox::elementWidth(const Element& element) const
{
	return float(element.widget->_minimalSize().x);
}

float VBox::boxLength() const
{
	return float(_position().height() - border_ * 2);
}

void VBox::setElementPosition(Element& element, float offset, float length)
{
	int border = this->border();
	Recti rect(border, border + round(offset), _position().width() - 2 * border, round(length));
	if(element.packing.fill){
		element.widget->_setPosition(rect + position_.left_top());
	}
	else{
		rect.top(rect.top() + (rect.height() - element.widget->_minimalSize().y) / 2);
		rect.height(element.widget->_minimalSize().y);
		element.widget->_setPosition(rect + position_.left_top());
	}
}

void VBox::setBoxSize(const Vect2i& size)
{
	minimalSize_.x = size.y;
	minimalSize_.y = size.x;
}

Widget* VBox::_nextWidget(Widget* last, FocusDirection focusDirection)
{
	switch(focusDirection)
	{
	case FOCUS_UP:
		return Container::_nextWidget(last, FOCUS_PREVIOUS);
	case FOCUS_DOWN:
		return Container::_nextWidget(last, FOCUS_NEXT);
	case FOCUS_LEFT:
		return 0;//Container::_nextWidget(last, FOCUS_FIRST);
	case FOCUS_RIGHT:
		return 0;//Container::_nextWidget(last, FOCUS_LAST);
	default:
		return Container::_nextWidget(last, focusDirection);
	}
}

//////////////////////////////////////////////////////////////////////////////
HBox::HBox(int spacing, int border)
: Box(spacing, border)
{
}

float HBox::elementLength(const Element& element) const
{
	return float(element.widget->_minimalSize().x);
}

float HBox::elementWidth(const Element& element) const
{
	return float(element.widget->_minimalSize().y);
}

float HBox::boxLength() const
{
	return float(_position().width() - border_ * 2);
}

void HBox::setElementPosition(Element& element, float offset, float length)
{
	int border = this->border();
	Recti rect(border + round(offset), border, round(length), _position().height() - 2 * border);
	if(element.packing.fill){
		element.widget->_setPosition(rect + position_.left_top());
	}
	else{
		if(length > element.widget->_minimalSize().x){
			rect.left(rect.left() + (rect.width() - element.widget->_minimalSize().x) / 2);
			rect.width(element.widget->_minimalSize().x);
			element.widget->_setPosition(rect + position_.left_top());
		}
		else
			element.widget->_setPosition(rect + position_.left_top());
	}
}

void HBox::setBoxSize(const Vect2i& size)
{
	minimalSize_.x = size.x;
	minimalSize_.y = size.y;
}

Widget* HBox::_nextWidget(Widget* last, FocusDirection focusDirection)
{
	switch(focusDirection)
	{
	case FOCUS_LEFT:
		return Container::_nextWidget(last, FOCUS_PREVIOUS);
	case FOCUS_RIGHT:
		return Container::_nextWidget(last, FOCUS_NEXT);
	case FOCUS_UP:
	case FOCUS_DOWN:
		return 0;
	default:
		return Container::_nextWidget(last, focusDirection);
	}
}

//////////////////////////////////////////////////////////////////////////////
}
