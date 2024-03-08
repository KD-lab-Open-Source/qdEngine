#pragma once
#include "interfaceeventrepresbase.h"

/**
	�������� ����������/��������� ��������� ������
 */
class PrevNextElementStateEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	PrevNextElementStateEventRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual ~PrevNextElementStateEventRepres(void);
	/// ���������� ��� ������ ���������� ������ �� ���������� ������
	virtual LPCTSTR getDefaultValue() const;
	/// ��������� ������ ���� ������ �� ���������� ������
	virtual void loadData(CWnd* pwnd) const;
};
