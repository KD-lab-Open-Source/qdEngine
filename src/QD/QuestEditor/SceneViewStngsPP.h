#pragma once

#include "AppSettingsBasePage.h"
#include "afxwin.h"
// SceneViewStngsPP dialog

/**
	Страничка свойств-приложения, которую вставляет редатор сцены
	Управляет величинами, на которые размер рабочей области превосходит 
	размеры сцены в соответствующем направлении
 */
class SceneViewStngsPP : public AppSettingsBasePage
{
//	DECLARE_DYNAMIC(SceneViewStngsPP)

public:
	SceneViewStngsPP();
	virtual ~SceneViewStngsPP();

// Dialog Data
	enum { IDD = IDD_PO_PROPPAGE_SCENE_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnEnChangeEdit();

	DECLARE_MESSAGE_MAP();

private:

	/// Вылечина отступа выше и ниже сцены
	UINT m_iTopBottom;
	/// Вылечина отступа слева и справа сцены
	UINT m_iLeftRight;
};
