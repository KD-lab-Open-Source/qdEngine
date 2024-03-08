// WizardSheet.h : header file
//
// This class defines custom modal property sheet 
// WizardSheet.
 
#ifndef __WIZARDSHEET_H__
#define __WIZARDSHEET_H__

/////////////////////////////////////////////////////////////////////////////
// WizardSheet

class WizardSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(WizardSheet);

	bool m_bShowSteps;
	CPropertyPage** m_ptrPages;
	unsigned m_nCount;

// Construction
public:
	WizardSheet(CPropertyPage** ptrPPs, int count, int iActive,
		UINT uiCap = IDS_PROPSHT_CAPTION, CWnd* pWndParent = NULL);

// Attributes
public:

	void ShowSteps(){
		m_bShowSteps = true;
	}
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WizardSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~WizardSheet();

	virtual BOOL OnInitDialog();
	
// Generated message map functions
protected:
	//{{AFX_MSG(WizardSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __WIZARDSHEET_H__
