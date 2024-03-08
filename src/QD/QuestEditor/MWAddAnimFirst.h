#if !defined(AFX_MWADDANIMFIRST_H__7B7C6E52_20F7_4EF3_87B2_706CC25C653C__INCLUDED_)
#define AFX_MWADDANIMFIRST_H__7B7C6E52_20F7_4EF3_87B2_706CC25C653C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MWAddAnimFirst.h : header file
//
#include <memory>

/////////////////////////////////////////////////////////////////////////////
// MWAddAnimFirst dialog
#include "wizardbasepage.h"

/**
	������ �������� � �������
	�� ��� ���������� ��� ����� �������� ��������
 */
class MWAddAnimFirst : public WizardBasePage
{
	DECLARE_DYNCREATE(MWAddAnimFirst)

// Construction
public:
	MWAddAnimFirst();
	~MWAddAnimFirst();

// Dialog Data
	//{{AFX_DATA(MWAddAnimFirst)
	enum { IDD = IDD_PROPPAGE_ADD_ANIM_1 };
	int		m_iway;
	//}}AFX_DATA

	VOID set_page_for_sel_exist(WizardBasePage* ptr){
		m_ptrPageForSelExisting = ptr;
	}
	
	VOID set_page_for_convert(WizardBasePage* ptr){
		m_ptrPageForConvert = ptr;
	}

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MWAddAnimFirst)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MWAddAnimFirst)
	afx_msg void OnRadioExist();
	afx_msg void OnRadioConv();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	/// �������� ����� ���������, ���� ������� ���� ������������ ��������
	WizardBasePage* m_ptrPageForSelExisting;
	/// �������� ����� ���������, ���� ������� �����������
	WizardBasePage* m_ptrPageForConvert;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MWADDANIMFIRST_H__7B7C6E52_20F7_4EF3_87B2_706CC25C653C__INCLUDED_)
