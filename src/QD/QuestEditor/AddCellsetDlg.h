#if !defined(AFX_ADDCELLSETDLG_H__23CE2BF0_F57A_48F9_ACE5_9D508D169886__INCLUDED_)
#define AFX_ADDCELLSETDLG_H__23CE2BF0_F57A_48F9_ACE5_9D508D169886__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddCellsetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AddCellsetDlg dialog
#include "selectpicture\\selectpicture.h"

/// ��������� ����� ����� � ���������
class AddCellsetDlg : public CDialog
{
	/// ���������, ���� ������� ��������
	class qdInventory* m_ptrInventory;
	/// ��������� �������
	class qdGameDispatcher* m_ptrGameDisp;
	bool m_bInitDialog;
// Construction
public:
	AddCellsetDlg(CWnd* pParent = NULL);   // standard constructor

	/// ������������� ��������� ���� � ���������
	VOID set_data(qdGameDispatcher* ptrDisp, qdInventory* ptrInv);
// Dialog Data
	//{{AFX_DATA(AddCellsetDlg)
	enum { IDD = IDD_DLG_ADD_CELL_SET };
	SelectPicture	m_wndPicSelector;
	int		m_iSX;
	int		m_iSY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddCellsetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AddCellsetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBntAddCellset();
	afx_msg void OnBtnAddType();
	afx_msg void OnChangeEditSx();
	afx_msg void OnBnClickedBtnRemoveType();
	//}}AFX_MSG
	afx_msg VOID OnImageChange(NMHDR * pNotifyStruct, LRESULT * result);
	DECLARE_MESSAGE_MAP();
	/// ��������� ����������� ��� ���� ����� �����
	VOID LoadCellTypes();
	/// ��������� ��������� ����� �����, ���������� ����
	BOOL AddCellset();
	/// ������� ��������� ��� �����
	bool remove_cell_type(int type);
	/// �������� ��� �����
	bool replace_cell_type_in_cellsets(int type_remove, int type_insert);
	/// ����������� �� ���� �������� � �������� ��� ���������
	bool replace_cell_type_in_objects(int type, int ins_type);
	/// ������� ��� ���������, ����������� ������������
	bool remove_all_invs();
	void setType(int type);
private:
	/// ������ ��� ����������� �������� ���� � ���� �������
	CString m_strType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_ADDCELLSETDLG_H__23CE2BF0_F57A_48F9_ACE5_9D508D169886__INCLUDED_)
