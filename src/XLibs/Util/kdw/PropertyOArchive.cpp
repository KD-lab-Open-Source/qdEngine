#include "StdAfx.h"
#include <math.h>

#include "PropertyOArchive.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/PropertyTree.h"

#include "kdw/_PropertyRowBuiltin.h"
#include "kdw/ConstStringList.h"
#include "kdw/PopupMenu.h"
#include "kdw/ComboBox.h"
#include "kdw/CheckComboBox.h"
#include "kdw/Entry.h"
#include "ClassMenu.h"

#include "kdw/Win32/Window.h"
#include "kdw/Win32/Drawing.h"

#include "Serialization/Serialization.h"
#include "Serialization/SerializationFactory.h"
#include "Serialization/EnumDescriptor.h"
#include "UnicodeConverter.h"

#ifndef TRANSLATE
# define TRANSLATE(x) x
#endif

namespace kdw{

void ClassMenuItemAdder::generateMenu(PopupMenuItem& createItem, const ComboStrings& comboStrings, bool includeEmpty)
{
	if(includeEmpty)
		(*this)(createItem, -1, TRANSLATE("[ Пустое значение ]"));
	ComboStrings::const_iterator it;
	int index = 0;
	FOR_EACH(comboStrings, it){
		ComboStrings path;
		splitComboList(path, TRANSLATE(it->c_str()), '\\');
		int level = 0;
		PopupMenuItem* item = &createItem;
		for(int level = 0; level < int(path.size()); ++level){
			const char* leaf = path[level].c_str();
			if(level == path.size() - 1){
				(*this)(*item, index++, leaf);
			}
			else{
				if(PopupMenuItem* subItem = item->find(leaf))
					item = subItem;
				else
					item = &item->add(leaf);
			}
		}
	}
}

// ---------------------------------------------------------------------------
REGISTER_CLASS(PropertyRow, PropertyRowContainer, "Список");

PropertyRowContainer::PropertyRowContainer(const char* name = "", const char* nameAlt = "", const char* typeName = "", const char* elementTypeName = "", bool readOnly = false)
: PropertyRow(name, nameAlt, typeName)
, fixedSize_(readOnly)
, elementTypeName_(elementTypeName)
{
	if(pulledUp())
		_setExpanded(true);		
}

struct ClassMenuItemAdderRowContainer : ClassMenuItemAdder{
	ClassMenuItemAdderRowContainer(PropertyRowContainer* row, PropertyTree* tree, bool insert = false) 
	: row_(row)
	, tree_(tree)
	, insert_(insert) {}    

	void operator()(PopupMenuItem& root, int index, const char* text){
		if(!insert_)
			root.add(text, index, tree_).connect(row_, &PropertyRowContainer::onMenuAppendPointerByIndex);
	}
protected:
	PropertyRowContainer* row_;
	PropertyTree* tree_;
	bool insert_;
};

bool PropertyRowContainer::onKeyDown(PropertyTree* tree, sKey key)
{
	if(key == sKey(VK_DELETE | sKey::SHIFT)){
		onMenuClear(tree->model());
		return true;
	}
	if(key == sKey(VK_INSERT)){
		onMenuAppendElement(tree);
		return true;
	}
	return __super::onKeyDown(tree, key);
}

bool PropertyRowContainer::onContextMenu(PopupMenuItem& root, PropertyTree* tree)
{
	if(!root.empty())
		root.addSeparator();

	PopupMenuItem& createItem = root.add(TRANSLATE("Добавить"), tree)
		.connect(this, &PropertyRowContainer::onMenuAppendElement)
		.setHotkey(sKey(VK_INSERT));

	if(!readOnly()){
		PropertyRow* row = defaultRow(tree->model());
		if(row && row->isPointer()){
			PropertyRowPointer* pointerRow = safe_cast<PropertyRowPointer*>(row);
			ClassMenuItemAdderRowContainer(this, tree).generateMenu(createItem, tree->model()->typeComboStrings(pointerRow->typeName()));
		}
		createItem.enable(true);
	}
	else
		createItem.enable(false);

	root.add(TRANSLATE("Удалить все"), tree->model()).connect(this, &PropertyRowContainer::onMenuClear)
		.setHotkey(sKey(VK_DELETE | sKey::SHIFT))
		.enable(!readOnly());
	return PropertyRow::onContextMenu(root, tree);
}


void PropertyRowContainer::onMenuClear(PropertyTreeModel* model)
{
	clear();
	model->rowChanged(this);
}

PropertyRow* PropertyRowContainer::defaultRow(PropertyTreeModel* model)
{
	PropertyRow* defaultType = model->defaultType(elementTypeName_);
	//ASSERT(defaultType);
	//ASSERT(defaultType->numRef() == 1);
	return defaultType;
}

const PropertyRow* PropertyRowContainer::defaultRow(const PropertyTreeModel* model) const
{
	const PropertyRow* defaultType = model->defaultType(elementTypeName_);
	return defaultType;
}


void PropertyRowContainer::onMenuAppendElement(PropertyTree* tree)
{
	PropertyRow* defaultType = defaultRow(tree->model());
	if(!defaultType){
		xassert(defaultType);
		return;
	}
	PropertyRow* clonedRow = defaultType->clone();
	clonedRow->setFullRow(true);
	add(clonedRow);
	if(expanded()){
		tree->model()->deselectAll();
		tree->model()->selectRow(clonedRow, true);
		tree->model()->setFocusedRow(clonedRow);
	}
	tree->expandRow(clonedRow);
	if(clonedRow->activateOnAdd()){
		tree->update(); // только для обновления indent-а новой строчки
		if(clonedRow->onActivate(tree))
			return;
	}
	tree->model()->rowChanged(clonedRow);
}

void PropertyRowContainer::onMenuAppendPointerByIndex(int index, PropertyTree* tree)
{
	PropertyRow* defaultType = defaultRow(tree->model());
	PropertyRow* clonedRow = defaultType->clone();
	clonedRow->setFullRow(true);
	add(clonedRow);
	PropertyRowPointer* pointer = safe_cast<PropertyRowPointer*>(clonedRow);
	if(expanded()){
		tree->model()->deselectAll();
		tree->model()->selectRow(clonedRow, true);
		tree->model()->setFocusedRow(clonedRow);
	}
	tree->expandRow(pointer);
	pointer->onMenuCreateByIndex(index, tree);
}

void PropertyRowContainer::onMenuChildInsertBefore(PropertyRow* child, PropertyTree* tree)
{
	PropertyRow* defaultType = tree->model()->defaultType(elementTypeName_);
	if(!defaultType)
		return;
	PropertyRow* clonedRow = defaultType->clone();
	clonedRow->setFullRow(true);
	addBefore(clonedRow, child);
	tree->model()->setFocusedRow(clonedRow);
	if(clonedRow->activateOnAdd()){
		tree->update(); // только для обновления indent-а новой строчки
		if(clonedRow->onActivate(tree))
			return;
	}
	tree->model()->rowChanged(clonedRow);
}

void PropertyRowContainer::onMenuChildRemove(PropertyRow* child, PropertyTreeModel* model)
{
	erase(child);
	model->rowChanged(this);
}

void PropertyRowContainer::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	RECT textRect = widgetRect;
	textRect.left += 4;
	textRect.right -= 4;

	HFONT oldFont = (HFONT)::SelectObject(dc, Win32::defaultFont());
	int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
	char buffer[14];
	sprintf(buffer, TRANSLATE("%i шт."), int(size()));
	const char* text = buffer;
	COLORREF textColor = selected() ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_BTNTEXT);
	COLORREF oldTextColor = ::SetTextColor(dc, textColor);
	DrawText(dc, text, strlen(text), &textRect, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
	::SetTextColor(dc, oldTextColor);
	::SelectObject(dc, oldFont);
	::SetBkMode(dc, oldBkMode);
}


void PropertyRowContainer::serializeValue(Archive& ar)
{
	ar.serialize(ConstStringWrapper(constStrings_, elementTypeName_), "elementTypeName", "ElementTypeName");
	ar.serialize(fixedSize_, "fixedSize", "fixedSize");
}

// ---------------------------------------------------------------------------

class AutoDropComboBox : public ComboBox{
public:
	void show(){
		ComboBox::show();
		showDropDown();
	}
};

// ---------------------------------------------------------------------------

class PropertyRowWidgetEnum : public PropertyRowWidget, public sigslot::has_slots{
public:
	PropertyRowWidgetEnum(PropertyRowEnum* row, PropertyTreeModel* model)
	: PropertyRowWidget(row, model)
	, comboBox_(new AutoDropComboBox())
	{
		comboBox_->set(row->descriptor()->comboListAlt(), row->descriptor()->nameAlt(row->value()), true);
		comboBox_->signalEdited().connect(this, &PropertyRowWidgetEnum::onChange);
	}
	~PropertyRowWidgetEnum()
	{
	}

	void onChange(){
		PropertyRowEnum* row = safe_cast<PropertyRowEnum*>(this->row());
		row->setVisibleIndex(comboBox_->selectedIndex());
		model()->rowChanged(row);
	}
	void commit(){}
	Widget* actualWidget() { return comboBox_; }
protected:
	ShareHandle<ComboBox> comboBox_;
};

REGISTER_CLASS(PropertyRow, PropertyRowEnum, "Enum");

PropertyRowEnum::PropertyRowEnum(const char* name, const char* nameAlt, int value, const EnumDescriptor* descriptor)
: PropertyRow(name, nameAlt, descriptor ? descriptor->typeName() : "")
, value_(value)
, descriptor_(descriptor)
{
}

void PropertyRowEnum::setVisibleIndex(int visibleIndex)
{
	const ComboStrings& strings = descriptor()->comboStringsAlt();
	int index = 0;
	int vi = 0;
	while(index < int(strings.size())){
		if(strings[index].c_str()[0] != '\0'){
			if(vi == visibleIndex){
				value_ = descriptor()->keyByNameAlt(strings[index].c_str());
				return;
			}
			++vi;
		}
        ++index;
	}
	ASSERT(0);
}

bool PropertyRowEnum::assignTo(void* object, int size)
{
	ASSERT(size == sizeof(int));
	*reinterpret_cast<int*>(object) = value();
	return true;
}

int PropertyRowEnum::value() const
{
	return value_;
}

PropertyRowWidget* PropertyRowEnum::createWidget(PropertyTreeModel* model)
{
	return readOnly_ ? 0 : new PropertyRowWidgetEnum(this, model);
}

void PropertyRowEnum::setValue(int value)
{
	value_ = value;
}

/*
void PropertyRowEnum::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	const char* text = descriptor_ ? descriptor_->nameAlt(value_) : "";
	Win32::drawComboBox(dc, widgetRect, text, Win32::defaultFont());
}
*/

void PropertyRowEnum::serializeValue(Archive& ar)
{
	ar.serialize(value_, "value", "Значение");
}

void PropertyRowEnum::serialize(Archive& ar)
{
	__super::serialize(ar);

	if(ar.isInput()){
		descriptor_ = EnumDescriptor::descriptorByTypeName(typeName_);
		if(!descriptor_){
			//xassert(0);
		}
	}	
}

std::string PropertyRowEnum::valueAsString() const{
	return descriptor_ ? descriptor_->nameAlt(value_) : "";
}

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

class AutoDropCheckComboBox : public CheckComboBox{
public:
	void show(){
		CheckComboBox::show();
		showDropDown();
	}
};

class PropertyRowWidgetBitVector : public PropertyRowWidget, public sigslot::has_slots{
public:
	PropertyRowWidgetBitVector(PropertyRowBitVector* row, PropertyTreeModel* model)
	: PropertyRowWidget(row, model)
	, comboBox_(new AutoDropCheckComboBox())
	{
		comboBox_->set(row->descriptor()->comboListAlt(), row->valueAlt());
		comboBox_->setDropDownHeight(50);
		comboBox_->signalEdited().connect(this, &PropertyRowWidgetBitVector::onChange);
		//comboBox_->showDropDown();
	}
	~PropertyRowWidgetBitVector() {}

	void onChange(){
		PropertyRowBitVector* row = safe_cast<PropertyRowBitVector*>(this->row());
		row->setValueAlt(comboBox_->value());
		//row->setIndex(comboBox_->selectedIndex());
		model()->rowChanged(row);
	}
	void commit(){
	}
	Widget* actualWidget() { return comboBox_; }
protected:
	ShareHandle<CheckComboBox> comboBox_;
};

REGISTER_CLASS(PropertyRow, PropertyRowBitVector, "BitVector");

PropertyRowBitVector::PropertyRowBitVector(const char* name, const char* nameAlt, int flags, const EnumDescriptor* descriptor)
: PropertyRow(name, nameAlt, descriptor ? descriptor->typeName() : "")
, descriptor_(descriptor)
, flags_(flags)
{
	if(descriptor){
		typeName_ = descriptor->typeName();
		//value_    = descriptor->nameCombination(flags);
		valueAlt_ = descriptor->nameAltCombination(flags);
	}
	else{
		typeName_ = "";
		//value_ = "";
		valueAlt_ = "";
	}
}

bool PropertyRowBitVector::assignTo(void* object, int size)
{
	ASSERT(size == sizeof(int));
	*reinterpret_cast<int*>(object) = flags();
	return true;
}

void PropertyRowBitVector::setValueAlt(const char* value)
{
	ComboStrings strings;
	ComboStrings::iterator sit;
	splitComboList(strings, value);
	int flags = 0;
	for(sit = strings.begin(); sit != strings.end(); ++sit){
		flags |= descriptor_->keyByNameAlt(sit->c_str());
	}
	flags_ = flags;
	valueAlt_ = value;
}


PropertyRowWidget* PropertyRowBitVector::createWidget(PropertyTreeModel* model)
{
	return readOnly_ ? 0 : new PropertyRowWidgetBitVector(this, model);
}

void PropertyRowBitVector::serializeValue(Archive& ar)
{
	ar.serialize(valueAlt_, "valueAlt", "ValueAlt");
	ar.serialize(flags_, "flags", "Flags");
}

/*
void PropertyRowBitVector::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRowState state)
{
	const char* text = valueAlt_.c_str();
	Win32::drawComboBox(dc, widgetRect, text, Win32::defaultFont());
}
*/

// ---------------------------------------------------------------------------
REGISTER_CLASS(PropertyRow, PropertyRowPointer, "ShareHandle");

PropertyRowPointer::PropertyRowPointer(const char* name, const char* nameAlt, const char* typeName,
									   const char* baseName, const char* derivedName, const char* derivedNameAlt)
: PropertyRow(name, nameAlt, typeName)
, derivedName_(derivedName ? derivedName : "")
, derivedNameAlt_(derivedNameAlt ? derivedNameAlt : "")
{
	updateTitle();
}

void PropertyRowPointer::updateTitle()
{
}

int PropertyRowPointer::factoryIndex()
{
	return -1;	
}

void PropertyRowPointer::onMenuCreateByIndex(int index, PropertyTree* tree)
{
	clear();
	if(index < 0){ // пустое значение
		derivedName_ = "";
	}
	else{
		ASSERT(typeName_);
		PropertyRow* def = tree->model()->defaultType(typeName_, index);
		ASSERT(def);
		if(def){
			ASSERT(def->numRef() == 1);
			cloneChildren(this, def);
			derivedName_ = def->typeName();
			const ComboStrings& strings = tree->model()->typeComboStrings(typeName_);
			ASSERT(index >= 0 && index < strings.size());
			derivedNameAlt_ = strings[index].c_str();
			tree->expandRow(this);
		}
	}
	tree->model()->rowChanged(this);
}


std::string PropertyRowPointer::valueAsString() const
{
	return derivedNameAlt_;
}

void PropertyRowPointer::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	if(pulledRow() && pulledRow()->hasIcon())
		pulledRow()->redraw(dc, iconRect, widgetRect, floorRect, this);
	RECT textRect = widgetRect;
	textRect.left += 4;
	textRect.right -= 1;

	HPEN pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW));
	HPEN oldPen = (HPEN)::SelectObject(dc, pen);

	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, GetSysColorBrush(COLOR_3DLIGHT));
	COLORREF oldBkColor = SetBkColor(dc, GetSysColor(COLOR_3DLIGHT));

	::RoundRect(dc, widgetRect.left, widgetRect.top + 2, widgetRect.right, widgetRect.bottom - 2, 6, 6);
	HFONT oldFont = (HFONT)::SelectObject(dc, Win32::defaultBoldFont());
	ASSERT(derivedNameAlt_);

	std::string str;
	const char* text = "";
	if(derivedNameAlt_[0]){
		const char* textStart = derivedNameAlt_;
		const char* p = derivedNameAlt_ + strlen(textStart);
		while(p > textStart){
			if(*(p - 1) == '\\')
				break;
			--p;
		}
		text = p;
		if(p != textStart){
			str = p;
			str += " (";
			str.append(textStart, p - 1);
			str += ")";
			text = str.c_str();
		}
	}
	else
		text = TRANSLATE("[ Пустое значение ]");
	
	DrawText(dc, text, strlen(text), &textRect, DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE);
	::SelectObject(dc, oldBrush);
	::SelectObject(dc, oldFont);
	::SelectObject(dc, oldPen);
	::DeleteObject(pen);
	SetBkColor(dc, oldBkColor);
}

struct ClassMenuItemAdderRowPointer : ClassMenuItemAdder{
	ClassMenuItemAdderRowPointer(PropertyRowPointer* row, PropertyTree* tree) : row_(row), tree_(tree) {}    
	void operator()(PopupMenuItem& root, int index, const char* text){
		root.add(text, index, tree_).connect(row_, &PropertyRowPointer::onMenuCreateByIndex);
	}
protected:
	PropertyRowPointer* row_;
	PropertyTree* tree_;
};

bool PropertyRowPointer::onContextMenu(PopupMenuItem &menu, PropertyTree* tree)
{
	if(!menu.empty())
		menu.addSeparator();
	PopupMenuItem0& createItem = menu.add(TRANSLATE("Заменить на"));
	ClassMenuItemAdderRowPointer(this, tree).generateMenu(createItem, tree->model()->typeComboStrings(typeName()));

	return PropertyRow::onContextMenu(menu, tree);
}

void PropertyRowPointer::serializeValue(Archive& ar)
{
	ar.serialize(ConstStringWrapper(constStrings_, derivedName_), "derivedName", "DerivedName");
	ar.serialize(ConstStringWrapper(constStrings_, derivedNameAlt_), "derivedNameAlt", "DerivedNameAlt");
}

// ---------------------------------------------------------------------------


#define REGISTER_NUMERIC(TypeName, postfix) \
	typedef PropertyRowNumeric<TypeName> PropertyRow##postfix; \
	REGISTER_CLASS(PropertyRow, PropertyRow##postfix, #TypeName);

using std::string;

REGISTER_NUMERIC(float, Float)
REGISTER_NUMERIC(double , Double)

REGISTER_NUMERIC(char, Char)
REGISTER_NUMERIC(signed char, SignedChar)
REGISTER_NUMERIC(unsigned char, UnsignedChar)

REGISTER_NUMERIC(short, Short)
REGISTER_NUMERIC(int, Int)
REGISTER_NUMERIC(long, Long)
REGISTER_NUMERIC(unsigned short, UnsignedShort)
REGISTER_NUMERIC(unsigned int, UnsignedInt)
REGISTER_NUMERIC(unsigned long, UnsignedLong)

#undef REGISTER_NUMERIC

// ---------------------------------------------------------------------------
PropertyRowWidgetNumeric::PropertyRowWidgetNumeric(PropertyRow* row, PropertyTreeModel* model, PropertyRowNumericInterface* numeric)
: PropertyRowWidget(row, model)
, numeric_(numeric)
, entry_(new Entry(""))
{
	entry_->setText(numeric_->valueAsString().c_str());

	entry_->signalEdited().connect(this, &PropertyRowWidgetNumeric::onChange);
	entry_->setSelection(EntrySelection(0, -1));
	entry_->setSwallowReturn(true);
}

void PropertyRowWidgetNumeric::onChange()
{
	numeric_->setValueFromString(entry_->text());
	model()->rowChanged(row());
}

void PropertyRowWidgetNumeric::commit()
{
	if(entry_)
		entry_->commit();
}

// ---------------------------------------------------------------------------

class PropertyRowWidgetString : public PropertyRowWidget, public sigslot::has_slots{
public:
	PropertyRowWidgetString(PropertyRowString* row, PropertyTreeModel* model)
	: PropertyRowWidget(row, model)
	, entry_(new Entry(row->value().c_str()))
	{
		entry_->signalEdited().connect(this, &PropertyRowWidgetString::onChange);
		entry_->setSelection(EntrySelection(0, -1));
		entry_->setSwallowReturn(true);
	}
	~PropertyRowWidgetString()
	{
		entry_->signalEdited().disconnect_all();
	}

	void onChange(){
		PropertyRowString* row = safe_cast<PropertyRowString*>(this->row());
		row->setValue(entry_->text());
		model()->rowChanged(row);
	}
	void commit(){
		if(entry_)
			entry_->commit();
	}
	Widget* actualWidget() { return entry_; }
protected:
	ShareHandle<Entry> entry_;
};

// ---------------------------------------------------------------------------
REGISTER_CLASS(PropertyRow, PropertyRowString, "string");

PropertyRowString::PropertyRowString(const char* name, const char* nameAlt, const std::string& value)
: PropertyRowImpl<std::string, PropertyRowString>(name, nameAlt, value)
{
}

PropertyRowString::PropertyRowString(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<std::string, PropertyRowString>(object, size, name, nameAlt, typeName)
{

}


bool PropertyRowString::assignTo(void* object, int size)
{
	ASSERT(size == sizeof(std::string));
	*reinterpret_cast<std::string*>(object) = value_;
	return true;
}



PropertyRowWidget* PropertyRowString::createWidget(PropertyTreeModel* model)
{
	return readOnly_ ? 0 : new PropertyRowWidgetString(this, model);
}

std::string PropertyRowString::valueAsString() const
{
	return value_;
}

// ---------------------------------------------------------------------------
REGISTER_CLASS(PropertyRow, PropertyRowBool, "bool");

PropertyRowBool::PropertyRowBool(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<bool, PropertyRowBool>(object, size, name, nameAlt, typeName)
{

}

PropertyRowBool::PropertyRowBool(const char* name, const char* nameAlt, bool value)
: PropertyRowImpl<bool, PropertyRowBool>(name, nameAlt, value)
{
}

bool PropertyRowBool::assignTo(void* object, int size)
{
	ASSERT(size == sizeof(bool));
	*reinterpret_cast<bool*>(object) = value_;
	return true;
}

void PropertyRowBool::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRow* hostRow)
{
	RECT rt = widgetRect;
	if(multiValue())
		Win32::drawGrayedCheck(dc, iconRect);
	else
		Win32::drawCheck(dc, iconRect, value());
}

bool PropertyRowBool::onActivateIcon(PropertyTree* tree)
{
	value() = !value();
	tree->model()->rowChanged(this);
	return true;
}

bool PropertyRowBool::onActivate(PropertyTree* tree)
{
	return onActivateIcon(tree);
}

// ---------------------------------------------------------------------------
REGISTER_CLASS(PropertyRow, PropertyRowComboListString, "ComboListString");

class PropertyRowWidgetComboListString : public PropertyRowWidget, public sigslot::has_slots{
public:
	PropertyRowWidgetComboListString(PropertyRowComboListString* row, PropertyTreeModel* model)
	: PropertyRowWidget(row, model)
	, comboBox_(new AutoDropComboBox())
	{
		comboBox_->set(row->value());
		comboBox_->signalEdited().connect(this, &PropertyRowWidgetComboListString::onChange);
	}
	~PropertyRowWidgetComboListString()
	{
	}

	void onChange(){
		PropertyRowComboListString* row = safe_cast<PropertyRowComboListString*>(this->row());
		ComboListString comboList;
		comboBox_->get(comboList);
		row->setValue(comboList);
		model()->rowChanged(row);
	}
	void commit(){}
	Widget* actualWidget() { return comboBox_; }
protected:
	ShareHandle<ComboBox> comboBox_;
};

PropertyRowComboListString::PropertyRowComboListString(const char* name, const char* nameAlt, const ComboListString& value)
: PropertyRowImpl<ComboListString, PropertyRowComboListString>(name, nameAlt, value)
{
}

PropertyRowComboListString::PropertyRowComboListString(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
: PropertyRowImpl<ComboListString, PropertyRowComboListString>(object, size, name, nameAlt, typeName)
{
}


/*
void PropertyRowComboListString::redraw(HDC dc, const RECT& iconRect, const RECT& widgetRect, const RECT& floorRect, PropertyRowState state)
{
	const char* text = value().value().c_str();
	Win32::drawComboBox(dc, widgetRect, text, Win32::defaultFont());
}
*/

PropertyRowWidget* PropertyRowComboListString::createWidget(PropertyTreeModel* model)
{
	return readOnly_ ? 0 : new PropertyRowWidgetComboListString(this, model);
}

std::string PropertyRowComboListString::valueAsString() const
{
	return value_.value();
}

// ---------------------------------------------------------------------------


void setUpdatedRecurse(PropertyRow* row, bool updated)
{
	row->setUpdated(updated);
	PropertyRow::iterator it;
	FOR_EACH(*row, it){
		PropertyRow* row = static_cast<PropertyRow*>(&**it);
		setUpdatedRecurse(row, updated);
	}
}

PropertyOArchive::PropertyOArchive(PropertyTreeModel* model)
: model_(model)
, currentNode_(0)
, lastNode_(0)
, updateMode_(false)
, rootNode_(0)
{
	ASSERT(model != 0);
	if(!rootNode()->empty()){
		updateMode_ = true;
		setUpdatedRecurse(model->root(), false);
	}
}



PropertyOArchive::PropertyOArchive(PropertyTreeModel* model, const char* typeName, const char* derivedTypeName, const char* derivedTypeNameAlt)
: model_(model)
, currentNode_(0)
, lastNode_(0)
, updateMode_(false)
, typeName_(typeName)
, derivedTypeName_(derivedTypeName)
, derivedTypeNameAlt_(derivedTypeNameAlt ? derivedTypeNameAlt : "")
, rootNode_(0)
{
	ASSERT(model != 0);
	if(derivedTypeName)
		model->addDefaultType(0, typeName, derivedTypeName, derivedTypeNameAlt);
	else
		model->addDefaultType(0, typeName);
}

PropertyOArchive::~PropertyOArchive()
{
	if(!typeName_.empty()){
		if(derivedTypeName_){
			rootNode_->setTypeName(derivedTypeName_);
			model_->addDefaultType(rootNode_, typeName_.c_str(), derivedTypeName_, derivedTypeNameAlt_.c_str());
		}
		else
			model_->addDefaultType(rootNode_, typeName_.c_str());
	}
}

PropertyRow* PropertyOArchive::rootNode()
{
	if(rootNode_)
		return rootNode_;
	else{
		ASSERT(model_);
		ASSERT(model_->root());
		return model_->root();
	}
}

void PropertyOArchive::enterNode(PropertyRow* row)
{
	currentNode_ = row;
}

PropertyRow* PropertyOArchive::addRow(ShareHandle<PropertyRow> newRow, bool block, PropertyRow* previousNode)
{
	const char* nameAlt = newRow->nameAlt();
	if(!previousNode) // FIXME перенести в место вызова
		previousNode = lastNode_;

	PropertyRow* result = 0;
	if(currentNode_ == 0){
		if(updateMode_){
			newRow->swapChildren(rootNode());
			newRow->setParent(0);
			model_->setRoot(newRow);
			newRow->setUpdated(true);
			newRow->setNameAlt(nameAlt);
			result = newRow;
		}
		else{
			if(typeName_.empty())
				model_->setRoot(newRow);
			else{
				rootNode_ = newRow;
			}
			result = newRow;
		}
		return result;
	}
	else{
		if(currentNode_->isContainer())
			newRow->setFullRow(true);
		if(updateMode_ || block){
			PropertyRow* row = currentNode_->find(newRow->name(), 0, newRow->typeName(), !block);

			// нужно для сохранения порядка, при внезапном его изменении порядка
			if(row && previousNode && previousNode->parent() == currentNode_){
				if(currentNode_->childrenIndex(row) != currentNode_->childrenIndex(previousNode) + 1){
					//newRow = row;
					currentNode_->erase(row);
					row = 0;
				}
			}

			if(row){
				currentNode_->replaceAndPreserveState(row, newRow);
				newRow->setUpdated(true);
				ASSERT(newRow->parent() == currentNode_);
				result = newRow;
				result->setNameAlt(nameAlt);
			}
			else{
				result = currentNode_->add(&*newRow, previousNode);
				result->setNameAlt(nameAlt);
				newRow->setUpdated(true);
			}
		}
		else{
			result = currentNode_->add(&*newRow);
		}
		return result;
	}
}

bool PropertyOArchive::openStructInternal(void* object, int size, const char* name, const char* nameAlt, const char* typeName, bool polymorphic)
{
	PropertyRowFactory& factory = PropertyRowFactory::instance();
	PropertyRowArg::set(object, size, name, nameAlt, typeName);
	PropertyRow* row = factory.create(typeName, true);
	PropertyRowArg::set(0, 0, 0, 0, 0);
 	if(!row)
		row = new PropertyRow(name, nameAlt, typeName);
	
	if(!row->isLeaf() || currentNode_ == 0){
		PropertyRow* previousRow = lastNode_;
		lastNode_ = currentNode_;
		enterNode(addRow(row, false, previousRow));

		return !currentNode_->isLeaf();
	}
	else{
		lastNode_ = addRow(row);
		return false;
	}
}

void PropertyOArchive::closeStruct(const char* name)
{
	if(currentNode_){
		currentNode_->childrenUpdated();
		lastNode_ = currentNode_;
		currentNode_ = currentNode_->parent();

		if(lastNode_ && updateMode_){
			TreeRow::iterator it;
			for(it = lastNode_->begin(); it != lastNode_->end();){
				PropertyRow* row = static_cast<PropertyRow*>(&**it);
				if(!row->updated())
					it = lastNode_->erase(it);
				else
					++it;
			}
		}

	}
}

bool PropertyOArchive::processEnum(int& value, const EnumDescriptor& descriptor, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowEnum(name, nameAlt, value, &descriptor));
	return true;
} 

bool PropertyOArchive::processBitVector(int& flags, const EnumDescriptor& descriptor, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowBitVector(name, nameAlt, flags, &descriptor));
	return true;
}


bool PropertyOArchive::processValue(std::string& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowString(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(std::wstring& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowString(name, nameAlt, w2a(value)));
	return true;
}

bool PropertyOArchive::processValue(bool& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowBool(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(char& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<char>(name, nameAlt, value));
	return true;
}

// ---

bool PropertyOArchive::processValue(signed char& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<signed char>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(signed short& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<signed short>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(signed int& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<signed int>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(signed long& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<signed long>(name, nameAlt, value));
	return true;
}

// ---

bool PropertyOArchive::processValue(unsigned char& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<unsigned char>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(unsigned short& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<unsigned short>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(unsigned int& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<unsigned int>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(unsigned long& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<unsigned long>(name, nameAlt, value));
	return true;
}

// ---

bool PropertyOArchive::processValue(float& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<float>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::processValue(double& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowNumeric<double>(name, nameAlt, value));
	return true;
}

bool PropertyOArchive::openContainer(void* array, int& number, const char* name, const char* nameAlt,
									 const char* typeName, const char* elementTypeName, int elementSize, bool readOnly)
{
	PropertyRow* container = new PropertyRowContainer(name, nameAlt, typeName, elementTypeName, readOnly);
	lastNode_ = currentNode_;
	enterNode(addRow(container, false));
	return true;
}

void PropertyOArchive::closeContainer(const char* name)
{
	closeStruct(name);
}

bool PropertyOArchive::processValue(ComboListString& value, const char* name, const char* nameAlt)
{
	lastNode_ = addRow(new PropertyRowComboListString(name, nameAlt, value));
	return true;
}

int PropertyOArchive::openPointer(void*& object, const char* name, const char* nameAlt,
								  const char* baseName,
								  const char* derivedName, const char* derivedNameAlt)
{
	lastNode_ = currentNode_;
	PropertyRow* row = new PropertyRowPointer(name, nameAlt, baseName, baseName, derivedName, derivedNameAlt);
	enterNode(addRow(row));
	return NULL_POINTER;
}

void PropertyOArchive::closePointer(const char* name, const char* typeName, const char* derivedName)
{
	closeStruct(name);
}


bool PropertyOArchive::openBlock(const char* name, const char* nameAlt)
{
	PropertyRow* row = new PropertyRow(nameAlt, nameAlt, "");
	lastNode_ = currentNode_;
	enterNode(addRow(row, true));
	return true;
}

void PropertyOArchive::closeBlock()
{
	closeStruct("block");
}


Archive* PropertyOArchive::openDefaultArchive(const char* typeName, const char* derivedTypeName, const char* derivedTypeNameAlt)
{
	if(derivedTypeName){
		if(!model_->defaultTypeRegistered(typeName, derivedTypeName))
			return new PropertyOArchive(model_, typeName, derivedTypeName, derivedTypeNameAlt);
	}
	else{
		if(!model_->defaultTypeRegistered(typeName))
			return new PropertyOArchive(model_, typeName, derivedTypeName, derivedTypeNameAlt);
	}
	return 0;
}

void PropertyOArchive::closeDefaultArchive(ShareHandle<Archive> base_ar, const char* typeName, const char* derivedTypeName, const char* derivedTypeNameAlt)
{
}

};
