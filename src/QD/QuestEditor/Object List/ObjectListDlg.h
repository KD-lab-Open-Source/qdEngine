#pragma once

#include "Main/QETreeListCtrl.h"
// ObjectListDlg dialog
class ObjectListImpl;

/// ����� �������, ���������� ��� ��������� ����������� ��� ����������� ������ ��������
class ObjectListDlg : public CDialog
{
	DECLARE_DYNAMIC(ObjectListDlg)

public:
	ObjectListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ObjectListDlg();

// Dialog Data
	enum { IDD = IDD_DLG_OBJECT_LIST };
	/// ���������� �������, ������������ ������
	QETreeListCtrl& getList();
	/// ��������� ������, ���������� ������ � ������
	void setObjectListImpl(ObjectListImpl* objListImpl);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);
	virtual void OnCancel();

	afx_msg void OnSize(UINT nType, int cx, int cy);



	DECLARE_MESSAGE_MAP()
protected:
	/// ����������� ������ m_list, ��� ��������� ������� �������
	void adjustListSize();
private:
	/// ������� ������������ ������
	QETreeListCtrl m_list;
	/// ������, ���������� ������ � ������
	ObjectListImpl* objectListImpl_;
};
