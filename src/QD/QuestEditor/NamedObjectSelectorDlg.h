#pragma once

#include "INamedObjListTo.h"
#include "INamedObjListReader.h"
#include "afxcmn.h"

// NamedObjectSelectorDlg dialog
/// Диалог выбора объектов
/**
	Диалог предназначен для вывода списка qdNamedObject с тем, чтобы отобрать нужные. 
	Нужные помечаются галочкой в чекбоксе.
	Загружаются объекты с помощью интерфейса INamedObjectListToList.
	Считываются INamedObjListReader. Загрузка происходит в OnInitDialog. 
	Считывание в OnOK
 */
class NamedObjectSelectorDlg : public CDialog
{
	DECLARE_DYNCREATE(NamedObjectSelectorDlg)

public:
	NamedObjectSelectorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~NamedObjectSelectorDlg();

// Dialog Data
	enum { IDD = IDD_DLG_NAMED_OBJECT_SELECTOR};

	/// Устанавливает интерфес загрузчика
	void setListLoader(INamedObjListToList *);
	/// Устанавливает интерфейс читателя
	void setListReader(INamedObjListReader *);
	/// Устанавливает заголовок окна
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
