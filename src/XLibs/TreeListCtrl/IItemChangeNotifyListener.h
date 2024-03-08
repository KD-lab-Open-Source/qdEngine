#if !defined ITEM_CHANGE_NOTIFY_LISTENER_H

#define ITEM_CHANGE_NOTIFY_LISTENER_H

#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _ITEM_CHANGE_INFO
{
	CTreeListItem* item;
	int iOldSubItem;///< ���� ���������� ����� TLC_SHOWSELACTIVE, ���������� �������� �������
	int iNewSubItem;///< ���� ���������� ����� TLC_SHOWSELACTIVE, ����� �������� �������
	UINT uNewState;
	UINT uOldState;
}ITEM_CHANGE_INFO;

class IItemChangeNotifyListener
{
public:
	/************************************************************************/
	/* ����������� false ��������� ��������� ���������
	/************************************************************************/
	//! ����� ���, ��� ����������
	virtual bool onItemExpanding(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ����� ����, ��� ����������
	virtual void onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ����� ���������� ���������
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ����� ��������� ���������
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;

	//! ���� ���� �����	TLC_CHECKBOX, �� �������� ����� ��������� ���������
	virtual bool onItemCheckChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ���� ���� �����	TLC_CHECKBOX, �� �������� ����� ��������� ���������
	virtual void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;

	//! ���� ���� �����	TLC_LOCKBOX, �� �������� ����� ��������� ���������
	virtual bool onItemLockChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ���� ���� �����	TLC_LOCKBOX, �� �������� ����� ��������� ���������
	virtual void onItemLockChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
};


#endif