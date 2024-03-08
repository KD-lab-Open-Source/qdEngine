#if !defined(AFX_MOUSEDLGCLICKCOND_H__6A0F13EE_1E6E_4EF6_B652_A19201B7AB91__INCLUDED_)
#define AFX_MOUSEDLGCLICKCOND_H__6A0F13EE_1E6E_4EF6_B652_A19201B7AB91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NoDataCondDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NoDataCondDlg dialog
#include "conddlgbase.h"
/**
	Диалог для условий, которым не надо задавать параметры
 */
class NoDataCondDlg : public CondDlgBase
{
// Construction
public:
	NoDataCondDlg(DWORD id, CWnd* pParent = NULL);   // standard constructor

	/// Могу ли добавить это условие к объекту. Если такое уже есть, то нельзя
	bool CanAdd(const class qdConditionalObject* pobj);
// Dialog Data
	//{{AFX_DATA(NoDataCondDlg)
	enum { IDD = IDD_DLG_COND_MOUSE_DLG_CLICK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NoDataCondDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NoDataCondDlg)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	bool IsCondDataComplete();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MOUSEDLGCLICKCOND_H__6A0F13EE_1E6E_4EF6_B652_A19201B7AB91__INCLUDED_)
