#pragma once

#include "oneobjectcond.h"
/**
	�������� ������ �� �������
 */
class MouseClickCond :
	public OneObjectCond
{
public:
	MouseClickCond(DWORD id, CWnd* parent = NULL);
	~MouseClickCond(void);
protected:
	/// ��������� �������
	virtual void LoadObjects();
	/// ���������� ������ � �������
	virtual void OnSelchangeComboObject();
};
