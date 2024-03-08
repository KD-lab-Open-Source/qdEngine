#if !defined(AFX_TwoObjectCond_H__9C4A941A_5ADC_49B7_A26D_147C73B2B93D__INCLUDED_)
#define AFX_TwoObjectCond_H__9C4A941A_5ADC_49B7_A26D_147C73B2B93D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TwoObjectCond.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TwoObjectCond dialog
#include "qd_condition.h"
#include "CondDlgBase.h"
#include "afxwin.h"

/**
	Базовый класс для условий, которым необходимо указать два объекта
 */
class TwoObjectCond : public CondDlgBase
{
	// Construction
public:
	TwoObjectCond(DWORD id, CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(TwoObjectCond)
	enum { IDD = IDD_DLG_COND_MOUSE_OBJ_CLICK };
	/// Второй список объектов
	CCJFlatComboBox	m_wndObj2;
	/// Первый список объектов. В этот список входят объекты, которыми можно кликать
	CCJFlatComboBox	m_wndObj;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TwoObjectCond)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TwoObjectCond)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/// Записывает первый объект в условие
	virtual afx_msg void OnSelchangeComboObject();
	/// Записывает второй объект в условие
	virtual afx_msg void OnSelchangeComboObject2();
	/// Загружает объекты в m_wndObj
	afx_msg void OnCbnSelchangeComboScenes();
	/// Загружает объекты в m_wndObj2
	afx_msg void OnCbnSelchangeComboScenes2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	bool IsCondDataComplete();
	//! Загружает объекты, которыми можно кликать
	virtual bool LoadInventoryObjects();
	//! Загружает объекты, по которым будут кликать
	virtual bool LoadObjects();
	/// Загружает список сцен в комбобокс
	bool LoadScenes(CCJFlatComboBox& box, CStatic& box_static);
	/// Возвращает сцену из первого списка
	class qdGameScene* get_scene();
	/// Возвращает сцену из второго списка
	class qdGameScene* get_scene2();
	/// Назначает подпись для второго комбобокса с объектами
	void setSecondObjectStaticText(CString const& str);
	/// Назначает подпись для первого комбобокса с объектами
	void setFirstObjectStaticText(CString const& str);
protected:
	/// Имеющиеся в наличии сцены
	CCJFlatComboBox m_scenes;
	/// Второй набор сцен
	CCJFlatComboBox m_scenes2;
	/// Подписи рядом с комбоксами сцен
	CStatic m_scene_static;
	CStatic m_scene_static2;
	/// Подписи рядом с комбоксами объектов
	CString secondObjectStaticText_;
	CString firstObjectStaticText_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_TwoObjectCond_H__9C4A941A_5ADC_49B7_A26D_147C73B2B93D__INCLUDED_)
