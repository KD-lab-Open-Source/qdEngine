#pragma once

#include "ObjInStateCond.h"

/**
	Передыдущее состояние объекта
 */
class ObjPrevStateCond : public ObjInStateCond
{
public:
	ObjPrevStateCond(DWORD id);
protected:
	/// Добавляет к списку состояний пункт "Ничего"
	virtual bool LoadStates(const qdGameObjectAnimated* pa);
	bool IsCondDataComplete();
};