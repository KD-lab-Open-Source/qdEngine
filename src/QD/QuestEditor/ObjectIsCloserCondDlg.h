#pragma once

#include "CondDlgBase.h"
#include "afxwin.h"

// ObjectIsCloserCondDlg dialog

/**
	Объект ближе к первому кадидату, чем ко второму
 */
class ObjectIsCloserCondDlg : public CondDlgBase
{
	DECLARE_DYNAMIC(ObjectIsCloserCondDlg)

public:
	ObjectIsCloserCondDlg(DWORD id);   // standard constructor
	virtual ~ObjectIsCloserCondDlg();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJECT_IS_CLOSER };

	bool IsCondDataComplete();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	/// Загружает списоки объектов
	afx_msg void OnCbnSelchangeComboScenes();
	/// Кладет первй в условие
	afx_msg void OnCbnSelchangeComboObject();
	/// Кладет второй в условие
	afx_msg void OnCbnSelchangeComboObject2();
	/// Кладет третий в условие
	afx_msg void OnCbnSelchangeComboObject3();

	DECLARE_MESSAGE_MAP();
	/// Возвращает выбранную сцену
	class qdGameScene* get_scene();

	/// Загружает объекты, исключая из загрузки указанные
	void loadObjects(CCJFlatComboBox& combo, 
					qdNamedObject const* excl, 
					qdNamedObject const* excl2);
	/// Доастает выбранный в комбо объект
	qdNamedObject* get_obj(CCJFlatComboBox& combo);
private:

	CCJFlatComboBox scenes_;
	CCJFlatComboBox firstObjects_;
	CCJFlatComboBox candidateObjects1_;
	CCJFlatComboBox candidateObjects2_;
};
