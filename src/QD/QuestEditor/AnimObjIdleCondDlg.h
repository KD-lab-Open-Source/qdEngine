#pragma once

#include "afxwin.h"
#include "CondDlgBase.h"


// AnimObjIdleCondDlg dialog

/**
	Объект не шевелится время больше, чем заданно
 */
class AnimObjIdleCondDlg : public CondDlgBase
{
	DECLARE_DYNAMIC(AnimObjIdleCondDlg)

public:
	AnimObjIdleCondDlg(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~AnimObjIdleCondDlg();
	/*virtual*/ bool IsCondDataComplete();

// Dialog Data
	enum { IDD = IDD_DLG_COND_ANIM_OBJ_IDLE_GRT_VAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	/// Список сцен
	CCJFlatComboBox m_cbScenes;
	/// Список объктов для выбранной сцены
	CCJFlatComboBox m_cbObjects;
	/// Значение времени простоя, превышение которго ведет к срабатыванию условия
	int m_iValue;
public:
	virtual BOOL OnInitDialog();
	/// На смену сцены перезагружаем объекты
	afx_msg void OnCbnSelchangeComboScenes();
	/// Назначаем в условие объект
	afx_msg void OnCbnSelchangeComboObject();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnChangeEdit();
};
