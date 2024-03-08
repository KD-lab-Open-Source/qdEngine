#include "StdAfx.h"
#include "Serialization/SerializationFactory.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/_PropertyRowBuiltin.h"

#include "kdw/PropertyTree.h"
#include "kdw/_PropertyTreeColumnDrawer.h"
#include "kdw/PropertyRow.h"

#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"
#include "Serialization/RangedWrapper.h"

namespace kdw{

template<class WrapperType, class ScalarType>
class PropertyRowRanged : public PropertyRowNumeric<WrapperType, PropertyRowRanged>{
public:
	static const bool Custom = true;
	PropertyRowRanged(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	PropertyRowRanged();
	int floorHeight() const{ return 12; }
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);

	bool onKeyDown(PropertyTree* tree, sKey key);
	bool onMouseDown(PropertyTree* tree, Vect2i point, bool& changed);
	void onMouseMove(PropertyTree* tree, Vect2i point);
	void onMouseUp(PropertyTree* tree, Vect2i point);

	void handleMouse(Vect2i point, const Recti& floorRect);
};

template<class WrapperType, class ScalarType>
void PropertyRowRanged<WrapperType, ScalarType>::handleMouse(Vect2i point, const Recti& floorRect)
{
	if(floorRect.point_inside(point)){
		float val = float(point.x - (floorRect.left() + 1)) / (floorRect.width() - 2);
		value().value() = val * value().range().length() + value().range().minimum();
		if(value().step() != 0)
			value().value() = value().value() - fmodf(value().value(), value().step());
		value().clip();
	}
}

template<class WrapperType, class ScalarType>
bool PropertyRowRanged<WrapperType, ScalarType>::onKeyDown(PropertyTree* tree, sKey key)
{
	if(key == VK_LEFT){
		value().value() = clamp(value().value() - value().step(),
			                    value().range().minimum(),
								value().range().maximum());
		tree->model()->rowChanged(this);
		return true;
	}
	if(key == VK_RIGHT){
		value().value() = clamp(value().value() + value().step(),
			                    value().range().minimum(),
								value().range().maximum());
		tree->model()->rowChanged(this);
		return true;
	}
	return __super::onKeyDown(tree, key);
}

template<class WrapperType, class ScalarType>
bool PropertyRowRanged<WrapperType, ScalarType>::onMouseDown(PropertyTree* tree, Vect2i point, bool& changed)
{
	Recti rowRect;
	tree->getRowRect(this, rowRect);
	Recti floorRect;
	tree->drawer()->getSubRectFloor(this, rowRect, floorRect);

	if(floorRect.point_inside(point)){
		handleMouse(point, floorRect);
		tree->redraw();
		return true;
	}
	return false;
}
template<class WrapperType, class ScalarType>
void PropertyRowRanged<WrapperType, ScalarType>::onMouseMove(PropertyTree* tree, Vect2i point)
{
	Recti rowRect;
	tree->getRowRect(this, rowRect);
	Recti floorRect;
	tree->drawer()->getSubRectFloor(this, rowRect, floorRect);
	handleMouse(point, floorRect);
	tree->redraw();
}

template<class WrapperType, class ScalarType>
void PropertyRowRanged<WrapperType, ScalarType>::onMouseUp(PropertyTree* tree, Vect2i point)
{
	tree->model()->rowChanged(this);
}


template<class WrapperType, class ScalarType>
PropertyRowRanged<WrapperType, ScalarType>::PropertyRowRanged(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowNumeric<WrapperType, PropertyRowRanged>(object, size, name, nameAlt, typeName)
{
	height_ = ROW_DEFAULT_HEIGHT;
}

template<class WrapperType, class ScalarType>
void PropertyRowRanged<WrapperType, ScalarType>::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	__super::redraw(dc, iconRect, widgetRect, floorRect, hostRow);
	ScalarType val = value();
	RECT rect = floorRect;
	rect.top += 1;
	rect.bottom -= 1;
	Win32::drawSlider(dc, rect, float(val - value().range().minimum()) / value().range().length(), false/*state == FOCUSED*/);
}

template<class WrapperType, class ScalarType>
PropertyRowRanged<WrapperType, ScalarType>::PropertyRowRanged()
{
}

typedef PropertyRowRanged<RangedWrapperf, float> PropertyRowRangedf;
typedef PropertyRowRanged<RangedWrapperi, int> PropertyRowRangedi;

DECLARE_SEGMENT(PropertyRowRanged)
REGISTER_PROPERTY_ROW(RangedWrapperf, PropertyRowRangedf);
REGISTER_PROPERTY_ROW(RangedWrapperi, PropertyRowRangedi);

}
