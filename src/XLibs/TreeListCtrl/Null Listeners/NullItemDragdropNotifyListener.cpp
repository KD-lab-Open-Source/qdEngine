#include "StdAfx.h"
#include ".\nullitemdragdropnotifylistener.h"

NullItemDragdropNotifyListener::NullItemDragdropNotifyListener(void)
{
}

NullItemDragdropNotifyListener::~NullItemDragdropNotifyListener(void)
{
}

//! Уведомляет о том, что курсор вошел в область окна
/*!
	\retval true
*/
bool NullItemDragdropNotifyListener::onDragEnter(CTreeListCtrl& source, 
												 TREELISTDRAGDROP* ptldd)
{
	return true;
}
//! Уведомляет о том, что курсор покинул область окна
void NullItemDragdropNotifyListener::onDragLeave(CTreeListCtrl& source, 
												 TREELISTDRAGDROP* ptldd)
{

}
//! Уведомляет о том, что тянем курсор над окном
/*!
	false - кидать нельзя
	true  - кинуть можно
*/
bool NullItemDragdropNotifyListener::onDragOver(CTreeListCtrl& source, 
												TREELISTDRAGDROP* ptldd)
{
	return true;
}
//! Уведомляет о том, что отжали кнопку мыши
/*!
	\retvalue false
*/
bool NullItemDragdropNotifyListener::onDrop(CTreeListCtrl& source, 
											TREELISTDRAGDROP* ptldd)
{
	return false;
}

//! Начинаем тянуть выделенные элементы левой кнопкой мыши
/*!
	true - можно начинать операцию
	false - нельзя начинать операцию
*/
bool NullItemDragdropNotifyListener::onBeginDrag(CTreeListCtrl& source, 
												 TREELISTDRAGDROP* ptldd)
{
	return true;
}
//! Начинаем тянуть выделенные элементы левой кнопкой мыши
/*!
	true - можно начинать операцию
	false - нельзя начинать операцию
*/
bool NullItemDragdropNotifyListener::onBeginRDrag(CTreeListCtrl& source, 
												  TREELISTDRAGDROP* ptldd)
{
	return true;
}
