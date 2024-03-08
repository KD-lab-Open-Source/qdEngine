#ifndef __KDW_OBJECTS_TREE_H_INCLUDED__
#define __KDW_OBJECTS_TREE_H_INCLUDED__

#include <typeinfo>
#include <iterator>
#include "kdw/Tree.h"
#include "kdw/TreeModel.h"
#include "Handle.h"

#include "Serialization\SerializationTypes.h"
#include "Serialization\Serializer.h"

namespace kdw{
class Win32Proxy;

template<class T>
	struct RemovePointer{
		typedef T type;
	};

template<class T>
	struct RemovePointer<T*>{
		typedef T type;
	};

class TreeObject;
typedef std::vector<TreeObject*> TreeObjects;

class ObjectsTree;

class KDW_API TreeObject : public kdw::StringTreeRow{
public:
	explicit TreeObject(const char* text = "") : kdw::StringTreeRow(text) {}

	virtual void onSelect(ObjectsTree* tree);
	virtual void onRightClick(ObjectsTree* tree){}
	virtual bool onDoubleClick(ObjectsTree* tree){ return false; }

	virtual void onDrop(const TreeObjects& objects, ObjectsTree* tree) {}
	virtual bool canBeDragged() const{ return false; }
	virtual bool onDragOver(const TreeObjects& objects, const ObjectsTree* tree) const{ return false; }
	virtual void onEndDrag(const TreeObjects& objects, ObjectsTree* tree){}

	virtual Serializer getSerializer(const char* name = "", const char* nameAlt = "") { return Serializer(); }

	void remove();

	TreeObject* parent() { return safe_cast<TreeObject*>(__super::parent()); }
	const TreeObject* parent() const{ return safe_cast<const TreeObject*>(__super::parent()); }
	void showCheckBox(bool show);
	void setCheck(bool checked);

	void expandParents();
	void focus();

    virtual ~TreeObject(){}

	bool checked_;
private:
};

class KDW_API ObjectsTree : public kdw::Tree{
public:
	typedef kdw::StringTreeModel Model;
	ObjectsTree();
	// virtuals:
	bool onRowLMBDoubleClick(kdw::TreeRow* row, const Recti& rowRect, Vect2i point);
	void onRowSelected(kdw::TreeRow* row);
	void onRowDeselected(kdw::TreeRow* row);
	void onRowRMBDown(kdw::TreeRow* row, const Recti& rowRect, Vect2i point);
	bool onRowKeyDown(kdw::TreeRow* row, sKey key);
	// ^^^

	virtual void selectObject(TreeObject* object, bool select = true, bool update = true, bool unsure = true);
	TreeObject* focusedObject(){ return safe_cast<TreeObject*>(model()->focusedRow()); }
	TreeObject* root() { return safe_cast<TreeObject*>(model()->root()); }
	const TreeObject* root() const{ return safe_cast<TreeObject*>(model()->root()); }
	typedef sigslot::signal0 SignalSelectionChanged;
	SignalSelectionChanged& signalSelectionChanged(){ return signalSelectionChanged_; }
protected:
	sigslot::signal0 signalSelectionChanged_;
};

template<class Type, class Base = TreeObject>
class TreeObjectImpl : public Base {
public:
	virtual const std::type_info& getTypeInfo () const {
		return typeid(Type);
	}
    TreeObjectImpl (const Type& _data, const char* _name) : data_ (const_cast<Type&> (_data)) {
        name_ = _name;
    }

    void* getPointer()const { return reinterpret_cast<void*>(&data_); }
	Serializer getSerializer(const char* name = "", const char* nameAlt = "") {
		return Serializer(data_, name, nameAlt);
	}
protected:
    Type& data_;
};

}

#endif
