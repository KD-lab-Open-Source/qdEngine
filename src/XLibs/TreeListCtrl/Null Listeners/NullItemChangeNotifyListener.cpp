#include "StdAfx.h"
#include ".\nullitemchangenotifylistener.h"

NullItemChangeNotifyListener::NullItemChangeNotifyListener(void)
{
}

NullItemChangeNotifyListener::~NullItemChangeNotifyListener(void)
{
}

//! ѕеред тем, как развернуть
bool NullItemChangeNotifyListener::onItemExpanding(CTreeListCtrl& tree, 
												   ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ѕосле того, как развернули
void NullItemChangeNotifyListener::onItemExpanded(CTreeListCtrl& tree, 
												  ITEM_CHANGE_INFO* pci)
{

}
//! ѕеред изменением состо€ни€
bool NullItemChangeNotifyListener::onItemStateChanging(CTreeListCtrl& tree, 
													   ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ѕосле изменени€ состо€ни€
void NullItemChangeNotifyListener::onItemStateChanged(CTreeListCtrl& tree, 
													  ITEM_CHANGE_INFO* pci)
{

}

//! ≈сли есть стиль	TLC_CHECKBOX, то приходит перед измененим состо€ни€
bool NullItemChangeNotifyListener::onItemCheckChanging(CTreeListCtrl& tree, 
													   ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ≈сли есть стиль	TLC_CHECKBOX, то приходит после изменени€ состо€ни€
void NullItemChangeNotifyListener::onItemCheckChanged(CTreeListCtrl& tree, 
													  ITEM_CHANGE_INFO* pci)
{
}

//! ≈сли есть стиль	TLC_LOCKBOX, то приходит перед измененим состо€ни€
bool NullItemChangeNotifyListener::onItemLockChanging(CTreeListCtrl& tree, 
													  ITEM_CHANGE_INFO* pci)
{
	return true;
}
//! ≈сли есть стиль	TLC_LOCKBOX, то приходит после изменени€ состо€ни€
void NullItemChangeNotifyListener::onItemLockChanged(CTreeListCtrl& tree, 
													 ITEM_CHANGE_INFO* pci)
{

}
