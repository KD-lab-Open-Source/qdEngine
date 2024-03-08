#pragma once
#include "OneObjectcond.h"

/**
	���� ����� �� ����
 */
class MouseZoneClickCond :
	public OneObjectCond
{
public:
	MouseZoneClickCond(DWORD id, CWnd* pParent =NULL);
	~MouseZoneClickCond(void);
protected:
	/// ���������� ��������� ������ � �������
	void OnSelchangeComboObject();
	/// ��������� ����
	virtual void LoadObjects();
	virtual bool IsCondDataComplete();
};
