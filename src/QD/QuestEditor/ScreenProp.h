#if !defined(AFX_SCREENPROP_H__37B251CC_2260_4530_899B_63BE62A071F8__INCLUDED_)
#define AFX_SCREENPROP_H__37B251CC_2260_4530_899B_63BE62A071F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScreenProp dialog
class qdGameDispatcher;
class qdGameScene;

#include "basepage.h"
#include "afxwin.h"
/**
	�������� �����
 */
class ScreenProp : public CBasePage
{
// Construction
	/// ��������� �������
	qdGameDispatcher*	m_ptrGameDisp;
	/// �����
	qdGameScene*		m_ptrScene;
	/// ����� �� �������������� �����
	/**
		���� ����� �������, ������ �����
	 */
	bool				m_bRedraw;
	/// ������������� ������ ���������
	int m_iSaveId;
public:
	ScreenProp(CWnd* pParent = NULL);   // standard constructor

	/// ��������� �������������� ����� � ���������
	bool set_data(qdGameDispatcher* ptrGD, qdGameScene* ptrGS);
// Dialog Data
	//{{AFX_DATA(ScreenProp)
	enum { IDD = IDD_PROPPAGE_SCENE_GENERAL };
	/// ������ ������� �������
	int		m_iCellX;
	/// ������ ������� �������
	int		m_iCellY;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScreenProp)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScreenProp)
	afx_msg void OnChangeScreenSize();
	afx_msg void OnDeltaposSpinSaveId(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG*);
	afx_msg void OnBnClickedCheckCycle();
	afx_msg void OnCbnSelchangeComboMinigames();
	afx_msg void OnCbnSelchangeComboInterfaceScreens();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
protected:
	/// ���������/������� ��������� ���� ��� �����
	void set_flag(bool bSetFlag, int iValue);
	/// ���������� ��� ��������, ��������� �� ������ ��������
	CString const getMinigameName() const;
	/// ��������� ������ �������
	void loadMinigames();
	/// ���������� ��� ������������� ������, ���������� �� ������ �������
	CString const getInterfaceScreenName() const;
	/// ��������� ������ ������� ����������
	void loadInterfaceScreens();
private:
	/// ��������� ��������� ������� �������
	bool apply_screen_params();
	/// ������ �� ������. �� �����
	bool apply_va_params();
	/// ��������� ��� ����������� ���������
	bool apply_params();
	/// �������������� ����� � �������� �����
	void redraw();
	//! ��������� �� �
	BOOL m_bCycleX;
	//! ��������� �� Y
	BOOL m_bCycleY;
	//! ���� qdGameScene::RESET_TRIGGERS_ON_LOAD
	BOOL resetTriggersOnLoading_;
	BOOL enablePersonageSwitch_;
	BOOL disableMainMenuExit_;
	//! ������ �������
	CCJFlatComboBox miniGames_;
	//! ������ ������� ����������
	CCJFlatComboBox interfaceScreens_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SCREENPROP_H__37B251CC_2260_4530_899B_63BE62A071F8__INCLUDED_)
