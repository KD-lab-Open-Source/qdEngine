#pragma once
#include "oneobjectcond.h"

/**
	������ �����
 */
class ObjectHiddenCond :
	public OneObjectCond
{
public:
	ObjectHiddenCond(DWORD id, CWnd * parent = NULL);
	~ObjectHiddenCond(void);
protected:
	/// ��������� �������
	virtual void LoadObjects();
	/// ������ ������ � �������
	virtual void OnSelchangeComboObject();
	virtual bool IsCondDataComplete();
};
