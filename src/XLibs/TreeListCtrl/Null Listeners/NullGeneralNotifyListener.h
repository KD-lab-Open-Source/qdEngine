#pragma once
#include "../igeneralnotifylistener.h"

class NullGeneralNotifyListener :
	public IGeneralNotifyListener
{
public:
	NullGeneralNotifyListener(void);
	~NullGeneralNotifyListener(void);
	virtual void onClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	/*!
		������������ �������� ����� ����� ������ ��� ������
		\retval true - ��� ����, ����� ��������� ��������� �� ���������
		\retval false - ��� ����, ����� ��������� ��������� �� ���������
	*/
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	virtual void onRDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	//! �� ����������
	virtual bool onHover(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	virtual void onKillFocus(CTreeListCtrl& tree);
	virtual void onSetFocus(CTreeListCtrl& tree);


	virtual void onReturn(CTreeListCtrl& tree);
	//! �� ����������
	virtual void onReleasedCapture(CTreeListCtrl& tree);
};
