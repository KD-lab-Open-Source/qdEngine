#if !defined(AFX_MOVESTOPDLG_H__0F3F3D48_0EF7_47A1_8AA4_5E179DCD48C3__INCLUDED_)
#define AFX_MOVESTOPDLG_H__0F3F3D48_0EF7_47A1_8AA4_5E179DCD48C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveStopDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MoveStopDlg dialog
#include "editlistctrl.h"
#include "qd_game_dispatcher.h"
#include "user_finder.h"

/**
	������ �������. ���������� ������ ������� �� ���� �����������(���� ��� ������)
 */
class MoveStopDlg : public CDialog
{
	qdGameDispatcher* m_ptrGameDisp;
	qdGameDispatcherBase* m_ptrGameDispBase;
	/// ���� �� ������� ���������
	bool m_bTreeChanged;
	/// �� ������������
	bool m_bAsk4Del;
	/// ������ ���������� ��������
	GameObjectStateList_t m_listStatesAffected;
// Construction
public:
	MoveStopDlg(CWnd* pParent = NULL);   // standard constructor

	/// ��������� ����������
	bool set_data(qdGameDispatcher* ptrGD, qdGameDispatcherBase* ptrGDB);

	bool has_change(){return m_bTreeChanged;}
// Dialog Data
	//{{AFX_DATA(MoveStopDlg)
	enum { IDD = IDD_MOVING_LIST };
	CEditListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MoveStopDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MoveStopDlg)
	afx_msg void OnButtonVarEdit2();
	afx_msg void OnButtonVarDel();
	afx_msg void OnButtonVarAdd2();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListVars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListVars(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	void OnEndEditList(NMHDR * pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP();
private:
	/// ��������� ����������� �������
	bool InsertColumns();
	/// ����������� ������ �������
	void AdjustColumnsWidth();
	/// ��������� ������ ������� �� m_ptrGameDispBase
	bool LoadAnimationSets();
	/// ��������� ���������� ������� � �������
	bool InsertAnimationSet(int iPos, qdAnimationSet* ptr);
	/// ��������� ������ �������������� �������
	bool EditAnimationSet(qdGameDispatcher* ptrGameDisp, qdAnimationSet* ptrAS);

	/// \brief ��������� ��� ����� �� �������� ���������
	/// ���� � ����� ��� ���������, �� ������ ��� � ������� ��������
	bool CheckScenes4Pers(qdAnimationSet* ptrASet);
	/// ���� � ����� ��� ���������, �� ������ ��� � ������� ��������
	bool CheckScene4Pers(qdGameScene* ptrScene, qdAnimationSet* ptrASet);
	
	/// ��������� ��������� � �����
	qdGameObjectMoving* AddPersonage(qdGameScene* ptrScene);

	/// ������� ������� �� ���������
	void RemoveGaitFromPers();
	/// �������� �� ������ ����� �� ������� �������.
	/**
		��������� ������������ �� ������� � ������ lpszName ���-����. 
		���� ������������, �� ���������� � ������������ ����� �� �������
	 */
	bool CanRemoveAS(LPCTSTR lpszName);

	/// \brief ���������,����� ��������� �� ������ ��������, �������� ����������
	/// ���������� ����� ��������������� ��������
	CString DetectLastState();

	/// �������� ��� ������� � ��������
	void ReplaceGait(LPCTSTR lpszOldName, LPCTSTR lpszNewName);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MOVESTOPDLG_H__0F3F3D48_0EF7_47A1_8AA4_5E179DCD48C3__INCLUDED_)
