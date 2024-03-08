#include "StdAfx.h"
#include "Serialization/SerializationFactory.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/_PropertyRowBuiltin.h"

#include "kdw/PropertyTree.h"
#include "kdw/_PropertyTreeColumnDrawer.h"
#include "kdw/PropertyRow.h"
#include "kdw/ColorChooserDialog.h"

#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"
#include "XMath/Colors.h"

namespace kdw{

template<class ColorType>
class PropertyRowColor : public PropertyRowImpl<ColorType, PropertyRowColor>{
public:
	static const bool Custom = true;
	PropertyRowColor(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	PropertyRowColor();
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);

	bool activateOnAdd() const{ return true; }
	bool onActivate(PropertyTree* tree);
	bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow);
	int minimalWidth() const{ return 40; }

};

template<class ColorType>
bool PropertyRowColor<ColorType>::onActivateWidget(PropertyTree* tree, PropertyRow* hostRow)
{
	return onActivate(tree);
}

template<class ColorType>
bool PropertyRowColor<ColorType>::onActivate(PropertyTree* tree)
{
	ColorChooserDialog dialog(tree, Color4f(value()));
	if(dialog.showModal()){
		value() = ColorType(dialog.get());
		tree->model()->rowChanged(this);
		return true;
	}
	return false;
}


template<class ColorType>
PropertyRowColor<ColorType>::PropertyRowColor(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<ColorType, PropertyRowColor>(object, size, name, nameAlt, typeName)
{
	height_ = ROW_DEFAULT_HEIGHT;
}

template<class ColorType>
void PropertyRowColor<ColorType>::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	__super::redraw(dc, iconRect, widgetRect, floorRect, hostRow);

	RECT rect = widgetRect;
	int roundness = 7;
	::SelectObject(dc, GetStockObject(NULL_PEN));
	::SelectObject(dc, GetSysColorBrush(COLOR_3DSHADOW));
	::RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, roundness, roundness);

	HRGN region = CreateRoundRectRgn(rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, roundness, roundness - 2);
	::SelectClipRgn(dc, region);

	Color4c c = Color4c(value());
	HBRUSH brush = CreateSolidBrush(RGB(c.r, c.g, c.b));

	FillRect(dc, &rect, brush);

	DeleteObject(brush);

	::SelectClipRgn(dc, 0);
	DeleteObject(region);

}

template<class ColorType>
PropertyRowColor<ColorType>::PropertyRowColor()
{
}

DECLARE_SEGMENT(PropertyRowColor)
REGISTER_PROPERTY_ROW(Color3c, PropertyRowColor<Color3c>); 
REGISTER_PROPERTY_ROW(Color4c, PropertyRowColor<Color4c>); 
REGISTER_PROPERTY_ROW(Color4f, PropertyRowColor<Color4f>); 

}
