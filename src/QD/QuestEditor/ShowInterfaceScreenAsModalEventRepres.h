#pragma once
#include "InterfaceEventRepresBase.h"

/**
	������� "������� ������������ �����"
 */
class ShowInterfaceScreenAsModalEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ShowInterfaceScreenAsModalEventRepres(void);
	virtual ~ShowInterfaceScreenAsModalEventRepres(void);
	/// ��� ������� ����������� ������
	virtual LPCTSTR getDefaultValue() const;
	/// ������ ���� �������
	virtual void loadData(CWnd* pwnd) const;
};
