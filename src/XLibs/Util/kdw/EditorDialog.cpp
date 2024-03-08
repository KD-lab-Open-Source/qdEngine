#include "StdAfx.h"
#include "EditorDialog.h"

#include "kdw/PropertyTree.h"
#include "FileUtils/FileUtils.h"
#include "kdw/Win32/Window.h"

#include "Serialization/XPrmArchive.h"

namespace kdw{

EditorDialog::EditorDialog(Serializer& serializer, const char* stateFileName, int flags, kdw::Widget* parent)
: kdw::Dialog(parent)
{
	init(serializer, stateFileName, flags);
}

EditorDialog::EditorDialog(Serializer& serializer, const char* stateFileName, int flags, HWND parent)
: kdw::Dialog(parent)
{
	init(serializer, stateFileName, flags);
}

void EditorDialog::init(Serializer& serializer, const char* stateFileName, int flags)
{
	stateFileName_ = stateFileName ? stateFileName : "";
	std::string title = "Property Editor";
	if(!stateFileName_.empty()){
		title += ": ";
		title += stateFileName_;
	}
	setTitle(title.c_str());
	setDefaultSize(Vect2i(450, 500));
	setDefaultPosition(kdw::POSITION_CENTER);
	
	setResizeable(true);

	tree_ = new kdw::PropertyTree;
	tree_->setImmediateUpdate(flags & kdw::IMMEDIATE_UPDATE);
	tree_->setHideUntranslated(flags & kdw::ONLY_TRANSLATED);
	tree_->setCompact(flags & kdw::COMPACT);
	tree_->attach(serializer);
	if(flags & kdw::EXPAND_ALL)
		tree_->expandAll();
	add(tree_);

	if(::isFileExists(stateFileName_.c_str())){
		XPrmIArchive ia(stateFileName_.c_str());
		ia.setFilter(SERIALIZE_STATE);
		ia.serialize(*this, "window", 0);
	}

	if(!(flags & kdw::IMMEDIATE_UPDATE))
		addButton(TRANSLATE("Перегрузить"), kdw::RESPONSE_RETRY, false);

	addButton(TRANSLATE("ОК"), kdw::RESPONSE_OK);
	addButton(TRANSLATE("Отмена"), kdw::RESPONSE_CANCEL);
}

void EditorDialog::onResponse(int response)
{
	if(response == kdw::RESPONSE_RETRY){
		tree_->apply();
		tree_->revert();
		return;
	}

	if(response == kdw::RESPONSE_OK && !tree_->immediateUpdate()){
		tree_->apply();
	}

	if(!stateFileName_.empty()){
		XPrmOArchive oa(stateFileName_.c_str());
		oa.setFilter(SERIALIZE_STATE);
		oa.serialize(*this, "window", 0);
	}
	Dialog::onResponse(response);
}

void EditorDialog::serialize(Archive& ar)
{
	__super::serialize(ar);
    ar.serialize(*tree_, "tree", 0);
}

}