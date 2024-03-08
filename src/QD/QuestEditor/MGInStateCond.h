#pragma once
#include "afxwin.h"


#include "conddlgbase.h"
// MGInStateCond dialog

class MGInStateCond : public CondDlgBase
{
//	DECLARE_DYNAMIC(MGInStateCond)

public:
	MGInStateCond(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~MGInStateCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_MG_IN_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
private:
	VOID LoadMiniGames();
	VOID LoadMiniGameStates(qdMiniGame* pg);
	VOID Data2Cond();
	bool IsCondDataComplete();
private:
	CCJFlatComboBox m_wndMGs;
	CCJFlatComboBox m_wndStates;
public:
	afx_msg void OnCbnSelchangeComboMgs();
	afx_msg void OnCbnSelchangeComboStates();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
