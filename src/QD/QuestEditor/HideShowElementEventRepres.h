#pragma once
#include "interfaceeventrepresbase.h"

/**
	������������ ������� "��������/������ �������"
 */
class HideShowElementEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	HideShowElementEventRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual ~HideShowElementEventRepres(void);
	/// ��� ������� ����������� �������� � ��������� ������
	virtual LPCTSTR getDefaultValue() const;
	/// ������ ��������� �� ���������� ������
	virtual void loadData(CWnd* pwnd) const;
};
