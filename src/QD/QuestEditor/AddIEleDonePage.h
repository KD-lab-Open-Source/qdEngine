#pragma once


// AddIEleDonePage dialog
#include "wizardbasepage.h"

/**
	��������� �������� � �������� ���������� �������� ����������.
	��� ����������� ���� �������� ���������� �������� ��������.
 */
class AddIEleDonePage : public WizardBasePage
{
	DECLARE_DYNAMIC(AddIEleDonePage)

public:
	/// ��������� �������, ��������� ������
	class obj_fabric{
	public:
		/// ����� ��������� ������
		virtual bool create_obj() = 0;
	};
	AddIEleDonePage();
	virtual ~AddIEleDonePage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_IELE_ADD_DONE };

	/// ��������� �������
	void set_obj_fabric(obj_fabric* pof);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	/// �������� ������� ������. �������� � ����������� ������������
	virtual BOOL OnSetActive();
private:
	/// �������, ��������� ������
	obj_fabric* m_ptrObjFabric;
	/// �������������� ��������� ������������
	CString m_strMessage;
};
