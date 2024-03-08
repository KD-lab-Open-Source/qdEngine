#pragma once

#include "WizardBasePage.h"

// SceneCountMDSPP dialog

/**
	Страничка запрашивает количество сцен для создания
 */
class SceneCountMDSPP : public WizardBasePage
{
	DECLARE_DYNAMIC(SceneCountMDSPP)

public:
	SceneCountMDSPP();
	virtual ~SceneCountMDSPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MDS_SCENE_COUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnEnChangeEdit();

	/**
		Дизейблит/енейблит кнопку Next в зависимости от m_iCount
	 */
	void check_next_btn_access();
private:
	//! количество сцен
	int m_iCount;
};
