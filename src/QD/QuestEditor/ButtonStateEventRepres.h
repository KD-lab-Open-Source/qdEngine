#pragma once
#include "interfaceeventrepresbase.h"

/**
	������� "�������� ������������ ��������� ������"
	����� �� �� ������ ����� ����� ��� ���� �� ������� � ������
 */
class ButtonStateEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ButtonStateEventRepres(void);
	virtual ~ButtonStateEventRepres(void);
	/// ��������� NULL
	virtual LPCTSTR getDefaultValue() const;
	/// ������ �� ������
	virtual void loadData(CWnd* pwnd) const;
};
