#if !defined(AFX_MWSELECTEXSTFILEPAGE_H__2051E105_C1E9_4869_B2AF_EEB509E5AFD4__INCLUDED_)
#define AFX_MWSELECTEXSTFILEPAGE_H__2051E105_C1E9_4869_B2AF_EEB509E5AFD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MWSelectExstFilePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MWSelectExstFilePage dialog

#include "wizardbasepage.h"

/**
	�������� ��� ������ ���������� ��� ���������� ��������
	���� �������� ������������, �� �� ���� �������� ����������� ����� ������ ��������
	��� ������� � ������.
	���� �� ������������, �����, ���� data->convert_way_ == CW_FOLDERS, ��������
	����� ���������, ����� ���� �������
 */
class MWSelectExstFilePage : public WizardBasePage
{
	DECLARE_DYNCREATE(MWSelectExstFilePage)

// Construction
public:
	MWSelectExstFilePage();
	~MWSelectExstFilePage();

// Dialog Data
	//{{AFX_DATA(MWSelectExstFilePage)
	enum { IDD = IDD_PROPPAGE_AA_SEL_EXIST };
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MWSelectExstFilePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MWSelectExstFilePage)
	afx_msg void OnBntSelFiles();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// ������ ����� ������ � ������� m_list
	/**
		� ����������� �� ����, ����� �������� ��� �����(����� ������, ���������, ��������)
		��������� ��������� ���������
	 */
	VOID UpdateFileControlsCaption();
	/// �������� ����� � �����. ����� ����� ��������� ��������� ��� ������
	bool SelectFiles();
	/// �������� �������
	bool SelectCatalog();
	/// �������� ����� ���������. ��������, ��� � �����, ����� ��������� ��������� ���
	bool SelectCatalogs();
	/// ����� �� ������ ������� ������� � m_list
	VOID Files2List();
	/// ����� �� ��������� ������ �� �������
	bool CanClickNext();
	/// ������ ������ Next ��������� ��� ��� � ����������� �� CanClickNext()
	void CheckNextBTNAccess();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MWSELECTEXSTFILEPAGE_H__2051E105_C1E9_4869_B2AF_EEB509E5AFD4__INCLUDED_)
