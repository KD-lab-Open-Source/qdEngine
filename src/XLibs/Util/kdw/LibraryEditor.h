#ifndef __LIBRARY_EDITOR_H_INCLUDED__
#define __LIBRARY_EDITOR_H_INCLUDED__

#include "Serialization/LibraryBookmark.h"
#include "Serialization/Serializer.h"
#include "kdw/VBox.h"
#include "kdw/Tabs.h"
#include "kdw/Plug.h"
#include "kdw/LibraryTreeObject.h"

namespace kdw{

class Tabs;
class Button;
class ProgressBar;
class LibraryTree;
class LibraryPropertyTree;
class LibraryTab;
class LibraryTabBase;
class PropertyTree;
class PropertyRow;
class HSplitter;


class LibraryEditor : public kdw::VBox, public TabChanger{
public:
	LibraryEditor(int border = 0);
	~LibraryEditor();

	void serialize(Archive& ar);
	void openBookmark(LibraryBookmark bookmark, bool enableHistory);
	void openBookmark(LibraryBookmark bookmark){
		openBookmark(bookmark, true);
	}
	typedef std::vector<ShareHandle<LibraryTabBase> > Tabs;
	typedef std::vector<LibraryBookmark> Bookmarks;

	void onElementCopied(const char* name, Serializer se);
	void onElementPasted(Serializer se);
	
	void setLayout(bool treeOnly);

	LibraryTree* tree(){ return tree_; }
	PropertyTree* propertyTree(){ return propertyTree_; }

	LibraryTabBase* currentTab(){ return currentTab_; }
	void setCurrentTab(LibraryTabBase* tab, bool enableHistory);
	void updateTabs();

	void addTab(LibraryTabBase* tab);
	LibraryTabBase* findTabByType(const std::type_info& id);
	LibraryTabBase* findTabByName(const char* name);
	LibraryTabBase* findTabByIndex(int index);
	int tabIndex(LibraryTabBase* tab);

	void onProgress(float progess); 

protected:
	void onBackButton();
	void onSaveButton();
	void onCloseButton();
	void onFollowReference(LibraryBookmark bookmark);
	void onPropertyChanged();
	void onPropertySelected();
	void onSearch(const PropertyRow* row);
	void onObjectSelected();

	/// возвращает следующую за ней вкладку
	LibraryTabBase* closeTab(LibraryTabBase* tab);
	void onSelectLibraryButton();
	void addToHistory(const LibraryBookmark& bookmark);

    void onTabMouseButtonDown(MouseButton button, int tabIndex);
	void onTabClose(LibraryTabBase* tab);
	void onTabChange(const TabChanger* changer);

	Bookmarks history_;
	Tabs openTabs_;
	LibraryTabBase* currentTab_;
	std::string libraryTabOpen_;

	::kdw::Tabs* tabs_;
	Button* backButton_;
	Button* selectLibraryButton_;
	Button* saveButton_;
	Button* closeButton_;

	// храним не ShareHandle-ам, т.к. будем перекладовать из одних контейнеров в другие
	ShareHandle<ProgressBar> progressBar_;
	ShareHandle<HSplitter> splitter_;
	ShareHandle<Box> layoutBox_;
	ShareHandle<LibraryTree> tree_;
	ShareHandle<PropertyTree> propertyTree_;
	ShareHandle<PlugBase> plug_;
};

}

#endif
