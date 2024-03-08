#pragma once
#include "mousezoneclickcond.h"

/**
	Мышь поверх зоны
 */
class MouseHoverZoneCond :
	public MouseZoneClickCond
{
public:
	MouseHoverZoneCond(DWORD id, CWnd * parent = NULL);
	~MouseHoverZoneCond(void);
};
