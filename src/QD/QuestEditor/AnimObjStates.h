#if !defined(AFX_ANIMOBJSTATES_H__6988C046_0E61_4FB2_83BB_713DABFF399E__INCLUDED_)
#define AFX_ANIMOBJSTATES_H__6988C046_0E61_4FB2_83BB_713DABFF399E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimObjStates.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AnimObjStates dialog
#include "editlistctrl.h"

class qdGameDispatcher;
class qdGameObjectAnimated;
/*!
  Страница свойств анимированного объекта, на которой отображается
  список состояний объекта
  Подозреваю, что уже не используется
*/

#include "basepage.h"
class AnimObjStates : public CBasePage
{
	DECLARE_DYNCREATE(AnimObjStates)

	//! Инициализируется ли диалог
	bool m_bInitDialog;
	//! Объект, состояния которого отображаются
	qdGameObjectAnimated	*m_ptrObj;
	//! Диспетчер игры
	qdGameDispatcher		*m_ptrDisp;

	// Construction
public:
	AnimObjStates();
	AnimObjStates(UINT nDlgID, UINT nCapID);
	~AnimObjStates();
	//! Назначает объект, чьи сотояния редактируются, и диспетчер игры
	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectAnimated* ptrObj);
	//! Проверяет является ли имя \a сотояния уникальным в пределах объекта
	bool TestName(int iItem, int iSubItem, LPCTSTR lpszText);
	// Dialog Data
	//{{AFX_DATA(AnimObjStates)
	enum { IDD = IDD_PROPPAGE_OBJECT_ANIM_STATES };
	//! Контрол, отображающий список сотояний
	CEditListCtrl	m_list;
	//}}AFX_DATA

	
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(AnimObjStates)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AnimObjStates)
	//! Добавляет состояние
	afx_msg void OnAddState();
	//! Запускает редактор свойств состояния
	afx_msg void OnChangeState();
	//! Удаляет состояние
	afx_msg void OnDelState();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnCond();
	//}}AFX_MSG
	afx_msg void OnEndItemEditList1(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	//! Загружает список состояний
	bool LoadStates();
	//! Вставляет состояние в указанную позицию в списке состояний
	bool AddState2List(int iIndex, qdGameObjectStateStatic* ptrState);
	//! Заполняет столбцы для состояния
	void UpdateState(int iIndex, qdGameObjectStateStatic* ptrState);
	//! Показывает диалог свойств для состояния, если это несостояние маска
	bool ShowEditStateDlg(qdGameObjectStateStatic* ptrState);
//	bool ShowCondDlg(qdGameObjectStateStatic* ptrState);
	//! Выравнивает ширину столбцов по длинее самой длинной строки
	void AdjustColumnWidth();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ANIMOBJSTATES_H__6988C046_0E61_4FB2_83BB_713DABFF399E__INCLUDED_)
