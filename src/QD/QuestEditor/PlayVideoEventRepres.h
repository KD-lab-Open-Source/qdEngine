#pragma once
#include "interfaceeventrepresbase.h"

/**
	������������ ������� "��������� �����"
 */
class PlayVideoEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	PlayVideoEventRepres(void);
	virtual ~PlayVideoEventRepres(void);
	/// ��� ������� ����������� �����
	virtual LPCTSTR getDefaultValue() const;
	/// ��������� ������ ���� �����
	virtual void loadData(CWnd* pwnd) const;
};
