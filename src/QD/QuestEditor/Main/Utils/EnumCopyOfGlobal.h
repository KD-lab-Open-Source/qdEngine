#pragma once

class qdGameDispatcher;
class qdGameObject;

#include "qd_fwd.h"

/// ������� ��� ����� ����������� �������, ��������������� ������� ��������
class EnumCopyOfGlobal
{
public:
	EnumCopyOfGlobal(void);
	~EnumCopyOfGlobal(void);
	typedef bool (*CHECK_FUN)(qdGameObject const* p);
/*!
 	��������� �� ���� ������. ���� ����� ����������� �������, ���������� ������.
	���� ��� ��������� ����� �������-������� ���������� true, �� ���(�����) �����������
	� ������
	\param name	- ��� ����������� �������
	\param f	- ��������� �� �������-������ ��� ��������-�����
	\param pres	- ��������� �� ������, ���� ����� ������� ��� ����� ��������� ����������
*/
	static void enumerate(qdGameDispatcher const* gameDisp, 
							LPCTSTR name, 
							CHECK_FUN f, qdGameObjectList* pres);
};
