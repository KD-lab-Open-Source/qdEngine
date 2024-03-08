#pragma once


// MovingActiveCond dialog
#include "conddlgbase.h"
#include "afxwin.h"

/**
	Активен указанный персонаж
 */
class MovingActiveCond : public CondDlgBase
{

public:
	MovingActiveCond(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~MovingActiveCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_MOVING_ACITVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	/// Персонажа в условие
	afx_msg void OnCbnSelchangeComboCharacter();
	/// Загружает список персонажей
	afx_msg void OnCbnSelchangeComboScenes();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	bool IsCondDataComplete();

	/// Загружает персонажей
	bool LoadPers();
	/// Загружает сцены
	bool LoadScenes();
	/// Доастает выбранную сцену
	class qdGameScene* get_scene();
private:
	/// Комбо с персонажами
	CCJFlatComboBox m_wndPers;
	/// Подпись у комбо сцен
	CStatic m_scenes_static;
	/// Комбо со сценами
	CCJFlatComboBox m_scenes;
};
