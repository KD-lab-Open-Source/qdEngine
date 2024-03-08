#pragma once
#include "../IItemDragdropNotifyListener.h"

class NullItemDragdropNotifyListener :public IItemDragdropNotifyListener
{
public:
	NullItemDragdropNotifyListener(void);
	virtual ~NullItemDragdropNotifyListener(void);
	//! �������� ������ ���������� �������� ����� ������� ����
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! �������� ������ ���������� �������� ����� ������� ����
	virtual bool onBeginRDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ������ ����� � ������� ����
	virtual bool onDragEnter(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ������ ������� ������� ����
	virtual void onDragLeave(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ����� ������ ��� �����
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ������ ������ ����
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
};
