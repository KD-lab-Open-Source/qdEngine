#pragma once
#include "mousezoneclickcond.h"

/**
	���� ������ ����
 */
class MouseHoverZoneCond :
	public MouseZoneClickCond
{
public:
	MouseHoverZoneCond(DWORD id, CWnd * parent = NULL);
	~MouseHoverZoneCond(void);
};
