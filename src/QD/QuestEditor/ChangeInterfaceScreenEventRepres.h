#pragma once
#include "interfaceeventrepresbase.h"

/**
	������� "������� ������������ �����"
 */
class ChangeInterfaceScreenEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ChangeInterfaceScreenEventRepres(void);
	virtual ~ChangeInterfaceScreenEventRepres(void);
	/// ��� ������� ����������� ������
	virtual LPCTSTR getDefaultValue() const;
	/// ������ ���� �������
	virtual void loadData(CWnd* pwnd) const;
};
