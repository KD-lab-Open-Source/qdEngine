#pragma once

#include "afxwin.h"
#include "PureTwoObjCondBase.h"

class TwoObjIntersCondDlg : public PureTwoObjCondBase
{
public:
	TwoObjIntersCondDlg(DWORD id, CWnd* pParent = NULL);
	~TwoObjIntersCondDlg();

	//! Функции загрузки объектов в комбобоксы
	/* virtual */ bool LoadToObjCombo1(const qdGameScene* scene);
	/* virtual */ bool LoadToObjCombo2(const qdGameScene* scene);

	//! Проверка правильности выбранных пользователем объектов
	/* virtual */ bool ValidObjects(const qdGameObject* obj1, const qdGameObject* obj2) const;
};
