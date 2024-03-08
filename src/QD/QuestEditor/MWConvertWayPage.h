#if !defined(AFX_MWCONVERTWAYPAGE_H__1D413E84_F6F1_4D4A_AFEC_06F32BC49B58__INCLUDED_)
#define AFX_MWCONVERTWAYPAGE_H__1D413E84_F6F1_4D4A_AFEC_06F32BC49B58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MWConvertWayPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MWConvertWayPage dialog
#include "WizardBasePage.h"
/**
	На этой странице определяем как будем конвертировать
 */
class MWConvertWayPage : public WizardBasePage
{
	DECLARE_DYNCREATE(MWConvertWayPage)

// Construction
public:
	MWConvertWayPage();
	~MWConvertWayPage();

	void set_next_page(WizardBasePage* pn){
		m_pNextPage = pn;
	}
// Dialog Data
	//{{AFX_DATA(MWConvertWayPage)
	enum { IDD = IDD_PROPPAGE_CONVERT_WAY };
	/// Хранит указанный способ
	int		m_iConvertWay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MWConvertWayPage)
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
	//{{AFX_MSG(MWConvertWayPage)
	afx_msg void OnRadioConvert1();
	afx_msg void OnRadioConvert2();
	afx_msg void OnRadioConvert3();
	afx_msg void OnRadioConvert4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// Следующая за этой страница в визарде
	WizardBasePage *m_pNextPage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MWCONVERTWAYPAGE_H__1D413E84_F6F1_4D4A_AFEC_06F32BC49B58__INCLUDED_)
