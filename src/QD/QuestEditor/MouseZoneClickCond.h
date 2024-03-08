#pragma once
#include "OneObjectcond.h"

/**
	Клик мышью по зоне
 */
class MouseZoneClickCond :
	public OneObjectCond
{
public:
	MouseZoneClickCond(DWORD id, CWnd* pParent =NULL);
	~MouseZoneClickCond(void);
protected:
	/// Записывает выбранный объект в условие
	void OnSelchangeComboObject();
	/// Загружает зоны
	virtual void LoadObjects();
	virtual bool IsCondDataComplete();
};
