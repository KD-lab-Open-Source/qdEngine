#if !defined(AFX_ANIMOBJBOUND_H__35E9B853_0244_4A34_8181_CA4F71C34E13__INCLUDED_)
#define AFX_ANIMOBJBOUND_H__35E9B853_0244_4A34_8181_CA4F71C34E13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimObjBound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AnimObjBound dialog

class qdGameObjectAnimated;

#include "basepage.h"
/*!
	�������� ������� �������������� �������, �� ������� ��������������
	�����.
*/
class AnimObjBound : public CBasePage
{
	DECLARE_DYNCREATE(AnimObjBound);
	//! ����, ��������� ����������������� �� ������
	bool m_bInitDialog;
	
	//! ��������� ����� ������� �� �
	int		m_iEditXBak;
	//! ��������� ����� ������� �� Y
	int		m_iEditYBak;
	//! ��������� ����� ������� �� Z
	int		m_iEditZBak;
	//! ����� �� ������ �����
	bool	m_bHasBound;
	
	//! �������� ������ �� ���������� ����� � ��������� �����
	VOID real2bak();
	//! �������� ������ �� ��������� ����� � ����������� ����
	VOID bak2real();

	//! �� ������� ������ � ���������� ����
	VOID Obj2Wnd();
	//! ������ �� ���������� ����� � ������
	VOID Wnd2Obj();

	//! ������, ������� �������� ��������
	qdGameObjectAnimated	*m_ptrObj;
// Construction
public:
	AnimObjBound();
	~AnimObjBound();

	//! ��������� ������, ������� �������� ��������
	bool set_data(qdGameObjectAnimated* ptrAnim);
// Dialog Data
	//{{AFX_DATA(AnimObjBound)
	enum { IDD = IDD_PROPPAGE_ANIM_OBJ_BOUND };
	//! ���������� ���� ������� �� �
	int		m_iEditX;
	//! ���������� ���� ������� �� Y
	int		m_iEditY;
	//! ���������� ���� ������� �� Z
	int		m_iEditZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(AnimObjBound)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AnimObjBound)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditX();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDeltaposSpinX1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinY1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinZ1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ANIMOBJBOUND_H__35E9B853_0244_4A34_8181_CA4F71C34E13__INCLUDED_)
