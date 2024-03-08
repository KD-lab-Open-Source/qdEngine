#ifndef __KDW_PROPERTY_ROW_H_INCLUDED__
#define __KDW_PROPERTY_ROW_H_INCLUDED__

#include "kdw/Widget.h"
#include "kdw/TreeModel.h"
#include "Serialization/Factory.h"
#include "Serialization/LibraryBookmark.h"
#include "Serialization/LibrariesManager.h"
#include "kdw/ConstStringList.h"
#include "sKey.h"
#include "kdw/Win32/Types.h"

namespace kdw{


class PropertyTree;
class PropertyRow;
class TreeModel;
class PropertyTreeModel;


class PropertyRowWidget : public PolymorphicBase{
public:
	PropertyRowWidget(PropertyRow* row, PropertyTreeModel* model)
	: dataRow_(row)
	, locationRow_(row)
	, model_(model)
	{
	}
	virtual ~PropertyRowWidget();
	virtual Widget* actualWidget() { return 0; }
	virtual void commit() = 0;
	PropertyRow* row() { return dataRow_; }
	void setLocationRow(PropertyRow* row){ locationRow_ = row; }
	PropertyRow* locationRow() { return locationRow_; }
	PropertyTreeModel* model() { return model_; }
protected:
	PropertyRow* dataRow_;
	PropertyRow* locationRow_;
	PropertyTreeModel* model_;
};

enum PropertyRowState{
	INACTIVE,
	FOCUSED,
	HOVERED,
	ACTIVATED,
};

class PopupMenuItem;

class KDW_API PropertyRow: public TreeRow{
public:
	static const int ROW_DEFAULT_HEIGHT = 19; // FIXME: DEFAULT_HEIGHT занят дефайном в TreeListCtrl-е
	static const bool Custom = true;
	//enum { Custom = true; }
	enum WidgetPosition{
		WIDGET_POSITION_COLUMN,
		WIDGET_POSITION_FIXED,
		WIDGET_POSITION_ROW,
		WIDGET_POSITION_ROW_AUTOHIDE
	};
	PropertyRow(const char* name = "", const char* nameAlt = "", const char* typeName = "");
	PropertyRow(void* object, int size, const char* name, const char* nameAlt, const char* typeName);

	const char* name() const{ return name_; }
	void setName(const char* name) { name_ = name; }
	const char* nameAlt() const{ return nameAlt_; }
	const char* nameAltUndecorated() const;
	void setNameAlt(const char* nameAlt);
	const char* typeName() const{ return typeName_; }
	void setTypeName(const char* typeName) { typeName_ = typeName; }

	virtual int textIndent() const{ return ROW_DEFAULT_HEIGHT; }
	PropertyRow* find(const char* name, const char* nameAlt, const char* typeName, bool skipUpdated = false);
	const PropertyRow* find(const char* name, const char* nameAlt, const char* typeName, bool skipUpdated = false) const;
	void intersect(const PropertyRow* row);

	template<class T>
	bool assignTo(T& object){
		return assignTo(reinterpret_cast<void*>(&object), sizeof(T));
	}
	virtual bool assignTo(void* object, int size) { return false; }
	virtual std::string valueAsString() const;

	virtual int minimalWidth() const{ return (pulledRow() || pulledContainer()) ? 100 : 0; }
	virtual const LibraryLocation* location() const{ return 0; }
	
	void adjust(int height, const Tree* tree);
	virtual int floorHeight() const{ return 0; }

	virtual void childrenUpdated();
	virtual WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	virtual bool hasIcon() const{ return false; }
	virtual void redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& lineRect, PropertyRow* hostRow);
	virtual PropertyRowWidget* createWidget(PropertyTreeModel* model) { return 0; }
	
	virtual bool isContainer() const{ return false; }
	virtual bool isPointer() const{ return false; }

	virtual bool isLeaf() const{ return false; }
	virtual bool isStatic() const{ return pulledRow_ == 0 && pulledContainer_ == 0; }
	virtual bool activateOnAdd() const{ return false; }
	bool canBeToggled() const;
	bool canBeDragged() const;
	bool canBeDroppedOn(const TreeRow* row, const TreeRow* beforeChild, const Tree* tree, bool direct) const;
	void dropInto(TreeRow* _destination, TreeRow* beforeChild, Tree* tree);

	virtual bool onSelect(PropertyTreeModel* model) { return true; }
	virtual bool onActivateIcon(PropertyTree* tree) { return false; }
	
	// возвращает изменилось ли значение строчки
	virtual bool onActivate(PropertyTree* tree) { return onActivateWidget(tree, this); }
	virtual bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow);
	
	// возваращает, нужно ли capture-ить мышь
	virtual bool onMouseDown(PropertyTree* tree, Vect2i point, bool& changed) { return false; }

	virtual bool onKeyDown(PropertyTree* tree, sKey key);
	virtual void onMouseMove(PropertyTree* tree, Vect2i point) {}
	virtual void onMouseUp(PropertyTree* tree, Vect2i point) {}

	PropertyRow* parent() { return static_cast<PropertyRow*>(TreeRow::parent()); }
	const PropertyRow* parent() const{ return static_cast<const PropertyRow*>(TreeRow::parent()); }
	
	virtual bool onContextMenu(PopupMenuItem &root, PropertyTree* tree);

	virtual void serializeValue(Archive& ar) {}
	virtual void serialize(Archive& ar);
	void setUpdated(bool updated) { updated_ = updated; }
	bool updated() const{ return updated_; }
	bool readOnly() const{ return readOnly_; }
	bool inDigest() const{ return inDigest_; }
	bool multiValue() const{ return multiValue_; }
	void setMultiValue(bool multiValue){ multiValue_ = multiValue; }
	bool fullRow() const{ return fullRow_; }
	void setFullRow(bool fullRow){ fullRow_ = fullRow; }

	// pulledRow - это та, что "вытягивается" на уровень вверх
	// (по символу '^' в начале nameAlt)
	void setPulledRow(PropertyRow* row){ pulledRow_ = row; }
	void setPulledContainer(PropertyRow* container){ pulledContainer_ = container; }

	bool pulledUp() const{ return pulledUp_; }
	PropertyRow* pulledRow() { return pulledRow_; }
	const PropertyRow* pulledRow() const{ return pulledRow_; }
	PropertyRow* pulledContainer() { return pulledContainer_; }
	const PropertyRow* pulledContainer() const{ return pulledContainer_; }

	PropertyRow* cloneChildren(PropertyRow* result, const PropertyRow* source) const;
	virtual PropertyRow* clone() const{
		return cloneChildren(new PropertyRow(name_, nameAlt_, typeName_), this);
	}

	PropertyRow* addBefore(PropertyRow* row, PropertyRow* before);

	static void setConstStrings(ConstStringList* constStrings){
		constStrings_ = constStrings;
	}

	const char* digest() const{ return digest_.c_str(); }
	void digestReset();
	void digestAppend(const char* text);
	std::string rowPath(const char* separator) const;

	bool operator==(const PropertyRow& rhs) const;
	bool operator!=(const PropertyRow& rhs) const{
		return !operator==(rhs);
	}
protected:
	bool updated_;
	bool readOnly_;
	bool fullRow_;
	bool pulledUp_;
	bool inDigest_;
	bool multiValue_;
	ShareHandle<PropertyRow> pulledRow_;
	ShareHandle<PropertyRow> pulledContainer_;
	std::string digest_;

	const char* name_;
	const char* nameAlt_;
	const char* typeName_;

	static ConstStringList* constStrings_;
};

template<bool value>
struct StaticBool{
	enum { Value = value };
};

class KDW_API PropertyRowArg{
public:
	static void set(void* object, int size, const char* name, const char* nameAlt, const char* typeName){
		object_ = object;
		size_ = size;
		name_ = name;
		nameAlt_ = nameAlt;
		typeName_ = typeName;
	}


	template<class Derived>
	static kdw::PropertyRow* createRow(StaticBool<true> custom)
	{
		PropertyRow* result;
		if(object_)
			result = new Derived(object_, size_, name_, nameAlt_, typeName_);
		else
			result = new Derived();
		set(0, 0, 0, 0, 0);
		return result;
	}

	template<class Derived>
	static PropertyRow* createRow(StaticBool<false> custom)
	{
		PropertyRow* result = new Derived();
		//xassert(object_ == 0);
		set(0, 0, 0, 0, 0);
		return result;
	}

	template<class Derived>
	static PropertyRow* createArg(){ 
		return createRow<Derived>(StaticBool<Derived::Custom>());
		/*
		if(object_){
			kdw::PropertyRow* result = new Derived(object_, size_, name_, nameAlt_, typeName_);
			set(0, 0, 0, 0, 0);
			return result;
		}
		else
			return new Derived();
		*/
	}

protected:
	static void* object_;
	static int size_;
	static const char* name_;
	static const char* nameAlt_;
	static const char* typeName_;
};

}


namespace kdw
{

class PropertyRowFactory : public Factory<std::string, PropertyRow, PropertyRowArg>{
public:
	// COPYPASTE from Factory.h
	template<class Derived>
	struct Creator : CreatorBase
	{
		Creator() {
		}
		Creator(const char* id){
			instance().add(id, *this);
		}
		PropertyRow* create() const{
			return instance().createArg<Derived>();
		}
		const char* typeName() const{ 
			return typeid(Derived).name(); 
		}

		void* vtable() const {
			static PropertyRow* object = create();
			return object ? *(int**)object : 0;
		}
	};
	// ^^^

	void buildMap();
	bool addReference(const char* referenceTypeName, LibraryInstanceFunc instanceFunc);
	const char* findReferencedLibrary(const char* referenceTypeName) const;

	static PropertyRowFactory& instance(){
		return Singleton<PropertyRowFactory>::instance();
	}
protected:
	typedef StaticMap<std::string, LibraryInstanceFunc> InstanceFuncByReferenceType;
	InstanceFuncByReferenceType instanceFuncByReferenceType_;
	typedef StaticMap<std::string, std::string> LibraryNameByReferenceType;
	LibraryNameByReferenceType libraryNameByReferenceType_;
};

template<>
struct FactorySelector<kdw::PropertyRow>{
	typedef PropertyRowFactory Factory;
};

#define REGISTER_PROPERTY_ROW(DataType, RowType) \
	REGISTER_CLASS_IN_FACTORY(kdw::PropertyRowFactory, typeid(DataType).name(), RowType); \
	REGISTER_CLASS(kdw::PropertyRow, RowType, #DataType);


struct Unspecified_Derived_Argument {};
template<class Type, class Derived = PropertyRowImpl<Type, Unspecified_Derived_Argument> >
class PropertyRowImpl : public PropertyRow{
public:
	PropertyRowImpl()
	: PropertyRow("", "", "")
	{
	}
	PropertyRowImpl(const char* name, const char* nameAlt, const Type& value, const char* typeName = 0)
	: PropertyRow(name, nameAlt, typeName ? typeName : typeid(Type).name())
	, value_(value)
	{
	}
	PropertyRowImpl(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
	: PropertyRow(name, nameAlt, typeName)
	, value_(*reinterpret_cast<Type*>(object))
	{
		xassert(sizeof(Type) == size);
	}
	bool assignTo(void* object, int size){
		*reinterpret_cast<Type*>(object) = value();
		return true;
	}
	bool isLeaf() const{ return true; }
	bool isStatic() const{ return false; }
	void setValue(const Type& value) { value_ = value; }
	Type& value() { return value_; }
	const Type& value() const{ return value_; }
	WidgetPosition widgetPosition() const{ return WIDGET_POSITION_COLUMN; }
	int minimalWidth() const{ return 100; }
	void serializeValue(Archive& ar){
		ar.serialize(value_, "value", "Value");
	}
	PropertyRow* clone() const{
		// если здесь возникла ошибка "error C2440: 'static_cast' : cannot convert from..."
		// скорее всего вы забыли указать Derived аргумент при наследовании от PropertyRowImpl
		return cloneChildren(new Derived((void*)(&value_), sizeof(value_), name_, nameAlt_, typeName_), static_cast<const Derived* const>(this));
	}
protected:
	Type value_; 
};

}

#endif
