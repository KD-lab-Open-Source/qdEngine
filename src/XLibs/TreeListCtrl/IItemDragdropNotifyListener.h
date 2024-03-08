#if !defined (ITEM_DRAGDROP_LISTENER_H)
#define ITEM_DRAGDROP_LISTENER_H

#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _TREELISTDRAGDROP
{
	//! �������, �� �������� �����
	CTreeListCtrl*	pSource;
	/*!
		��������������� ���������� ��������
	*/
	//! ���� ���, ������� ���������
	CTreeListItem*	hoverItem;									// item pointer
	HCURSOR hCursor;
// TODO: �������� ���� � ��������
} TREELISTDRAGDROP, FAR* LPTREELISTDRAGDROP;


class IItemDragdropNotifyListener
{
public:
	//! ���������� � ���, ��� ������ ������ ���������� �������� ����� ������� ����
	/*!
		TREELISTDRAGDROP.hoverItem == NULL
		TREELISTDRAGDROP.hCursor - ignored
		\retval true - ����� �������� ��������
		\retval false - ������ �������� ��������
	*/
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! ���������� � ���, ��� ������ ������ ���������� �������� ������ ������� ����
	/*!
		\a !!!�� ������� ������ �� ����������!!!
		TREELISTDRAGDROP.hoverItem == NULL
		TREELISTDRAGDROP.hCursor - ignored
		true - ����� �������� ��������
		false - ������ �������� ��������
	*/
	virtual bool onBeginRDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;

	//! ���������� � ���, ��� ������ ����� � ������� ����
	/*!
		���������� � ���, ��� ������ ����� � ������� ����. 
		TREELISTDRAGDROP.hoverItem == NULL
		\retval true - source ��������� ��������� �������� Drag&Drop
		\retval false - source �� ��������� ��������� �������� Drag&Drop. ��������
		�� �����������. � source �� �������� ���������� �����������
	*/
	virtual bool onDragEnter(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! ���������� � ���, ��� ������ ������� ������� ����
	/*!
		���������� � ���, ��� ������ ����� �� ������� ����. �� �������� � ����, 
		� ������� ���� ���� �������� Drag&Drop
		TREELISTDRAGDROP.hoverItem == NULL
	*/
	virtual void onDragLeave(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! ���������� � ���, ��� ����� ������ ��� �����
	/*!
		TREELISTDRAGDROP.hoverItem == ����, ��� ������� �����
		false - ������ ������
		true  - ������ �����
	*/
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! ���������� � ���, ��� ������ ������ ����
	/*!
		TREELISTDRAGDROP.hCursor - ignored
		\retval true - �������� ������� ���������
		\retval false - �������� �� �������
		������������ �������� ������ ������������
	*/
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;

};

#endif//ITEM_DRAGDROP_LISTENER_H