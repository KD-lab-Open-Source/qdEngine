#pragma once

class qdGameDispatcher;
class qdGameDispatcherBase;

///	\brief ����� �������� � ���� �������� ������������� � �������� � ����
///	������� �������� ��������
class AddAnimMultiWizard
{
public:
	AddAnimMultiWizard(void);
	~AddAnimMultiWizard(void);
	/// ��������� ������
	/**
		\return true - ���� ���� ������� ���������
	 */
	bool show(CWnd * parent, class qdGameDispatcher* ptrDisp, 
			qdGameDispatcherBase* ptrGDB, UINT idCap = 0);
};
