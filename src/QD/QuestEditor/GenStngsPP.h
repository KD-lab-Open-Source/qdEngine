// GenStngsPP.h : header file
//

#ifndef __GENSTNGSPP_H__
#define __GENSTNGSPP_H__

/////////////////////////////////////////////////////////////////////////////
// GenSettingsPP dialog

#include "AppSettingsBasePage.h"

/**
	��������� ����� �������. ����� ���������� ��������� 
	���������, � ��������� ��� ����� ���� � ������ �������
 */
class GenSettingsPP : 
	public AppSettingsBasePage
{

// Construction
public:
	GenSettingsPP();
	~GenSettingsPP();

// Dialog Data
	//{{AFX_DATA(GenSettingsPP)
	enum { IDD = IDD_PO_PROPPAGE_GEN_SETTINGS };
	/// ��������/��������� ��������
	BOOL	m_bAutoSave;
	///  ����������, ����� ������� ��������� ��������
	int		m_iASPeriod;
	/// ����������� �� ��������� ����� ��� ����� ���� ����� � ������
	BOOL	m_bCollapseScenes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(GenSettingsPP)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(GenSettingsPP)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	/// ������������� ���� �����������
	afx_msg void OnBnClickedCheckCollapseScenes();
	/// ������������� ���� �����������
	afx_msg void OnBnClickedCheckUseAutosave();
	/// ������������� ���� �����������
	afx_msg void OnEnChangeEditAsPeriod();
};



#endif // __GENSTNGSPP_H__
