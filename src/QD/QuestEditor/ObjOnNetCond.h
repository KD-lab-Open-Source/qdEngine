#if !defined(AFX_OBJONNETCOND_H__DA4E6B4C_578D_4686_B12D_DFDC275D29F6__INCLUDED_)
#define AFX_OBJONNETCOND_H__DA4E6B4C_578D_4686_B12D_DFDC275D29F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjOnNetCond.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ObjOnNetCond dialog
#include "qd_condition.h"
#include "CondDlgBase.h"
#include "afxwin.h"

/**
	Объект в зоне
 */
class ObjOnNetCond : public CondDlgBase
{
// Construction
public:
	ObjOnNetCond(DWORD id, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ObjOnNetCond)
	enum { IDD = IDD_DLG_COND_OBJ_ON_NET };
	CCJFlatComboBox	m_wndZones;
	CCJFlatComboBox	m_wndObjs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ObjOnNetCond)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ObjOnNetCond)
	virtual BOOL OnInitDialog();
	/// Кладет объект в условие
	afx_msg void OnSelchangeComboObject();
	/// Кладет зону в условие
	afx_msg void OnSelchangeComboZone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	bool IsCondDataComplete();
	/// Загружает объекты
	bool LoadObjects();
	/// Загружает зоны
	bool LoadZones();
	/// Загружает сцены
	bool LoadScenes();
	/// Возвращает выбранную сцену
	class qdGameScene* get_scene();
	CCJFlatComboBox m_scenes;
public:
	CStatic m_scenes_static;
	/// Загружает объекты и зоны для выбранной сцены
	afx_msg void OnCbnSelchangeComboScenes();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_OBJONNETCOND_H__DA4E6B4C_578D_4686_B12D_DFDC275D29F6__INCLUDED_)
