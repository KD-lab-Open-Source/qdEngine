#include "stdafx.h"
#include <functional>
#include <set>
#include "Serialization\Serialization.h"
#include "FileUtils\FileUtils.h"

#include "Serialization\Dictionary.h"
#include "Serialization\LibraryWrapper.h"
#include "Serialization\LibrariesManager.h"
#include "Serialization\LibraryBookmark.h"
#include "Serialization\SerializationFactory.h"

#include "kdw/LibraryTab.h"
#include "kdw/LibraryTreeObject.h"
#include "kdw/LibraryTree.h"
#include "kdw/LibraryEditor.h"
#include "kdw/LibraryElementCreateDialog.h"
#include "kdw/PropertyRow.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/PropertyOArchive.h"
#include "kdw/PropertyTree.h"
#include "kdw/Clipboard.h"
#include "kdw/Win32Proxy.h"
#include "kdw/WaitCursor.h"
//#include "CreateAttribDlg.h"
#include "kdw/Dialog.h"
#include "kdw/Label.h"
#include "kdw/Entry.h"
#include "kdw/CheckBox.h"

#ifdef __UNIT_ATTRIBUTE_H__
# error "UnitAttribute.h included!"
#endif

namespace kdw{


REGISTER_CLASS(LibraryTabBase, LibraryTabEditable, "LibraryTabEditable");
REGISTER_CLASS(LibraryTabBase, LibraryTabSearch, "LibraryTabSearch");

LibraryTreeObject* objectByElementName(const char* elementName, const char* subElementName, TreeObject* parent);

LibraryTabBase::LibraryTabBase(LibraryEditor* editor, const LibraryBookmark& bookmark)
: editor_(editor)
, bookmark_(bookmark)
{
}

void LibraryTabBase::serialize(Archive& ar)
{
	ar.serialize(bookmark_, "bookmark", 0);
}

void LibraryTabBase::setBookmark(const LibraryBookmark& bookmark)
{
	bookmark_ = bookmark;
}

void LibraryTabBase::onSelect()
{
	xassert(editor_);
	//editor_->propertyTree()->signalChanged().connect(this, &LibraryTabBase::onAttribElementChanged);
	//editor_->propertyTree()->signalSelected().connect(this, &LibraryTabBase::onAttribElementSelected);
	//editor_->tree()->signalSelectionChanged().connect(this, &LibraryTabBase::onObjectSelected);
}


LibraryTree* LibraryTabBase::tree()
{
	return editor_->tree();
}

PropertyTree* LibraryTabBase::propertyTree()
{
	return editor_->propertyTree();
}


// ----------------------------------------------------------------------------

static bool isDigit(char c)/*{{{*/
{
	static bool initialized = false;
	static bool table[256];
	if(!initialized){
		for(int i = 0; i < 256; ++i)
			table[i] = false;
		table['0'] = table['1'] = table['2'] = table['3'] = table['4']
		= table['5'] = table['6'] = table['7'] = table['8'] = table['9'] = true;
		initialized = true;
	}
	return table[(unsigned char)c];
}/*}}}*/

std::string makeName(const char* reservedComboList, const char* nameBase)
{
	if(nameBase[0] == '\0')
		nameBase = "unnamed";

	if(indexInComboListString(reservedComboList, nameBase) < 0)
		return std::string(nameBase);
	std::string name_base = nameBase;

	// обрезаем циферки
	std::string::iterator ptr = name_base.end() - 1;
	if(name_base.size() > 1){
		while(ptr != name_base.begin() && isDigit(*ptr))
			ptr--;
		name_base = std::string(name_base.begin(), ptr + 1);
	}

	// обрезаем завершающие пробелы
	ptr = name_base.end() - 1;
	if(name_base.size() > 1){
		while(ptr != name_base.begin() && (*ptr) == ' ')
			ptr--;
		name_base = std::string(name_base.begin(), ptr + 1);
	}

	int index = 1;
	std::string default_name = name_base;
	while(indexInComboListString(reservedComboList, default_name.c_str()) >= 0){
		char num_buf[11];
		sprintf(num_buf, "%02i", index);

		XBuffer buf;
		buf < name_base.c_str();
		buf < num_buf;
		default_name = static_cast<const char*>(buf);
		++index;
	}
	return default_name;
}

// ----------------------------------------------------------------------------
LibraryGroupTreeObject::LibraryGroupTreeObject(LibraryCustomEditor* customEditor, const char* groupName)
: LibraryTreeObject(customEditor)
, groupName_(groupName)
{
	if(customEditor && !strcmp(groupName, ""))
		setText(customEditor->library()->editName());
	else
		setText(groupName);
}

void LibraryGroupTreeObject::onRightClick(ObjectsTree* _tree)
{
	if(!customEditor_)
		return;

	LibraryTree* tree = safe_cast<LibraryTree*>(_tree);
	kdw::PopupMenu menu(300);
	kdw::PopupMenuItem& root = menu.root();
	root.add(TRANSLATE("Добавить"), tree)
		.connect(this, &LibraryGroupTreeObject::onMenuCreate);

	onMenuConstruction(root, _tree);

	if(groupName_ == "" && customEditor_->library()->editorDynamicGroups()){
		root.addSeparator();
		root.add(TRANSLATE("Добавить группу"), tree)
			.connect(this, &LibraryGroupTreeObject::onMenuCreateGroup);
	}

	menu.spawn(tree);
}

std::string LibraryGroupTreeObject::fullGroupName() const
{
	std::string result = groupName_;
	const LibraryGroupTreeObject* current = dynamic_cast<const LibraryGroupTreeObject*>(parent());
	while(current){
		const LibraryGroupTreeObject* parent = dynamic_cast<const LibraryGroupTreeObject*>(current->parent());
		if(parent){
			result = std::string(current->groupName()) + "\\" + result;
			current = parent;
		}
		else
			break;
	}
	return result;
}

void LibraryGroupTreeObject::onMenuCreateGroup(LibraryTree* tree)
{
	if(customEditor_){
		const char* groupsComboList = customEditor_->library()->editorGroupsComboList();

		kdw::LibraryElementCreateDialog createDialog(tree, false, false, makeName(groupsComboList, TRANSLATE("Новая группа")).c_str());
		if(createDialog.showModal() == RESPONSE_OK){
			std::string attribName = makeName(groupsComboList, createDialog.name());
			customEditor_->library()->editorAddGroup(attribName.c_str());
			LibraryTabEditable::buildLibraryTree(tree, tree->root(), customEditor_); // suicide
			focusObjectByGroupName(attribName.c_str(), tree->root());
		}
	}
}

void LibraryGroupTreeObject::onMenuCreate(LibraryTree* tree)
{
	if(customEditor_){
		std::string groupName = fullGroupName();
		bool canBePasted = true;
		LibraryElementCreateDialog createDialog(tree, canBePasted, true, TRANSLATE("Новый элемент"));

		if(createDialog.showModal() == RESPONSE_OK){
			std::string attribName = makeName(customEditor_->library()->editorComboList(), createDialog.name());

			std::string newName = customEditor_->library()->editorAddElement(attribName.c_str(), groupName.c_str());
			xassert(newName != "");

			if(newName != ""){
				LibraryTabEditable::buildLibraryTree(tree, tree->root(), customEditor_); // suicide
				if(LibraryElementTreeObject* object =
					safe_cast<LibraryElementTreeObject*>(objectByElementName(newName.c_str(), "", tree->root()))){
					object->setElementName(newName.c_str());
					if(canBePasted && createDialog.paste())
						object->onMenuPaste(tree);
					//object->rebuild();
					tree->selectObject(object);
				}
				else
					xassert(0);
			}
		}
	}
}

void LibraryGroupTreeObject::onSelect(ObjectsTree* tree)
{
	bookmark_.setLibraryName(customEditor_->library()->name());
	__super::onSelect(tree);
}

void LibraryTreeObject::onSelect(ObjectsTree* _tree)
{
	__super::onSelect(_tree);
	LibraryTree* tree = safe_cast<LibraryTree*>(_tree);
    tree->signalSelectionChanged().emit();
}


// ----------------------------------------------------------------------------
LibraryElementTreeObject::LibraryElementTreeObject(LibraryCustomEditor* customEditor, const char* elementName)
: LibraryTreeObject(customEditor)
, elementName_(elementName)
, elementIndex_(-1)
{
	setElementName(elementName);
}


LibraryTreeObject* LibraryElementTreeObject::onPropertyChanged()
{
	int index = customEditor_->library()->editorFindElement(elementName_.c_str());
	if(index != elementIndex_){
		const char* name = customEditor_->library()->editorElementName(elementIndex_);

		setElementName(name);
		customEditor_->library()->editorElementRenamed();
	}
	return this;
}


void LibraryElementTreeObject::onRightClick(ObjectsTree* _tree)
{
	LibraryTree* tree = safe_cast<LibraryTree*>(_tree);
	kdw::PopupMenu menu(100);

	kdw::PopupMenuItem& root = menu.root();

	onMenuConstruction(root, tree);

	if(!root.empty())
		root.addSeparator();

	if(customEditor_->library()->editorAllowRename()){
		root.add(TRANSLATE("Переименовать"), tree)
			.connect(this, &LibraryElementTreeObject::onMenuRename);
	}

	root.add(TRANSLATE("Копировать"), tree)
		.connect(this, &LibraryElementTreeObject::onMenuCopy);


	const char* typeName = customEditor_->library()->editorElementSerializer(elementName_.c_str(), "", "", true).typeName();
	std::string groupName = customEditor_->library()->editorElementGroup(elementName_.c_str());

	kdw::Clipboard clipboard(tree);
	bool canBePasted = typeName ? clipboard.canBePastedOn(typeName) : false;

	root.add(TRANSLATE("Вставить"), tree)
	.connect(this, &LibraryElementTreeObject::onMenuPaste)
	.enable(canBePasted);

	root.addSeparator();
	root.add(TRANSLATE("Искать ссылку"), tree)
	.connect(this, &LibraryElementTreeObject::onMenuSearch);

	root.addSeparator();
	root.add(TRANSLATE("Удалить"), tree)
	.connect(this, &LibraryElementTreeObject::onMenuDelete)
	.setHotkey(sKey(VK_DELETE));

	menu.spawn(tree);
}



static LibraryTreeObject* findByGroupName(const char* groupName, LibraryTreeObject* parent)
{
	LibraryTreeObject::iterator it;
	FOR_EACH(*parent, it){
		LibraryTreeObject* obj = safe_cast<LibraryTreeObject*>(&**it);
		if(obj->isGroup()){
			LibraryGroupTreeObject* group = safe_cast<LibraryGroupTreeObject*>(obj);
			if(strcmp(group->groupName(), groupName) == 0)
				return obj;
		}
		if(LibraryTreeObject* result = findByGroupName(groupName, obj))
			return result;
	}
	return 0;
}


void LibraryElementTreeObject::onMenuDelete(LibraryTree* tree)
{
	tree->tab()->onObjectDeleted();

	LibraryTreeObject* parent = safe_cast<LibraryTreeObject*>(this->parent());
	TreeObject::iterator foundIt = std::find(parent->begin(), parent->end(), this);
	ASSERT(foundIt != parent->end());
	TreeObject::iterator it = foundIt;
	++it;

	std::string elementName;
	std::string groupName;
	if(it != parent->end()){
		LibraryElementTreeObject* nextElement = safe_cast<LibraryElementTreeObject*>(&**it);
		elementName = nextElement->elementName();
	}
	else if(foundIt != parent->begin()){
		--foundIt;
		if(LibraryElementTreeObject* previousElement = dynamic_cast<LibraryElementTreeObject*>(&**foundIt))
			elementName = previousElement->elementName();
	}
	else{
		if(parent->isGroup()){
			LibraryGroupTreeObject* group = safe_cast<LibraryGroupTreeObject*>(parent);
			groupName = group->groupName();
		}
	}

	customEditor_->library()->editorElementErase(elementName_.c_str());
	
	LibraryTabEditable::buildLibraryTree(tree, tree->root(), customEditor_); // suicide
	if(!elementName.empty()){
		focusObjectByElementName(tree, elementName.c_str(), "", tree->root());
	}
	else if(!groupName.empty()){
		LibraryTreeObject* obj = findByGroupName(groupName.c_str(), safe_cast<LibraryTreeObject*>(tree->root()));
		tree->expandRow(obj);
		tree->model()->setFocusedRow(obj);
	}
	tree->update();
}

void LibraryElementTreeObject::setElementName(const char* elementName)
{
	xassert(!customEditor_ || customEditor_->library()->editorFindElement(elementName) != -1);
	elementName_ = elementName;
	setText(elementName);
}

Serializer LibraryElementTreeObject::getSerializer(const char* name, const char* nameAlt)
{
	return customEditor_->library()->editorElementSerializer(elementName_.c_str(), name, nameAlt, false);
}


bool LibraryElementTreeObject::onKeyDown(sKey key, ObjectsTree* tree)
{
	if(key == VK_DELETE){
		onMenuDelete(safe_cast<LibraryTree*>(tree));
		return true;
	}
	return false;
}

void LibraryElementTreeObject::onMenuPaste(LibraryTree* tree)
{
	kdw::Clipboard clipboard(tree);

	if(!clipboard.empty()){
		Serializer ser(customEditor_->library()->editorElementSerializer(elementName(), "", "", true));
		clipboard.paste(ser);
		rebuild();
		tree->signalSelectionChanged().emit();
	}
}

void LibraryElementTreeObject::onMenuCopy(LibraryTree* tree)
{
	kdw::Clipboard clipboard(tree);

	EditorLibraryInterface* library = customEditor_->library();
	int index = library->editorFindElement(elementName_.c_str());
	Serializer serializer = library->editorElementSerializer(index, "", "", true);
	clipboard.copy(serializer);
}

void LibraryElementTreeObject::onMenuRename(LibraryTree* tree)
{
	EditorLibraryInterface* library = customEditor_->library();

	LibraryElementCreateDialog dlg(false, false, TRANSLATE("Переименновать"), elementName_.c_str());
	if(dlg.showModal() == RESPONSE_OK){
		std::string newName = makeName(library->editorComboList(), dlg.name());
		library->editorElementSetName(elementName_.c_str(), newName.c_str());
		elementName_ = newName;
		setText(elementName_.c_str());
		tree->tab()->onObjectDeleted();
		tree->ensureVisible(this);
		onSelect(tree);
	}	
}

void LibraryElementTreeObject::onMenuSearch(LibraryTree* tree)
{
	xassert(customEditor_->library());
	if(customEditor_->library()){
		LibraryTabEditable* editable = safe_cast<LibraryTabEditable*>(tree->tab());
		editable->onSearchLibraryElement(customEditor_->library()->name(), elementName_.c_str());
	}
}

bool LibraryElementTreeObject::canBeDragged() const
{
	return customEditor_->library()->editorAllowDrag();
}

bool LibraryElementTreeObject::canBeDroppedOn(const TreeRow* _row, const TreeRow* beforeChild, const Tree* tree, bool direct) const
{
	const LibraryTreeObject* row = safe_cast<const LibraryTreeObject*>(_row);

	if(!customEditor_->library()->editorAllowDrag())
		return false;
	if(row == this)
		return false;

	return row->isGroup() && row->parent() != 0;
}

void LibraryElementTreeObject::dropInto(TreeRow* _destination, TreeRow* _beforeChild, Tree* _tree)
{
	LibraryTreeObject* beforeChild = safe_cast<LibraryTreeObject*>(_beforeChild);
	LibraryTreeObject* destination = safe_cast<LibraryTreeObject*>(_destination);
	LibraryTree* tree = safe_cast<LibraryTree*>(_tree);
	if(destination->isGroup() && destination->parent() != 0){
		std::string elementName = this->elementName();
		LibraryGroupTreeObject* group = safe_cast<LibraryGroupTreeObject*>(destination);
		if(beforeChild == 0){
			customEditor_->library()->editorElementSetGroup(elementName.c_str(), group->groupName());
			customEditor_->library()->editorElementMoveBefore(elementName.c_str(), 0);

			LibraryTabEditable::buildLibraryTree(tree, tree->root(), customEditor_); // suicide
			focusObjectByElementName(tree, elementName.c_str(), "", tree->root());
		}
		else if(beforeChild->isElement()){
			LibraryElementTreeObject* destinationElement = safe_cast<LibraryElementTreeObject*>(beforeChild);

			LibraryGroupTreeObject* sourceGroup = safe_cast<LibraryGroupTreeObject*>(parent());

			if(strcmp(sourceGroup->groupName(), group->groupName()) != 0)
				customEditor_->library()->editorElementSetGroup(elementName.c_str(), group->groupName());

			customEditor_->library()->editorElementMoveBefore(elementName.c_str(), destinationElement->elementName());

			LibraryTabEditable::buildLibraryTree(tree, tree->root(), customEditor_); // suicide
			focusObjectByElementName(tree, elementName.c_str(), "", tree->root());
		}
	}
}

void LibraryElementTreeObject::onSelect(ObjectsTree* tree)
{
	elementIndex_ = customEditor_->library()->editorFindElement(elementName_.c_str());
	bookmark_.setElementName(elementName_.c_str());
	bookmark_.setLibraryName(customEditor_->library()->name());
	__super::onSelect(tree);
}

// ----------------------------------------------------------------------------

void LibraryCustomEditorFactory::queueRegistration(CreatorBase& creator_op, LibraryInstanceFunc func)
{
	registrationQueue_[func] = &creator_op;
}

void LibraryCustomEditorFactory::add(const char* libraryName, CreatorBase& creator_op)
{
	if(creators_.find(libraryName) != creators_.end())
		xassertStr(0 && "Повторная регистрация в фабрике", libraryName);

	creators_[libraryName] = &creator_op;
}

const LibraryCustomEditorFactory::CreatorBase* LibraryCustomEditorFactory::find(const char* libraryName, bool silent)
{
	Creators::const_iterator it = creators_.find(libraryName);
	if(it != creators_.end())
		return it->second;

	xassert(silent && "Неопознанный идентификатор класса");
	return 0;
}

LibraryCustomEditor* LibraryCustomEditorFactory::create(const char* libraryName, bool silent)
{
	EditorLibraryInterface* library = LibrariesManager::instance().find(libraryName);
	if(library){
		if(const CreatorBase* creator = find(libraryName, true))
			return creator->create();
		else
			return new LibraryCustomEditor(library);
	}
	else
		return 0;
}

void LibraryCustomEditorFactory::registerQueued()
{
	if(!registrationQueue_.empty()){
		RegistrationQueue::iterator it;
		FOR_EACH(registrationQueue_, it){
			EditorLibraryInterface& library = it->first();
			add(library.name(), *it->second);
		}
		registrationQueue_.clear();
	}
}

bool fixDuplicates(EditorLibraryInterface* library)
{
	static std::vector<LibraryWrapperBase*> fixedLibraries;
	if(std::find(fixedLibraries.begin(), fixedLibraries.end(), library) != fixedLibraries.end())
		return false;

	bool result = false;
	typedef std::vector<std::string> Names;
	Names names;
	int count = library->editorSize();
	for(int i = 0; i < count; ++i){
		const char* name = library->editorElementName(i);
		Names::iterator it = std::find(names.begin(), names.end(), name);
		if(it != names.end()){
			std::string newName = makeName(library->editorComboList(), name);
			library->editorElementSetName(i, newName.c_str());
			result = true;
			names.push_back(newName);
		}
		else{
			names.push_back(name);
		}
	}

	fixedLibraries.push_back(library);
	return result;
}

// ----------------------------------------------------------------------------

void LibraryTabEditable::buildLibraryTree(LibraryTree* tree, TreeObject* rootObject, LibraryCustomEditor* customEditor)/*{{{*/
{
	xassert(rootObject);
	rootObject->clear();

	typedef StaticMap<std::string, TreeObject*> Groups;
	Groups groups;

	xassert(customEditor);
	if(customEditor && customEditor->library()){
		//if(fixDuplicates(library))
		//	::AfxMessageBox(TRANSLATE("Библиотека содержала дубликаты, некоторые элементы были переименнованы"), MB_ICONWARNING | MB_OK);

		TreeObject* root = groups[""] = rootObject->add(customEditor->createGroupTreeObject(""));

		TreeObject* parent = root;

		const char* groupsComboList = customEditor->library()->editorGroupsComboList();
		if(groupsComboList){
			ComboStrings groupStrings;
			ComboStrings::iterator it;
			splitComboList(groupStrings, groupsComboList);
			FOR_EACH(groupStrings, it){
				const char* groupString = it->c_str();
				ComboStrings groupPath;
				ComboStrings::iterator pit;
				splitComboList(groupPath, groupString, '\\');

				std::string currentPath = "";
				TreeObject* lastObject = parent;
				FOR_EACH(groupPath, pit){
					if(!currentPath.empty())
						currentPath += "\\";

					currentPath += pit->c_str();
					TreeObject*& groupObject = groups[currentPath.c_str()];
					if(!groupObject){

						lastObject = lastObject->add(customEditor->createGroupTreeObject(pit->c_str()));
						groupObject = lastObject;
					}
					else
						lastObject = groupObject;
				}
			}
		}

		int count = customEditor->library()->editorSize();
		for(int index = 0; index < count; ++index){
			std::string name = customEditor->library()->editorElementName(index);
			if(name != ""){
				std::string group = customEditor->library()->editorElementGroup(index);
				parent = root;

				Groups::iterator it = groups.find(group);
				TreeObject*& groupObject = groups[group];
				if(it != groups.end())
					parent = groupObject;
				else{
					xassert(!groupsComboList);
					parent = groupObject = parent->add(customEditor->createGroupTreeObject(group.c_str()));
				}

				LibraryElementTreeObject* object = 0;
				object = customEditor->createTreeObject(name.c_str());
				parent->add(object);
				object->rebuild();
			}
		}
		//groups[""]->expand(); // FIXME
		tree->update();
	}
}/*}}}*/

bool focusObjectByGroupName(const char* name, TreeObject* parent)/*{{{*/
{
	TreeObject::iterator it;
	FOR_EACH(*parent, it){
		LibraryTreeObject* object = safe_cast<LibraryTreeObject*>(&**it);
		if(object->isGroup()){
			if(!strcmp(((LibraryGroupTreeObject*)object)->groupName(), name)){
				object->focus();
				return true;
			}
		}
		if(focusObjectByGroupName(name, object))
			return true;
	}
	return false;
}/*}}}*/

LibraryTreeObject* objectByElementName(const char* elementName, const char* subElementName, TreeObject* parent)
{
	TreeObject::iterator it;
	FOR_EACH(*parent, it){
		LibraryTreeObject* object = safe_cast<LibraryTreeObject*>(&**it);
		if(object->isElement()){
			if(!strcmp(((LibraryElementTreeObject*)object)->elementName(), elementName)){
				if(LibraryTreeObject* subElement = object->subElementByName(subElementName))
					return subElement;
				else
					return object;
			}
		}
		if(TreeObject* object = objectByElementName(elementName, "", safe_cast<TreeObject*>(&**it)))
			return safe_cast<LibraryTreeObject*>(object);
	}
	return 0;
}

bool focusObjectByElementName(LibraryTree* tree, const char* elementName, const char* subElementName, TreeObject* parent)/*{{{*/
{
	if(LibraryTreeObject* object = objectByElementName(elementName, subElementName, parent)){
		tree->model()->deselectAll();
		tree->selectObject(object);
		return true;
	}
	else 
		return false;
}/*}}}*/

// ----------------------------------------------------------------------------

bool LibraryGroupTreeObject::canBeDragged() const
{
	return false;
}

LibraryTabEditable::LibraryTabEditable(LibraryEditor* editor, const LibraryBookmark& bookmark)
: LibraryTabBase(editor, bookmark)
, firstSelect_(true)
, customEditor_(0)
{
	if(editor)
		setLibrary(bookmark_.libraryName());
}


#pragma warning(push)
#pragma warning(disable: 4101) // unreferenced local variable

void LibraryTabEditable::onMenuFindUnused()
{
	kdw::WaitCursor waitCursor;
	
	LibraryTabSearch* tab = safe_cast<LibraryTabSearch*>(editor_->findTabByType(typeid(LibraryTabSearch)));
	if(!tab){
		tab = new LibraryTabSearch(editor_);
		editor_->addTab(tab);
	}
	else
		tab->items().clear();

	tab->findUnused(library()->name(), bindMethod(*editor_, &LibraryEditor::onProgress));
	editor_->setCurrentTab(tab, true);
	editor_->updateTabs();
}

void LibraryTabEditable::onSearchLibraryElement(const char* libraryName, const char* elementName)
{
	kdw::WaitCursor waitCursor;
	
	LibraryTabSearch* tab = safe_cast<LibraryTabSearch*>(editor_->findTabByType(typeid(LibraryTabSearch)));
	if(!tab){
		tab = new LibraryTabSearch(editor_);
		editor_->addTab(tab);
	}
	else
		tab->items().clear();

	tab->findLibraryReference(libraryName, elementName, bindMethod(*editor_, &LibraryEditor::onProgress));
	editor_->setCurrentTab(tab, true);
	editor_->updateTabs();
}

void LibraryTabEditable::onSearch(const PropertyRow* node)
{
	kdw::WaitCursor waitCursor;

	kdw::PropertyRow root;
	root.add(node->clone());

	LibraryTabSearch* tab = safe_cast<LibraryTabSearch*>(editor_->findTabByType(typeid(LibraryTabSearch)));
	if(!tab){
		tab = new LibraryTabSearch(editor_);
		editor_->addTab(tab);
	}
	else
		tab->items().clear();

	tab->findTreeNode(root, false, bindMethod(*editor_, &LibraryEditor::onProgress));
	editor_->setCurrentTab(tab, true);
	editor_->updateTabs();
}


void LibraryTabEditable::onMenuSort()
{
	xassert(customEditor_->library());
	customEditor_->library()->editorSort();
	buildLibraryTree(editor_->tree(), editor_->tree()->root(), customEditor_);
	setBookmark(bookmark_);
}

void LibraryTabEditable::onMenuConstruction(PopupMenuItem& root)
{
	root.add(TRANSLATE("Отсортировать")).connect(this, &Self::onMenuSort);
	root.add(TRANSLATE("Найти неиспользуемые...")).connect(this, &Self::onMenuFindUnused);
}

EditorLibraryInterface* LibraryTabEditable::library() const
{
	return customEditor_ ? customEditor_->library() : 0;
}

void LibraryTabEditable::setLibrary(const char* libraryName)
{
	customEditor_ = LibraryCustomEditorFactory::instance().create(libraryName);
}

void LibraryTabEditable::onSelect()
{
	firstSelect_ = true;
	__super::onSelect();

	xassert(editor_);
	LibraryTree* tree = editor_->tree();
	if(editor_->propertyTree() && editor_->tree()){
		kdw::WaitCursor waitCursor;
		editor_->propertyTree()->detach();
		xassert(customEditor_);
		buildLibraryTree(tree, tree->root(), customEditor_);
		tree->update();
		setBookmark(bookmark_);
		loadTabState();
	}
}
#pragma warning(pop)

void LibraryTabEditable::loadTabState(bool affectTree)
{
	if(affectTree)
		editor_->tree()->setState(treeState_);
	if(library() && library()->editorFindElement(bookmark_.elementName()) >= 0){
		std::string group = library()->editorElementGroup(bookmark_.elementName());
		TreeStateByGroup::iterator it = propertyTreeStates_.find(group.c_str());
		if(it != propertyTreeStates_.end()){
			TreeState& state = it->second;
			editor_->propertyTree()->setState(state);
		}
	}
}

void LibraryTabEditable::saveTabState()
{
	treeState_ = editor_->tree()->state();
	if(library() && library()->editorFindElement(bookmark_.elementName()) >= 0){
		std::string group = library()->editorElementGroup(bookmark_.elementName());

		TreeState& state = propertyTreeStates_[group];
		state = editor_->propertyTree()->state();
	}

	if(plug_ && plugObject_){
		plug_->assignTo(plugObject_->getSerializer());
		plugObject_ = 0;
	}
}

void LibraryTabEditable::onClose()
{
	saveTabState();
}

void LibraryTabEditable::serialize(Archive& ar)
{
	__super::serialize(ar);

	ar.serialize(treeState_, "treeState", 0);

	ar.serialize(propertyTreeStates_, "propertyTreeStates", 0);

	if(ar.isInput())
		setLibrary(bookmark_.libraryName());
}


void LibraryTabEditable::onPropertyChanged()
{
	xassert(library());

	LibraryTreeObject* object = safe_cast<LibraryTreeObject*>(editor_->tree()->focusedObject());
	if(object){
		object = object->onPropertyChanged();
		if(object){
			editor_->tree()->selectObject(object, true);
			if(object->isElement()){
				LibraryElementTreeObject* elementObject = safe_cast<LibraryElementTreeObject*>(object);
				bookmark_.setElementName(elementObject->elementName());
			}
		}
	}
	else
		xassert(0 && "Broken tree selection");
}

PlugBase* LibraryTabEditable::plug()
{
	if(plug_)
		return plug_;
	Serializer serializer = customEditor_->library()->editorElementSerializer(0, "", "", false);
	return plug_ = PlugFactory::instance().create(serializer.typeName(), true);
}

void LibraryTabEditable::onObjectSelected()
{
	xassert(library());
	LibraryTreeObject* object = tree()->focusedObject();

	if(!firstSelect_)
		saveTabState();
	else
		firstSelect_ = false;
	xassert(editor_);

	//xassert(strcmp(bookmark.libraryName(), bookmark_.libraryName()) == 0);
	bookmark_ = object->bookmark();
	if(customEditor_){
		bookmark_.setLibraryName(customEditor_->library()->name());
	}

	if(strcmp(bookmark_.elementName(), "") != 0){
		if(!plug_){
			editor_->propertyTree()->attach(object->getSerializer());
			loadTabState(false);
			editor_->propertyTree()->selectItemByPath(bookmark_.attribEditorPath());
		}
		else{
			plug_->assignFrom(object->getSerializer());
			plugObject_ = object;
		}
	}
	else{
		if(!plug_)
			editor_->propertyTree()->detach();
		else
			plug_->detach();
	}
}

void LibraryTabEditable::onObjectDeleted()
{
	plugObject_ = 0;
}

void LibraryTabEditable::onPropertySelected()
{
	ComboStrings path = editor_->propertyTree()->focusedPath();
	bookmark_.setAttribEditorPath(path);
}

bool LibraryTabEditable::isValid() const
{
	return library() != 0;
}

const char* LibraryTabEditable::title() const
{
	if(library()){
		return TRANSLATE(library()->editName());
	}
	else{
		return bookmark_.libraryName();
	}
}


void LibraryTabEditable::setBookmark(const LibraryBookmark& bookmark)
{
	focusObjectByElementName(editor_->tree(), bookmark.elementName(),
		                     bookmark.subElementName(), editor_->tree()->root());
	editor_->propertyTree()->selectItemByPath(bookmark.attribEditorPath());
	LibraryTabBase::setBookmark(bookmark);
}

// ----------------------------------------------------------------------------

class LibraryBookmarkTreeObject : public LibraryTreeObject, public sigslot::has_slots{
public:
	LibraryBookmarkTreeObject(LibraryEditor* editor, const LibraryBookmark& bookmark, const char* name, int index = -1)
	: LibraryTreeObject()
	, bookmark_(bookmark)
	, editor_(editor)
	, index_(index)
	{
		customEditor_ = LibraryCustomEditorFactory::instance().create(bookmark.libraryName());
		setText(name);
	}
	bool isBookmark() const{ return true; }

	void onSelect(ObjectsTree* tree){
		__super::onSelect(tree);
	}

	void onMenuFollowReference(ObjectsTree* tree)
	{
		onDoubleClick(tree);
	}

	void onMenuRemoveLibraryElement(ObjectsTree* tree)
	{
		std::string elementName = bookmark_.elementName();
		if(customEditor_->library()){
			LibraryTabSearch* tab = safe_cast<LibraryTabSearch*>(editor_->currentTab());
			SearchResultItems& items = tab->items();
			SearchResultItems::iterator it = std::find(items.begin(), items.end(), bookmark_);
			if(it != items.end()){
				items.erase(it);
			}
			else{
				xassert(0);
			}

			customEditor_->library()->editorElementErase(elementName.c_str());
			remove();
		}
	}

	void onRightClick(ObjectsTree *tree){

		kdw::PopupMenu m(100);

		kdw::PopupMenuItem& menu = m.root();

		menu.add(TRANSLATE("Следовать по ссылке..."), tree)
			.connect(this, &LibraryBookmarkTreeObject::onMenuFollowReference);
		//	.setDefault();		
		menu.addSeparator();
		menu.add(TRANSLATE("Удалить элемент библиотеки"), tree)
			.connect(this, &LibraryBookmarkTreeObject::onMenuRemoveLibraryElement);

		m.spawn(tree);
	}

	bool onDoubleClick(ObjectsTree* tree){
		editor_->openBookmark(bookmark_, true);
		return true; // значит что дерево изменили и этим объектом пользоваться нельзя
	}

	const LibraryBookmark& bookmark() const{ return bookmark_; }
	int elementIndex() const{ return index_; }
protected:
	LibraryBookmark bookmark_;
	LibraryEditor* editor_;
	int index_;
};

// ----------------------------------------------------------------------------

static void treeNodeFindElementReferences(TreePathes& result, const kdw::PropertyRow* const root,
										  const char* libraryName, const char* elementName, int index = 0, const TreePath& pathBase = TreePath())
{
	if(root->hidden())
		return;

	kdw::PropertyRowFactory& factory = kdw::PropertyRowFactory::instance();

	TreePath path(pathBase);
	if(root->parent() && root->parent()->isContainer())
		path.push_back(TreePathLeaf(0, index));
	else
		path.push_back(TreePathLeaf(root->name(), index));

	bool found = false;
	const char* referencedLibrary = factory.findReferencedLibrary(root->typeName());
	if(referencedLibrary && strcmp(referencedLibrary, libraryName) == 0){
		const LibraryLocation* location = root->location();
		if(location){
			LibraryBookmark bookmark;
			if(location->getBookmark(bookmark)){
				if(strcmp(bookmark.elementName(), elementName) == 0){
					result.push_back(path);
					found = true;
				}
			}
		}
	}
	if(!found){
		kdw::PropertyRow::const_iterator it;
		int i = 0;
		FOR_EACH(*root, it){
			const kdw::PropertyRow* row = safe_cast<const kdw::PropertyRow*>(&**it);
			treeNodeFindElementReferences(result, row, libraryName, elementName, i++, path);
		}
	}

}

static void treeNodeFindTreeNode(TreePathes& result, const kdw::PropertyRow* const root,
								 const kdw::PropertyRow& row, bool sameName, int index = 0, const TreePath& pathBase = TreePath())
{
	if(root->hidden())
		return;

	TreePath path(pathBase);
	if(root->parent() && root->parent()->isContainer())
		path.push_back(TreePathLeaf(0, index));
	else
		path.push_back(TreePathLeaf(root->name(), index));

	kdw::PropertyRow::const_iterator rit;
	bool found = false;
	FOR_EACH(row, rit){
		kdw::PropertyRow* r = safe_cast<kdw::PropertyRow*>(&**rit);
		if(*root == *r){
			result.push_back(path);
			found = true;
			break;
		}
	}
	if(!found){
		kdw::PropertyRow::const_iterator it;
		int i = 0;
		FOR_EACH(*root, it){
			const kdw::PropertyRow* root = safe_cast<const kdw::PropertyRow*>(&**it);
			treeNodeFindTreeNode(result, root, row, sameName, i++, path);
		}
	}
}

typedef std::set<std::string> LibraryElements;

static void treeNodeFindLibraryReferences(LibraryElements& result, const kdw::PropertyRow* root, const char* libraryName, int index = 0)
{
	kdw::PropertyRowFactory& factory = kdw::PropertyRowFactory::instance();

	bool found = false;
	const char* referencedLibrary = factory.findReferencedLibrary(root->typeName());
	if(referencedLibrary && referencedLibrary[0] && strcmp(referencedLibrary, libraryName) == 0){
		const LibraryLocation* location = root->location();
		if(location){
			LibraryBookmark bookmark;
			if(location->getBookmark(bookmark))
				result.insert(bookmark.elementName());
		}
	}
	if(!found){
		kdw::PropertyRow::const_iterator it;
		int i = 0;
		FOR_EACH(*root, it){
			const kdw::PropertyRow* row = safe_cast<const kdw::PropertyRow*>(&**it);
			if(!row->hidden())
				treeNodeFindLibraryReferences(result, row, libraryName, i++);
		}
	}

}

static void comboStringsFromTreePath(XBuffer& buf, ComboStrings& comboStrings, const TreePath& path)
{
	TreePath::const_iterator cit;
	FOR_EACH(path, cit){
		if(cit != path.begin()){
			if(!cit->name.empty()){
				buf < "/" < cit->name.c_str();
				comboStrings.push_back(cit->name);
			}
			else{
				XBuffer buf2(12, 1);
				buf2 <= cit->index;
				buf < "/" < buf2;
				comboStrings.push_back(static_cast<const char*>(buf2));
			}
		}
	}
};

void LibraryTabSearch::onSelect()
{
	editor_->tree()->root()->clear();
	SearchResultItems& items = this->items();
	SearchResultItems::iterator it;

	int index = 0;
	FOR_EACH(items, it){
		LibraryBookmark& bookmark = *it;

		XBuffer buf(256, 1);

		buf < bookmark.libraryName() < ", " < bookmark.elementName() < ": ";
		ComboStrings::const_iterator it;
		FOR_EACH(bookmark.attribEditorPath(), it){
			buf < "/";
			buf < it->c_str();
		}
		TreeObject* object = editor_->tree()->root()->add(new LibraryBookmarkTreeObject(editor_, bookmark, buf, index));
		if(index == selectedIndex_)
			object->focus();
		++index;
	}
}

void LibraryTabSearch::onMenuDeleteAll()
{
	SearchResultItems::iterator it;
	FOR_EACH(searchResultItems_, it){
		LibraryBookmark& bookmark = *it;
		EditorLibraryInterface* library = LibrariesManager::instance().find(bookmark.libraryName());
		if(int index = library->editorFindElement(bookmark.elementName()))
			library->editorElementErase(index);
	}
	searchResultItems_.clear();
    onSelect();
}

void LibraryTabSearch::onMenuConstruction(PopupMenuItem& root)
{
	root.add(TRANSLATE("Сохранить список как текст..."))
		.connect(this, &LibraryTabSearch::onMenuSaveAsText);
	root.addSeparator();
	root.add(TRANSLATE("Удалить все найденные элементы"))
		.connect(this, &LibraryTabSearch::onMenuDeleteAll);
}

void LibraryTabSearch::onObjectSelected()
{
	if(tree()->focusedObject()){
		bookmark_ = tree()->focusedObject()->bookmark();
		selectedIndex_ = atoi(bookmark_.elementName());
	}
}


ComboStrings makeLibraryPathFromTreePath(const TreePath& treePath,  const char* elementName)
{
	XBuffer buf(256, 1);
	buf < elementName;
	buf < ": ";

	ComboStrings path;
	comboStringsFromTreePath(buf, path, treePath);
	return path;
}


static void libraryFindTreeNode(EditorLibraryInterface& library, SearchResultItems& items, const kdw::PropertyRow& root,
								int& index, int totalCount, SearchProgressCallback progressCallback)
{
	int count = library.editorSize();
	for(int i = 0; i < count; ++i){
		kdw::PropertyTreeModel model;
		kdw::PropertyOArchive oa(&model);

		Serializer element = library.editorElementSerializer(i, "", "element", true);
		element.serialize(oa);

		TreePathes pathes;
		if(!model.root()->empty()){
			treeNodeFindTreeNode(pathes, model.root(), root, false);

			TreePathes::iterator pit;
			FOR_EACH(pathes, pit){
				items.push_back(LibraryBookmark(library.name(), library.editorElementName(i),
								makeLibraryPathFromTreePath(*pit, library.editorElementName(i))));
			}
			float percent = float(index) / float(totalCount);
			progressCallback(percent);
		}
		++index;
	}
}

static void libraryFindLibraryReferences(EditorLibraryInterface& library, LibraryElements& elements, const char* libraryName,
										 int& index, int totalCount, SearchProgressCallback progressCallback)
{	
	int count = library.editorSize();

	for(int i = 0; i < count; ++i){
		Serializer element = library.editorElementSerializer(i, "", "element", true);
		kdw::PropertyTreeModel model;
		kdw::PropertyOArchive oa(&model);
		element.serialize(oa);

		if(!model.root()->empty()){
			treeNodeFindLibraryReferences(elements, model.root(), libraryName);
			float percent = float(index) / float(totalCount);
			progressCallback(percent);
		}
		++index;
	}
}

static void libraryFindElementReferences(EditorLibraryInterface& library, SearchResultItems& items, const char* libraryName, const char* elementName, int& index, int totalCount, SearchProgressCallback progressCallback)
{
	int count = library.editorSize();
	for(int i = 0; i < count; ++i){
		Serializer element = library.editorElementSerializer(i, "", "element", true);
		kdw::PropertyTreeModel model;
		kdw::PropertyOArchive oa(&model);
		element.serialize(oa);

		TreePathes pathes;
		if(!model.root()->empty()){
			treeNodeFindElementReferences(pathes, model.root(), libraryName, elementName);

			TreePathes::iterator pit;
			FOR_EACH(pathes, pit){
				XBuffer buf(256, 1);
				buf < library.editorElementName(i);
				buf < ": ";

				ComboStrings path;
				comboStringsFromTreePath(buf, path, *pit);

				LibraryBookmark bookmark(library.name(), library.editorElementName(i), path);
				items.push_back(bookmark);
			}
			float percent = float(index) / float(totalCount);
			progressCallback(percent);
		}
		++index;
	}
}

// ---------------------------------------------------------------------------

void LibraryTabSearch::findUnused(const char* libraryName, SearchProgressCallback progress)
{
	EditorLibraryInterface* library = LibrariesManager::instance().find(libraryName);
	if(!library)
		return;

	int totalCount = 0;
	items().clear();

	LibrariesManager::Libraries& libraries = LibrariesManager::instance().libraries();
	LibrariesManager::Libraries::iterator it;

	FOR_EACH(libraries, it){
		EditorLibraryInterface& library = it->second();
		totalCount += library.editorSize();
	}

	LibraryElements usedElements;

	int index = 0;
	FOR_EACH(libraries, it){
		EditorLibraryInterface& library = it->second();
		libraryFindLibraryReferences(library, usedElements, libraryName, index, totalCount, progress);
	}

	int count = library->editorSize();
	for(int i = 0; i < count; ++i){
		const char* name = library->editorElementName(i);
		LibraryElements::iterator it = usedElements.find(name);
		if(it == usedElements.end()){
			XBuffer buf(256, 1);
			buf < library->editorElementName(i);
			buf < ": ";

			LibraryBookmark bookmark(libraryName, name, ComboStrings());
			items().push_back(bookmark);
		}
	}
	if(progress)
		progress(0.0f);
}

void LibraryTabSearch::findLibraryReference(const char* libraryName, const char* elementName, SearchProgressCallback progress)
{
	int totalCount = 0;
	items().clear();

	LibrariesManager::Libraries& libraries = LibrariesManager::instance().libraries();
	LibrariesManager::Libraries::iterator it;

	FOR_EACH(libraries, it){
		if(it->second){
			EditorLibraryInterface& library = it->second();
			totalCount += library.editorSize();
		}
	}

	int index = 0;
	FOR_EACH(libraries, it){
		if(it->second){
			EditorLibraryInterface& library = it->second();
			libraryFindElementReferences(library, items(), libraryName, elementName, index, totalCount, progress);
		}
	}
	if(progress)
		progress(0.0f);
}

void LibraryTabSearch::findTreeNode(const kdw::PropertyRow& root, bool sameName, SearchProgressCallback progress)
{
	int totalCount = 0;
	items().clear();

	LibrariesManager::Libraries& libraries = LibrariesManager::instance().libraries();
	LibrariesManager::Libraries::iterator it;

	FOR_EACH(libraries, it){
		EditorLibraryInterface& library = it->second();
		totalCount += library.editorSize();
	}

	int index = 0;
	FOR_EACH(libraries, it){
		EditorLibraryInterface& library = it->second();
		libraryFindTreeNode(library, items(), root, index, totalCount, progress);
	}
	if(progress)
		progress(0.0f);
}

void LibraryTabSearch::serialize(Archive& ar)
{
	ar.serialize(searchResultItems_, "searchResultItems", 0);
}

LibraryTabSearch::LibraryTabSearch(LibraryEditor* editor, const LibraryBookmark& bookmark)
: LibraryTabBase(editor, bookmark)
, selectedIndex_(-1)
{
	setBookmark(bookmark);
	title_ = "Результаты поиска";
	XBuffer buf;
	buf < "Search_" <= reinterpret_cast<unsigned int>(this);
	bookmark_.setLibraryName(buf);
}

void LibraryTabSearch::setBookmark(const LibraryBookmark& bookmark)
{
	selectedIndex_ = atoi(bookmark.elementName());
	LibraryTabBase::setBookmark(bookmark);
}

void LibraryTabSearch::onMenuSaveAsText()
{
	/* FIXME
	string result;
	const char* filter = "(*.txt)|*.txt||";
	CFileDialog dlg(FALSE, filter, "search_results.txt", OFN_LONGNAMES|OFN_HIDEREADONLY|OFN_NOCHANGEDIR, filter);
	dlg.m_ofn.lpstrTitle = TRANSLATE("Сохранить результаты поиска...");
	dlg.m_ofn.lpstrInitialDir = ".";
	int response = dlg.DoModal();
	if(response == IDOK){
		std::string fileName = dlg.GetPathName();
		XStream file(fileName.c_str(), XS_OUT);

		SearchResultItems::iterator it;
		FOR_EACH(items(), it){
			LibraryBookmark& bookmark = *it;
			file.write(bookmark.elementName(), strlen(bookmark.elementName()));
			file.write("\n");
		}
		file.close();
	}
	else{
	}
	*/
}



// ----------------------------------------------------------------------------

LibraryCustomEditor::LibraryCustomEditor(EditorLibraryInterface* library)
: library_(library)
{

}

LibraryElementTreeObject* LibraryCustomEditor::createTreeObject(const char* elementName)
{
	xassert(library_);
	return new LibraryElementTreeObject(this, elementName);
}


LibraryGroupTreeObject* LibraryCustomEditor::createGroupTreeObject(const char* groupName)
{
	xassert(library_);
	return new LibraryGroupTreeObject(this, groupName);
}


Serializer LibraryCustomEditor::serializeableByBookmark(const LibraryBookmark& bookmark, bool editOnly)
{
	xassert(library_);
	return library_->editorElementSerializer(bookmark.elementName(), "element", "element", editOnly);
}

LibraryCustomEditor::~LibraryCustomEditor()
{

}

}

// ----------------------------------------------------------------------------

void LibraryBookmark::serialize(Archive& ar)
{
	ar.serialize(libraryName_, "libraryName", 0);
	ar.serialize(elementName_, "elementName", 0);
	ar.serialize(subElementName_, "subElementName", 0);
	ar.serialize(attribEditorPath_, "attribEditorPath", 0);
}
