#include "stdafx.h"
#include "..\resource.h"
#include "qd_named_object_reference.h"

#include ".\objectlistimpl.h"
#include "Main/QETreeListCtrl.h"
#include "../ptree_api.h"

ObjectListImpl::ObjectListImpl(QETreeListCtrl* listWindow):
  listWindow_(listWindow)
{
}

ObjectListImpl::~ObjectListImpl(void)
{
}

void ObjectListImpl::setListWindow(QETreeListCtrl* listWindow){
	ASSERT(listWindow);
	listWindow_ = listWindow;
}
QETreeListCtrl* ObjectListImpl::getListWindow() const{
	return listWindow_;
}

bool ObjectListImpl::init(){
	CString str;
//	str.LoadString(IDS_OBJECT_COLUMN);
	if (!listWindow_->InsertColumn(str))
		return false;
	listWindow_->SetColumnWidth(0, 400);
	return true;
}

bool ObjectListImpl::showObjectInList(qdNamedObjectReference const* objRef){
	return false;
}

HITEM ObjectListImpl::addObjectToList(qdNamedObjectReference const* objRef, 
									  HITEM hItemRoot){
	if (!objRef)
		return NULL;
	std::auto_ptr<qdNamedObjectReference> copy(new qdNamedObjectReference(*objRef));
	if (!copy.get())
		return NULL;
	CString labelString = getObjectListLabel(*copy);
	CTreeListItem * parent = TLI_ROOT;
	if (hItemRoot != OL_ROOT)
		parent = reinterpret_cast<CTreeListItem*>(hItemRoot);

	CTreeListItem* item = 
		listWindow_->InsertItem(labelString, parent);
	if (item)
	{
		listWindow_->SetItemData(item, reinterpret_cast<DWORD_PTR>(copy.release()));
//		listWindow_->SetColumnWidth(0, TLSCW_AUTOSIZE_USEHEADER);
	}
	return reinterpret_cast<HITEM>(item);
}

bool ObjectListImpl::removeObjectFromList(qdNamedObjectReference const* objRef){
	return false;
}

void ObjectListImpl::clearList(){
	listWindow_->DeleteAllItems();
}

BOOL ObjectListImpl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

//! Получаем строку,которая будет фигурировать в списке
CString ObjectListImpl::getObjectListLabel(qdNamedObjectReference const& objRef)
{
	static TCHAR const levelDelimeter[] = _T("::");

	CString res;
	for(int i = 0; i < objRef.num_levels(); ++i)
	{
		if (!res.IsEmpty())
			res += levelDelimeter;
		res += objRef.object_name(i);
	}
	return res;
}

bool ObjectListImpl::onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{
	qdNamedObjectReference* ref = reinterpret_cast<qdNamedObjectReference*>(
		listWindow_->GetItemData(info->item));
	if (ref)
	{
		if (qdNamedObject const* obj = ref->object())
			ptree_api::ShowObjectInTree(obj);
	}
	return false;
}

void ObjectListImpl::onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info)
{
	qdNamedObjectReference* ref = reinterpret_cast<qdNamedObjectReference*>(
		listWindow_->GetItemData(info->item));
	delete ref;
}
