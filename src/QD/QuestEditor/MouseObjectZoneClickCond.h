#pragma once
#include "twoobjectcond.h"

/**
	���� �������� �� ����
 */
class MouseObjectZoneClickCond :
	public TwoObjectCond
{
public:
	MouseObjectZoneClickCond(DWORD id, CWnd* parent = NULL);
	~MouseObjectZoneClickCond(void);
protected:
	/// ��������� ����
	virtual bool LoadObjects();
	/// ���������� ���� � �������
	virtual void OnSelchangeComboObject2();
	virtual bool IsCondDataComplete();
};

/**
	������ �� ���� ��� �����. �����, ����� ������� ��������
 */
class MouseObjectHoverZoneCond:
	public MouseObjectZoneClickCond
{
public:
	MouseObjectHoverZoneCond(DWORD id, CWnd* parent = NULL);
	~MouseObjectHoverZoneCond(void);
};