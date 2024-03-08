#if !defined(AFX_OneObjectCond_H__8A35CF9A_BE04_4CFE_A21C_E2200A1B3D1D__INCLUDED_)
#define AFX_OneObjectCond_H__8A35CF9A_BE04_4CFE_A21C_E2200A1B3D1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OneObjectCond.h : header file
//

#include "qd_condition.h"
#include "CondDlgBase.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// OneObjectCond dialog
/**
	Базовый класс для условий, у которых парметром является только один объект
 */
class OneObjectCond : public CondDlgBase
{
protected:
// Construction
public:
	OneObjectCond(DWORD id, CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(OneObjectCond)
	enum { IDD = IDD_DLG_COND_MOUSE_CLICK };
	CCJFlatComboBox	m_wndObj;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OneObjectCond)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OneObjectCond)
	virtual afx_msg void OnSelchangeComboObject();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual afx_msg void OnCbnSelchangeComboScenes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	bool IsCondDataComplete();
	/// Загружает зоны
	void LoadZones();
	/// Загружает объекты
	virtual void LoadObjects();
	/// Загружает сцены
	virtual bool LoadScenes();
	/// Возвращает выбранную сцену
	class qdGameScene* get_scene();

	/// Setter for objectComboStaticText_
	void setObjectComboStaticText(CString str);

protected:
	/// Список сцен
	CCJFlatComboBox m_scenes;
	/// Подпись у кобобокса
	CStatic m_scene_static;
	/// Подпись у кобобокса с объектами
	CString objectComboStaticText_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_OneObjectCond_H__8A35CF9A_BE04_4CFE_A21C_E2200A1B3D1D__INCLUDED_)
