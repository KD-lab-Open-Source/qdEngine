#pragma once
#include "interfaceeventrepresbase.h"
/**
	������������ ������� "��������� ���������"
 */
class ActivatePersonageEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ActivatePersonageEventRepres(void);
	virtual ~ActivatePersonageEventRepres(void);
	/// ������� ������ ����������� ����� � ���������� ��� ���
	virtual LPCTSTR getDefaultValue() const;
	/// ��������� ���������� ������ ���� ����������. �������� ����� �� ���� ���� �������
	virtual void loadData(CWnd*pwnd) const;
};
