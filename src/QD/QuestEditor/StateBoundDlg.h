#if !defined(AFX_STATEBOUNDDLG_H__327B966C_5602_4C0B_80D6_64533914904A__INCLUDED_)
#define AFX_STATEBOUNDDLG_H__327B966C_5602_4C0B_80D6_64533914904A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StateBoundDlg.h : header file
//

#include <memory>

/////////////////////////////////////////////////////////////////////////////
// StateBoundDlg dialog
class qdGameObjectState;

/**
	����������� ������� ���������
 */
class StateBoundDlg : public CDialog
{
	/// ���� �� ������������� �������
	bool m_bInitDialog;
	/// ����� ��������� ��� ��������������
	std::auto_ptr<qdGameObjectState> m_ptrStateCopy;
	qdGameObjectState* m_pSrcState;///< �������� ���������
// Construction
public:
	StateBoundDlg(CWnd* pParent = NULL);   // standard constructor

	/// ��������� ��������� ��������� ���� ����������� ��������������
	enum eEndEditMode {EEM_DISCARD,///< ��������� �������������
		EEM_SAVE///< ��������� �����������
	};
	/// ���������� ��������������
	void EndEdit(eEndEditMode mode);
	/// ��������� �������������� ��������� 
	void set_data(qdGameObjectState* pstate);
	/// ���������� ������������� ���������
	qdGameObjectState* getEditedState() const;
	/// ��������� ������ � ��������� ����
	void UpdateStateData();
	// Dialog Data
	//{{AFX_DATA(StateBoundDlg)
	enum { IDD = IDD_DLG_STATE_BOUND };
	int		m_iX;///< ������ �� �
	int		m_iY;///< ����� �� �
	int		m_iZ;///< ������ �� z
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StateBoundDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(StateBoundDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	/// ������ �� ��������� � ���������
	void wnd2state();
	/// ������ �� ��������� � ��������
	void state2wnd();
};

/// ��������� ������ �������������� �������� ���������
bool EditStateBound(qdGameObjectState* pstate);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STATEBOUNDDLG_H__327B966C_5602_4C0B_80D6_64533914904A__INCLUDED_)
