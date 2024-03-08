#include "StdAfx.h"
#include "kdw/Win32Proxy.h"
#include "kdw/ObjectsTree.h"
#include "kdw/TreeView.h"

namespace kdw{

void TreeObject::onSelect(ObjectsTree* tree)
{
	tree->signalSelectionChanged().emit();
}

void TreeObject::remove()
{
	TreeObject* p = parent();
	p->erase(this);
}

void TreeObject::showCheckBox(bool show)
{
	/* FIXME
	xassert(tree_ && item_);
	if(show)
		tree_->SetItemState(item_, TLIS_SHOWCHECKBOX, 0);
	else
		tree_->SetItemState(item_, 0, TLIS_SHOWCHECKBOX);
		*/
}

void TreeObject::setCheck(bool checked)
{
	checked_ = checked;
}

void TreeObject::focus()
{
	// FIXME
}

// ---------------------------------------------------------------------------

ObjectsTree::ObjectsTree()
{
	Model* model = new Model;
	model->setRoot(new TreeObject());
	setModel(model);
	setColumn(0, new kdw::StringTreeColumnDrawer(this), 1.0f);
	setSelectFocused(true);
}


bool ObjectsTree::onRowLMBDoubleClick(kdw::TreeRow* row, const Recti& rowRect, Vect2i point)
{
	xassert(row);
	if(!safe_cast<TreeObject*>(row)->onDoubleClick(this))
		return __super::onRowLMBDoubleClick(row, rowRect, point);
	return true;
}

void ObjectsTree::onRowSelected(kdw::TreeRow* row)
{
	__super::onRowSelected(row);
	safe_cast<TreeObject*>(row)->onSelect(this);
}

void ObjectsTree::onRowDeselected(kdw::TreeRow* row)
{
	__super::onRowDeselected(row);
	safe_cast<TreeObject*>(row)->onSelect(this);
}

void ObjectsTree::onRowRMBDown(kdw::TreeRow* row, const Recti& rowRect, Vect2i point)
{
	safe_cast<TreeObject*>(row)->onRightClick(this);
	__super::onRowRMBDown(row, rowRect, point);
}

bool ObjectsTree::onRowKeyDown(kdw::TreeRow* row, sKey key)
{
	return __super::onRowKeyDown(row, key);
}

void ObjectsTree::selectObject(TreeObject* object, bool select, bool update, bool ensure)
{
	if(ensure)
		ensureVisible(object);
	model()->selectRow(object, true);
	if(ensure)
		model()->setFocusedRow(object);
	object->onSelect(this);
	if(update){
		ensureVisible(object);
		this->update();
	}
}

}
