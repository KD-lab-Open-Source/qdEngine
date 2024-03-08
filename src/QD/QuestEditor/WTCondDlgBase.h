#pragma once


// WTCondDlgBase
#include "CondDlgBase.h"

/**
	Базовый диалог для работы с "временем выполнения" состояния
 */
class WTCondDlgBase : public CondDlgBase
{
	DECLARE_DYNAMIC(WTCondDlgBase)
public:
	WTCondDlgBase(UINT nIDTemplate, DWORD id,
		UINT uiStatesComboId, UINT uiStatesScenesComboId, UINT uiSceneStaticId);
	virtual ~WTCondDlgBase();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	/// Вставляет состояние в условие
	afx_msg virtual void OnCbnSelchangeComboStates();
	/// Перезагружает список сцен
	afx_msg virtual void OnCbnSelchangeComboStateScene();

	DECLARE_MESSAGE_MAP()

	/// Загрузка списка сцен
	virtual bool LoadScenes();
	virtual BOOL OnInitDialog();

	CCJFlatComboBox& getStateCombo(){
		return m_wndStatesCombo;
	}
	CCJFlatComboBox& getStatesSceneCombo(){
		return m_wndStatesSceneCombo;
	}

	/// Возвращает выбранную сцену
	qdGameScene* getScene();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	/// Комбо для состояния
	CCJFlatComboBox m_wndStatesCombo;
	/// Комбо для сцен
	CCJFlatComboBox m_wndStatesSceneCombo;
	/// ИД комбо для состояний в темплейте диалога
	/**
		Нужен, чтобы увязать m_wndStatesSceneCombo с контролом в диалоге
	 */
	UINT m_uiStatesComboId;
	/// ИД комбо сцен в темплейте диалога
	/**
		Нужен, чтобы увязать m_wndStatesSceneCombo с контролом в диалоге
	 */
	UINT m_uiStatesScenesComboId;
	/// ИД элемента статик рядом с комбо для сцен
	UINT m_uiSceneStaticId;
};


