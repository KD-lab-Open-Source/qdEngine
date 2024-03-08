#if !defined(AFX_IELIKECOMBOBOX_H__5119E36B_142E_454B_9DF2_25457A8C6588__INCLUDED_)
#define AFX_IELIKECOMBOBOX_H__5119E36B_142E_454B_9DF2_25457A8C6588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IELikeComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IELikeComboBox window

class IELikeComboBox : public CComboBox
{
	bool m_bDelText;
// Construction
public:
	IELikeComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IELikeComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~IELikeComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(IELikeComboBox)
	afx_msg void OnEditupdate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IELIKECOMBOBOX_H__5119E36B_142E_454B_9DF2_25457A8C6588__INCLUDED_)
