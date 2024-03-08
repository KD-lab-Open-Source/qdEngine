#ifndef __CURVE_EDITOR_ELEMENT_H_INCLUDED__
#define __CURVE_EDITOR_ELEMENT_H_INCLUDED__

#include "Handle.h"
#include "XMath/XMath.h"	
#include "XTL/Rect.h"
struct HDC__;
typedef HDC__* HDC;

namespace kdw{

struct ViewTransform{
	ViewTransform()
		: offset_(Vect2f::ZERO)
		, scale_(1.0f, 1.0f)
		, outerBounds_(0.0f, 0.0f, 1.0f, 1.0f)
		, innerBounds_(0.0f, 0.0f, 1.0f, 1.0f)
		, viewOuterBounds_(0, 0, 1, 1)
		, viewInnerBounds_(0, 0, 1, 1)
	{}
	void set(const Vect2f& offset, const Vect2f& scale, Rectf& innerBounds, Rectf& outerBounds){
		offset_ = offset;
		scale_ = scale;
		innerBounds_ = innerBounds;
		outerBounds_ = outerBounds;
		viewInnerBounds_ = (*this)(innerBounds);
		viewOuterBounds_ = (*this)(outerBounds);
	}
	Vect2i operator()(Vect2f point) const{
		return (point + offset_) * scale_;
	}
	Recti operator()(const Rectf& rect) const{
		Vect2i left_top = (*this)(rect.left_bottom());
		Vect2i right_bottom = (*this)(rect.right_top());
		return Recti(left_top, right_bottom - left_top);
	}
	int transformX(float x) const{
		return int((x + offset_.x) * scale_.x);
	}
	int transformY(float y) const{
		return int((y + offset_.y) * scale_.y);
	}
	float untransformX(int x) const{
		return float(x) / scale_.x - offset_.x;
	}
	float untransformY(int y) const{
		return float(y) / scale_.y - offset_.y;
	}
	Vect2f untransform(Vect2i point) const{
		return Vect2f(point.x / scale_.x, point.y / scale_.y) - offset_;
	}
	Rectf untransform(const Recti& rect) const{
		Vect2f left_top = untransform(rect.left_bottom());
		Vect2f right_bottom = untransform(rect.right_top());
		return Rectf(left_top, right_bottom - left_top);
	}
	const Rectf& innerBounds() const{ return innerBounds_; }
	const Recti& viewInnerBounds() const{ return viewInnerBounds_; }

	const Rectf& outerBounds() const{ return outerBounds_; }
	const Recti& viewOuterBounds() const{ return viewOuterBounds_; }
	
	Vect2f scale() const{ return scale_; }
protected:
	Vect2f offset_;
	Vect2f scale_;
	Recti viewInnerBounds_;
	Recti viewOuterBounds_;
	Rectf innerBounds_;
	Rectf outerBounds_;
};
	
class CurveEditor;
class CurveEditorElement : public ShareHandleBase{
public:
	CurveEditorElement();
	virtual void click(Vect2f position, const ViewTransform& view) {}
	virtual CurveEditorElement* hitTest(Vect2f position, const ViewTransform& view) { return 0; }
	virtual bool getBoundingBox(Rectf& rect){ return false; }
	const Vect2f& position() const{ return position_; }
	virtual void redraw(HDC dc, const ViewTransform& view);
	virtual void move(Vect2f delta, const ViewTransform& view) {}
	virtual void commit() {}
	virtual void rebuild() {}
	virtual void remove() {}
	bool removable() const{ return removable_; }
	void setRemovable(bool removable){ removable_ = removable; }

	void setEditor(CurveEditor* editor){ editor_ = editor; }
	CurveEditor* editor(){ return editor_; }
protected:
	CurveEditor* editor_;
	Vect2f position_;
	bool removable_;
};

class CurveEditorTimeline : public CurveEditorElement{
public:
	CurveEditorTimeline(CurveEditor* editor);
	bool getBoundingBox(Rectf& rect);
	void redraw(HDC dc, const ViewTransform& view);
	void move(Vect2f delta, const ViewTransform& view);
	void commit();
	CurveEditorElement* hitTest(Vect2f position, const ViewTransform& view);
protected:
	Recti timelineRect(const ViewTransform& view);

    CurveEditor* editor_;
};

}

#endif
