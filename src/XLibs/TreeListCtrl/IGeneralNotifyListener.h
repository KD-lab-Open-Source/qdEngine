#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _GENERAL_NOTIFY_INFO {
	CTreeListItem* item;///< ����, ��-�������� ������
	int iSubItem;///< ����� �������
	UINT nFlags;///< �����, ���������� �� HitTest
}GENERAL_NOTIFY_INFO;
class IGeneralNotifyListener
{
public:
	virtual void onClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;
	/*!
		������������ �������� ����� ����� ������ ��� ������
		\retval true - ��� ����, ����� ��������� ��������� �� ���������
		\retval false - ��� ����, ����� ��������� ��������� �� ���������
	*/
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;

	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;
	//! �� ����������
	virtual void onRDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;

	//! �� ����������
	virtual bool onHover(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;

	virtual void onKillFocus(CTreeListCtrl& tree) = 0;
	virtual void onSetFocus(CTreeListCtrl& tree) = 0;

	virtual void onReleasedCapture(CTreeListCtrl& tree) = 0;

	virtual void onReturn(CTreeListCtrl& tree) = 0;
};
