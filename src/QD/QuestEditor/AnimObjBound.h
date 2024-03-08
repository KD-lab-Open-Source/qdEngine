#if !defined(AFX_ANIMOBJBOUND_H__35E9B853_0244_4A34_8181_CA4F71C34E13__INCLUDED_)
#define AFX_ANIMOBJBOUND_H__35E9B853_0244_4A34_8181_CA4F71C34E13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimObjBound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AnimObjBound dialog

class qdGameObjectAnimated;

#include "basepage.h"
/*!
	Страница свойств анимированного объекта, на которой редакитируется
	боунд.
*/
class AnimObjBound : public CBasePage
{
	DECLARE_DYNCREATE(AnimObjBound);
	//! Флаг, говорящий ининциализируется ли диалог
	bool m_bInitDialog;
	
	//! Резервная копия размера по Х
	int		m_iEditXBak;
	//! Резервная копия размера по Y
	int		m_iEditYBak;
	//! Резервная копия размера по Z
	int		m_iEditZBak;
	//! Имеет ли объект боунд
	bool	m_bHasBound;
	
	//! Копирует данные из изменяемых полей в резервные копии
	VOID real2bak();
	//! Копирует данные из резервных копий в изменняемые поля
	VOID bak2real();

	//! Из объекта данные в изменяемые поля
	VOID Obj2Wnd();
	//! Данные из изменяемых полей в объект
	VOID Wnd2Obj();

	//! Объект, размеры которого меняются
	qdGameObjectAnimated	*m_ptrObj;
// Construction
public:
	AnimObjBound();
	~AnimObjBound();

	//! Назначает объект, размеры которого меняются
	bool set_data(qdGameObjectAnimated* ptrAnim);
// Dialog Data
	//{{AFX_DATA(AnimObjBound)
	enum { IDD = IDD_PROPPAGE_ANIM_OBJ_BOUND };
	//! Изменяемое поле размера по Х
	int		m_iEditX;
	//! Изменяемое поле размера по Y
	int		m_iEditY;
	//! Изменяемое поле размера по Z
	int		m_iEditZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(AnimObjBound)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AnimObjBound)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditX();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDeltaposSpinX1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinY1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinZ1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ANIMOBJBOUND_H__35E9B853_0244_4A34_8181_CA4F71C34E13__INCLUDED_)
