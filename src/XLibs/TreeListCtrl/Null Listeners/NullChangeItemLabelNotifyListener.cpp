#include "StdAfx.h"
#include ".\NullChangeItemLabelNotifyListener.h"
#include "../TreeListColumnInfo.h"

NullChangeItemLabelNotifyListener::NullChangeItemLabelNotifyListener(void)
{
}

NullChangeItemLabelNotifyListener::~NullChangeItemLabelNotifyListener(void)
{
}

//! ���������� ����� ������� �������������� ������.
bool NullChangeItemLabelNotifyListener::onBeginLabelEdit(CTreeListCtrl& source, 
							  CHANGE_LABEL_NOTIFY_INFO* info)
{
	return true;
}


//! ���������� � ���, ��� �������������� ���������
bool NullChangeItemLabelNotifyListener::onEndLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* info)
{
	return false;
}

//! ���������� �������, ������� ����� ��������������� �����
DWORD NullChangeItemLabelNotifyListener::onRequestCtrl(CTreeListCtrl& source, 
						   CHANGE_LABEL_NOTIFY_INFO* info)
{
	return TLM_STATIC;
}
//! ����������� �������� ����� ���, ��� �������� ������� ��������������
/*!
	����������� �������� ����� ���, ��� �������� ������� ��������������
	� ����� �� ��� ������������ ����� ���������� ��������� ��������(������� 
	���������� ������) ��� ������ �������� �� ���� �������
	������������ �������� ������������ 
*/
bool NullChangeItemLabelNotifyListener::onBeginControl(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* info)
{
	return false;
}

//! ����������� �������� ����� �������� ��������
void NullChangeItemLabelNotifyListener::onEndControl(CTreeListCtrl& source, 
						  CHANGE_LABEL_NOTIFY_INFO* info)
{
}
