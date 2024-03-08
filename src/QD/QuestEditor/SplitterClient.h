#if !defined(AFX_SPLITTERCLIENT_H__4E08ED41_38B0_4EC1_A33A_78B2A2DF679E__INCLUDED_)
#define AFX_SPLITTERCLIENT_H__4E08ED41_38B0_4EC1_A33A_78B2A2DF679E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitterClient.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitterClient window
/**
	 ласс-окно нужен дл€ того, чтобы была возможность мен€ть в правой части окна 
	приложени€ редактор триггеров и редактор сцены местами. “ак же управл€ет размерами
	окон этих редакторов.
 */
class CSplitterClient : public CWnd
{
	DECLARE_DYNCREATE(CSplitterClient);

	/// ќкно, которое отображаетс€ в текущий момент в правой части
	CWnd* m_pResizedChild;

	/// –аст€гивает m_pResizedChild на все окно
	void ResizeChild();
// Construction
public:
	CSplitterClient();

	/// Setter m_pResizedChild
	void SetResizedChild(CWnd* ptrChild);
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterClient)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitterClient();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitterClient)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SPLITTERCLIENT_H__4E08ED41_38B0_4EC1_A33A_78B2A2DF679E__INCLUDED_)
