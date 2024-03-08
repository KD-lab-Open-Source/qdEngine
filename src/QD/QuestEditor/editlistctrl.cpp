// EditListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "EditListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/************************************************************************/
/*                                                                      */
/************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CXEditCtrl
class CXEditCtrl : public CEdit
{
	DECLARE_DYNAMIC(CXEditCtrl);
public:
	CXEditCtrl(){}
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXEditCtrl)
protected:
	virtual VOID PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CXEditCtrl(){}
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CXEditCtrl)
	afx_msg VOID OnKillFocus(CWnd* pNewWnd);
	afx_msg VOID OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CXComboCtrl window

class CXComboCtrl : public CCJFlatComboBox
{
public:
	CXComboCtrl();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXComboCtrl)
protected:
	virtual VOID PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CXComboCtrl();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CXComboCtrl)
	afx_msg VOID OnKillFocus(CWnd* pNewWnd);
	afx_msg VOID OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg VOID OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CXEditCtrl, CEdit)

BEGIN_MESSAGE_MAP(CXEditCtrl, CEdit)
//{{AFX_MSG_MAP(CXEditCtrl)
ON_WM_KILLFOCUS()
ON_WM_KEYDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

VOID CXEditCtrl::PostNcDestroy() 
{
	delete this;
}

VOID CXEditCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
//	if(pNewWnd != GetParent())
	((CEditListCtrl*)GetParent())->DestroyAndUpdateEdit(true);
}

VOID CXEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	
	if((GetWindowLong(m_hWnd, GWL_STYLE) & ES_MULTILINE) == 0)
	{
		if(nChar == VK_RETURN)
			((CEditListCtrl*)GetParent())->DestroyAndUpdateEdit(true);
		if(nChar == VK_ESCAPE)
			((CEditListCtrl*)GetParent())->DestroyAndUpdateEdit(false);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXComboCtrl

CXComboCtrl::CXComboCtrl()
{
}

CXComboCtrl::~CXComboCtrl()
{
}


BEGIN_MESSAGE_MAP(CXComboCtrl, CCJFlatComboBox)
	//{{AFX_MSG_MAP(CXComboCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXComboCtrl message handlers

VOID CXComboCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);

//	if(pNewWnd != GetParent())
	((CEditListCtrl*)GetParent())->DestroyAndUpdateEdit(true);
}

VOID CXComboCtrl::PostNcDestroy() 
{
	ReleaseCapture();
	CCJFlatComboBox::PostNcDestroy();
	delete this;	
}

VOID CXComboCtrl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CCJFlatComboBox::OnShowWindow(bShow, nStatus);
	
	CString str;
	CSize   sz;
	int     dx=0;
	CDC*    pDC = GetDC();
	int count = GetCount();
	for (int i=0;i < count;++i)
	{
		GetLBText( i, str );
		sz = pDC->GetTextExtent(str);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
	ReleaseDC(pDC);
	
	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += 2*::GetSystemMetrics(SM_CXEDGE);
	
	// Set the width of the list box so that every item is completely visible.
	SetDroppedWidth(dx);

	CRect r;
	GetWindowRect(r);
//	dx = max(dx, r.Width());
	SetWindowPos(0, 0, 0, r.Width(), r.Height(), SWP_NOMOVE |SWP_NOOWNERZORDER);
}


VOID CXComboCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CCJFlatComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
	
	if(nChar == VK_RETURN)
		((CEditListCtrl*)GetParent())->DestroyAndUpdateEdit(true);

	if(nChar == VK_ESCAPE)
		((CEditListCtrl*)GetParent())->DestroyAndUpdateEdit(false);
}


int CXComboCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJFlatComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
//	SetCapture();	
	return 0;
}

void CXComboCtrl::OnDestroy() 
{
	CCJFlatComboBox::OnDestroy();

//	ReleaseCapture();
}

void CXComboCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect r;
	GetClientRect(r);
	if (!r.PtInRect(point))
	{
		((CEditListCtrl*)GetParent())->DestroyAndUpdateEdit(true);
		return;
	}
	
	CCJFlatComboBox::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl

CEditListCtrl::CEditListCtrl()		 
{
	m_pEdit = 0;
	m_bEditItem = false;
	m_iSelectedSubItem = -1;
}

CEditListCtrl::~CEditListCtrl()
{
	DestroyAndUpdateEdit(false);
}


BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEditListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl message handlers

LRESULT CEditListCtrl::SendCustomMsg(unsigned id, NM_LCE_INFO* pnli)
{
	return GetParent()->SendMessage(WM_NOTIFY, id, reinterpret_cast<LPARAM>(pnli));
}

VOID CEditListCtrl::PreSubclassWindow() 
{
	SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
}

BOOL CEditListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam) {
		case VK_RETURN:
			if(m_pEdit)
			{
				DestroyAndUpdateEdit(true);
				return TRUE;
			}
		case VK_ESCAPE:
			if(m_pEdit)
			{
				DestroyAndUpdateEdit(false);
				return TRUE;
			}
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

VOID CEditListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	DestroyAndUpdateEdit(true);
	LVHITTESTINFO hti;
	hti.pt = point;

	SubItemHitTest(&hti);
	if (hti.flags & LVHT_ONITEMLABEL)
	{
		if (m_iSelectedSubItem != hti.iSubItem)
		{
			NM_LCE_INFO hdr;
			memset(&hdr, 0, sizeof(hdr));
			hdr.hdr.code = NM_LCE_SUBITEM_CHANGING;
			hdr.hdr.hwndFrom = m_hWnd;
			hdr.hdr.idFrom = 0;
			
			hdr.item.iItem = hti.iItem;
			hdr.item.iSubItem = m_iSelectedSubItem;
			if (m_iSelectedSubItem == -1)
				hdr.iEditCtrlType = ELC_ITEM_EDIT_CTRL_NONE;
			else
				hdr.iEditCtrlType = m_vecColumnEditType[m_iSelectedSubItem];
			
			if(GetParent()->SendMessage(WM_NOTIFY, 
				NM_LCE_SUBITEM_CHANGING, reinterpret_cast<LPARAM>(&hdr)) != TRUE)
			{
				m_iSelectedSubItem = hti.iSubItem;
				RedrawItems(hti.iItem, hti.iItem);

				hdr.hdr.code = NM_LCE_SUBITEM_CHANGED;
				hdr.item.iSubItem = m_iSelectedSubItem;

				GetParent()->SendMessage(WM_NOTIFY, NM_LCE_SUBITEM_CHANGED, (LPARAM)&hdr);
			}
		}
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

VOID CEditListCtrl::ShowEditControl(int row, int col, int iEditType)
{
	DestroyAndUpdateEdit(true);

	NM_LCE_INFO hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.hdr.code = NM_LCE_EDITREQUEST;
	hdr.hdr.hwndFrom = m_hWnd;
	hdr.hdr.idFrom = 0;

	hdr.item.iItem = row;
	hdr.item.iSubItem = col;
	hdr.iEditCtrlType = iEditType;

	if(GetParent()->SendMessage(WM_NOTIFY, NM_LCE_EDITREQUEST, (LPARAM)&hdr) == TRUE)
		return;

	//edit or combo?
	CRect rc;
	if(CalculateEditPosition(row, col, rc))
	{
		switch(iEditType) {
		case ELC_ITEM_EDIT_CTRL_EDIT:
			m_pEdit = CreateEdit(rc, GetItemText(row,col), false);
			break;
		case ELC_ITEM_EDIT_CTRL_COMBO:
			m_pEdit = CreateCombo(rc, GetItemText(row, col));
			break;
		}
		m_cellEdit.x = row;
		m_cellEdit.y = col;
		_ASSERT(m_pEdit);

		hdr.pCtrl = m_pEdit;
		hdr.hdr.code = NM_LCE_SETUP_CTRL;

		GetParent()->SendMessage(WM_NOTIFY, NM_LCE_SETUP_CTRL, (LPARAM)&hdr);
		m_pEdit->ShowWindow(SW_SHOW);
		m_pEdit->BringWindowToTop();
	}
}

CWnd* CEditListCtrl::CreateEdit(CRect& rc, LPCTSTR lpszText, BOOL bMulti)
{
	int ws = WS_CHILD|WS_BORDER|ES_AUTOHSCROLL;
	if(bMulti)
	{
		ws |= ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL;
		rc.bottom += rc.Height()*2;
	}

	CXEditCtrl* pCtrl = new CXEditCtrl;
	pCtrl->Create(ws, rc, this, 1);
	pCtrl->SetFont(GetFont());
	pCtrl->SetFocus();
	pCtrl->SetWindowText(lpszText);
	pCtrl->SetSel(0, -1);
	return pCtrl;
}

CWnd* CEditListCtrl::CreateCombo(CRect& rc, LPCTSTR lpszText)
{
	rc.bottom += rc.Height()*10;
	rc.right += rc.Width()/3;
	CRect r;
	GetClientRect(&r);
	if(r.right < (rc.right + (::GetSystemMetrics(SM_CXVSCROLL)<<1)))
	{
		rc.OffsetRect(r.right - rc.right -1 - (::GetSystemMetrics(SM_CXVSCROLL)<<1), 0);
	}

	CXComboCtrl* pCtrl = new CXComboCtrl;
	pCtrl->Create(WS_CHILD|WS_VSCROLL|CBS_DROPDOWNLIST, rc, this, 2);
	pCtrl->SetFont(GetFont());
	pCtrl->SetFocus();
	return pCtrl;
}

BOOL CEditListCtrl::CalculateEditPosition(int row, int col, CRect& r)
{
	if(!GetItemRect(row,r,LVIR_BOUNDS)) 
		return FALSE;

	int x=0;
	int cx=0;
	for(int i=0;i<=col;++i)
	{
		x+=cx;
		cx=GetColumnWidth(i);
	}

	r.left		+= x;
	r.right		=  r.left+cx;
	r.bottom	+= 2;
	return TRUE;
}
VOID CEditListCtrl::DestroyAndUpdateEdit(bool bSave)
{
	if(!m_pEdit || !::IsWindow(m_pEdit->m_hWnd))
		return;

	CString s, strItemText(GetItemText(m_cellEdit.x, m_cellEdit.y));

	CWnd* ptrEdit = m_pEdit;
	m_pEdit = 0;
	
	if (bSave)
		ptrEdit->GetWindowText(s);


	if(s.IsEmpty()||s.CompareNoCase(strItemText))
	{
		NM_LCE_INFO hdr;
		hdr.hdr.code = NM_LCE_ENDEDIT;
		hdr.hdr.hwndFrom = m_hWnd;
		hdr.hdr.idFrom = 0;

		hdr.item.iItem = m_cellEdit.x;
		hdr.item.iSubItem = m_cellEdit.y;
		
		hdr.pCtrl = ptrEdit;
		hdr.iEditCtrlType = m_vecColumnEditType[hdr.item.iSubItem];

		hdr.item.mask = LVIF_TEXT;
		LPTSTR pszBuff = s.GetBufferSetLength(s.GetLength()+1);

		hdr.item.pszText = pszBuff;
		hdr.item.cchTextMax = s.GetLength();

		LONG res = GetParent()->SendMessage(WM_NOTIFY, 0, (LPARAM)&hdr);
		ASSERT(pszBuff == hdr.item.pszText);
		s.ReleaseBuffer();
		if(res != FALSE)
			SetItemText(m_cellEdit.x, m_cellEdit.y, s);
	}
	CRect r;
	ptrEdit->GetWindowRect(r);
	ptrEdit->DestroyWindow();

	ScreenToClient(r);
	InvalidateRect(r);
}

bool CEditListCtrl::is_editable_subitem(int iCol)
{
	return (m_vecColumnEditType[iCol] != ELC_ITEM_EDIT_CTRL_NONE);
}

VOID CEditListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if(is_editable_subitem(m_iSelectedSubItem))
		ShowEditControl(pDispInfo->item.iItem, m_iSelectedSubItem, 
										m_vecColumnEditType[m_iSelectedSubItem]);
	else
	{
		NM_LCE_INFO hdr;
		hdr.hdr.code = NM_LCE_EDITCONTROL_REQUEST;
		hdr.hdr.hwndFrom = m_hWnd;
		hdr.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);

		hdr.item.iItem		= pDispInfo->item.iItem;
		hdr.item.iSubItem	= m_iSelectedSubItem;

		hdr.pCtrl = NULL;
		hdr.iEditCtrlType = m_vecColumnEditType[hdr.item.iSubItem];
		if (SendCustomMsg(NM_LCE_EDITCONTROL_REQUEST, &hdr))
			ShowEditControl(pDispInfo->item.iItem, m_iSelectedSubItem, hdr.iEditCtrlType);
		else
			DestroyAndUpdateEdit(true);
	}
	
	*pResult = TRUE;
}

CWnd* CEditListCtrl::EditSubItem(int irow, int icol, int iEditType)
{
	if (iEditType == ELC_ITEM_EDIT_CTRL_NONE) 
		return NULL;
	ASSERT(-1 < icol && icol < static_cast<int>(m_vecColumnEditType.size()));
	DestroyAndUpdateEdit(true);
	ShowEditControl(irow, icol, iEditType);
	return m_pEdit;
}

CWnd* CEditListCtrl::EditSubItem(int irow, int icol)
{
	ASSERT(-1 < icol && icol < static_cast<int>(m_vecColumnEditType.size()));
	DestroyAndUpdateEdit(true);

	if (m_vecColumnEditType[icol] == ELC_ITEM_EDIT_CTRL_NONE) 
		return NULL;

	ShowEditControl(irow, icol, m_vecColumnEditType[icol]);
	return m_pEdit;
}

int CEditListCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	int res = CListCtrl::InsertColumn(nCol, pColumn);
	if(res != -1)
	{
		m_vecColumnEditType.insert(m_vecColumnEditType.begin()+res, ELC_ITEM_EDIT_CTRL_NONE);
	}

	return res;
}
int CEditListCtrl::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	int res = CListCtrl::InsertColumn(nCol, lpszColumnHeading, nFormat,nWidth, nSubItem);
	if(res != -1)
	{
		m_vecColumnEditType.insert(m_vecColumnEditType.begin()+res, ELC_ITEM_EDIT_CTRL_NONE);
	}
	return res;
}

bool CEditListCtrl::SetEditCtrlType(int nCol, int iType)
{
	_ASSERT(-1 < nCol && nCol < (int)m_vecColumnEditType.size());
	m_vecColumnEditType[nCol] = iType;
	return true;
}


VOID CEditListCtrl::DrawSubItem(LPDRAWITEMSTRUCT lpdis, int iSubItem,CRect& rc)
{
	CString str (GetItemText(lpdis->itemID, iSubItem));
	DrawText(lpdis->hDC, str, str.GetLength(), &rc,DT_END_ELLIPSIS);
	if (lpdis->itemState&ODS_SELECTED&&iSubItem == m_iSelectedSubItem)
		Rectangle(lpdis->hDC, rc.left-2, rc.top+1, rc.right, rc.bottom-1);
}

VOID CEditListCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	ASSERT(lpdis->CtlType == ODT_LISTVIEW);

	COLORREF oldColor = 0;
	if (lpdis->itemState&ODS_SELECTED)
	{
		oldColor = ::SetTextColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		FillRect(lpdis->hDC, &lpdis->rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
	}
	else 
	{
		oldColor = ::SetTextColor(lpdis->hDC, GetSysColor(COLOR_WINDOWTEXT));
		FillRect(lpdis->hDC, &lpdis->rcItem, GetSysColorBrush(COLOR_WINDOW));
	}
	
	CString str;
	CRect rc;
	int nCount = GetHeaderCtrl()->GetItemCount();
	CRect rcIcon;
	GetSubItemRect(0, 0, LVIR_ICON, rcIcon);

		
	int mode = SetROP2(lpdis->hDC, R2_NOT);
	HGDIOBJ prev = SelectObject(lpdis->hDC, GetStockObject(NULL_BRUSH));
	HGDIOBJ oldP = SelectObject(lpdis->hDC, CreatePen(PS_DOT, 1, 0));
	
	for(int i = nCount; --i>0;)
	{
		GetSubItemRect(lpdis->itemID, i, LVIR_BOUNDS, rc);
		rc.left += 3;
		DrawSubItem(lpdis, i, rc);
	}
	GetItemRect(lpdis->itemID, rc, LVIR_BOUNDS);
	rc.right = rc.left + GetColumnWidth(0)-1;
	rc.left += 3;
	DrawSubItem(lpdis, 0, rc);
	
	DeleteObject(SelectObject(lpdis->hDC, oldP));
	
	SelectObject(lpdis->hDC, oldP);
	SelectObject(lpdis->hDC, prev);
	SetROP2(lpdis->hDC, mode);
	
	
	::SetTextColor(lpdis->hDC, oldColor);
}
