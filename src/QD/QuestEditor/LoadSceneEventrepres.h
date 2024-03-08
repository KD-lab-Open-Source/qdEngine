#pragma once
#include "interfaceeventrepresbase.h"

/**
	������������ ������� "��������� �����"
 */
class LoadSceneEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	LoadSceneEventRepres(UINT_PTR descr_id);
	virtual ~LoadSceneEventRepres(void);
	/// ��� ������ ���������� �����
	virtual LPCTSTR getDefaultValue() const;
	/// ��������� ������ ���� ����
	virtual void loadData(CWnd* pwnd) const;
private:
	/**
		������� ��� �������� ���� � ���������
	 */
	class AddToCombo;
};
