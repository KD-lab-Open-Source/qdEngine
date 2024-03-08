#if !defined(AFX_ObjPosPropPage_H__D323D681_4EC2_4928_B295_A5C10AD4D661__INCLUDED_)
#define AFX_ObjPosPropPage_H__D323D681_4EC2_4928_B295_A5C10AD4D661__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjPosPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ObjPosPropPage dialog

class qdGameDispatcher;
class qdGameScene;
class qdGameCamera;
class qdGameObjectAnimated;

#include "basepage.h"
/**
	�������� ������� ����������� ������������� ������� ������
 */
class ObjPosPropPage : public CBasePage
{
	DECLARE_DYNCREATE(ObjPosPropPage)

	//! true �� ������� ������ ����
	bool m_bInitDialog;
	
	
	qdGameDispatcher		*m_ptrGameDisp;
	qdGameObject			*m_ptrObj;
	qdGameScene				*m_ptrScene;
	//! ������� �������� ������� �� �����
	int		m_iNetProjD;
	
	//! ����� �� �������������� �����, ���� ����� �������, ������ �����
	bool	m_bRedraw;

	//! ��������� ���������
	void apply_params();
	//! ��������� ��������, ������������ ��������� �������
	void apply_pos_params();
	// Construction

	// ��������� �����
	//! ����� �������� � ����������
	int		m_iScrXBak;
	//! ����� �������� Y ����������
	int		m_iScrYBak;
	//! ����� � ����������
	int 	m_iXBak;
	//! ����� Y ����������
	int 	m_iYBak;
	//! ����� Z ����������
	int 	m_iZBak;
	//! ����� �������
	int 	m_iDepthBak;
	

	//! \brief ������� ��������� �������� �������� ���������
	//! � ��������� ������
	void real2bak();
	//! ��������������� ��������� �� ��������� �����
	void bak2real();
	
	//! \brief ��������� �������� ����������� � ���������� �������� ����.
	//! ��������� ������������� �������� ��������� � ����� ������� ����.
	void screen2world();
	//! ��������� �������� ����������� � ���������� �������� ����
	Vect3f screen2world(int x, int y, int z);
	
	//! ������� � screen2world
	void world2screen();
	void world2screen(Vect3f pos, int& x, int& y, int &z);
	
	//! ������ ����������� �������� ���������� �� ������� �������
	void DisablePositionCtrls();
	
	//! ������������� ��������� � ������� ������
	void AdjustSpins();
	
	//! �� ��������� ������ ���������� ������ �������� ����
	Vect3f c2lt(Vect3f pos);
	//! �� ��������� ������ �������� ���� ���������� ������
	Vect3f lt2c(Vect3f pos);
	
	//! ����������� ������ �� ������� �� �������
	void Obj2Wnd();

	//! �� ��������� ������� ��������� �� ����� ��������� ������� �������
	void changeNetProjScrD(int delta);
	// Construction
public:
	ObjPosPropPage();
	~ObjPosPropPage();

	bool set_data(qdGameDispatcher* ptrGD, qdGameObject* ptrObj);
	
// Dialog Data
	//{{AFX_DATA(ObjPosPropPage)
	enum { IDD = IDD_PROPPAGE_OBJECT_POS };
	int		m_iScrX;
	int		m_iScrY;
	int		m_iX;
	int		m_iY;
	int		m_iZ;
	int		m_iDepth;
	BOOL	m_bUseLeftTop;
	BOOL	m_bFixedScreenCoords;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(ObjPosPropPage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(ObjPosPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUseLeftTop();
	afx_msg void OnFixedScreenCoords();
	//}}AFX_MSG
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeWorldPos();
	afx_msg void OnEnChangeScrPos();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
	
	//! ������������ ���� ��������� �����
	bool redraw();
	//! ������ ENTER � ���������, �������������� �������� �������
	void OnEnterInScrPos();
	//! ������ ENTER � ���������, �������������� ������� �������
	void OnEnterInWorldPos();
	//! ������ ENTER � ��������, �������������� ������� �������� �� �����
	void OnEnterInNetProjScrD();

	afx_msg void OnEnKillfocusInScrPos();
	afx_msg void OnEnKillfocusInWorldPos();
	afx_msg void OnDeltaposSpinNetProjScrD(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ObjPosPropPage_H__D323D681_4EC2_4928_B295_A5C10AD4D661__INCLUDED_)
