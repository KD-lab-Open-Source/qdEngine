#if !defined(AFX_APPSTNGSDLG_H__088DAED8_DFF4_438A_B73A_3D95DA99CF58__INCLUDED_)
#define AFX_APPSTNGSDLG_H__088DAED8_DFF4_438A_B73A_3D95DA99CF58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppStngsDlg.h : header file
//
#include <vector>
#include <boost/shared_ptr.hpp>
#include "IAppSettingsUser.h"

/////////////////////////////////////////////////////////////////////////////
// AppStngsDlg dialog

/**
	�����-������, ���������� � ���� ��� ��������� �������� ����������.
	��������� ������ ��������� � �.�. ������ �� ����������� �������� ���� ��� � 
	CPropertySheet
 */
class AppStngsDlg : public CDialog
{
// Construction
	typedef boost::shared_ptr<AppSettingsBasePage> PPPtr_t;

	typedef std::vector<PPPtr_t> PageList_t;
	/// ������ ���������, �������� ��������� ������
	PageList_t m_pages;
public:
	AppStngsDlg(CWnd* pParent = NULL);   // standard constructor
	/// ���������  ���������, ������� � � �����
	void AddUser(IAppSettingsUser* puser);
// Dialog Data
	//{{AFX_DATA(AppStngsDlg)
	enum { IDD = IDD_DLG_APP_SETTINGS };
	CTabCtrl	m_wndTab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AppStngsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AppStngsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	/// ������� �������� ����������
	afx_msg void OnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult);
	/// ������� �������� ������������
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// ��������� �������� �� ������ � ������
	bool InsertPages();
	/// ��������� ������� � ������
	bool InsertPage(PPPtr_t ptrPage, int iItem);
	/// ��������/���������� ������� ��������. 
	/**
		������������ ����������� �������.
		������� �������� ������������ ��� m_wndTab.GetCurSel()
	 */
	void SelectCurPage(BOOL bSelect);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_APPSTNGSDLG_H__088DAED8_DFF4_438A_B73A_3D95DA99CF58__INCLUDED_)
