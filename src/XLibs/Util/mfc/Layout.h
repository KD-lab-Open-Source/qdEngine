#ifndef __LAYOUT_H_INCLUDED__
#define __LAYOUT_H_INCLUDED__

#include "XTL\Rect.h"
#include "Handle.h"
#include <list>

class LayoutElement : public ShareHandleBase{
public:
	LayoutElement();
	virtual ~LayoutElement() {};

	
	virtual void relayout() {}; // вызывается дочерними элементами

	virtual void setParent(LayoutElement* parent);

	const Vect2i& minimalSize() const{ return minimalSize_; }

	virtual void setPosition(const Recti& position);
	const Recti& position() const{ return position_; }

	virtual void setBorder(int border);
	int border() const{ return border_; };
protected:
	Recti position_;
	LayoutElement* parent_;
	Vect2i minimalSize_;
	int border_;
};


class LayoutBox : public LayoutElement{
protected:
	struct Packing{
	public:
		Packing(bool _expand = false, bool _fill = false, int _padding = 0)
		: expand(_expand) , fill(_fill) , padding(_padding)
		{}

		bool expand;
		bool fill;
		int padding;
	};
public:
	LayoutBox(int spacing, int border);
	void pack(LayoutElement* element, bool expand = false, bool fill = false, int padding = 0);
	void setPosition(const Recti& position);
	void clear();
	void relayout();
protected:
	struct LayoutBoxElement : public ShareHandleBase{
		ShareHandle<LayoutElement> element;
		Packing packing;
	};

	void arrange();

	virtual float elementLength(LayoutBoxElement* element) const = 0;
	virtual float elementWidth(LayoutBoxElement* element) const = 0;

	virtual float boxLength() const = 0;
	virtual void setElementPosition(LayoutBoxElement* element, float offset, float length) = 0;
	virtual void setBoxSize(const Vect2i& size) = 0;

	typedef std::list< ShareHandle<LayoutBoxElement> > Elements;
	Elements elements_;
	int spacing_;
};

class LayoutVBox : public LayoutBox{
public:
	LayoutVBox(int spacing = 0, int border = 0);
protected:
	void setElementPosition(LayoutBoxElement* element, float offset, float length);
	float elementLength(LayoutBoxElement* element) const;
	float elementWidth(LayoutBoxElement* element) const;
	float boxLength() const;
	void setBoxSize(const Vect2i& size);
};

class LayoutHBox : public LayoutBox{
public:
	LayoutHBox(int spacing = 0, int border = 0);
protected:
	void setElementPosition(LayoutBoxElement* element, float offset, float length);
	float elementLength(LayoutBoxElement* element) const;
	float elementWidth(LayoutBoxElement* element) const;
	float boxLength() const;
	void setBoxSize(const Vect2i& size);
};


#endif
