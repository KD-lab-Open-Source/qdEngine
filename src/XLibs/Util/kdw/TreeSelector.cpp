#include "stdafx.h"
#include "XTL/SafeCast.h"
#include "kdw/TreeView.h"
#include "kdw/TreeSelector.h"
#include "kdw/WaitCursor.h"

namespace kdw{

TreeSelectorDialog::TreeSelectorDialog(HWND parent)
: Dialog(parent)
, builder_(0)
, tree_(new BuildableTree())
{
	init();
}

TreeSelectorDialog::TreeSelectorDialog(Widget* parent)
: Dialog(parent)
, builder_(0)
, tree_(new BuildableTree())
{
	init();
}

void TreeSelectorDialog::init()
{
	setTitle(TRANSLATE("Выбор..."));
	setDefaultSize(Vect2i(400, 500));
	setMinimizeable(false);
	setResizeable(true);

	VBox* box = new VBox();
	add(box);
	box->add(tree_, false, true, true);
	box->add(new HLine(), false, false, false);
	filterEntry_ = new Entry();
	filterEntry_->signalChanged().connect(this, &TreeSelectorDialog::onFilterChanged);
	box->add(filterEntry_, false, false, false);

	addButton(TRANSLATE("ОК"), RESPONSE_OK);
	addButton(TRANSLATE("Отмена"), RESPONSE_CANCEL);
}

#pragma warning(push)
#pragma warning(disable: 4101) // unreferenced local variable
void TreeSelectorDialog::setBuilder(TreeBuilder* builder)
{
	builder_ = builder;
	WaitCursor waitCursor;
	TreeBuilder::Object* object = builder_->buildTree(tree_->root());
	tree_->root()->matchFilter("");
	if(object && !object->matching())
		object = 0;
	tree_->root()->buildFiltered();
	if(object && object->matching()){
		tree_->model()->selectRow(object, true);
		tree_->model()->setFocusedRow(object);
		tree_->update();
		tree_->ensureVisible(object);
	}
	else
		tree_->update();
}
#pragma warning(pop)

void TreeSelectorDialog::show()
{
	__super::show();
	if(tree().model())
		if(TreeRow* focusedOne = tree().model()->focusedRow())
			tree().ensureVisible(focusedOne);
}

void TreeSelectorDialog::onResponse(int response)
{
	if(response == kdw::RESPONSE_OK){
		const TreeModel::Selection& selection = tree_->model()->selection();
		if(!selection.empty() && builder_){
			TreeRow* selectedRow = tree_->model()->rowFromPath(selection.front());
			if(builder_->select(safe_cast<TreeBuilder::Object*>(selectedRow)))
				__super::onResponse(response);
		}
	}
	else
		__super::onResponse(response);
}

void TreeSelectorDialog::onFilterChanged()
{
	if(builder_) {
		//tree_->AllowRedraw(FALSE);
		tree_->model()->clear();
		std::string filter = filterEntry_->text();
		TreeBuilder::Object* focus = builder_->buildTree(tree_->root());
	
		tree_->root()->matchFilter(filter.c_str());
		if(focus && !focus->matching())
			focus = 0;
		tree_->root()->buildFiltered();

		if(filter != "")
			tree_->root()->expandAll(tree_);
		if(focus)
			tree_->model()->setFocusedRow(focus);
		tree_->update();
		//tree_->AllowRedraw(TRUE);
	}
}

static bool stringMatchFilter(const char* name, const char* _filter)
{
	std::string nameString(name);
	CharLowerBuff(&nameString[0], nameString.size());
	if(_filter[0] == 0)
		return true;
	std::string filter = _filter;
	CharLowerBuff(&filter[0], filter.size());
	if(nameString.find(filter) != std::string::npos)
		return true;
	else
		return false;
}

TreeObjectFilteredBase::TreeObjectFilteredBase(const char* text)
: StringTreeRow(text)
, matchFilter_(false)
, removeIfEmpty_(false)
{
}

bool TreeObjectFilteredBase::onDoubleClick(Tree* _tree)
{
	TreeSelectorDialog* dialog = safe_cast<TreeSelectorDialog*>(_tree->mainWindow());
	dialog->onResponse(RESPONSE_OK);
	return false;
}

TreeObjectFilteredBase* TreeObjectFilteredBase::addFiltered(TreeObjectFilteredBase* object, bool sort)
{
	object->matchFilter_ = false;
	if(sort){
		Children::iterator it;
		for(it = potentialChildren_.begin(); it != potentialChildren_.end(); ++it){
			if(stricmp((*it)->name(), object->name()) > 0){
				potentialChildren_.insert(it, object);
				return object;
			}
		}
	}
	potentialChildren_.push_back(object);
	return object;
}

bool TreeObjectFilteredBase::matchFilter(const char* filter)
{
	matchFilter_ = stringMatchFilter(name(), filter);
	Children::iterator it;

	bool haveMatchingChildren = false;
	FOR_EACH(potentialChildren_, it){
		TreeObjectFilteredBase* child = *it;
		if(child->matchFilter(filter)){
			matchFilter_ = true;
			haveMatchingChildren = true;
		}
	}
	if(!haveMatchingChildren && removeIfEmpty_)
		matchFilter_ = false;
	return matchFilter_;
}

bool TreeObjectFilteredBase::buildFiltered()
{
	if(!matchFilter_)
		return false;

	Children::iterator it;
	FOR_EACH(potentialChildren_, it){
		TreeObjectFilteredBase* child = *it;
		if(child->matching())
			__super::add(child);
		child->buildFiltered();
	}
	potentialChildren_.clear();
	return true;
}

TreeBuilder::Object* BuildableTree::root()
{
	return safe_cast<TreeBuilder::Object*>(model()->root());
}

BuildableTree::BuildableTree()
{
	StringTreeModel* model = new StringTreeModel();
	model->setRoot(new TreeBuilder::Object(""));
	setSelectFocused(true);
	setModel(model);
	setColumn(0, new StringTreeColumnDrawer(this), 0.0f);
}

bool BuildableTree::onRowLMBDoubleClick(TreeRow* row, const Recti& rowRect, Vect2i point)
{
	TreeBuilder::Object* object = safe_cast<TreeBuilder::Object*>(row);
	return object->onDoubleClick(this);
}

}
