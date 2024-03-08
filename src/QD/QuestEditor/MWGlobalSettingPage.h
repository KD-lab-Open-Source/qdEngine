// MWGlobalSettingPage.h: interface for the MWGlobalSettingPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MWGLOBALSETTINGPAGE_H__5FCD0BCD_A830_4EF4_B785_9326085FCD81__INCLUDED_)
#define AFX_MWGLOBALSETTINGPAGE_H__5FCD0BCD_A830_4EF4_B785_9326085FCD81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizardBasePage.h"
#include "cammoderep.h"
#include "afxwin.h"
/**
	Единственная страничка визарда глобальных настроек.
	Выбираем только разрешение
 */
class MWGlobalSettingPage : public WizardBasePage
{
	DECLARE_DYNCREATE(MWGlobalSettingPage);
	
	/// указатель на глобальный диспетчер
	class qdGameDispatcher* m_ptrGameDisp;
	// Construction
public:
	MWGlobalSettingPage();
	~MWGlobalSettingPage();
	
	bool set_dispatcher(qdGameDispatcher* ptrDisp);
	// Dialog Data
	//{{AFX_DATA(MWGlobalSettingPage)
	enum { IDD = IDD_PROPPAGE_GAME_SETTINGS4START_WIZARD };
	CCJFlatComboBox	m_combo;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MWGlobalSettingPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnWazardFinish();
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	afx_msg void OnCbnSelchangeComboCameraMode();
	//}}AFX_VIRTUAL
	
	BOOL OnInitDialog();
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MWGlobalSettingPage)
	afx_msg void OnSelchangeResolution();
	afx_msg void OnCbnSelchangeComboScreen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	/// Загружает возможные разрешения
	bool LoadResolutions();
	bool LoadCameraModes();
	bool UpdateWizardBtnsState();
	/// Загружает список экранов интерфейса в box
	bool LoadInterfaceScreens(CCJFlatComboBox& box);

	void SetupStartupScene();
	void SetupMainMenu();
	void SetupIngameInvScr();//с инвентори
	void SetupIngameScr();//без инвентори
private:
	//! Скорость скроллирования камере в пикселях в секутду
	int m_iScrollSpeed;
	//! На каком расстоянии от центра объекта до края окна начинается скроллирование
	int m_iScroolDistance;
};

#endif //!defined(AFX_MWGLOBALSETTINGPAGE_H__5FCD0BCD_A830_4EF4_B785_9326085FCD81__INCLUDED_)
