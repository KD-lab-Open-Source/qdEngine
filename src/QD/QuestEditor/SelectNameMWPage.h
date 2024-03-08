#pragma once

#include "WizardBasePage.h"
// SelectNameMWPage dialog

interface INameSetter;
interface INameTester;

/**
	��������� ������ ����� ��� �������� ����������
 */
class SelectNameMWPage : public WizardBasePage
{
	DECLARE_DYNAMIC(SelectNameMWPage)

public:
	SelectNameMWPage();
	virtual ~SelectNameMWPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SELECT_NAME };

	/// ��������� m_pStringSetter
	void SetNameSetter(INameSetter* psetter);
	/// ��������� m_pNameTester
	void SetNameTester(INameTester* pTester);
	/// ��������� ��������� ����
	void SetCaption(const CString& cap);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	/**
		���� ��� ������, �� ������ Next �����������.
		����� ���������� �� ������ ���������� ����� ���������� 
		� ���� ������ � Next
	 */
	afx_msg void OnEnChangeEditName();
private:
	/// ��������� ���
	CString m_strName;
	/// ��������� ������, ������������ ��� �������
	INameSetter* m_pStringSetter;
	/// ��������� �����, ������������ ������������ �����
	INameTester* m_pNameTester;
	/// ��������� �������
	CString m_strCaption;
};
