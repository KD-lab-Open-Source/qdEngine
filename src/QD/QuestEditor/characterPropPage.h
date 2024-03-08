#if !defined(AFX_CharacterGaitsPage_H__0548E911_31C4_4CC4_9350_0055806DA0C2__INCLUDED_)
#define AFX_CharacterGaitsPage_H__0548E911_31C4_4CC4_9350_0055806DA0C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CharacterGaitsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CharacterGaitsPage dialog

class qdGameObjectMoving;
class qdGameDispatcher;

#include "basepage.h"
#include "editlistctrl.h"

/*!
  Класс страницы свойств персонажа, работающий с походками
*/

class CharacterGaitsPage : public CBasePage
{
	DECLARE_DYNCREATE(CharacterGaitsPage);

	//! установлен в true до первого показа окна
	bool m_bInitDialog;
	//! Диспетчер игры
	qdGameDispatcher *m_ptrDisp;
	//! Объект, с походками которого работаем
	qdGameObjectMoving * m_ptrObj;

// Construction
public:
	CharacterGaitsPage();
	~CharacterGaitsPage();

// Dialog Data
	//{{AFX_DATA(CharacterGaitsPage)
	enum { IDD = IDD_PROPPAGE_OBJECT_ANIM_STATES };
	//! Список походок
	CEditListCtrl	m_list;
	//}}AFX_DATA
	//! Назначает диспетчер игры и объект
	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj);
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CharacterGaitsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	BOOL OnApply();
	//! проверяет не повторяется ли имя. Если нет, то назначает его
	bool TestName(int iItem, int iSubItem, LPCTSTR lpszText);
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CharacterGaitsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	//! Команда добавить походку
	afx_msg void OnAddState();
	//! Команда исправить походку
	afx_msg void OnChangeState();
	//! Команда удалить походку
	afx_msg void OnDelState();
	afx_msg void OnEndItemEditList1(NMHDR* pNMHDR, LRESULT* pResult);
	//! Команда редактировать условия
	afx_msg void OnBtnCond();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//! Загружает список походок в контрол
	bool LoadStates();
	//! Вставляет походку в список
	bool AddState2List(int iIndex, qdGameObjectStateWalk* ptrState);
	//! Для указанной походки заполняет текстом столбцы контрола 
	void UpdateState(int iIndex, qdGameObjectStateWalk* ptrState);
	//! Показывает диалог свойств состояния
	bool ShowEditStateDlg(qdGameObjectStateWalk* ptrState);
	//! Выравнивает ширину столбцов по тексту
	void AdjustColumnWidth();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_CharacterGaitsPage_H__0548E911_31C4_4CC4_9350_0055806DA0C2__INCLUDED_)
