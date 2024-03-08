	#include "StdAfx.h"
#include "Serialization/SerializationFactory.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/_PropertyRowBuiltin.h"

#include "kdw/PropertyTree.h"
#include "kdw/_PropertyTreeColumnDrawer.h"
#include "kdw/PropertyRow.h"
#include "kdw/GradientDialog.h"

#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"
#include "Render/src/Gradients.h"
#include "Render/src/NParticle.h"

void drawGradient(HDC dc, const RECT& rect, const KeysColor& gradient, bool alphaOnly);

namespace kdw{


template<class GradientType, PositionStyle style, int type>
class PropertyRowGradient : public PropertyRowImpl<GradientType, PropertyRowGradient>{
public:
	static const bool Custom = true;
	PropertyRowGradient(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	PropertyRowGradient();
	WidgetPosition widgetPosition()const{ return WIDGET_POSITION_ROW; }
	int floorHeight() const{ return 20; }
	int minimalWidth() const{ return 0; }
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);

	bool onActivate(PropertyTree* tree);

	bool onMouseDown(PropertyTree* tree, Vect2i point, bool& changed);
	void onMouseMove(PropertyTree* tree, Vect2i point);
	void onMouseUp(PropertyTree* tree, Vect2i point);

	void handleMouse(Vect2i point, const Recti& floorRect);
};

template<class GradientType, PositionStyle style, int type>
void PropertyRowGradient<GradientType, style, type>::handleMouse(Vect2i point, const Recti& floorRect)
{
}
	
template<class GradientType, PositionStyle style, int type>
bool PropertyRowGradient<GradientType, style, type>::onMouseDown(PropertyTree* tree, Vect2i point, bool& changed)
{
	Recti rowRect;
	tree->getRowRect(this, rowRect);
	Recti floorRect;
	tree->drawer()->getSubRectFloor(this, rowRect, floorRect);

	if(floorRect.point_inside(point)){
		changed = onActivate(tree);
	}
	return false;
}

template<class GradientType, PositionStyle style, int type>
void PropertyRowGradient<GradientType, style, type>::onMouseMove(PropertyTree* tree, Vect2i point)
{
}

template<class GradientType, PositionStyle style, int type>
void PropertyRowGradient<GradientType, style, type>::onMouseUp(PropertyTree* tree, Vect2i point)
{
}

template<class GradientType, PositionStyle style, int type>
bool PropertyRowGradient<GradientType, style, type>::onActivate(PropertyTree* tree)
{
	GradientDialog dialog(tree, value(), style, type);
	if(dialog.showModal() == RESPONSE_OK){
		static_cast<KeysColor&>(value()) = dialog.get();
		tree->model()->rowChanged(this);
		return true;
	}
	return false;
}


template<class GradientType, PositionStyle style, int type>
PropertyRowGradient<GradientType, style, type>::PropertyRowGradient(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<GradientType, PropertyRowGradient>(object, size, name, nameAlt, typeName)
{
	height_ = ROW_DEFAULT_HEIGHT;
}

template<class GradientType, PositionStyle style, int type>
void PropertyRowGradient<GradientType, style, type>::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	if(true){ // с ClipRgn получается медленно 
		RECT rect = floorRect;
		int roundness = 7;
		::SelectObject(dc, GetStockObject(NULL_PEN));
		::SelectObject(dc, GetSysColorBrush(COLOR_3DSHADOW));
		::RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, roundness, roundness);
		
		HRGN region = CreateRoundRectRgn(rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, roundness, roundness - 2);
		::SelectClipRgn(dc, region);

		::FillRect(dc, &rect, GetSysColorBrush(COLOR_BTNFACE));
		::SelectObject(dc, GetSysColorBrush(COLOR_WINDOW));
		RoundRect(dc, rect.left + 1 + 1, rect.top + 1 + 1, rect.right - 1 + 1, rect.bottom - 1 + 1, roundness, roundness - 2);

		const GradientType& gradient = value();
		rect = floorRect;
		rect.top += 1;
		rect.bottom -= 1;
		drawGradient(dc, rect, gradient, (type & GRADIENT_STYLE_NO_COLOR) != 0);

		::SelectClipRgn(dc, 0);
		DeleteObject(region);
	}
	else{
		RECT rect = floorRect;
		rect.bottom -= 1;
		FillRect(dc, &rect, GetSysColorBrush(COLOR_3DSHADOW));
		const GradientType& gradient = value();
		rect.top += 1;
		//rect.bottom -= 1;
		rect.left += 1;
		rect.right -= 1;
		drawGradient(dc, rect, gradient, type & GRADIENT_STYLE_NO_COLOR != 0);
	}

}

template<class GradientType, PositionStyle style, int type>
PropertyRowGradient<GradientType, style, type>::PropertyRowGradient()
{
}

#define REGISTER_GRADIENT_EDITOR(type, position_mode, style) \
	typedef PropertyRowGradient<type, position_mode, style> PropertyRowGradient##type; \
	REGISTER_PROPERTY_ROW(type, PropertyRowGradient##type); 

DECLARE_SEGMENT(PropertyRowGradient)
REGISTER_GRADIENT_EDITOR(KeysColor,                   GRADIENT_POSITION_FLOAT, 0)
REGISTER_GRADIENT_EDITOR(TimeNoAlphaGradient,         GRADIENT_POSITION_TIME,  GRADIENT_STYLE_NO_ALPHA)
REGISTER_GRADIENT_EDITOR(BytePositionNoColorGradient, GRADIENT_POSITION_BYTE,  GRADIENT_STYLE_NO_COLOR)
REGISTER_GRADIENT_EDITOR(WaterGradient,               GRADIENT_POSITION_BYTE,  GRADIENT_STYLE_NO_COLOR)
REGISTER_GRADIENT_EDITOR(SkyGradient,                 GRADIENT_POSITION_TIME,  GRADIENT_STYLE_NO_ALPHA | GRADIENT_STYLE_CYCLED)
REGISTER_GRADIENT_EDITOR(SkyAlphaGradient,            GRADIENT_POSITION_TIME,  GRADIENT_STYLE_CYCLED)


// ---------------------------------------------------------------------------

}
