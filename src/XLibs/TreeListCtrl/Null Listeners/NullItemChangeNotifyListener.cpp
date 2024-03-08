#include "StdAfx.h"
#include ".\nullitemchangenotifylistener.h"

NullItemChangeNotifyListener::NullItemChangeNotifyListener(void)
{
}

NullItemChangeNotifyListener::~NullItemChangeNotifyListener(void)
{
}

//! ����� ���, ��� ����������
bool NullItemChangeNotifyListener::onItemExpanding(CTreeListCtrl& tree, 
												   ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ����� ����, ��� ����������
void NullItemChangeNotifyListener::onItemExpanded(CTreeListCtrl& tree, 
												  ITEM_CHANGE_INFO* pci)
{

}
//! ����� ���������� ���������
bool NullItemChangeNotifyListener::onItemStateChanging(CTreeListCtrl& tree, 
													   ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ����� ��������� ���������
void NullItemChangeNotifyListener::onItemStateChanged(CTreeListCtrl& tree, 
													  ITEM_CHANGE_INFO* pci)
{

}

//! ���� ���� �����	TLC_CHECKBOX, �� �������� ����� ��������� ���������
bool NullItemChangeNotifyListener::onItemCheckChanging(CTreeListCtrl& tree, 
													   ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ���� ���� �����	TLC_CHECKBOX, �� �������� ����� ��������� ���������
void NullItemChangeNotifyListener::onItemCheckChanged(CTreeListCtrl& tree, 
													  ITEM_CHANGE_INFO* pci)
{
}

//! ���� ���� �����	TLC_LOCKBOX, �� �������� ����� ��������� ���������
bool NullItemChangeNotifyListener::onItemLockChanging(CTreeListCtrl& tree, 
													  ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ���� ���� �����	TLC_LOCKBOX, �� �������� ����� ��������� ���������
void NullItemChangeNotifyListener::onItemLockChanged(CTreeListCtrl& tree, 
													 ITEM_CHANGE_INFO* pci)
{

}
