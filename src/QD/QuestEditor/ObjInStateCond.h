#pragma once


#include "CondDlgBase.h"
#include "afxwin.h"
// ObjInStateCond dialog

/**
	Объект в состоянии
 */
class ObjInStateCond : public CondDlgBase
{

public:
	ObjInStateCond(DWORD id, CWnd* pParent = NULL);
	virtual ~ObjInStateCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJ_IN_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	/// Кладет имя объекта в условие и перезагруажет список состояний
	afx_msg virtual void OnCbnSelchangeComboObject();
	/// Кладет имя состояния в условие
	afx_msg virtual void OnCbnSelchangeComboStates();
	/// перезагружает список объектов
	afx_msg virtual void OnCbnSelchangeComboScenes();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();

	bool IsCondDataComplete();
	/// Загружает список объектов из выбранной сцены
	bool LoadObjects();
	/// Загружает список состояний выбранного объекта
	virtual bool LoadStates(const qdGameObjectAnimated* pa);
	/// Загружает список сцен
	bool LoadScenes();
	/// Возвращает выбранную сцену
	class qdGameScene* get_scene();
	CCJFlatComboBox& getStatesCombo(){
		return m_wndStates;
	}
protected:
	/// Список объектов
	CCJFlatComboBox m_wndObjs;
	/// Список состояний
	CCJFlatComboBox m_wndStates;
	/// Список сцен
	CCJFlatComboBox m_wndScenes;
	/// Подпись у списка сцен
	CStatic m_wndSceneStatic;
};
