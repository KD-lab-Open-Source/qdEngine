#pragma once

#include "ObjInStateCond.h"

/**
	����������� ��������� �������
 */
class ObjPrevStateCond : public ObjInStateCond
{
public:
	ObjPrevStateCond(DWORD id);
protected:
	/// ��������� � ������ ��������� ����� "������"
	virtual bool LoadStates(const qdGameObjectAnimated* pa);
	bool IsCondDataComplete();
};