#pragma once
#include "OneObjectCond.h"

/**
	���� ��� ��������
 */
class MouseHoverCond :
	public OneObjectCond
{
public:
	MouseHoverCond(DWORD id, CWnd* pParent = NULL);
	~MouseHoverCond(void);
protected:
	/// ���������� ������ � �������
	virtual void OnSelchangeComboObject();
	/// ��� ��������� ������� "��� ����� �����"
	virtual void LoadObjects();
};
