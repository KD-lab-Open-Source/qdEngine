#if !defined(AFX_WALKSTOPDIRCOND_H__04231D35_B8F9_4677_BEA7_1F076222C1E8__INCLUDED_)
#define AFX_WALKSTOPDIRCOND_H__04231D35_B8F9_4677_BEA7_1F076222C1E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WalkStopDirCond.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// WalkStopDirCond dialog
#include "qd_condition.h"
#include "CondDlgBase.h"
#include "afxwin.h"

/**
	Направление походки/остановки
 */
class WalkStopDirCond : public CondDlgBase
{
// Construction
public:
	WalkStopDirCond(DWORD id, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(WalkStopDirCond)
	enum { IDD = IDD_DLG_COND_WALK_STOP_DIR };
	CCJFlatComboBox	m_wndComboDir;
	CCJFlatComboBox	m_wndPers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WalkStopDirCond)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(WalkStopDirCond)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPers();
	afx_msg void OnSelchangeComboDir();
	afx_msg void OnCbnSelchangeComboGaits();
	afx_msg void OnCbnSelchangeComboScenes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	bool IsCondDataComplete();

	/// Загружает список персонажей
	bool LoadPers();
	/// Список направлений
	bool LoadDirs();
	/// Список сцен
	bool LoadScenes();
	/// Список походок
	bool LoadGaits();
	/// Возвращает выбранную сцену
	class qdGameScene* get_scene();
	/// Возвращает выбранную походку
	class qdAnimationSet* get_gait();
private://types
	/**
		Функтор, вставляющий походку в комбобокс
	 */
	class GaitInserter;
private:
	CCJFlatComboBox m_scenes;
	CStatic m_scenes_static;
	/// Список доступных походок
	CCJFlatComboBox m_wndGaits;
	bool m_bInitDialog;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_WALKSTOPDIRCOND_H__04231D35_B8F9_4677_BEA7_1F076222C1E8__INCLUDED_)
