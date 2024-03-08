#if !defined(AFX_SCREENPROP_H__37B251CC_2260_4530_899B_63BE62A071F8__INCLUDED_)
#define AFX_SCREENPROP_H__37B251CC_2260_4530_899B_63BE62A071F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScreenProp dialog
class qdGameDispatcher;
class qdGameScene;

#include "basepage.h"
#include "afxwin.h"
/**
	Свойства сцены
 */
class ScreenProp : public CBasePage
{
// Construction
	/// Диспетчер проекта
	qdGameDispatcher*	m_ptrGameDisp;
	/// Сцена
	qdGameScene*		m_ptrScene;
	/// можно ли перерисовывать сцену
	/**
		если сцена активна, значит можно
	 */
	bool				m_bRedraw;
	/// идентификатор ячейки автосейва
	int m_iSaveId;
public:
	ScreenProp(CWnd* pParent = NULL);   // standard constructor

	/// Назначает обрабатываемую сцену и диспетчер
	bool set_data(qdGameDispatcher* ptrGD, qdGameScene* ptrGS);
// Dialog Data
	//{{AFX_DATA(ScreenProp)
	enum { IDD = IDD_PROPPAGE_SCENE_GENERAL };
	/// Ширина игровой области
	int		m_iCellX;
	/// Высота игровой области
	int		m_iCellY;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScreenProp)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScreenProp)
	afx_msg void OnChangeScreenSize();
	afx_msg void OnDeltaposSpinSaveId(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG*);
	afx_msg void OnBnClickedCheckCycle();
	afx_msg void OnCbnSelchangeComboMinigames();
	afx_msg void OnCbnSelchangeComboInterfaceScreens();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
protected:
	/// Назначает/убирает указанный флаг для сцены
	void set_flag(bool bSetFlag, int iValue);
	/// Возвращает имя миниигры, выбранной из списка миниигры
	CString const getMinigameName() const;
	/// Загружает список миниигр
	void loadMinigames();
	/// Возвращает имя интерфейсного экрана, выбранного из списка экранов
	CString const getInterfaceScreenName() const;
	/// Загружает список экранов интерфейса
	void loadInterfaceScreens();
private:
	/// Применяет параметры игровой области
	bool apply_screen_params();
	/// Ничего не делает. Не нужна
	bool apply_va_params();
	/// Применяет все изменненные параметры
	bool apply_params();
	/// Перерисовывает сцену в редаторе сцены
	void redraw();
	//! Зациклить по Х
	BOOL m_bCycleX;
	//! Зациклить по Y
	BOOL m_bCycleY;
	//! Флаг qdGameScene::RESET_TRIGGERS_ON_LOAD
	BOOL resetTriggersOnLoading_;
	BOOL enablePersonageSwitch_;
	BOOL disableMainMenuExit_;
	//! Список миниигр
	CCJFlatComboBox miniGames_;
	//! Список экранов интерфейса
	CCJFlatComboBox interfaceScreens_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SCREENPROP_H__37B251CC_2260_4530_899B_63BE62A071F8__INCLUDED_)
