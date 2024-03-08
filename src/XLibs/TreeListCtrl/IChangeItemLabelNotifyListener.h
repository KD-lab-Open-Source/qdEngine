#ifndef CHANGE_ITEM_LABEL_NOTIFY_LISTENER_H
#define CHANGE_ITEM_LABEL_NOTIFY_LISTENER_H

class CTreeListCtrl;
class CTreeListItem;

typedef struct _CHANGE_LABEL_NOTIFY_INFO{
	CTreeListItem	*pItem;
	int				iCol;
	CWnd			*pEditControl;//�������, ������� ������������ ��������������			
	CString			text;
}CHANGE_LABEL_NOTIFY_INFO;

class IChangeItemLabelNotifyListener
{
public:
	//! ���������� ����� ������� �������������� ������.
	/*!
		������� false ��� ����, ����� ��������� ��������������
	*/
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

	virtual bool onEndLabelEdit(CTreeListCtrl& source,
						CHANGE_LABEL_NOTIFY_INFO* info) = 0;

	//! ���������� �������, ������� ����� ��������������� �����
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;
	//! ����������� �������� ����� ���, ��� �������� ������� ��������������
	/*!
		����������� �������� ����� ���, ��� �������� ������� ��������������
		� ����� �� ��� ������������ ����� ���������� ��������� ��������(������� 
		���������� ������) ��� ������ �������� �� ���� �������.
		� ������ ������ ���������� �������������� ��������������� �������
		������������ �������� ������������ 
	*/
	virtual bool onBeginControl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

	//! ����������� �������� ����� �������� ��������
	/*!
		���� � onBeginControl �������� ���� �������, �� ����� ����� ����� ��� �������
	*/
	virtual void onEndControl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

};

#endif