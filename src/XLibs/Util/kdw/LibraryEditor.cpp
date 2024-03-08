#include "StdAfx.h"
#include "Serialization/SerializationFactory.h"
#include "Serialization/LibraryWrapper.h"

#include "kdw/LibraryEditor.h"
#include "kdw/LibraryEditorDialog.h"
#include "kdw/HBox.h"
#include "kdw/HSplitter.h"
#include "kdw/Button.h"
#include "kdw/ProgressBar.h"
#include "kdw/Tabs.h"
#include "kdw/LibraryTab.h"
#include "kdw/LibraryTree.h"
#include "kdw/LibraryTreeObject.h"
#include "kdw/WaitCursor.h"
#include "kdw/PropertyTree.h"
#include "kdw/PropertyRow.h"
#include "kdw/TreeSelector.h"
#include "kdw/Dialog.h"
#include <CrtDbg.h>

namespace kdw{

#pragma warning(push)
#pragma warning(disable: 4101) //  C4101: 'waitCursor' : unreferenced local variable
LibraryEditor::LibraryEditor(int border)
: VBox(2, border)
, currentTab_(0)
, progressBar_(0)
, tabs_(0)
, selectLibraryButton_(0)
, saveButton_(0)
, closeButton_(0)
, backButton_(0)
{

	HBox* topBox = new HBox();
	add(topBox);
	{
		backButton_ = new Button(TRANSLATE("<< &Назад"));
		backButton_->signalPressed().connect(this, &LibraryEditor::onBackButton);
		backButton_->setSensitive(false);
		topBox->add(backButton_);

		tabs_ = new ::kdw::Tabs();
		tabs_->signalMouseButtonDown().connect(this, &LibraryEditor::onTabMouseButtonDown);
		tabs_->signalChanged().connect(this, &LibraryEditor::onTabChange);
		topBox->add(tabs_, true, true, true);

		selectLibraryButton_ = new Button(TRANSLATE("&Добавить библиотеку..."));
		selectLibraryButton_->signalPressed().connect(this, LibraryEditor::onSelectLibraryButton);
		topBox->add(selectLibraryButton_);
	}


	layoutBox_ = new HBox(); // будет только один элемент, H или V - не важно
	add(layoutBox_, true, true, true);
	{
		progressBar_ = new ProgressBar();
		// add вызываем в setLayout

		splitter_ = new HSplitter();
		// add вызываем в setLayout
		{
			tree_ = new LibraryTree();
			// add вызываем в setLayout

			propertyTree_ = new PropertyTree();
			propertyTree_->setHasLibrarySupport(true);
			// add вызываем в setLayout
		}
	}


	{
		WaitCursor waitCursor;
		PropertyRowFactory::instance().buildMap();
		LibraryCustomEditorFactory::instance().registerQueued();
	}

	propertyTree()->signalSearch().connect(this, &LibraryEditor::onSearch);
	propertyTree()->signalFollowReference().connect(this, &LibraryEditor::onFollowReference);
	propertyTree()->signalChanged().connect(this, &LibraryEditor::onPropertyChanged);
	propertyTree()->signalSelected().connect(this, &LibraryEditor::onPropertySelected);
	tree()->signalSelectionChanged().connect(this, &LibraryEditor::onObjectSelected);

	showAll();

	setCurrentTab(0, false);
	updateTabs();
}
#pragma warning(pop)



LibraryEditor::~LibraryEditor()
{
	setCurrentTab(0, true);
	if(layoutBox_)
		layoutBox_->clear();
	tree_ = 0;
    propertyTree_ = 0;
}

void LibraryEditor::serialize(Archive& ar)
{
	if(ar.filter(SERIALIZE_STATE)){
		ar.serialize(openTabs_, "openTabs", 0);
		if(ar.isInput()){
			Tabs::iterator it;
			for(it = openTabs_.begin(); it != openTabs_.end(); ){
				LibraryTabBase* tab = *it;
				xassert(tab);
				if(tab && tab->isValid()){
					tab->setEditor(this);
					++it;
				}
				else{
					it = openTabs_.erase(it);
				}
			}
		}
		ar.serialize(*splitter_, "splitter", 0);
		int selectedTab = tabIndex(currentTab_);
		ar.serialize(selectedTab, "selectedTab", 0);
		if(ar.isInput()){
			LibraryTabBase* tab = findTabByIndex(selectedTab);
			setCurrentTab(tab, false);
			updateTabs();
		}

	}
}

void LibraryEditor::openBookmark(LibraryBookmark bookmark, bool enableHistory)
{
	const char* name = bookmark.libraryName();
    LibraryTabBase* libraryTab = findTabByName(name);
	bool createNewTab = !libraryTab;
    if(createNewTab){
        libraryTab = new LibraryTabEditable(this, bookmark);
        openTabs_.push_back(libraryTab);
    }
	setCurrentTab(libraryTab, enableHistory);
	if(!createNewTab)
        libraryTab->setBookmark(bookmark);
	else
		updateTabs();
}


LibraryTabBase* LibraryEditor::closeTab(LibraryTabBase* tab)
{
    Tabs::iterator it;
    it = std::find(openTabs_.begin(), openTabs_.end(), tab);
    xassert(it != openTabs_.end());
	tab->onClose();

    LibraryTabBase* newTab = 0;
    it = openTabs_.erase(it);
    if(openTabs_.empty())
        newTab = 0;
    else{
        if(it == openTabs_.end())
            newTab = openTabs_.back();
        else
            newTab = *it;
    }
	if(tab == currentTab_)
		currentTab_ = 0;
	return newTab;
}



void LibraryEditor::onTabClose(LibraryTabBase* tab)
{
	setCurrentTab(closeTab(tab), false);
	updateTabs();
}

void LibraryEditor::onTabMouseButtonDown(MouseButton button, int tabIndex)
{
    xassert(tabIndex >= 0 && tabIndex < openTabs_.size());
	PopupMenu menu(100);
    LibraryTabBase* tab = openTabs_[tabIndex];
    if(button == MOUSE_BUTTON_RIGHT){
        PopupMenuItem& root = menu.root();
		tab->onMenuConstruction(root);
		if(!root.empty())
			root.addSeparator();
        root.add(TRANSLATE("Закрыть"), tab)
            .connect(this, &LibraryEditor::onTabClose);
        root.add(TRANSLATE("Закрыть другие"))
            .enable(false);

        menu.spawn(this);
    }
    else if(button == MOUSE_BUTTON_MIDDLE){
        onTabClose(tab);
	}
}


void LibraryEditor::onElementCopied(const char* name, Serializer se)
{

}

void LibraryEditor::onElementPasted(Serializer se)
{
}

void LibraryEditor::setLayout(bool treeOnly)
{
	_CrtCheckMemory();
	xassert(layoutBox_);
	if(treeOnly){
		if(tree_->parent() != layoutBox_){
			splitter_->clear();
			layoutBox_->clear();

			kdw::VBox* vbox = new kdw::VBox();
			vbox->add(tree_, true, true, true);
			vbox->add(progressBar_);
			layoutBox_->add(vbox, true, true, true);
		}
	}
	else{
		plug_ = currentTab()->plug();
		xassert(splitter_);
		xassert(tree_);
		xassert(propertyTree_ || plug_);
		//if(tree_->parent() != splitter_ || plug_){
			_CrtCheckMemory();
			splitter_->clear();
			xassert(layoutBox_);
			layoutBox_->clear();
			_CrtCheckMemory();
		        
			splitter_->add(tree_, 0.4f);
			if(plug_)
				splitter_->add(plug_->asWidget());
			else
				splitter_->add(propertyTree_);
			layoutBox_->add(splitter_, true, true, true);
			_CrtCheckMemory();
		//}

	}
	layoutBox_->showAll();
	_CrtCheckMemory();
}

void LibraryEditor::setCurrentTab(LibraryTabBase* tab, bool enableHistory)
{
	if(currentTab_){
		if(enableHistory)
			addToHistory(currentTab_->bookmark());
		currentTab_->onClose();
	}

	currentTab_ = tab;

	//tree()->signalSelectionChanged().disconnect_all();
	tree()->setTab(tab);
	int index = tabIndex(tab);
	tabs_->setSelectedTab(index, this);

	/*
	propertyTree()->signalChanged().disconnect_all();
	propertyTree()->signalSelected().disconnect_all();
    propertyTree()->signalSearch().disconnect_all();
    propertyTree()->signalFollowReference().disconnect_all();
	*/
	if(tab){
		tab->onSelect();
		setLayout(tab->singleLayout());
		tab->onSelect();
	}
	else{
		tree()->root()->clear();
		propertyTree()->detach();
	}
	tree()->update();
}

void LibraryEditor::updateTabs()
{
    tabs_->clear();

    Tabs::iterator it;
    int currentTabIndex = -1;
    int index = 0;
    FOR_EACH(openTabs_, it){
		LibraryTabBase* tab = *it;
		xassert(tab);
        if(*it == currentTab_)
            currentTabIndex = index;
        tabs_->add(tab->title());
		++index;
    }
	xassert(!currentTab_ || currentTabIndex != -1); 
    tabs_->setSelectedTab(currentTabIndex, this);
}

void LibraryEditor::addTab(LibraryTabBase* tab)
{
	openTabs_.push_back(tab);
}

int LibraryEditor::tabIndex(LibraryTabBase* tab)
{
	Tabs::iterator it;
	int index = 0;
	FOR_EACH(openTabs_, it){
		if(tab == *it)
			return index;
		++index;
	}
	return -1;
}

LibraryTabBase* LibraryEditor::findTabByType(const std::type_info& id)
{
	Tabs::iterator it;
	FOR_EACH(openTabs_, it){
		LibraryTabBase* tab = *it;
		const char* name1 = typeid(*tab).name();
		const char* name2 = id.name();
		if(strcmp(name1, name2) == 0)
			return *it;
	}
	return 0;
}


LibraryTabBase* LibraryEditor::findTabByName(const char* name)
{
	Tabs::iterator it;
	FOR_EACH(openTabs_, it){
		LibraryTabBase* tab = *it;
		const char* tabName = tab->bookmark().libraryName();
		if(strcmp(tabName, name) == 0)
			return *it;
	}
	return 0;
}

LibraryTabBase* LibraryEditor::findTabByIndex(int index)
{
	int i = 0;
	Tabs::iterator it;
	FOR_EACH(openTabs_, it){
		if(i == index)
			return *it;
		++i;
	}
	return 0;
}

void LibraryEditor::onProgress(float progress)
{
	if(progressBar_)
		progressBar_->setPosition(progress);
}

void LibraryEditor::onBackButton()
{
	if(!history_.empty()){
		LibraryBookmark bookmark = history_.back();
		history_.pop_back();
		openBookmark(bookmark, false);	

		backButton_->setSensitive(!history_.empty());
	}
	else{
		xassert(0);
	}	
}

void LibraryEditor::onFollowReference(LibraryBookmark bookmark)
{
	openBookmark(bookmark);
}

void LibraryEditor::onSearch(const PropertyRow* row)
{
	if(LibraryTabBase* tab = currentTab()){
		tab->onSearch(row);
	}
}

void LibraryEditor::onObjectSelected()
{
	if(LibraryTabBase* tab = currentTab())
		tab->onObjectSelected();
}

void LibraryEditor::onPropertyChanged()
{
	if(LibraryTabBase* tab = currentTab()){
		tab->onPropertyChanged();
	}
}

void LibraryEditor::onPropertySelected()
{
	if(LibraryTabBase* tab = currentTab()){
		tab->onPropertySelected();
	}
}

class LibrariesTreeBuilder : public TreeBuilder{
public:
	LibrariesTreeBuilder(std::string& name)
	: name_(name)
	{}
protected:
    std::string& name_;

    Object* buildTree(Object* root){
        LibrariesManager::Libraries& libs = LibrariesManager::instance().editorLibraries();
        LibrariesManager::Libraries::iterator it;
		Object* result = 0;
        FOR_EACH(libs, it){
            const std::string& name = it->first;
            LibraryInstanceFunc instance = it->second;

            Object* o = root->add(new Object(instance().editName(), name.c_str()), true);
			if(name_ == name)
				result = o;
        }
		return result;
    }
	bool select(Object* object){
		if(object->selectable()){
            name_ = object->key();
			return true;
		}
		else{
			return false;
		}
	}
private:
};

void LibraryEditor::onSelectLibraryButton()
{
	kdw::TreeSelectorDialog selectorDialog(this);
    std::string name;
    LibrariesTreeBuilder builder(name);
    selectorDialog.setBuilder(&builder);
	if(selectorDialog.showModal() == kdw::RESPONSE_OK){
        openBookmark(LibraryBookmark(name.c_str()), false);
    }
}

void LibraryEditor::addToHistory(const LibraryBookmark& bookmark)
{
	history_.push_back(bookmark);
	backButton_->setSensitive(true);
}

void LibraryEditor::onTabChange(const TabChanger* changer)
{
	if(changer == this)
		return;

	int index = tabs_->selectedTab();
	if(index >= 0 && index < openTabs_.size()){
		Tabs::iterator it = openTabs_.begin();
		std::advance(it, index);
		setCurrentTab(*it, false);
	}
}

// ---------------------------------------------------------------------------

LibraryEditorDialog::LibraryEditorDialog(Widget* owner)
: Dialog(owner)
{
	init();
}

LibraryEditorDialog::LibraryEditorDialog(HWND owner)
: Dialog(owner)
{
	init();
}

int LibraryEditorDialog::showModal(const char* libraryName)
{
	if(strlen(libraryName) > 0)
		libraryEditor_->openBookmark(LibraryBookmark(libraryName));
	return __super::showModal();
}

void LibraryEditorDialog::serialize(Archive& ar)
{
	__super::serialize(ar);
	ar.serialize(*libraryEditor_, "libraryEditor", "Редактор библиотек");
}

void LibraryEditorDialog::init()
{
	setTitle(TRANSLATE("Редактор библиотек"));
	setResizeable(true);
	setDefaultSize(Vect2i(1000, 600));
	addButton(TRANSLATE("Сохранить"), RESPONSE_OK);
	addButton(TRANSLATE("Закрыть"), RESPONSE_CANCEL);

	libraryEditor_ = new LibraryEditor();
	add(libraryEditor_);
}

}
