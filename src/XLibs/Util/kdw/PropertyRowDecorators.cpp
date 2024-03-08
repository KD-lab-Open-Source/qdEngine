#include "StdAfx.h"
#include "PropertyRow.h"
#include "Serialization/Decorators.h"
#include "Serialization/SerializationFactory.h"
#include "kdw/PropertyTree.h"
#include "kdw/_PropertyTreeColumnDrawer.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/Win32/Window.h"
#include "kdw/Win32/Drawing.h"

namespace kdw{

class PropertyRowButton : public PropertyRowImpl<ButtonDecorator, PropertyRowButton>{
public:
	PropertyRowButton();
	PropertyRowButton(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	bool onMouseDown(PropertyTree* tree, Vect2i point, bool& changed);
	void onMouseMove(PropertyTree* tree, Vect2i point);
	void onMouseUp(PropertyTree* tree, Vect2i point);
	bool onActivate(PropertyTree* tree);
	int floorHeight() const{ return 3; }
protected:
	bool underMouse_;
};

PropertyRowButton::PropertyRowButton(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<ButtonDecorator, PropertyRowButton>(object, size, name, nameAlt, typeName)
, underMouse_(false)
{
}

PropertyRowButton::PropertyRowButton()
: underMouse_(false)
{
}
	

void PropertyRowButton::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow)
{
	//DrawFrameControl(dc, const_cast<RECT*>(&widgetRect), DFC_BUTTON, value().pressed ? DFCS_PUSHED : DFCS_BUTTONPUSH);
	bool pressed = underMouse_ && value();
	RECT buttonRect = widgetRect;
	//buttonRect.top -= 1;
	buttonRect.left -= 1;
	buttonRect.bottom += 2;

	FillRect(dc, &buttonRect, GetSysColorBrush(COLOR_BTNFACE));
	DrawEdge(dc, &buttonRect, pressed ? EDGE_SUNKEN : EDGE_RAISED, BF_RECT);
	SetBkMode(dc, TRANSPARENT);
	const char* text = value().text ? value().text : nameAltUndecorated();

	
	RECT textRect = widgetRect;
	textRect.left += 2;
	textRect.right -= 2;
	if(pressed){
		textRect.left += 1;
		textRect.right += 1;
		textRect.top += 1;
		textRect.bottom += 1;
	}

	HGDIOBJ oldFont = SelectObject(dc, Win32::defaultFont());
	DrawText(dc, text, strlen(text), &textRect, DT_CENTER |DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	::SelectObject(dc, oldFont);	
}

bool PropertyRowButton::onMouseDown(PropertyTree* tree, Vect2i point, bool& changed)
{
	Recti rowRect;
	tree->getRowRect(this, rowRect);
	Recti widgetRect;
	tree->drawer()->getSubRectWidget(this, rowRect, widgetRect);
	widgetRect.height(widgetRect.height() + 2);
	if(widgetRect.point_inside(point)){
		value().pressed = !value().pressed;
		underMouse_ = true;
		tree->redraw();
		return true;
	}
	return false;
}

void PropertyRowButton::onMouseMove(PropertyTree* tree, Vect2i point)
{
	Recti rowRect;
	tree->getRowRect(this, rowRect);
	Recti widgetRect;
	tree->drawer()->getSubRectWidget(this, rowRect, widgetRect);
	widgetRect.height(widgetRect.height() + 2);
	bool underMouse = widgetRect.point_inside(point);
	if(underMouse != underMouse_){
		underMouse_ = underMouse;
		tree->redraw();
	}
}

void PropertyRowButton::onMouseUp(PropertyTree* tree, Vect2i point)
{
	Recti rowRect;
	tree->getRowRect(this, rowRect);
	Recti widgetRect;
	widgetRect.height(widgetRect.height() + 2);
	tree->drawer()->getSubRectWidget(this, rowRect, widgetRect);
	if(widgetRect.point_inside(point)){
		tree->model()->rowChanged(this);
    }
	else{
		value().pressed = false;
		tree->redraw();
	}
}

bool PropertyRowButton::onActivate(PropertyTree* tree)
{
	value().pressed = false;
	return true;
}

DECLARE_SEGMENT(PropertyRowDecorators)
REGISTER_PROPERTY_ROW(ButtonDecorator, PropertyRowButton);

// ------------------------------------------------------------------------------------------

class PropertyRowHLine : public PropertyRowImpl<HLineDecorator, PropertyRowHLine>{
public:
	PropertyRowHLine();
	PropertyRowHLine(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
};

PropertyRowHLine::PropertyRowHLine(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<HLineDecorator, PropertyRowHLine>(object, size, name, nameAlt, typeName)
{
}

PropertyRowHLine::PropertyRowHLine()
{
}
	

void PropertyRowHLine::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow)
{
	int halfHeight = widgetRect.top + (widgetRect.bottom - widgetRect.top) / 2;
	RECT hlineRect = { widgetRect.left, halfHeight - 1, widgetRect.right, halfHeight + 1 };

	DrawEdge(dc, &hlineRect, BDR_SUNKENOUTER, BF_RECT);
}

REGISTER_PROPERTY_ROW(HLineDecorator, PropertyRowHLine);

// ------------------------------------------------------------------------------------------

class PropertyRowNot : public PropertyRowImpl<NotDecorator, PropertyRowNot>{
public:
	PropertyRowNot();
	PropertyRowNot(void* object, int size, const char* name, const char* nameAlt, const char* typeName);
	bool onActivateIcon(PropertyTree* tree);
	bool onActivate(PropertyTree* tree);
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	bool hasIcon() const{ return true; }
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_ROW_AUTOHIDE; }
};

PropertyRowNot::PropertyRowNot(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<NotDecorator, PropertyRowNot>(object, size, name, nameAlt, typeName)
{
}

PropertyRowNot::PropertyRowNot()
{
}
	
bool PropertyRowNot::onActivateIcon(PropertyTree* tree)
{
	value() = !value();
	tree->model()->rowChanged(this);
	return true;
}

bool PropertyRowNot::onActivate(PropertyTree* tree)
{
	return onActivateIcon(tree);
}

void PropertyRowNot::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow)
{
	Win32::drawNotCheck(dc, iconRect, value());
}

REGISTER_PROPERTY_ROW(NotDecorator, PropertyRowNot);

}
