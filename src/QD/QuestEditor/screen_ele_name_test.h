#pragma once
#include "name_interf.h"

/**
	���������� INameTester ���������� ���������� ��� ��� �������� ����������
 */
class screen_ele_name_test : public INameTester
{
	/// �����, ���������� �������
	class qdInterfaceScreen* m_pScreen;
public:
	explicit screen_ele_name_test(qdInterfaceScreen* pScreen);
	~screen_ele_name_test(void);
	/// �������� ���������� ���, �������� �� ����� ��������� ������
	virtual const CString get_unique_name(const CString& str) const;
};
