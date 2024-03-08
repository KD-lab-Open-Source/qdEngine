#if !defined(AFX_HARBOUND_H__96813AD9_70A6_4A0F_80ED_271B811C9DBB__INCLUDED_)
#define AFX_HARBOUND_H__96813AD9_70A6_4A0F_80ED_271B811C9DBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// charBound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CharBound dialog
#include "basepage.h"

class qdGameObjectMoving;
class qdGameDispatcher;
/*!
  Страница свойств, на которой редактируется размеры баунда персонажа
*/

class CharBound : public CBasePage
{
	DECLARE_DYNCREATE(CharBound);
	//! True до конца работы OnInitDialog
	bool m_bInitDialog;
	//! Диспетчер игры 
	qdGameDispatcher	*m_ptrGameDisp;
	//! Персонаж, баунд которого изменяем
	qdGameObjectMoving	*m_ptrObj;

	//! Копия исходного размера по Х
	int m_iXBoundBak;
	//! Копия исходного размера по У
	int m_iYBoundBak;
	//! Копия исходного размера по Z
	int m_iZBoundBak;
	//! Копия исходного масштаба
	float m_fScaleBak;
	//! Флаг, говорящий о том, что состояние имеет границы
	bool m_bStateHasBound;
	
	//! Данные из объекта копируются в переменные диалога
	void Obj2Dlg();
	//! Данные из переменных диалога копируются в их *Bak версии
	void real2bak();
	//! Данные из Bak версий копируются в переменные диалога
	void bak2real();
	//! Из переменных диалога копирует данные в объект
	void apply_params();
	//! Перерисовывает окно редактора сцены
	void redraw();
	//! Устанавливает область изменения для SpinControl'ов
	void AdjustSpinRanges();

	//! Определяет является ли объект глобальным
	bool IsGlobalObj();

	//! Для всех копий персонажа назначает установленные параметры баунда
	void SetBoundToCopy();
	//! Делает недоступными контролы, редактирующие размеры баунда
	void DisableBndCtrls();
	//!\brief сохраняем положение картики для всех состоняний объекта, которые
	//! не имеют собственной границы
	void SaveStatesScrLayout(Vect3f const& old_bnd, Vect3f const& bnd);
// Construction
public:
	CharBound();
	~CharBound();

// Dialog Data
	//{{AFX_DATA(CharBound)
	enum { IDD = IDD_PROPPAGE_CHARACTER_BOUND };
	/*!
	  переменные диалога
	*/
	int		m_iXBound;
	int		m_iYBound;
	int		m_iZBound;
	float	m_fScale;
	//}}AFX_DATA

	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj);

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CharBound)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CharBound)
	afx_msg void OnChangeBound();
	afx_msg void OnChangeEditScale();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAutoBnd();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_HARBOUND_H__96813AD9_70A6_4A0F_80ED_271B811C9DBB__INCLUDED_)
