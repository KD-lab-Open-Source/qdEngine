#include "StdAfx.h"

#include "kdw/Tree.h"
#include "kdw/PropertyTree.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/PopupMenu.h"
#include "kdw/_PropertyRowBuiltin.h"
#include "Serialization/SerializationFactory.h"
#include "Serialization/LibraryWrapper.h"
#include "Serialization/BinaryArchive.h"
#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"

namespace kdw{

// ---------------------------------------------------------------------------

void PropertyRowFactory::buildMap()
{
    if(libraryNameByReferenceType_.size() != instanceFuncByReferenceType_.size()){
		InstanceFuncByReferenceType::iterator it;
        FOR_EACH(instanceFuncByReferenceType_, it){
			const char* referenceTypeName = it->first.c_str();
			LibraryInstanceFunc instanceFunc = it->second;
			EditorLibraryInterface& library = instanceFunc();
			libraryNameByReferenceType_[referenceTypeName] = library.name();
        }
    }
}

bool PropertyRowFactory::addReference(const char* referenceTypeName, LibraryInstanceFunc instanceFunc)
{
	instanceFuncByReferenceType_[referenceTypeName] = instanceFunc;
	return true;
}

const char* PropertyRowFactory::findReferencedLibrary(const char* referenceTypeName) const
{
	LibraryNameByReferenceType::const_iterator it = libraryNameByReferenceType_.find(referenceTypeName);
	if(it == libraryNameByReferenceType_.end())
		return 0;
	return it->second.c_str();
}

// ---------------------------------------------------------------------------
KDW_API void* PropertyRowArg::object_(0);
KDW_API int PropertyRowArg::size_(0);
KDW_API const char* PropertyRowArg::name_(0);
KDW_API const char* PropertyRowArg::nameAlt_(0);
KDW_API const char* PropertyRowArg::typeName_(0);

// ---------------------------------------------------------------------------

ConstStringList* PropertyRow::constStrings_ = 0;

PropertyRow::PropertyRow(const char* name, const char* nameAlt, const char* typeName)
: name_(name)
, typeName_(typeName)
, updated_(false)
, pulledUp_(false)
, inDigest_(false)
, readOnly_(false)
, fullRow_(false)
, multiValue_(false)
{
	hidden_ = false;
	height_ = ROW_DEFAULT_HEIGHT;
	ASSERT(typeName);
	setNameAlt(nameAlt ? TRANSLATE(nameAlt) : 0);
}

PropertyRow::PropertyRow(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: name_(name)
, typeName_(typeName)
, updated_(false)
, pulledUp_(false)
, inDigest_(false)
, readOnly_(false)
, fullRow_(false)
, multiValue_(false)
{
	setNameAlt(nameAlt ? TRANSLATE(nameAlt) : 0);
}

const char* PropertyRow::nameAltUndecorated() const
{
	const char* name = nameAlt_;
	while(true){
		if(name[0] == '&' || name[0] == '!' || name[0] == '^' || name[0] == '_' || name[0] == '<' || name[0] == '=')
			++name;
		else
			break;
	}
	return name;
}

void PropertyRow::setNameAlt(const char* nameAlt)
{
	hidden_ = false;
	if(nameAlt){
		bool omitNameAlt = false;
		const char* ptr = nameAlt;
		while(true){
			if(*ptr == '^')
				pulledUp_ = true;
			else if(*ptr == '<'){
				fullRow_ = true;
				omitNameAlt = true;
			}
			else if(*ptr == '&')
				inDigest_ = true;
			else if(*ptr == '!')
				readOnly_ = true;
			else
				break;
			++ptr;
		}
		nameAlt_ = nameAlt;
		if(*ptr == '\0' && !omitNameAlt)
			hidden_ = true;
	}
	else{
		nameAlt_ = "";
		hidden_ = true;
	}
}

PropertyRow* PropertyRow::addBefore(PropertyRow* row, PropertyRow* before)
{
	if(before == 0)
		children_.push_back(row);
	else{
		iterator it = std::find(children_.begin(), children_.end(), before);
		if(it != children_.end())
			children_.insert(it, row);
		else
			children_.push_back(row);
	}
	row->setParent(this);
	return row;
}

void PropertyRow::digestAppend(const char* text)
{
	if(!digest_.empty())
		digest_ += ", ";
	digest_ += text;
}

void PropertyRow::digestReset()
{
	digest_.clear();
}

std::string PropertyRow::valueAsString() const
{
	if(!digest_.empty())
		return std::string("[ ") + digest_ + " ]";
	else
		return "";
}

PropertyRow* PropertyRow::cloneChildren(PropertyRow* result, const PropertyRow* source) const
{
	PropertyRow::const_iterator it;
	for(it = source->begin(); it != source->end(); ++it){
		const PropertyRow* sourceChild = static_cast<PropertyRow*>(&**it);
		result->add(sourceChild->clone());
	}

	return result;
}

static void drawStaticText(HDC dc, PropertyRow* row, PropertyRow* hostRow, const RECT& widgetRect)
{
	RECT rt = widgetRect;
	HFONT oldFont = (HFONT)::SelectObject(dc, Win32::defaultFont());
	::InflateRect(&rt, -3, -2);
	int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
	COLORREF textColor = GetSysColor(hostRow->selected() ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT);
	COLORREF oldColor = ::SetTextColor(dc, textColor);
	std::string text = row->valueAsString();
	::DrawText(dc, text.c_str(), strlen(text.c_str()), &rt, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	::SetBkMode(dc, oldBkMode);
	::SetTextColor(dc, oldColor);
	::SelectObject(dc, oldFont);
}

void PropertyRow::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	if(pulledRow_)
		pulledRow_->redraw(dc, iconRect, widgetRect, floorRect, this);
	else if(Custom){
		if(multiValue())
			Win32::drawEdit(dc, widgetRect, " ... ", Win32::defaultFont(), false, true);
		else if(readOnly())
			drawStaticText(dc, this, hostRow, widgetRect);
		else
			Win32::drawEdit(dc, widgetRect, valueAsString().c_str(), Win32::defaultFont(), false);
	}
}

void PropertyRow::serialize(Archive& ar)
{
	serializeValue(ar);

	ar.serialize(ConstStringWrapper(constStrings_, name_), "name", "&name");
	ar.serialize(ConstStringWrapper(constStrings_, nameAlt_), "nameAlt", "nameAlt");
	ar.serialize(ConstStringWrapper(constStrings_, typeName_), "type", "type");
	ar.serialize(reinterpret_cast<std::vector<ShareHandle<PropertyRow> >&>(children_), "children", "!^children");	
	if(ar.isInput()){
		setNameAlt(nameAlt_);
		TreeRow::iterator it;
		for(it = begin(); it != end(); ){
			TreeRow* row = *it;
			if(row){
				row->setParent(this);
				++it;
			}
			else{
				it = erase(it);
			}
		}
	}
}

void PropertyRow::childrenUpdated()
{
	digestReset();
	TreeRow::iterator it;
	pulledRow_ = 0;
	for(it = begin(); it != end(); ++it){
		PropertyRow* row = static_cast<PropertyRow*>(&**it);
		if(row->inDigest())
			digestAppend(row->valueAsString().c_str());
		if(row->pulledUp()){
			if(row->isContainer()){
				if(!pulledContainer())
					setPulledContainer(row);
			}
			else if(!pulledRow())
				setPulledRow(row);
		}
	}
}

bool PropertyRow::onActivateWidget(PropertyTree* tree, PropertyRow* hostRow)
{
	if(pulledRow())
		return pulledRow()->onActivateWidget(tree, this);
	return tree->spawnWidget(this, hostRow);
}


void PropertyRow::adjust(int height, const Tree* _tree)
{
	const PropertyTree* tree = (const PropertyTree*)(_tree);
	if(isContainer() && pulledUp())
		height_ = 3;
	else
		height_ = (tree->hideUntranslated() && hidden_) ? 0 : height;	
}



PropertyRow* PropertyRow::find(const char* name, const char* nameAlt, const char* typeName, bool skipUpdated)
{
	iterator it;
	FOR_EACH(children_, it){
		PropertyRow* row = static_cast<PropertyRow*>(&**it);
		if(((row->name() == name) || strcmp(row->name(), name) == 0) &&
		   ((nameAlt == 0) || (row->nameAlt() != 0 && strcmp(row->nameAlt(), nameAlt) == 0)) &&
		   ((typeName == 0) || (row->typeName() != 0 && strcmp(row->typeName(), typeName) == 0)) &&
		   (!skipUpdated || !row->updated()))
			return row;
	}
	return 0;

}
const PropertyRow* PropertyRow::find(const char* name, const char* nameAlt, const char* typeName, bool skipUpdated) const
{
	return const_cast<PropertyRow* const>(this)->find(name, nameAlt, typeName, skipUpdated);
}

bool PropertyRow::onKeyDown(PropertyTree* tree, sKey key)
{
	if(parent() && parent()->isContainer()){
		PropertyRowContainer* container = safe_cast<PropertyRowContainer*>(parent());
		if(key == sKey(VK_DELETE)){
			container->onMenuChildRemove(this, tree->model());
			return true;
		}
		if(key == sKey(VK_INSERT | sKey::SHIFT)){
			container->onMenuChildInsertBefore(this, tree);
			return true;
		}
	}
	return false;
}

bool PropertyRow::onContextMenu(PopupMenuItem &root, PropertyTree* tree)
{
	if(parent() && parent()->isContainer()){
		if(!root.empty())
			root.addSeparator();
		PropertyRowContainer* container = safe_cast<PropertyRowContainer*>(parent());
		root.add(TRANSLATE("Добавить перед элементом"), this, tree)
			.connect(container, &PropertyRowContainer::onMenuChildInsertBefore)
			.setHotkey(sKey(VK_INSERT | sKey::SHIFT))
			.enable(!container->readOnly());
		root.add(TRANSLATE("Удалить"), this, tree->model())
			.connect(container, &PropertyRowContainer::onMenuChildRemove)
			.setHotkey(sKey(VK_DELETE))
			.enable(!container->readOnly());
	}
	if(tree->canSearch()){
		if(!root.empty())
			root.addSeparator();
		root.add(TRANSLATE("Искать в библиотеках..."), static_cast<PropertyRow*>(this))
			.connect(tree, &PropertyTree::onSearch);
	}
	{
		if(!root.empty())
			root.addSeparator();
		if(tree->rowHasVisibleChildren(this)){
			root.add(expanded() ? TRANSLATE("Свернуть") : TRANSLATE("Развернуть"), static_cast<TreeRow*>(this), !expanded())
				.connect(static_cast<Tree*>(tree), &Tree::expandRow);
		}
		root.add(TRANSLATE("Развернуть все"), static_cast<TreeRow*>(this))
				.connect(static_cast<Tree*>(tree), &Tree::expandAll);
	}

	return !root.empty();
}

std::string PropertyRow::rowPath(const char* separator) const
{
	typedef std::vector<const PropertyRow*> Rows;
	Rows rows;

	const PropertyRow* row = this;
	while(row){
		rows.insert(rows.begin(), row);
		row = row->parent();
	}

	Rows::iterator it = rows.begin();
	std::string result = "";
	for(; it != rows.end(); ++it){
		const PropertyRow* row = *it;
		if(row->nameAlt()[0] != '\0'){
			if(!result.empty())
				result += separator;
			result += row->nameAlt();;
		}
	}
	return result;
}


bool PropertyRow::operator==(const PropertyRow& rhs) const
{
	if(strcmp(typeName(), rhs.typeName()) != 0)
		return false;
	if(size() != rhs.size())
		return false;

	BinaryOArchive lhsValue;
	const_cast<PropertyRow*>(this)->serializeValue(lhsValue);
	BinaryOArchive rhsValue;
	const_cast<PropertyRow*>(&rhs)->serializeValue(rhsValue);
	if(lhsValue != rhsValue)
		return false;

	PropertyRow::const_iterator lit = begin();
	PropertyRow::const_iterator rit = rhs.begin();
	for(; lit != end(); ++lit, ++rit){
		PropertyRow* l = safe_cast<PropertyRow*>(&**lit);
		PropertyRow* r = safe_cast<PropertyRow*>(&**rit);
		if(strcmp(l->name(), r->name()) != 0)
			return false;
		if(strcmp(l->nameAlt(), r->nameAlt()) != 0)
			return false;
		if(*l != *r)
			return false;
	}
	return true;
}
/*
void PropetyRow::setParent(TreeRow* _row)
{
	__super::setParent(_row);
	if(hidden_)
		expanded_
}
*/

bool PropertyRow::canBeToggled() const
{
	return !(isContainer() && pulledUp());
}

bool PropertyRow::canBeDragged() const
{
	if(parent()){
		if(parent()->isContainer())
			return true;
	}
	return false;
}

bool PropertyRow::canBeDroppedOn(const TreeRow* _row, const TreeRow* _beforeChild, const Tree* tree, bool direct) const
{
	ASSERT(_row);
	const PropertyRow* row = safe_cast<const PropertyRow*>(_row);
	const PropertyRow* beforeChild = safe_cast<const PropertyRow*>(_beforeChild);
	const PropertyTreeModel* model = safe_cast<const PropertyTreeModel*>(tree->model());

	if(direct && row->pulledUp())
		return false;

	const PropertyRow* destRow = row;
	if(row->pulledContainer())
		destRow = row->pulledContainer();

	if(destRow->isContainer()){
		const PropertyRowContainer* container = safe_cast<const PropertyRowContainer*>(destRow);
				
		if((container->hasFixedSize() || container->readOnly()) && parent() != destRow)
			return false;

		if(beforeChild && beforeChild->parent() != destRow)
			return false;

		const PropertyRow* defaultRow = container->defaultRow(model);
		if(defaultRow && strcmp(defaultRow->typeName(), typeName()) == 0)
			return true;
	}
	return false;	
}

void PropertyRow::dropInto(TreeRow* _destination, TreeRow* _beforeChild, Tree* tree)
{
	ShareHandle<PropertyRow> ref(this);

	PropertyRow* destination = safe_cast<PropertyRow*>(_destination);
	PropertyRow* beforeChild = safe_cast<PropertyRow*>(_beforeChild);
	PropertyTreeModel* model = safe_cast<PropertyTreeModel*>(tree->model());
	if(destination->pulledContainer())
		destination = destination->pulledContainer();
	if(destination->isContainer()){
		PropertyRowContainer* container = safe_cast<PropertyRowContainer*>(destination);
		PropertyRow* oldParent = parent();
		oldParent->erase(this);
		destination->addBefore(this, beforeChild);
		model->setFocusedRow(this);
		model->selectRow(this, true);
		model->rowChanged(oldParent);
		if(oldParent != destination)
			model->rowChanged(this);
		
	}
}

void PropertyRow::intersect(const PropertyRow* row)
{
	if(empty())
		return;
	iterator it;
	for(it = children_.begin(); it != children_.end();){
		PropertyRow* rowIt = static_cast<PropertyRow*>(&**it);
		if(!rowIt->hidden() || rowIt->pulledUp()){
			const PropertyRow* rowSub = row->find(rowIt->name(), rowIt->nameAlt(), rowIt->typeName());
			if(rowSub){
				if(rowSub->empty() && !rowSub->isContainer()){
					if(*rowIt != *rowSub)
						rowIt->setMultiValue(true);
					++it;
					continue;
				}
				else if(!rowSub->isLeaf() && !rowSub->isContainer()){
					rowIt->intersect(rowSub);
					++it;
					continue;
				}
			}
		}
		it = children_.erase(it);
	}
}

/*
void TreeNode::intersect(const TreeNode* node)
{
	if (empty ())
		return;

	for (iterator it = children_.begin(); it != children_.end(); ) {
		if((*it)->hidden()){
			it = children_.erase (it);
		}
		else{
			TreeNode* node_it = *it;
			const TreeNode* node_sub = node->find (node_it->name(), node_it->type());
			if(node_sub){
				if(node_sub->empty () && node_sub->editType() != TreeNode::VECTOR){
					if(strcmp (node_sub->value(), node_it->value()) != 0){
						node_it->setValue ("");
						node_it->setUnedited (true);
					}
					++it;
				}
				else{
					if(node_sub->editor() == 0 && node_sub->editType() != TreeNode::VECTOR && node_sub->hidden() == false){
						node_it->intersect (node_sub);
						++it;
					}
					else{
						it = children_.erase(it);
					}
				}
			}
			else{
				it = children_.erase(it);
			}
		}
	}
	return;
}
*/

REGISTER_CLASS(PropertyRow, PropertyRow, "Структура");
// ---------------------------------------------------------------------------

PropertyRowWidget::~PropertyRowWidget()
{
	if(actualWidget())
		actualWidget()->_setParent(0);
}

FORCE_SEGMENT(PropertyRowGradient)
FORCE_SEGMENT(PropertyRowFileSelector)
FORCE_SEGMENT(PropertyRowHotkey)
FORCE_SEGMENT(PropertyRowRanged)
FORCE_SEGMENT(PropertyRowComboListColor)
FORCE_SEGMENT(PropertyRowDecorators)
FORCE_SEGMENT(PropertyRowColor)
}

