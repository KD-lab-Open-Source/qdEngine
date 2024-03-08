#pragma once

#include "CondDlgBase.h"
#include "afxwin.h"

// AnyPersonageOnNetDlgCond dialog
/**
	Любой персонаж в зоне сетки
 */
class AnyPersonageOnNetDlgCond : public CondDlgBase
{
	DECLARE_DYNAMIC(AnyPersonageOnNetDlgCond)

public:
	AnyPersonageOnNetDlgCond(DWORD condId, CWnd* pParent = NULL); // standard constructor
	virtual ~AnyPersonageOnNetDlgCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_ANY_MOVING_ON_NET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/// Загружаем заново зоны
	afx_msg void OnCbnSelchangeComboScenes();
	/// Назначаем зону в условие
	afx_msg void OnCbnSelchangeComboZone();

	DECLARE_MESSAGE_MAP()
	virtual bool IsCondDataComplete();
	virtual BOOL OnInitDialog();

	/// Загружает зоны для сцены
	bool LoadZones(qdGameScene* pscene);

	/// возвращает выбранную зону
	qdGameScene* get_scene();
private:
	/// Список зон, принадлежащих выбранной сцене
	CCJFlatComboBox m_wndZones;
	/// Список сцен
	CCJFlatComboBox m_wndScenes;
};
