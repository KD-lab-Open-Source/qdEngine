#include "stdafx.h"
#include "Layout.h"

LayoutElement::LayoutElement()
: border_(0)
, minimalSize_(10, 10)
, position_(0, 0, 1, 1)
, parent_(0)
{
}

void LayoutElement::setPosition(const Recti& position)
{
	position_ = position;
}

void LayoutElement::setBorder(int border)
{
	border_ = border;
}

void LayoutElement::setParent(LayoutElement* parent)
{
	parent_ = parent;
	parent_->relayout();
}

//////////////////////////////////////////////////////////////////////////////

LayoutBox::LayoutBox(int spacing, int border)
: spacing_(spacing)
{
	border_ = border;
}

void LayoutBox::pack(LayoutElement* element, bool expand, bool fill, int padding)
{
	LayoutBoxElement* box_element = new LayoutBoxElement;

	box_element->element = element;
	box_element->packing = Packing(expand, fill, padding);
	
	elements_.push_back(box_element);

	element->setParent(this);
	//arrange();
}

void LayoutBox::relayout()
{
	Elements::iterator it;

	float length = 0;
	float width = 0;
	FOR_EACH(elements_, it){
		LayoutElement* element = (*it)->element;
		const Packing& packing = (*it)->packing;

		length += elementLength(*it);
		width = max(elementWidth(*it), width);
		length += element->border() * 2.0f;
		length += packing.padding * 2.0f;
	}
	length += float(spacing_) * float(max(0, elements_.size() - 1)) + border_ * 2.0f;
	width += border_ * 2.0f;
	setBoxSize(Vect2i(round(length), round(width)));
	
	if(parent_)
		parent_->relayout();
}

void LayoutBox::arrange()
{
	float length = boxLength();

	float fixed_length = 0.0f;
	Elements::iterator it;

	int sizeable_elements = elements_.size();
	FOR_EACH(elements_, it){

		fixed_length += (*it)->packing.padding * 2.0f;
		fixed_length += (*it)->element->border() * 2.0f;
		fixed_length += elementLength(*it);
			
		if((*it)->packing.expand == false){
			--sizeable_elements;
		}
	}
	fixed_length += float(max(0, elements_.size() - 1)) * spacing_;

	float sizeable_length = length - fixed_length;
	
	float element_length = 0.0f;

	if(sizeable_elements > 0 && sizeable_length > FLT_EPS)
		element_length = sizeable_length / float(sizeable_elements);

	float offset = 0.0f;
	FOR_EACH(elements_, it){
		float len = (*it)->packing.expand ? element_length + elementLength(*it): elementLength(*it);

		len += (*it)->element->border() * 2.0f;
		offset += (*it)->packing.padding;
		setElementPosition(*it, offset, len);

		offset += (*it)->packing.padding + len;
		offset += float(spacing_);
	}

}

void LayoutBox::setPosition(const Recti& position)
{
	__super::setPosition(position);
	arrange();
}

void LayoutBox::clear()
{
	elements_.clear();
}
//////////////////////////////////////////////////////////////////////////////
LayoutVBox::LayoutVBox(int spacing, int border)
: LayoutBox(spacing, border)
{
}

float LayoutVBox::elementLength(LayoutBoxElement* element) const
{
	return element->element->minimalSize().y;
}

float LayoutVBox::elementWidth(LayoutBoxElement* element) const
{
	return element->element->minimalSize().x;
}

float LayoutVBox::boxLength() const
{
	return position().height() - border_ * 2;
}

void LayoutVBox::setElementPosition(LayoutBoxElement* element, float offset, float length)
{
	int border = element->element->border(); 
	Recti rect(border, round(offset), position().width() - 2 * border - 2 * border_, round(length));
	if(element->packing.fill){
		element->element->setPosition(rect + position_.left_top() + Vect2i(border_, border_));
	}
	else{
		rect.top(rect.top() + (rect.height() - element->element->minimalSize().y) / 2);
		rect.height(element->element->minimalSize().y);
		element->element->setPosition(rect + position_.left_top() + Vect2i(border_, border_));
	}
}

void LayoutVBox::setBoxSize(const Vect2i& size)
{
	minimalSize_.x = size.y;
	minimalSize_.y = size.x;
}
//////////////////////////////////////////////////////////////////////////////
LayoutHBox::LayoutHBox(int spacing, int border)
: LayoutBox(spacing, border)
{
}

float LayoutHBox::elementLength(LayoutBoxElement* element) const
{
	return element->element->minimalSize().x;
}

float LayoutHBox::elementWidth(LayoutBoxElement* element) const
{
	return element->element->minimalSize().y;
}

float LayoutHBox::boxLength() const
{
	return position().width() - border_ * 2;
}

void LayoutHBox::setElementPosition(LayoutBoxElement* element, float offset, float length)
{
	int border = element->element->border(); 
	Recti rect(round(offset), border, round(length), position().height() - 2 * border - 2 * border_);
	if(element->packing.fill){
		element->element->setPosition(rect + position_.left_top() + Vect2i(border_, border_));
	}
	else{
		rect.left(rect.left() + (rect.width() - element->element->minimalSize().x) / 2);
		rect.width(element->element->minimalSize().x);
		element->element->setPosition(rect + position_.left_top() + Vect2i(border_, border_));
	}
}

void LayoutHBox::setBoxSize(const Vect2i& size)
{
	minimalSize_.x = size.x;
	minimalSize_.y = size.y;
}
//////////////////////////////////////////////////////////////////////////////
