#if !defined(AFX_FLOATBAR_H__D8C8BBE2_87AB_4392_912F_5EB80EB877E9__INCLUDED_)
#define AFX_FLOATBAR_H__D8C8BBE2_87AB_4392_912F_5EB80EB877E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FloatBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFloatBar dialog

class CFloatBar : public CDialogBar
{
// Construction
public:
	CFloatBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFloatBar)
	enum { IDD = IDD_FLOAT_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFloatBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFloatBar)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOATBAR_H__D8C8BBE2_87AB_4392_912F_5EB80EB877E9__INCLUDED_)
