#if !defined(AFX_ZONEPROPPAGE_H__5EB49B05_C9CE_4F9A_A905_B753346C2395__INCLUDED_)
#define AFX_ZONEPROPPAGE_H__5EB49B05_C9CE_4F9A_A905_B753346C2395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZonePropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ZonePropPage dialog

#include "basepage.h"
#include "custom controls\cjcolorpicker.h"

class qdGridZone;
class qdGameDispatcher;
class qdGameScene;

/**
	�������� �������� ����
 */
class ZonePropPage : public CBasePage
{
	DECLARE_DYNCREATE(ZonePropPage);

	/// ������������� ����
	qdGridZone* m_ptrZone;
	/// ��������� �������
	qdGameDispatcher *m_ptrGameDisp;
	/// ���� �� ������������� �������
	bool m_bInitDialog;

	/// ��������� ������

	int	 m_iHBak;
	int	 m_iDefStateBak;
	int  m_bNeedCursorBak;

	/// ������ ��������� �����
	void real2bak();
	/// ��������������� �� ��������� �����
	void bak2real();

	/// ��������� ���� � �������� �������
	void zone2wnd();
	/// ������ �� ���� � ����
	void wnd2zone();
	/// �������������� ����� � ��������� �����
	void redraw();

// Construction
public:
	ZonePropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ZonePropPage)
	enum { IDD = IDD_PROPPAGE_NET_ZONE };
	int		m_iH;
	int		m_iDefState;
	//}}AFX_DATA

	/// ��������� ��������� ���� � ������������� ����
	bool set_data(qdGameDispatcher* ptrGD, qdGridZone* ptrZone);
	/// ��������� ��������� ��� � �����
	static void ReapplyZones(qdGameScene* ptrScene);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ZonePropPage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckNeedCursor();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ZonePropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditHeight();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRadioPass();
	afx_msg void OnRadioPass2();
	afx_msg void OnEnChangeEditTransparency();
	LRESULT OnColorPicSelChange(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//! ����� �� ���� ������
	BOOL m_bNeedCursor;
	/// ������������
	int transparency_;
	int transparencyBack_;
	/// ���� ����
	CCJColorPicker shadowColor_;
	COLORREF shadowColorBack_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ZONEPROPPAGE_H__5EB49B05_C9CE_4F9A_A905_B753346C2395__INCLUDED_)
