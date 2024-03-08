#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _ITEM_OPERATION_INFO {
	CTreeListItem* item;
}ITEM_OPERATION_INFO;
class IItemOperationNotifyListener
{
public:
	virtual void onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
	/*!
	������������ �������� ����� ���� ������ ��� ������ LIST
	\retval false - ��� ����, ����� ��������� ���������������� ����� onDeleteItem
	\retval true - ��� ����, ����� ��������� ���������������� ����� onDeleteItem
	*/
	virtual bool onDeleteAllItems(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
	//! ���������� � ���, ��� item ��� ��������
	virtual void onInsertItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
};
