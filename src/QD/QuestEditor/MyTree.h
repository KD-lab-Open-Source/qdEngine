#if !defined(AFX_MYTREE_H__16C2AE21_5D2D_11D2_9CFF_008048FB92E0__INCLUDED_)
#define AFX_MYTREE_H__16C2AE21_5D2D_11D2_9CFF_008048FB92E0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MyTree.h : header file
//

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CMyTree window
// drag and drop action


//�������� ����� ���������������
//������ ����� ��� ��� ������
const unsigned NM_MT_CAN_DRAG_REQ = (NM_FIRST + 1U);
typedef struct tagNM_MT_CAN_DRAG
{
	NMHDR hdr;
	TVITEM *pItem;
	size_t item_count;
} NM_MT_CAN_DRAG;

const unsigned NM_MT_CAN_DROP_REQ = (NM_FIRST + 2U);
typedef struct tagNM_MT_CAN_DROP{
	NMHDR hdr;
	TVITEM *pItem;
	size_t item_count;
	TVITEM itemDest;
}NM_MT_CAN_DROP;

const unsigned NM_MT_DROP_REQ = (NM_FIRST + 3U);
typedef struct tagNM_MT_DROP{
	NMHDR hdr;
	TVITEM *pItem;
	size_t item_count;
	TVITEM itemDest;
	DWORD btn;//����� ������ ����������
} NM_MT_DROP;

const unsigned NM_MT_GET_DRAG_CURSOR = (NM_FIRST + 4U);
enum eGDC_Action{GDC_NO, GDC_YES,};
typedef struct tagNM_MT_GET_DRAG_CUR{
	NMHDR hdr;
	eGDC_Action	eCurAction;
}NM_MT_GET_DRAG_CUR;


/**
	������� ������� ������� ������, �������������� ������������ � ����-���-����

	� ������������� �� ����������. ����� ���������� CTreeListCtrl
 */
class CMyTree : public CTreeCtrl
{
	/// ��� ������ ��� ��������������
	enum eDragDropAction{DD_NONE,///< ������ �� ������
		DD_COPY, ///< ��������
		DD_MOVE///< ���������
	};
	/// ���� ����� �������������
	enum eScrollDirection{
		NO_SCROLL, ///< ������ �� ����� �������������
		SCROLL_DOWN, ///< ����
		SCROLL_UP///< �����
	};
// Construction
public:
	typedef std::vector< HTREEITEM > ITEM_LIST;

	CMyTree();

// Attributes
public:

// Operations
public:

	/// ���� ����� ��� � CTreeCtrl::InsertItem
	HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct);
	/// ���� ����� ��� � CTreeCtrl::InsertItem
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
		HTREEITEM hParent, HTREEITEM hInsertAfter);
	/// ���� ����� ��� � CTreeCtrl::InsertItem
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT,
		HTREEITEM hInsertAfter = TVI_LAST);
	/// ���� ����� ��� � CTreeCtrl::InsertItem
	HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage,
		HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	size_t GetSelectedCount() { 
		return item_list.size();
	}
	/// ������ ��������� ���������
	const HTREEITEM* GetSelected() const;

	/// �������� ����� �� ���� �� ����������� ����������
	void ClearSelection(HTREEITEM hitemNotEqual = NULL);
	/// ����������� �������� �������� ��� hitemDest
	void CopyChildItemTo(HTREEITEM hParent, HTREEITEM hitemDest);
	/// ��������� �������� �������� ��� hitemDest
	void MoveChildItemTo(HTREEITEM hParent, HTREEITEM hitemDest);

	/// ��������� ���� � ��� ��� ������� ��� hitemDest
	HTREEITEM MoveItemTo(HTREEITEM hItem, HTREEITEM hitemDest);
	/// �������� ���� � ��� ��� ������� ��� hitemDest
	HTREEITEM CopyItemTo(HTREEITEM hItem, HTREEITEM hitemDest);

	virtual ~CMyTree();

	/// ������� ���� �� ������
	HTREEITEM FindItem(HTREEITEM hParentItem, const CString& strText);
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTree)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRenameItem();
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	/// ����� �� ������� ����
	virtual BOOL CanDrop(HTREEITEM hItem);

private:
	/// �� ������ ����&����
	void OnCancel();

	/// ��������� ���� � ��������� ��� ������ ����
	HTREEITEM TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop);
	/// ��������� ��������� ���� ��� ������ ����
	void TransferChildItem(HTREEITEM hPar, HTREEITEM hitemDest);


	/// ���������� ��������� ��������� ������ ���� � ��� ��������
	BOOL SetItemsImage(UINT state, HTREEITEM hItem);
	/// ��������������� � ������ � ����� ���������
	void RestoreSelection();

	/// ��������� ������ �� ������������ ����
	BOOL isParentSelected(HTREEITEM hitemChild);

	/// ������� �� item_list
	BOOL removefromil(HTREEITEM hItem);

	/// ���������� ����� ������� � ����� ������ ��������� ����
	BOOL GetSelectionBound(HTREEITEM *hUpperBound,HTREEITEM *hLowerBound);
	/// ������� ������� ������ �� ���� �������� �����
	void DeselectsubEntrys(HTREEITEM hRoot);
	/// ��������� �������� �� ���� �������� ��� hitemSuspectedParent
	BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
	
	HTREEITEM GetLastVisible();

	void FillTVIBuffer(std::vector<TVITEM>& buff) const;
	void FillTVI(TVITEM& item, HTREEITEM hitem);
	void OnButtonUp(DWORD dwBtn);

	HCURSOR GetCursor(eGDC_Action act);
	LRESULT SendCustomNotify(NMHDR *hdr);
	eScrollDirection NeedScroll(CPoint const& point) const;
private:
	/// ����, � �������� ���������� ���������
	/// ��� ������������� �����
	HTREEITEM m_hShiftSelBegin;

	/// ������ ��������� ���������
	ITEM_LIST item_list;

	/// ����, ������� �����
	HTREEITEM	m_hitemDrag;
	/// ����, �� ������� ����� �������
	HTREEITEM	m_hitemDrop;


	eDragDropAction m_dwAction;
	///< ���� �� ������� ������������� ����
	BOOL m_bDragging;
	int m_ItemHeight;///< �������� � onmousemove
	/// ����� �� ������
	bool m_bCanDrop;
	eScrollDirection m_eScrollDirection;
	/// ������ ��� �������������� ��� ����&����
	UINT_PTR m_uiScrollTimer;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_MYTREE_H__16C2AE21_5D2D_11D2_9CFF_008048FB92E0__INCLUDED_)
