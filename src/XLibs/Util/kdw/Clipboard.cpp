#include "StdAfx.h"
#include "Handle.h"
#include "kdw/Clipboard.h"
#include "kdw/Widget.h"
#include "kdw/PropertyRow.h"
#include "kdw/_PropertyRowBuiltin.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/PropertyOArchive.h"
#include "kdw/PropertyIArchive.h"
#include "kdw/PropertyEditor.h"
#include "_PropertyRowBuiltin.h"
#include "kdw/Win32/Window.h"

#include "Serialization/SerializationFactory.h"
#include "Serialization/Serializer.h"
#include "Serialization/BinaryArchive.h"

#include <windows.h>


namespace kdw{

const DWORD CLIPBOARD_HEADER = 'v000';

Clipboard::Clipboard(Widget* owner, ConstStringList* constStrings, PropertyTreeModel* model)
: widget_(owner)
, constStrings_(constStrings ? constStrings : &ownConstStrings_)
, model_(model)
{
	clipboardFormat_ = RegisterClipboardFormat("kdw.Clipboard0");
	ASSERT(clipboardFormat_ != 0);
}

Clipboard::~Clipboard()
{

}

bool Clipboard::copy(Serializer& se)
{
	PropertyRow::setConstStrings(constStrings_);
	PropertyTreeModel model;
	PropertyOArchive oa(model_ ? model_ : &model);
	se.serialize(oa);
	bool result = copy(model.root());
	PropertyRow::setConstStrings(0);
	return result;
}

bool Clipboard::paste(Serializer& se)
{
	ConstStringList constStrings;
	PropertyRow::setConstStrings(constStrings_ ? constStrings_ : &constStrings);
	PropertyTreeModel model;
	PropertyOArchive oa(model_ ? model_ : &model);
	se.serialize(oa);
	bool result = false;
	PropertyRow* dest = model.root();
	if(paste(dest)){
		PropertyIArchive ia(&model);
		se.serialize(ia);
		result = true;
	}
	PropertyRow::setConstStrings(0);
	return result;
}


bool Clipboard::canBePastedOn(const char* destinationType)
{
	PropertyRow::setConstStrings(constStrings_);

	ShareHandle<PropertyRow> retrievedRow;
	if(!pasteRaw(Serializer(retrievedRow, "row", "Row")) || !retrievedRow){
		PropertyRow::setConstStrings(0);
		return false;
	}
	bool result = strcmp(retrievedRow->typeName(), destinationType) == 0;
	PropertyRow::setConstStrings(0);
	return result;
}


bool Clipboard::empty()
{
	ASSERT(widget_);
	Win32::Window32* window = kdw::_findWindow(widget_);
	ASSERT(window);

	if(!::OpenClipboard(*window))
		return false;

	bool result = false;
	HGLOBAL memoryHandle = (HGLOBAL)(::GetClipboardData(clipboardFormat_));
	if(memoryHandle){
		if(char* mem = (char*)GlobalLock(memoryHandle)){
            result = GlobalSize(memoryHandle) > 0;
			GlobalUnlock(memoryHandle);
		}
	}
	::CloseClipboard();
	return !result;
}

bool Clipboard::paste(PropertyRow* dest, bool onlyCheck)
{
	ConstStringList constStrings;
	PropertyRow::setConstStrings(constStrings_);

	ShareHandle<PropertyRow> source;
	if(!pasteRaw(Serializer(source, "row", "Row")) || !source){
		PropertyRow::setConstStrings(0);
		return false;
	}
	
	bool result = false;
	if(strcmp(dest->typeName(), source->typeName()) == 0){
		result = true;
		if(!onlyCheck){
			if(dest->isPointer() && !source->isPointer()){
				PropertyRowPointer* d = safe_cast<PropertyRowPointer*>(dest);

				const char* derivedName = d->typeName();
				const char* derivedNameAlt = d->typeName();
				PropertyRowPointer* newSourceRoot = new PropertyRowPointer(d->name(), d->nameAlt(), d->typeName(), d->typeName(), derivedName, derivedNameAlt);
				source->swapChildren(newSourceRoot);
				source = newSourceRoot;
			}
			const char* name = dest->name();
			const char* nameAlt = dest->nameAlt();
			source->setName(name);
			source->setNameAlt(nameAlt);
			if(dest->parent())
				dest->parent()->replaceAndPreserveState(dest, source, false);
			else{
				dest->clear();
				dest->swapChildren(source);
				//*dest = *source;
			}
		}
	}
	else if(dest->isContainer()){
		if(model_){
			PropertyRowContainer* container = static_cast<PropertyRowContainer*>(dest);
			PropertyRow* elementRow = model_->defaultType(container->elementTypeName());
			ASSERT(elementRow);
			if(strcmp(elementRow->typeName(), source->typeName()) == 0){
				result = true;
				if(!onlyCheck){
					PropertyRow* dest = elementRow;
					if(dest->isPointer() && !source->isPointer()){
						PropertyRowPointer* d = safe_cast<PropertyRowPointer*>(dest);

						const char* derivedName = d->typeName();
						const char* derivedNameAlt = d->typeName();
						PropertyRowPointer* newSourceRoot = new PropertyRowPointer(d->name(), d->nameAlt(), d->typeName(), d->typeName(), derivedName, derivedNameAlt);
						source->swapChildren(newSourceRoot);
						source = newSourceRoot;
					}

					container->add(source.get());
				}
			}
		}
	}
	
	PropertyRow::setConstStrings(0);
	return result;
}

bool Clipboard::copy(PropertyRow* row)
{
	PropertyRow::setConstStrings(constStrings_);
	ASSERT(widget_);
	Win32::Window32* window = kdw::_findWindow(widget_);
	ASSERT(window);

    ShareHandle<PropertyRow> clonedRow(row->clone());
	BinaryOArchive oa;
	if(!oa.serialize(clonedRow, "row", "Row")){
		PropertyRow::setConstStrings(0);
		return false;
	}

	if(::OpenClipboard(*window)){
		HGLOBAL memoryHandle = GlobalAlloc(GPTR, oa.size());
		xassert(memoryHandle);
		if(!memoryHandle){
			PropertyRow::setConstStrings(0);
			return false;
		}
		
		void* mem = GlobalLock(memoryHandle);
		if(!mem){
			xassert(0 && "GlobalLock failed!");
			PropertyRow::setConstStrings(0);
			return false;
		}
		CopyMemory(mem, oa.data(), oa.size());
		GlobalUnlock(memoryHandle);

		VERIFY(::SetClipboardData(clipboardFormat_, memoryHandle));
		::CloseClipboard();
		PropertyRow::setConstStrings(0);
		return true;
	}
	PropertyRow::setConstStrings(0);
	return false;
}

bool Clipboard::pasteRaw(Serializer& se)
{
	ASSERT(widget_);
	Win32::Window32* window = kdw::_findWindow(widget_);
	ASSERT(window);

	if(!::OpenClipboard(*window))
		return false;

	bool result = false;
	HGLOBAL memoryHandle = (HGLOBAL)(::GetClipboardData(clipboardFormat_));
	if(memoryHandle){
		if(char* mem = (char*)GlobalLock(memoryHandle)){
            int memSize = GlobalSize(memoryHandle);
			BinaryIArchive ia(mem, memSize);
			result = se.serialize(ia);
			GlobalUnlock(memoryHandle);
		}
		else
			xassert(0);
	}

	::CloseClipboard();
	return result;
}

int Clipboard::smartPaste(Serializer& se)
{
	xassert(0 && "Not implemented");
	return paste(se) ? 1 : 0;
}

}
