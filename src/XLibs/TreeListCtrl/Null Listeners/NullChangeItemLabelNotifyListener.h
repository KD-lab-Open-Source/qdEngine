#pragma once
#include "../ichangeitemlabelnotifylistener.h"

class NullChangeItemLabelNotifyListener :
	public IChangeItemLabelNotifyListener
{
public:
	NullChangeItemLabelNotifyListener(void);
	virtual ~NullChangeItemLabelNotifyListener(void);


	//! ���������� ����� ������� �������������� ������.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! ���������� � ���, ��� �������������� ���������
	virtual bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! ���������� �������, ������� ����� ��������������� �����
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
	//! ����������� �������� ����� ���, ��� �������� ������� ��������������
	virtual bool onBeginControl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! ����������� �������� ����� �������� ��������
	virtual void onEndControl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
};
