#if !defined(AFX_ADDANIMFIRSTWPG_H__F69DFE56_8F03_4074_A217_553AF2A1FE8B__INCLUDED_)
#define AFX_ADDANIMFIRSTWPG_H__F69DFE56_8F03_4074_A217_553AF2A1FE8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddAnimFirstWPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AddAnimFirstWPg dialog


#include "basepage.h"

//add animation action
const int AAA_CONV_CATALOG											= 0;
const int AAA_EXISTING												= 1;
const int AAA_CONV_FOR_SCENES										= 2;
const int AAA_CONV_FOR_SCENES_AND_CRT								= 3;
const int AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT		= 4;

const int SAVE_TO_CURRENT			= 0;
const int SAVE_TO_GLOBAL			= 1;

class qdGameDispatcher;
class qdGameDispatcherBase;

/**
	Не используется
 */
class AddAnimFirstWPg : public CBasePage
{
	DECLARE_DYNCREATE(AddAnimFirstWPg);

	int m_iPagePlace;
	CStringVect_t m_vecStrs;

	qdGameDispatcher		*m_ptrGameDisp;
	qdGameDispatcherBase	*m_ptrDispBase;
// Construction
public:
	AddAnimFirstWPg();
	~AddAnimFirstWPg();

	bool set_place(int nPlace){
		m_iPagePlace = nPlace;
		return true;
	}

	bool set_data(qdGameDispatcher* ptrGameDisp, qdGameDispatcherBase* ptrDispBase);
// Dialog Data
	//{{AFX_DATA(AddAnimFirstWPg)
	enum { IDD = IDD_PROPPAGE_AA_HOW };
	BOOL	m_bCrtObj;
	int		m_iAction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(AddAnimFirstWPg)
	public:
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
//	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AddAnimFirstWPg)
	afx_msg void OnButton1();
	afx_msg void OnRadioExist();
	afx_msg void OnRadioConvDisp();
	afx_msg void OnRadioConvertForScene();
	afx_msg void OnBtnRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	
	void SelectBrowseBtnCap();
	void UpdatePlaceState();
	bool SelectDiskObj();

	bool SelectFiles();
	bool SelectCatalog();
	bool SelectCatalogs();
	bool UpdateWizardBtnsState();
	bool UpdateRunBtn();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ADDANIMFIRSTWPG_H__F69DFE56_8F03_4074_A217_553AF2A1FE8B__INCLUDED_)
