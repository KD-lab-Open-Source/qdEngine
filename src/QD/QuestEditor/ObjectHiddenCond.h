#pragma once
#include "oneobjectcond.h"

/**
	Объект скрыт
 */
class ObjectHiddenCond :
	public OneObjectCond
{
public:
	ObjectHiddenCond(DWORD id, CWnd * parent = NULL);
	~ObjectHiddenCond(void);
protected:
	/// Загружает объекты
	virtual void LoadObjects();
	/// Кладет объект в условие
	virtual void OnSelchangeComboObject();
	virtual bool IsCondDataComplete();
};
