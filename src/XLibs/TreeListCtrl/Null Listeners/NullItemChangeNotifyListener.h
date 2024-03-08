#pragma once

#include "../IItemChangeNotifyListener.h"

class NullItemChangeNotifyListener : public IItemChangeNotifyListener
{
public:
	NullItemChangeNotifyListener(void);
	~NullItemChangeNotifyListener(void);

	//! ����� ���, ��� ����������
	virtual bool onItemExpanding(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ����� ����, ��� ����������
	virtual void onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ����� ���������� ���������
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ����� ��������� ���������
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ���� ���� �����	TLC_CHECKBOX, �� �������� ����� ��������� ���������
	virtual bool onItemCheckChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ���� ���� �����	TLC_CHECKBOX, �� �������� ����� ��������� ���������
	virtual void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

	//! ���� ���� �����	TLC_LOCKBOX, �� �������� ����� ��������� ���������
	virtual bool onItemLockChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ���� ���� �����	TLC_LOCKBOX, �� �������� ����� ��������� ���������
	virtual void onItemLockChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

};
