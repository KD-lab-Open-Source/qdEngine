#pragma once


// ObjectDistanceCond dialog
#include "CondDlgBase.h"
#include "afxwin.h"
/**
	Расстояние между двумя объетами
 */
class ObjectDistanceCond : public CondDlgBase
{
public:
	ObjectDistanceCond(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~ObjectDistanceCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJ_DISTANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/// Загружает объекты в m_wndObjs2 и кладет первый объект в условие
	afx_msg void OnCbnSelchangeComboObject();
	/// Загружает объекты в m_wndObjs1
	afx_msg void OnCbnSelchangeComboScenes();
	/// Кладет дистанцию в условие
	afx_msg void OnDeltaposSpinDistance(NMHDR *pNMHDR, LRESULT *pResult);
	/// Кладет второй объект в условие
	afx_msg void OnCbnSelchangeComboObject2();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	/// Загружает список сцен
	bool LoadScenes();
	/// Загружает объекты в m_wndObjs1
	bool LoadObjects();
	bool IsCondDataComplete();
	/// Выдает выбранную сцену
	class qdGameScene* get_scene();
	/// Выдает выбранный в комбобоксе объект
	qdGameObject* get_object(CCJFlatComboBox& cb);
private:
	CCJFlatComboBox m_wndObjs1;
	CCJFlatComboBox m_wndObjs2;
	CCJFlatComboBox m_scenes;
	CStatic m_scene_static;
	//! Расстояние между объектами по сетке
	float m_fDist;
};
