#pragma once
#include "twoobjectcond.h"

/**
	Клик объектом по зоне
 */
class MouseObjectZoneClickCond :
	public TwoObjectCond
{
public:
	MouseObjectZoneClickCond(DWORD id, CWnd* parent = NULL);
	~MouseObjectZoneClickCond(void);
protected:
	/// Загружает зоны
	virtual bool LoadObjects();
	/// Записывает зону в условие
	virtual void OnSelchangeComboObject2();
	virtual bool IsCondDataComplete();
};

/**
	Объект на мыши над зоной. Нужен, чтобы подписи поменять
 */
class MouseObjectHoverZoneCond:
	public MouseObjectZoneClickCond
{
public:
	MouseObjectHoverZoneCond(DWORD id, CWnd* parent = NULL);
	~MouseObjectHoverZoneCond(void);
};