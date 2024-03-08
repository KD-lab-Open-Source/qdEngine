#if !defined(AFX_TESETTINGS_H__8A9359F9_CC88_4C6A_AD39_F436CA080470__INCLUDED_)
#define AFX_TESETTINGS_H__8A9359F9_CC88_4C6A_AD39_F436CA080470__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TESettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TESettings dialog
#include "AppSettingsBasePage.h"

/**
	Класс странички настроек для редактора триггеров. Страничка находится в 
	диалоге настроек приложения
 */
class TESettings : public AppSettingsBasePage
{
//	DECLARE_DYNCREATE(TESettings);
	// Construction
public:
	TESettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TESettings)
	enum { IDD = IDD_PO_PROPPAGE_TE_SETTINGS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TESettings)
	protected:
		virtual BOOL OnApply();
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TESettings)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	/// Ширина ячейки
	int		m_iX;
	/// Высота ячейки
	int		m_iY;
	/// Делать приязку к узлам сетки
	BOOL	m_iBindToNetNode;
	/// Показывать всплывающее меню при создании линка
	BOOL	m_bShowMenuOnLinkCreate;
public:
	afx_msg void OnBnClickedButtonCondListEditor();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_TESETTINGS_H__8A9359F9_CC88_4C6A_AD39_F436CA080470__INCLUDED_)
