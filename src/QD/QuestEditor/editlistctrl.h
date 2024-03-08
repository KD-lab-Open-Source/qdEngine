#if !defined(AFX_EDITLISTCTRL_H__AB089162_8B4E_44BE_9BEA_B7C299C8694C__INCLUDED_)
#define AFX_EDITLISTCTRL_H__AB089162_8B4E_44BE_9BEA_B7C299C8694C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditListCtrl.h : header file
//

#pragma warning(disable:4666)
#include <vector>


///���������� ��� ��������, � ������� ��������
///������������ ��������������
const int ELC_ITEM_EDIT_CTRL_NONE = 0;
const int ELC_ITEM_EDIT_CTRL_EDIT = 1;
const int ELC_ITEM_EDIT_CTRL_COMBO = 2;

///���� �����������
///����� ��������������. ��� ����������� �� ��������
const unsigned NM_LCE_ENDEDIT = (NM_FIRST + 1U);

///���������� ��� ����, ����� ���������� ����� �� �������������
///���������� �����
const unsigned NM_LCE_EDITREQUEST	= (NM_FIRST + 2U);

/// ���������� ��� ����, �����  ���������, ���� ���� ������� 
///����������� ����������. ��������� ���, �������� ����� � ��� �����
///��������, ��������� ���������
const unsigned NM_LCE_SETUP_CTRL	= (NM_FIRST + 3U);

///�������� ����� ��������� �������� �������
const unsigned NM_LCE_SUBITEM_CHANGING = (NM_FIRST + 4U);
const unsigned NM_LCE_SUBITEM_CHANGED = (NM_FIRST + 5U);

///������ ���� �������� ��������������
const unsigned NM_LCE_EDITCONTROL_REQUEST = (NM_FIRST + 6U);

typedef struct tagNM_LCE_INFO
{
	NMHDR hdr;
	LVITEM item;
	//�������, ������� �������������� ��������������
	CWnd* pCtrl;
	int iEditCtrlType; 
} NM_LCE_INFO;

/////////////////////////////////////////////////////////////////////////////
// CXEditCtrl window


/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl window

enum
{
	ELC_SIMPLEEDIT,
	ELC_MULTIEDIT,
	ELC_FLAGS,
	ELC_FILESELECT
};

/**
	���������� ListCtrl  � ���, ����� ����� ���� ������������� ���� � ����� �������
*/
class CEditListCtrl : public CListCtrl
{
	friend class CXEditCtrl;
	friend class CXComboCtrl;
	typedef std::vector<int> IntVect_t;

	CWnd*        m_pEdit;
	CPoint       m_cellEdit;

	/// ������� edit-������� 
	CWnd* CreateEdit(CRect& rc, LPCTSTR lpszText, BOOL bMulti);
	/// ������� ���������-������� 
	CWnd* CreateCombo(CRect& rc, LPCTSTR lpszText);

	/// �������� ������� � ������ �������
	void ShowEditControl(int row, int col, int iEditType);
	/// ��������� ������� ��� �������������� ��������
	BOOL CalculateEditPosition(int row, int col, CRect& r);
	/// ��������� ������������� ������� � �������� ����� � ������������ ������
	void DestroyAndUpdateEdit(bool bSave);

	/// ������ �� ������������� ������ �������
	bool is_editable_subitem(int iCol);

	/// ������ ����� �������������� ��� �������
	IntVect_t m_vecColumnEditType;

	/// ����� ���������� �������
	int m_iSelectedSubItem;
	/// ����������� �� �������
	bool m_bEditItem;
public:

	CEditListCtrl();

	/// ������������� ��� ��������, ������� ����� ��������������� �������� � �������
	bool SetEditCtrlType(int nCol, int iType);
	/// ���������� ��� ��������, ������� ������������� �������� � ������
	int GetEditCtrlType(int nCol) const{
		return m_vecColumnEditType[nCol];
	}
	/// ���������� ����� ���������� �������
	int GetSelectedSubitem() const{
		return m_iSelectedSubItem;
	}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual VOID PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditListCtrl();

	/// �������� �������. ����� ����� �� ��� � CListCtrl::InsertColumn
	/**
		������������� ������ ������� � ���� �������������� ��������
	 */
	int InsertColumn( int nCol, const LVCOLUMN* pColumn );
	/// �������� �������. ����� ����� �� ��� � CListCtrl::InsertColumn
	/**
		������������� ������ ������� � ���� �������������� ��������
	 */
	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, 
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 );

	/// ��������� �������������� ��������
	CWnd* EditSubItem(int iItem, int iSubItem);
	/// ��������� �������������� �������� � ��������� ������� ��� �������������� ��������
	CWnd* EditSubItem(int iItem, int iSubItem, int iEditType);
	
	VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Generated message map functions
protected:
	//{{AFX_MSG(CEditListCtrl)
	afx_msg VOID OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg VOID OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	VOID DrawSubItem(LPDRAWITEMSTRUCT lpdis, int iSubItem, CRect& rc);
	LRESULT SendCustomMsg(unsigned id, NM_LCE_INFO* pnli);
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_EDITLISTCTRL_H__AB089162_8B4E_44BE_9BEA_B7C299C8694C__INCLUDED_)
