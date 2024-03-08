#include "StdAfx.h"
#include "Serialization/SerializationFactory.h"
#include "kdw/PropertyRow.h"
#include "kdw/PropertyTreeModel.h"

#include "kdw/ColorComboBox.h"

namespace kdw{

class AutoDropColorComboBox : public ColorComboBox{
public:
	void show(){
		ColorComboBox::show();
		showDropDown();
	}
};

class PropertyRowComboListColor : public PropertyRowImpl<ComboListColor, PropertyRowComboListColor>{
public:
	enum { Custom = true };
	PropertyRowComboListColor();
	PropertyRowComboListColor(void* object, int size, const char* name, const char* nameAlt, const char* typeName);

	// virtuals:
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	PropertyRowWidget* createWidget(PropertyTreeModel* model);
	std::string valueAsString() const;
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	bool assignTo(void* object, int size){
		reinterpret_cast<ComboListColor*>(object)->setIndex(value().index());
		return true;
	}
	// ^^^
};

DECLARE_SEGMENT(PropertyRowComboListColor)
REGISTER_PROPERTY_ROW(ComboListColor, PropertyRowComboListColor);

class PropertyRowWidgetComboListColor : public PropertyRowWidget, public sigslot::has_slots{
public:
	PropertyRowWidgetComboListColor(PropertyRowComboListColor* row, PropertyTreeModel* model)
	: PropertyRowWidget(row, model)
	, comboBox_(new AutoDropColorComboBox())
	{
		comboBox_->set(row->value());
		comboBox_->signalEdited().connect(this, &PropertyRowWidgetComboListColor::onChange);
	}
	~PropertyRowWidgetComboListColor()
	{
	}

	void onChange(){
		PropertyRowComboListColor* row = safe_cast<PropertyRowComboListColor*>(this->row());
		ComboListColor value = row->value();
		value = value.comboList().at(comboBox_->selectedIndex());
		row->setValue(value);
		model()->rowChanged(row);
	}
	void commit(){}
	Widget* actualWidget() { return comboBox_; }
protected:
	ShareHandle<ColorComboBox> comboBox_;
};

PropertyRowComboListColor::PropertyRowComboListColor()
{
}

PropertyRowComboListColor::PropertyRowComboListColor(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<ComboListColor, PropertyRowComboListColor>(object, size, name, nameAlt, typeName)
{
}

void PropertyRowComboListColor::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	// COPY_PASTE
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

PropertyRowWidget* PropertyRowComboListColor::createWidget(PropertyTreeModel* model)
{
	return readOnly_ ? 0 : new PropertyRowWidgetComboListColor(this, model);
}

std::string PropertyRowComboListColor::valueAsString() const
{
	XBuffer buf;
	buf <= value().value().r < ", "
		 <= value().value().g < ", "
		 <= value().value().b < ", "
		 <= value().value().a;
	return static_cast<const char*>(buf);
}

}