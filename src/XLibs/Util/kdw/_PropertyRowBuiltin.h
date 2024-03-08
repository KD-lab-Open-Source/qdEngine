#ifndef __KDW_BUILTIN_PROPERTY_ROW_H_INCLUDED__
#define __KDW_BUILTIN_PROPERTY_ROW_H_INCLUDED__

#include "kdw/PropertyRow.h"
#include "kdw/Entry.h"
#include "kdw/Win32/Drawing.h"

class EnumDescriptor;

namespace kdw{

class PropertyTreeModel;

class PropertyRowContainer : public PropertyRow, public sigslot::has_slots{
public:
	enum { Custom = false };
	PropertyRowContainer(const char* name, const char* nameAlt, const char* typeName, const char* elementTypeName, bool readOnly);
	bool isContainer() const{ return true; }
	bool onContextMenu(PopupMenuItem& item, PropertyTree* tree);
	bool onKeyDown(PropertyTree* tree, sKey key);

	void onMenuAppendElement(PropertyTree* tree);
	void onMenuAppendPointerByIndex(int index, PropertyTree* model);
	void onMenuClear(PropertyTreeModel* model);

	void onMenuChildInsertBefore(PropertyRow* child, PropertyTree* model);
	void onMenuChildRemove(PropertyRow* child, PropertyTreeModel* model);

	PropertyRow* clone() const{
		return cloneChildren(new PropertyRowContainer(name_, nameAlt_, typeName_, elementTypeName_, readOnly_), this);
	}
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	bool isStatic() const{ return false; }
	int minimalWidth() const{ return 50; }
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_ROW_AUTOHIDE; }

	PropertyRow* defaultRow(PropertyTreeModel* model);
	const PropertyRow* defaultRow(const PropertyTreeModel* model) const;
	void serializeValue(Archive& ar);

	const char* elementTypeName() const{ return elementTypeName_; }
	bool hasFixedSize() const{ return fixedSize_; }
protected:
	bool fixedSize_;
	const char* elementTypeName_;
};

class PropertyRowEnum : public PropertyRow{
public:
	enum{ Custom = false };
	PropertyRowEnum(const char* name = "", const char* nameAlt = "", int value = 0, const EnumDescriptor* descriptor = 0);
	int value() const;
	void setValue(int index);
	void setVisibleIndex(int index);
	bool assignTo(void* object, int size);
	bool isStatic() const{ return false; }
	const EnumDescriptor* descriptor() { return descriptor_; }
	PropertyRowWidget* createWidget(PropertyTreeModel* model);

//	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	PropertyRow* clone() const{ return new PropertyRowEnum(name_, nameAlt_, value_, descriptor_);	}
	void serialize(Archive& ar);

	void serializeValue(Archive& ar);
	std::string valueAsString() const;
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	virtual int minimalWidth() const{ return 100; }
protected:
	int value_;
	const EnumDescriptor* descriptor_;
};

class PropertyRowBitVector : public PropertyRow{
public:
	enum{ Custom = false };
	PropertyRowBitVector(const char* name = "", const char* nameAlt = "", int flags = 0, const EnumDescriptor* descriptor = 0);
	const char* value() const{ return value_.c_str(); }
	const char* valueAlt() const{ return valueAlt_.c_str(); }
	void setValueAlt(const char* value);
	int flags() const { return flags_; }
	void serializeValue(Archive& ar);
	bool assignTo(void* object, int size);
	std::string valueAsString() const{ return valueAlt_; }
	bool isStatic() const{ return false; }
	const EnumDescriptor* descriptor() { return descriptor_; }
	PropertyRowWidget* createWidget(PropertyTreeModel* model);

	//void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRowState state);
	PropertyRow* clone() const{
		return new PropertyRowBitVector(name_, nameAlt_, flags_, descriptor_);
	}
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	virtual int minimalWidth() const{ return 100; }
protected:
	std::string value_;
	std::string valueAlt_;
	int flags_;
	const EnumDescriptor* descriptor_;
};

class PropertyRowPointer : public PropertyRow, public sigslot::has_slots{
public:
	enum { Custom = false };
	PropertyRowPointer(const char* name = "", const char* nameAlt = "", const char* typeName = "",
					   const char* baseName = "", const char* derivedName = "", const char* derivedNameAlt = "");

	WidgetPosition widgetPosition() const{ return /*(parent() && parent()->isContainer() && digest()[0] == '\0') ? WIDGET_POSITION_ROW : */WIDGET_POSITION_COLUMN; }

	int factoryIndex();
	const char* derivedName() const{ return derivedName_; }
	const char* derivedNameAlt() const{ return derivedNameAlt_; }
	bool onContextMenu(PopupMenuItem &root, PropertyTree* tree);
	void onMenuCreateByIndex(int index, PropertyTree* tree);
	bool isStatic() const{ return false; }
	bool isPointer() const{ return true; }
	int minimalWidth() const{ return 100; }
	std::string valueAsString() const;
	PropertyRow* clone() const{
		return cloneChildren(new PropertyRowPointer(name_, nameAlt_, typeName_, typeName_, derivedName_, derivedNameAlt_), this);
	}
	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	void serializeValue(Archive& ar);
protected:
	void updateTitle();


	const char* derivedName_;
	const char* derivedNameAlt_;

	std::string title_;
};


class PropertyRowBool : public PropertyRowImpl<bool, PropertyRowBool>{
public:
	enum { Custom = false };
	PropertyRowBool(const char* name = "", const char* nameAlt = "", bool value = false);
	PropertyRowBool(void* object, int size, const char* name, const char* nameAlt, const char* typeName); // понадобилс€ из за PropertyRowImpl

	void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	bool assignTo(void* object, int size);
	int minimalWidth() const{ return 15; }
	bool onActivateIcon(PropertyTree* tree);
	bool onActivate(PropertyTree* tree);
	std::string valueAsString() const
	{
		return value_ ? nameAlt() : "";
	//	return value_ ? TRANSLATE("ƒа") : TRANSLATE("Ќет");
	}
	bool hasIcon() const{ return true; }
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_ROW_AUTOHIDE; }
	// ^^^
protected:
};

class PropertyRowString : public PropertyRowImpl<std::string, PropertyRowString>{
public:
	enum { Custom = false };
	PropertyRowString(const char* name = "", const char* nameAlt = "", const std::string& value = std::string());
	PropertyRowString(void* object, int size, const char* name, const char* nameAlt, const char* typeName); // понадобилс€ из за PropertyRowImpl
	bool assignTo(void* object, int size);
	PropertyRowWidget* createWidget(PropertyTreeModel* model);
	std::string valueAsString() const;
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	// ^^^
};

class PropertyRowComboListString : public PropertyRowImpl<ComboListString, PropertyRowComboListString>{
public:
	enum { Custom = false };
	PropertyRowComboListString(const char* name = "", const char* nameAlt = "", const ComboListString& value = ComboListString());
	PropertyRowComboListString(void* object, int size, const char* name, const char* nameAlt, const char* typeName); // понадобилс€ из за PropertyRowImpl

	// virtuals:
	//void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRowState state);
	PropertyRowWidget* createWidget(PropertyTreeModel* model);
	std::string valueAsString() const;
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	bool assignTo(void* object, int size){
		*reinterpret_cast<ComboListString*>(object) = value().value();
		return true;
	}
	// ^^^
};

// используетс€ widget-ом редактриовани€
class PropertyRowNumericInterface{
public:
	virtual void setValueFromString(const char* str) = 0;
	virtual std::string valueAsString() const = 0;
};


class PropertyRowWidgetNumeric : public PropertyRowWidget, public sigslot::has_slots{
public:
    PropertyRowWidgetNumeric(PropertyRow* row, PropertyTreeModel* mode, PropertyRowNumericInterface * numeric);
	~PropertyRowWidgetNumeric(){}

	void onChange();
	void commit();

	Widget* actualWidget() { return entry_; }
protected:
	ShareHandle<Entry> entry_;
	PropertyRowNumericInterface* numeric_;
};


template<class T>
std::string numericAsString(T value)
{
	XBuffer buf;
	buf.SetDigits(6);
	buf <= value;
	return static_cast<const char*>(buf);
}



template<class Derived, class Default>
struct SelectNumericDerived{
	typedef Derived Type;
};

template<class Default>
struct SelectNumericDerived<Unspecified_Derived_Argument, Default>{
	typedef Default Type;
};

template<class Type, class _Derived = Unspecified_Derived_Argument >
class PropertyRowNumeric : public PropertyRowImpl<Type, typename SelectNumericDerived<_Derived, PropertyRowNumeric>::Type>, public PropertyRowNumericInterface{
public:
	enum { Custom = false };
	typedef typename SelectNumericDerived<_Derived, PropertyRowNumeric>::Type Derived;
	PropertyRowNumeric(const char* name = "", const char* nameAlt = "", Type value = Type())
	: PropertyRowImpl<Type, Derived>((void*)(&value), sizeof(Type), name, nameAlt, typeid(Type).name())
	{
	}
	PropertyRowNumeric(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
	: PropertyRowImpl<Type, Derived>(object, size, name, nameAlt, typeName)
	{
	}
	int minimalWidth() const{ return 60; }
	PropertyRowWidget* createWidget(PropertyTreeModel* model){
		return readOnly_ ? 0 : new PropertyRowWidgetNumeric(this, model, this);
	}
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }

	void setValueFromString(const char* str){
		value_ = atof(str);
	}
	std::string valueAsString() const{
		std::string val = numericAsString(Type(value_));
		return val.c_str();
	}

};

}

#endif
