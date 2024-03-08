#pragma once

#include "Main/QETreeListCtrl.h"
// ObjectListDlg dialog
class ObjectListImpl;

/// Класс диалога, содержащий все конторолы необходимые для отображения списка объектов
class ObjectListDlg : public CDialog
{
	DECLARE_DYNAMIC(ObjectListDlg)

public:
	ObjectListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ObjectListDlg();

// Dialog Data
	enum { IDD = IDD_DLG_OBJECT_LIST };
	/// Возвращает контрол, отображающий список
	QETreeListCtrl& getList();
	/// Назначает объект, реализущий работу с дерево
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
	/// Выравнивает размер m_list, при изменении размера диалога
	void adjustListSize();
private:
	/// Контрол отображающий список
	QETreeListCtrl m_list;
	/// Объект, реализущий работу с дерево
	ObjectListImpl* objectListImpl_;
};
