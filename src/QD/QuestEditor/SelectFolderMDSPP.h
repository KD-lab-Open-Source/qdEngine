#pragma once

#include "WizardBasePage.h"

// SelectFolderMDSPP dialog

/**
	�������� ��������, � ������� ����� ����������� ��������� ���������
 */
class SelectFolderMDSPP : public WizardBasePage
{
	DECLARE_DYNAMIC(SelectFolderMDSPP)

public:
	SelectFolderMDSPP();
	virtual ~SelectFolderMDSPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MDS_SELECT_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedButton();
	/// @brief ���������/�������� Next � ����������� �� ����, 
	/// ���������� �� ��������� �����
	void check_next_btn_access();
	/// ��������� ������ ������ ����� �� �����
	CString get_disk_folder();
private:
	/// �������� ��������� �����
	CString m_strFolder;
public:
	/// ������ ��������� ����� � ������ �������
	virtual BOOL OnKillActive();
};
