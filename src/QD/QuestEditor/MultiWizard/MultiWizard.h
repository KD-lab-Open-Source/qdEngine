#if !defined(AFX_MULTIWIZARD_H__4063091A_02BC_4770_8EEF_585D77F084F3__INCLUDED_)
#define AFX_MULTIWIZARD_H__4063091A_02BC_4770_8EEF_585D77F084F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MultiWizard
#include <list>

class MWBasePage;
class MWWizard;

/**
	����� ��������� ��������� ������-�������. �������� �������� �������� �������
	�������� �� ��������� ���. ������� ��������� ������� ���, ����� ���� 
	� ��� �� ������ ����� ���� ��������� ��� ��������, ��� � � ��������� � �������.
	��������, ������ ���������� �������� ����� ���� ������ ��� �������� ��� � ��� 
	�������� ������ ������� ������ � �������� ���������� �����.

	����� ��������� ������ �����. � ���� ������ ������ ������ ���������� ���� ���.
	����� ��������� ����� ������������� ���� ����
 */
class MultiWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(MultiWizard);

	friend class MWBasePage;
	/// ��� ������ ��������
	typedef std::list<MWWizard*> WizardList_t;
// Construction
public:
	MultiWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	MultiWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Operations
public:

	bool AddWizard(MWWizard* pw);

	VOID RemoveWizard(MWWizard* pw);
	VOID SuppresWizardChange(BOOL bSupp = TRUE);

	//������������� ��������� � ���������� ����������
	bool ActivateWizard(MWWizard* pw);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MultiWizard)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	afx_msg VOID OnTreeItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnTreeItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnNextWizard();
	afx_msg VOID OnPrevWizard();
// Implementation
public:
	virtual ~MultiWizard();

	// Generated message map functions
protected:
	
	//{{AFX_MSG(MultiWizard)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	VOID AdjustPageLayout(MWBasePage* page);
	VOID OnEndWizard();

	CRect GetFirstPageRect();
	VOID MoveStandartButtons();

	VOID AddNavigationButtons();
	VOID FillTree();

	VOID CheckAccessToNWB();
	VOID CheckAccessToPWB();

	bool ShowWizard(MWWizard* pw);
	bool SuspendWizard(MWWizard* pw);

	void add_pages(MWWizard* pw);
private:
	/// ������ � ������� ��������
	CTreeCtrl m_wndTree;
	/// ������ ������� � ���������� �������
	CButton m_wndNextWizard;
	/// ������ ������� � ����������� �������
	CButton m_wndPrevWizard;
	/// ������ ����������� ��������
	WizardList_t m_wizards;
	/// ������� ������
	WizardList_t::iterator m_cur_wizard;
	/// ������������� ��������� ������ ��� ���������
	UINT m_uiCapID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MULTIWIZARD_H__4063091A_02BC_4770_8EEF_585D77F084F3__INCLUDED_)
