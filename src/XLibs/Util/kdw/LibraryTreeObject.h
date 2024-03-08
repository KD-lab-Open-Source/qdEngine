#ifndef __LIBRARY_TREE_OBJECT_H_INCLUDED__
#define __LIBRARY_TREE_OBJECT_H_INCLUDED__

#include "XTL/StaticMap.h"
#include "Serialization/Serializer.h"
#include "Serialization/LibraryBookmark.h"
#include "kdw/ObjectsTree.h"
#include "kdw/PopupMenu.h"

class EditorLibraryInterface;
class LibraryBookmark;

namespace kdw{
class PopupMenuItem;

class LibraryCustomEditor;
class LibraryElementTreeObject;
class LibraryGroupTreeObject;
class LibraryTree;

class LibraryCustomEditor : public ShareHandleBase{
public:
	LibraryCustomEditor(EditorLibraryInterface* library);
	virtual ~LibraryCustomEditor();

	virtual LibraryElementTreeObject* createTreeObject(const char* elementName);
	virtual LibraryGroupTreeObject* createGroupTreeObject(const char* groupName);
	virtual Serializer serializeableByBookmark(const LibraryBookmark& bookmark, bool editOnly);

	EditorLibraryInterface* library() { return library_; }
protected:
	EditorLibraryInterface* library_;
};

// ----------------------------------------------------------------------------

class LibraryTreeObject : public TreeObject{
public:
	LibraryTreeObject(LibraryCustomEditor* customEditor = 0)
	: customEditor_(customEditor){}

	void onSelect(ObjectsTree* tree);
	virtual void rebuild() {}
	virtual LibraryTreeObject* onPropertyChanged(){ return this; }
    virtual bool isGroup() const{ return false; }
    virtual bool isElement() const{ return false; }
    virtual bool isBookmark() const { return false; }


	virtual bool onKeyDown(unsigned int key) { return false; }

	void* getPointer() const { return (void*)(this); };
	const type_info& getTypeInfo() const{ return typeid(this); }
	Serializer getSerializer(const char* name = "",const char* nameAlt = ""){ return Serializer(); }
	virtual LibraryTreeObject* subElementByName(const char* subElementName = "") { return 0; }
	const LibraryBookmark bookmark() const{ return bookmark_; }
protected:
	LibraryBookmark bookmark_;
	ShareHandle<LibraryCustomEditor> customEditor_;
};

class LibraryElementTreeObject : public LibraryTreeObject, public sigslot::has_slots
{
public:
    bool isElement() const{ return true; }
    LibraryElementTreeObject(LibraryCustomEditor* customEditor, const char* elementName);

	virtual void onMenuConstruction(kdw::PopupMenuItem& root, ObjectsTree* tree) {}
	virtual void rebuild() {}
	LibraryTreeObject* onPropertyChanged();
    void onRightClick(ObjectsTree* tree);
	virtual void onMenuDelete(LibraryTree* tree);
	void onMenuRename(LibraryTree* tree);
	void onMenuSearch(LibraryTree* tree);
	void onMenuCopy(LibraryTree* tree);
	void onMenuPaste(LibraryTree* tree);
	bool onKeyDown(sKey key, ObjectsTree* tree);

	bool onDoubleClick(ObjectsTree* tree) { return false; };
	bool canBeDragged() const;
	bool canBeDroppedOn(const TreeRow* _row, const TreeRow* beforeChild, const Tree* tree, bool direct) const;
	void dropInto(TreeRow* _destination, TreeRow* _beforeChild, Tree* tree);
    void onSelect(ObjectsTree* tree);

	const char* elementName() const{ return elementName_.c_str(); }
	void setElementName(const char* elementName);
	Serializer getSerializer(const char* name, const char* nameAlt);
private:

	std::string elementName_;
	int elementIndex_;
};/*}}}*/

class LibraryGroupTreeObject : public LibraryTreeObject, public sigslot::has_slots{/*{{{*/
public:
	LibraryGroupTreeObject(LibraryCustomEditor* customEditor, const char* groupName);
	virtual void onMenuConstruction(kdw::PopupMenuItem& root, ObjectsTree* tree) {}

	void onRightClick(ObjectsTree* tree);
	std::string fullGroupName() const;
	void onMenuCreateGroup(LibraryTree* tree);
	virtual void onMenuCreate(LibraryTree* tree);
	void onSelect(ObjectsTree* tree);
	bool canBeDragged() const;
	bool isGroup() const{ return true; }
	const char* groupName() const { return groupName_.c_str(); }
protected:
	std::string groupName_;
};/*}}}*/

class LibraryCustomEditorFactory{/*{{{*/
public:
    typedef LibraryCustomEditor Product;

    typedef EditorLibraryInterface&(*LibraryInstanceFunc)(void);

    struct CreatorBase{
        virtual Product* create() const = 0;
    };

	template<class Derived>
	struct Creator :  CreatorBase{
		Creator() : libraryInstanceFunc_(0) {}
		Creator(LibraryInstanceFunc libraryInstanceFunc)
        : libraryInstanceFunc_(libraryInstanceFunc)
        {
			LibraryCustomEditorFactory::instance().queueRegistration(*this, libraryInstanceFunc);
		}
        Product* create() const{
            return new Derived(&libraryInstanceFunc_());
        }
        LibraryInstanceFunc libraryInstanceFunc_;
	};


	void add(const char* libraryName, CreatorBase& creator_op);
	void queueRegistration(CreatorBase& creator_op, LibraryInstanceFunc func);

    Product* create(const char* libraryName, bool silent = false);
	const CreatorBase* find(const char* libraryName, bool silent = false);

	void registerQueued();

	static LibraryCustomEditorFactory& instance(){ return Singleton<LibraryCustomEditorFactory>::instance(); }
private:
    typedef StaticMap<std::string, CreatorBase*> Creators;
    Creators creators_;

	typedef StaticMap<LibraryInstanceFunc, CreatorBase*> RegistrationQueue;
	RegistrationQueue registrationQueue_;
};/*}}}*/

#define REGISTER_LIBRARY_CUSTOM_EDITOR(libraryInstanceFunc, editorType) \
	kdw::LibraryCustomEditorFactory::Creator<editorType> LibraryCustomEditorFactory##editorType(reinterpret_cast<kdw::LibraryCustomEditorFactory::LibraryInstanceFunc>(libraryInstanceFunc)); \
	int factoryDummy##LibraryCustomEditorFactory##editorType = 0;

bool focusObjectByElementName(LibraryTree* tree, const char* elementName, const char* subElementName, TreeObject* object);
bool focusObjectByGroupName(const char* elementName, TreeObject* object);
LibraryTreeObject* objectByElementName(const char* elementName, const char* subElementName, TreeObject* parent);

}

#endif
