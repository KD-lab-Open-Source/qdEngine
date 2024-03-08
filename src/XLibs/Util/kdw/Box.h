#ifndef __KDW_BOX_H_INCLUDED__
#define __KDW_BOX_H_INCLUDED__

#include "kdw/Container.h"
//class Recti;

namespace kdw{

class _DummyWidget : public Widget{
public:
	bool isVisible()const { return true; }
};

struct BoxPacking{
public:
	BoxPacking(bool _expand = false, bool _fill = false, int _padding = 0)
	: expand(_expand) , fill(_fill) , padding(_padding)
	{}
	void serialize(Archive& ar);

	bool expand;
	bool fill;
	int padding;
};

class KDW_API Box: public Container{
public:
	Box(int spacing, int border);
	~Box();

	/// добавить контрол
	void add(Widget* widget, bool placeInEnd = true, bool expand = false, bool fill = false, int padding = 0);
	void remove(Widget* widget);
	void remove(int index);
	int size() const;

	void setSpacing(int spacing);
	int spacing() const{ return spacing_; }
	void setClipChildren(bool clipChildren);

	/// удалить все контролы
	void clear();
	
	// virtuals
	void visitChildren(WidgetVisitor& visitor) const;
	void serialize(Archive& ar);
	// ^^

	void _setParent(Container* container);
	void _setPosition(const Recti& position);
	void _relayoutParents();
	void _arrangeChildren();
	void _updateVisibility();
	void _setFocus();
protected:
	struct Element : public ShareHandleBase{
		BoxPacking packing;
		ShareHandle<Widget> widget;
		void serialize(Archive& ar);
	};

	virtual float elementLength(const Element& element) const = 0;
	virtual float elementWidth(const Element& element) const = 0;

	virtual float boxLength() const = 0;
	virtual void setElementPosition(Element& element, float offset, float length) = 0;
	virtual void setBoxSize(const Vect2i& size) = 0;

	typedef std::list<Element> Elements;
	Elements elements_;
	kdw::_DummyWidget* dummy_;
	bool clipChildren_;
	int spacing_;
	int focusedElementIndex_;
};


}

#endif
