#if !defined(AFX_MOVINGDLG_H__F2D604FE_2B59_4B1A_838E_7F33AF01F94F__INCLUDED_)
#define AFX_MOVINGDLG_H__F2D604FE_2B59_4B1A_838E_7F33AF01F94F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MovingDlg.h : header file
//

#include "editlistctrl.h"
#include <set>

#include "DispsListsMerger.h"
#include "qd_animation_set.h"
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// MovingDlg dialog
class qdGameDispatcher;
class qdGameDispatcherBase;
class qdAnimationSet;
class qdAnimationInfo;
class qdAnimationSetPreview;

class GDI_grDispatcher;

class MovingDlg : public CDialog
{
	/// ������������� �������
	qdAnimationSet *m_ptrObj;
	/// ��������� ��������, �������� ����������� �������
	qdGameDispatcherBase* m_ptrGameDispBase;
	/// ��������� �������
	qdGameDispatcher* m_ptrGameDisp;
	/// ���������� ������ �������� �� ���������
	std::auto_ptr<CStringSet_t> m_ptrAnimList;
	/// �������������� �� ��������
	bool *m_pbShowAnimation;
	/// ��������� ��� ����������� �������
	std::auto_ptr<GDI_grDispatcher> m_ptrXGR;
	/// ���������� ��������
	std::auto_ptr<qdAnimationSetPreview> m_ptrASP;

	CBrush m_brBackColor;
	CBrush m_brGridColor;
	/// ��������� ��������
	qdAnimationInfo* m_ptrCurrAnimInfo;
	/// ���� �� �������������
	bool m_bInitDialog;

	/// ��������� �����
	qdAnimationSet m_BackAnimSet;

	CSliderCtrl m_wndAnimPhase;
// Construction
public:
	MovingDlg(CWnd* pParent = NULL);   // standard constructor

	/// ��������� ��������� ������� � �������
	bool set_data(qdGameDispatcher* ptrDips, qdAnimationSet* ptrObj);
// Dialog Data
	//{{AFX_DATA(MovingDlg)
	enum { IDD = IDD_DIALOG_MOVING };
	CEditListCtrl	m_list;
	/// ������ ��������, ��� ������� �������� ��������
	CCJFlatComboBox	animations_;
	/// ����� ��������� � ����� ��������
	int		animation_;
	/// ���������� �����������
	int		m_dirs;
	BOOL	m_bRunAnim;
	int		m_iCamPitch;
	int		m_iCamFocus;
	/// ������ �����
	int		m_iGridSize;
	/// ������ ���������
	int		m_iPersHeight;
	/// ������������� �����������
	BOOL	m_bFlipVert;
	/// ����������� ��������
	BOOL	m_bLoop;
	/// �������� �������������
	BOOL	m_bFlipHorz;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MovingDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:

	BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
	//{{AFX_MSG(MovingDlg)
	afx_msg void OnDirSet();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckShow();
	afx_msg void OnChangeEditPersHeight();
	afx_msg void OnChangeEditGridSize();
	afx_msg void OnChangeEditCamPitch();
	afx_msg void OnChangeEditCamFocus();
	afx_msg void OnBtnBackColor();
	afx_msg void OnBtnColor();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCheckFlipVert();
	afx_msg void OnCheckLoop();
	afx_msg void OnCheckFlipHorz();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnEndEditList(NMHDR * pNMHDR, LRESULT* pResult);
	afx_msg void OnSetupCtrlList(NMHDR * pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnToBeg();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnToEnd();
	afx_msg void OnDeltaposSpinStartAngle(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP();

	bool InsertColumns();

	/// ������� ������ ������ ���� �������� ��� ����������
	bool MakeAnimationList();
	/// ��������� ������ �������� �������
	bool LoadMovings();
	/// ����������� ������ ��������, ��� ����� �� ��������� ����� 
	void AdjustColumnsWidth();
	/// ��������� ���� �������/��������� � ������ 
	bool InsertAnimInfo(int iDir, qdAnimationInfo* walk_info, qdAnimationInfo* stand_info, qdAnimationInfo* start_info, qdAnimationInfo* stop_info);
	/// ��������� ���� ��������� ��������
	bool ShowAnimation();

	/// ������� � ����������� ����������� ���������
	bool CreateXGraph();
	/// ������� � ����������� qdAnimationPreview
	bool CreateAnimPreview();
	/// ��������� ������ � �������� �������
	void AnimPreview2Wnd();
	/// ������������ ����������� ��������� ����-���������
	void AdjustSpints();

	/// \brief ���������� ������������ ���-������: IDC_CHECK_FLIP_VERT,
	/// IDC_CHECK_FLIP_HORZ, IDC_CHECK_LOOP
	void EnableCheckBoxes(BOOL bEnable);
	/// ���������� ��������� ���-������ �� ���� �� ��������
	void SetChecksState(qdAnimationInfo* ptrAI);

	/// ���������� ���� ��������
	float GetPhase() const;
	/// ������������� ���������� ��������� ���� ��������
	void SetPhaseRange();
	/// ��������� ���� ��� �����������
	void SetASPPhase();
	/// ��������� ������������ ������ ��������� �� ��������
	void AccessToAnimCtrls();
	/// ��������� ������� ��� ������ ��������� �� ��������
	void LoadBitmaps();
private:
	/// ��������� ���� ��� �������
	float m_fStartAngle;
	/// ���������
	float m_fAcceleration;
	/// ������������ ��������
	float m_fMaxSpeed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MOVINGDLG_H__F2D604FE_2B59_4B1A_838E_7F33AF01F94F__INCLUDED_)
