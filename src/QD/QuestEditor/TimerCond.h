#if !defined(AFX_TIMERCOND_H__D7FA991C_F7FD_4D53_A904_D94423F59AC3__INCLUDED_)
#define AFX_TIMERCOND_H__D7FA991C_F7FD_4D53_A904_D94423F59AC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimerCond.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TimerCond dialog
#include "qd_condition.h"
#include "CondDlgBase.h"

/**
	Таймер
 */
class TimerCond : public CondDlgBase
{
	bool m_bInitDialog;
// Construction
public:
	TimerCond(DWORD id, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TimerCond)
	enum { IDD = IDD_DLG_COND_TIMER };
	float	m_fDuration;///< qdCondition::TIMER_PERIOD
	int		m_iRnd;///< qdCondition::TIMER_RND
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TimerCond)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnEnChangeEditDuration();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TimerCond)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditRnd();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDeltaposSpinDuration(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	bool IsCondDataComplete();
	/// Параметры условия в диалог
	void CondToWnd();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_TIMERCOND_H__D7FA991C_F7FD_4D53_A904_D94423F59AC3__INCLUDED_)
