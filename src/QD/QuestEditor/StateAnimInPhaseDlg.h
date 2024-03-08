#pragma once

#include "CondDlgBase.h"
#include "afxwin.h"
// StateAnimInPhaseDlg dialog

/**
	Анимация состояния в фазе
 */
class StateAnimInPhaseDlg : public CondDlgBase
{

public:
	StateAnimInPhaseDlg(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~StateAnimInPhaseDlg();

// Dialog Data
	enum { IDD = IDD_DLG_COND_ANIM_PHASE};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/// Записывает состояние в условие 
	afx_msg void OnCbnSelchangeComboStates();
	/// Перезагружает состояния
	afx_msg void OnCbnSelchangeComboObject();
	/// Перезагружает объекты
	afx_msg void OnCbnSelchangeComboScenes();

	/// Кладет начало промежутка в условие
	afx_msg void OnDeltaposSpinBegPhase(NMHDR *pNMHDR, LRESULT *pResult);
	/// Кладет конец промежутка в условие
	afx_msg void OnDeltaposSpinEndPhase(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	bool IsCondDataComplete();

	/// ничего не делает
	void CondToDlg();
	/// Загружает сцены
	bool LoadScenes();
	/// Загружает объекты
	bool LoadObjects();
	/// Достает сцену из кобо
	class qdGameScene* get_scene();
	/// Загружает состояния для объекта
	bool LoadStates(qdGameObjectAnimated const* pobj);
private:
	/// Начало промежутка для фазы
	int m_iFrom;
	/// Конец промежутка для фазы
	int m_iTo;
	CCJFlatComboBox m_wndStates;
	CCJFlatComboBox m_wndObjs;
	CCJFlatComboBox m_wndScenes;
	CStatic m_wndSceneStatic;
};
