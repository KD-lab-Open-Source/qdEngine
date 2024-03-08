#pragma once

#include "INamedObjListTo.h"
#include "INamedObjListReader.h"
#include "afxcmn.h"

// NamedObjectSelectorDlg dialog
/// ������ ������ ��������
/**
	������ ������������ ��� ������ ������ qdNamedObject � ���, ����� �������� ������. 
	������ ���������� �������� � ��������.
	����������� ������� � ������� ���������� INamedObjectListToList.
	����������� INamedObjListReader. �������� ���������� � OnInitDialog. 
	���������� � OnOK
 */
class NamedObjectSelectorDlg : public CDialog
{
	DECLARE_DYNCREATE(NamedObjectSelectorDlg)

public:
	NamedObjectSelectorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~NamedObjectSelectorDlg();

// Dialog Data
	enum { IDD = IDD_DLG_NAMED_OBJECT_SELECTOR};

	/// ������������� �������� ����������
	void setListLoader(INamedObjListToList *);
	/// ������������� ��������� ��������
	void setListReader(INamedObjListReader *);
	/// ������������� ��������� ����
	void setCapId(UINT_PTR uiCapID);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	void InsertColumns();
private:
	INamedObjListToList *m_ptrListLoader;
	INamedObjListReader *m_ptrListReader;
	CListCtrl m_list;
	UINT_PTR m_uiCapId; 
};
