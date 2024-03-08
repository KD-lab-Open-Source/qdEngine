#include "StdAfx.h"
#include ".\nullitemoperationnotifylistener.h"

NullItemOperationNotifyListener::NullItemOperationNotifyListener(void)
{
}

NullItemOperationNotifyListener::~NullItemOperationNotifyListener(void)
{
}

void NullItemOperationNotifyListener::onDeleteItem(CTreeListCtrl& tree, 
												   ITEM_OPERATION_INFO* info)
{

}

bool NullItemOperationNotifyListener::onDeleteAllItems(CTreeListCtrl& tree, 
													   ITEM_OPERATION_INFO* info)
{
	return false;
}

void NullItemOperationNotifyListener::onInsertItem(CTreeListCtrl& tree, 
												   ITEM_OPERATION_INFO* info)
{

}
