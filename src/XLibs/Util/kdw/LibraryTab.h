#ifndef __LIBRARY_TAB_H_INCLUDED__
#define __LIBRARY_TAB_H_INCLUDED__

#include "Handle.h"
#include "Serialization/LibraryBookmark.h"
#include "XTL/sigslot.h"
#include "XTL/Functor.h"
#include "kdw/TreePath.h" // TreeState, не такой жирный header, как Tree.h
#include "kdw/Plug.h" 

class EditorLibraryInterface;
class Archive;
class Serializer;

namespace kdw{
class PropertyRow;
class PopupMenuItem;
class TreeObject;
class LibraryTree;
class LibraryTreeObject;
class LibraryEditor;
class LibraryCustomEditor;

class LibraryTabBase : public ShareHandleBase, public sigslot::has_slots{
	typedef LibraryTabBase Self;
public:
    LibraryTabBase(LibraryEditor* editor, const LibraryBookmark& bookmark);
	virtual bool isSearchResults() const{ return false; }
	virtual bool singleLayout() const{ return false; }

	virtual bool isValid() const { return true; }
	virtual const char* title() const { return ""; };

	virtual void setEditor(LibraryEditor* editor) { editor_ = editor; }
    virtual void setBookmark(const LibraryBookmark& bookmark);
    const LibraryBookmark& bookmark() const { return bookmark_; }

	virtual void onObjectSelected() {}
	virtual void onObjectDeleted() {}
	virtual void onPropertyChanged() {}
	virtual void onPropertySelected() {}
	virtual void onMenuConstruction(PopupMenuItem& root) {};

    virtual void serialize(Archive& ar);
	virtual void onSelect();
	virtual void onClose() {};
	virtual ~LibraryTabBase() {}
	
	LibraryTree* tree();
	PropertyTree* propertyTree();
	virtual PlugBase* plug() { return 0; }

	virtual void onSearch(const PropertyRow* node){ xassert(0); }
	virtual void onFollowReference(const LibraryBookmark& bookmark){ xassert(0); }

protected:

	void onMenuSaveAsText();

    LibraryEditor* editor_;
    LibraryBookmark bookmark_;
};

class LibraryTabEditable : public LibraryTabBase{
	typedef LibraryTabEditable Self;
public:
    LibraryTabEditable(LibraryEditor* editor = 0, const LibraryBookmark& bookmark = LibraryBookmark());

    const char* title() const;
	bool isValid() const;

    void setBookmark(const LibraryBookmark& bookmark);
	void onObjectSelected();
	void onObjectDeleted();
	void onPropertyChanged();
	void onPropertySelected();

	void onSearchLibraryElement(const char* libraryName, const char* elementName);
	void onSearch(const PropertyRow* row);

	void onMenuSort();
	void onMenuFindUnused();
	void onMenuConstruction(PopupMenuItem& root);
	void setLibrary(const char* libraryName);

    void serialize(Archive& ar);
    void onSelect();
	void onClose();
    EditorLibraryInterface* library() const;

	PlugBase* plug();

	static void buildLibraryTree(LibraryTree* tree, TreeObject* rootObject, LibraryCustomEditor* customEditor);

protected:
	void saveTabState();
	void loadTabState(bool affectTree = true);

private:
	ShareHandle<LibraryCustomEditor> customEditor_;
	ShareHandle<PlugBase> plug_;
	ShareHandle<LibraryTreeObject> plugObject_;

	bool firstSelect_;
	typedef StaticMap<std::string, TreeState> TreeStateByGroup;
	TreeState treeState_;
	TreeStateByGroup propertyTreeStates_;
};

typedef std::list<LibraryBookmark> SearchResultItems;
typedef Functor1<void, float> SearchProgressCallback;

class LibraryTabSearch : public LibraryTabBase{
public:
	LibraryTabSearch(LibraryEditor* editor = 0, const LibraryBookmark& bookmark = LibraryBookmark());
	bool isSearchResults() const{ return true; }
	bool singleLayout() const{ return true; }
	
	void findUnused(const char* libraryName, SearchProgressCallback progress);
	void findTreeNode(const kdw::PropertyRow& root, bool sameName, SearchProgressCallback progress);
	void findLibraryReference(const char* libraryName, const char* elementName, SearchProgressCallback progress);

    void setBookmark(const LibraryBookmark& bookmark);
	void onObjectSelected();
	void onMenuConstruction(PopupMenuItem& root);

	void onSelect();

    void serialize(Archive& ar);
	const char* title() const{ return title_.c_str(); }

	SearchResultItems& items() { return searchResultItems_; }
protected:
	void onMenuSaveAsText();
	void onMenuDeleteAll();
	SearchResultItems searchResultItems_;

	int selectedIndex_;
	std::string title_;
};

std::string makeName(const char* reservedComboList, const char* nameBase);
}

#endif
