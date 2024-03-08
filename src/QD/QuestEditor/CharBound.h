#if !defined(AFX_HARBOUND_H__96813AD9_70A6_4A0F_80ED_271B811C9DBB__INCLUDED_)
#define AFX_HARBOUND_H__96813AD9_70A6_4A0F_80ED_271B811C9DBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// charBound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CharBound dialog
#include "basepage.h"

class qdGameObjectMoving;
class qdGameDispatcher;
/*!
  �������� �������, �� ������� ������������� ������� ������ ���������
*/

class CharBound : public CBasePage
{
	DECLARE_DYNCREATE(CharBound);
	//! True �� ����� ������ OnInitDialog
	bool m_bInitDialog;
	//! ��������� ���� 
	qdGameDispatcher	*m_ptrGameDisp;
	//! ��������, ����� �������� ��������
	qdGameObjectMoving	*m_ptrObj;

	//! ����� ��������� ������� �� �
	int m_iXBoundBak;
	//! ����� ��������� ������� �� �
	int m_iYBoundBak;
	//! ����� ��������� ������� �� Z
	int m_iZBoundBak;
	//! ����� ��������� ��������
	float m_fScaleBak;
	//! ����, ��������� � ���, ��� ��������� ����� �������
	bool m_bStateHasBound;
	
	//! ������ �� ������� ���������� � ���������� �������
	void Obj2Dlg();
	//! ������ �� ���������� ������� ���������� � �� *Bak ������
	void real2bak();
	//! ������ �� Bak ������ ���������� � ���������� �������
	void bak2real();
	//! �� ���������� ������� �������� ������ � ������
	void apply_params();
	//! �������������� ���� ��������� �����
	void redraw();
	//! ������������� ������� ��������� ��� SpinControl'��
	void AdjustSpinRanges();

	//! ���������� �������� �� ������ ����������
	bool IsGlobalObj();

	//! ��� ���� ����� ��������� ��������� ������������� ��������� ������
	void SetBoundToCopy();
	//! ������ ������������ ��������, ������������� ������� ������
	void DisableBndCtrls();
	//!\brief ��������� ��������� ������� ��� ���� ���������� �������, �������
	//! �� ����� ����������� �������
	void SaveStatesScrLayout(Vect3f const& old_bnd, Vect3f const& bnd);
// Construction
public:
	CharBound();
	~CharBound();

// Dialog Data
	//{{AFX_DATA(CharBound)
	enum { IDD = IDD_PROPPAGE_CHARACTER_BOUND };
	/*!
	  ���������� �������
	*/
	int		m_iXBound;
	int		m_iYBound;
	int		m_iZBound;
	float	m_fScale;
	//}}AFX_DATA

	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj);

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CharBound)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CharBound)
	afx_msg void OnChangeBound();
	afx_msg void OnChangeEditScale();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAutoBnd();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_HARBOUND_H__96813AD9_70A6_4A0F_80ED_271B811C9DBB__INCLUDED_)
