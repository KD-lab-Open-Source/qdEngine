#include "StdAfx.h"
#include ".\nullitemdragdropnotifylistener.h"

NullItemDragdropNotifyListener::NullItemDragdropNotifyListener(void)
{
}

NullItemDragdropNotifyListener::~NullItemDragdropNotifyListener(void)
{
}

//! ���������� � ���, ��� ������ ����� � ������� ����
/*!
	\retval true
*/
bool NullItemDragdropNotifyListener::onDragEnter(CTreeListCtrl& source, 
												 TREELISTDRAGDROP* ptldd)
{
	return true;
}
//! ���������� � ���, ��� ������ ������� ������� ����
void NullItemDragdropNotifyListener::onDragLeave(CTreeListCtrl& source, 
												 TREELISTDRAGDROP* ptldd)
{

}
//! ���������� � ���, ��� ����� ������ ��� �����
/*!
	false - ������ ������
	true  - ������ �����
*/
bool NullItemDragdropNotifyListener::onDragOver(CTreeListCtrl& source, 
												TREELISTDRAGDROP* ptldd)
{
	return true;
}
//! ���������� � ���, ��� ������ ������ ����
/*!
	\retvalue false
*/
bool NullItemDragdropNotifyListener::onDrop(CTreeListCtrl& source, 
											TREELISTDRAGDROP* ptldd)
{
	return false;
}

//! �������� ������ ���������� �������� ����� ������� ����
/*!
	true - ����� �������� ��������
	false - ������ �������� ��������
*/
bool NullItemDragdropNotifyListener::onBeginDrag(CTreeListCtrl& source, 
												 TREELISTDRAGDROP* ptldd)
{
	return true;
}
//! �������� ������ ���������� �������� ����� ������� ����
/*!
	true - ����� �������� ��������
	false - ������ �������� ��������
*/
bool NullItemDragdropNotifyListener::onBeginRDrag(CTreeListCtrl& source, 
												  TREELISTDRAGDROP* ptldd)
{
	return true;
}
