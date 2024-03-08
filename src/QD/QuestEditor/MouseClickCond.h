#pragma once

#include "oneobjectcond.h"
/**
	Кликнули мышкой по объекту
 */
class MouseClickCond :
	public OneObjectCond
{
public:
	MouseClickCond(DWORD id, CWnd* parent = NULL);
	~MouseClickCond(void);
protected:
	/// Загружает объекты
	virtual void LoadObjects();
	/// Записывает объект в условие
	virtual void OnSelchangeComboObject();
};
