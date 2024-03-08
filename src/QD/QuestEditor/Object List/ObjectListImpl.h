#pragma once

#include "IObjectList.h"
#include <TreeListCtrl/Null Listeners/NullGeneralNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullItemOperationNotifyListener.h>

class QETreeListCtrl;
class qdNamedObjectReference;

/// ������, ����������� ������ �� �������. ������� ��������� ��� �������� �� �������
class ObjectListImpl : public NullGeneralNotifyListener,
					   public NullItemOperationNotifyListener
{
public:
	ObjectListImpl(QETreeListCtrl* listWindow);
	~ObjectListImpl(void);

	/// ��������� �������, ���������� ������
	void setListWindow(QETreeListCtrl* listWindow);
	/// ���������� �������, ���������� ������
	QETreeListCtrl* getListWindow() const;

	/// ���������� ������ � ������
	bool showObjectInList(qdNamedObjectReference const* );
	/// ��������� ������ � ������
	HITEM addObjectToList(qdNamedObjectReference const*, HITEM hItemRoot);
	/// ������� ������ �� ������
	bool removeObjectFromList(qdNamedObjectReference const* );
	/// ������� ������
	void clearList();

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	/// ��������� � listWindow_ ������� � ���������� ��� ������
	bool init();
protected:
	//! �������� ������,������� ����� ������������ � ������
	static CString getObjectListLabel(qdNamedObjectReference const& objRef);

	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	virtual void onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info);

private:
	/// �������, ����������� ������
	QETreeListCtrl* listWindow_;
};
