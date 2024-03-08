// CJFlatComboBox.cpp : implementation file
//
// Copyright © 1998-99 Kirk W. Stowell
//		mailto:kstowell@codejockeys.com
//		http://www.codejockeys.com/kstowell/
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. If the source code in this file is used in 
// any  commercial application then a statement along the lines of 
// "Portions Copyright © 1998 Kirk Stowell" must be included in 
// the startup banner, "About" box or printed documentation. An email 
// letting me know that you are using it would be nice as well. That's 
// not much to ask considering the amount of work that went into this.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// Expect bugs!
// 
// Please use and enjoy, and let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into 
// this file. 
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 5/17/99 9:24p $
 * $Revision: 9 $
 * $Archive: /CodeJockey/CJ60Lib/CJFlatComboBox.cpp $
 *
 * $History: CJFlatComboBox.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 9:24p
 * Updated in $/CodeJockey/CJ60Lib
 * Gilberto Botaro [seafox@techno.com.br] - Added code for auto
 * completion.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 5/03/99    Time: 10:44p
 * Updated in $/CodeJockey/CJ60Lib
 * Fixed code ommisions.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 5/03/99    Time: 8:42p
 * Updated in $/CodeJockey/CJ60Lib
 * Danilo Lax [Danilo_Lax@www2.alpeng.com] - The control is painted flat
 * when the mouse leaves the box, even if the control has the focus, now
 * checking for the focus in OnTimer(.
 * 
 * Removed, unhandled msgs are processed by
 * CComboBox::PreTranslateMessage(pMsg). Used reflected msgs
 * BN_GETFOCUS/CBN_KILLFOCUS instead of WM_GETFOCUS/WM_KILLFOCUS.
 * 
 * ::GetFocus() doesn't return CFlatComboBox::m_hWnd, even if the control
 * has the focus (don't know why, maybe ::GetFocus() only returns the
 * handle to the EditField part of the ComboBox), used flag m_bHasFocus
 * instead.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:26p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 1/22/99    Time: 4:26p
 * Updated in $/CodeJockey/CJ60Lib
 * Fixed accelerator problem reported by YOSH YAMASHITA
 * (yosh@muj.biglobe.ne.jp) with Matt Weagle's solution of overloading
 * PreTranslateMessage().
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 1/02/99    Time: 4:25p
 * Updated in $/CodeJockey/CJ60Lib
 * Renamed class from CJComboBox to CCJFlatComboBox, changed DRAWSTATE to
 * enum, removed method Offset(), and replaced with a direct call to
 * ::GetSystemMetrics().
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 11/02/98   Time: 4:24p
 * Updated in $/CodeJockey/CJ60Lib
 * Added OnSetFocus(), OnKillFocus() and modified OnPaint() so the control
 * remains beveled until it actually looses focus - (Vachik
 * Hovhannissian).
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 10/05/98   Time: 4:24p
 * Updated in $/CodeJockey/CJ60Lib
 * Fixed offset bug to use GetSystemMetrics(), instead of hard coded
 * values -
 * Todd Brannam (tbrannam@msn.com).
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/02/98   Time: 4:23p
 * Created in $/CodeJockey/CJ60Lib
 * First release version.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJFlatComboBox.h"
#include ".\cjflatcombobox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LRESULT FAR PASCAL CCJFlatComboBox::ComboBoxListBoxProc(HWND hWnd, 
												  UINT nMsg, 
												  WPARAM wParam, 
												  LPARAM lParam)
{
	CCJFlatComboBox* comboBox = 
		reinterpret_cast<CCJFlatComboBox*>(GetWindowLong(hWnd, GWL_USERDATA));

	WNDPROC oldWndProc = comboBox->oldListBoxProc_;
	switch (nMsg) {
	case WM_LBUTTONUP: 
		{
			if (!comboBox->getCanCloaseListBox())
			{
				// Don't do anything here. This causes the combobox popup
				// windows to remain open after a selection has been made
				comboBox->setCanCloseListBox(true);
				return 0;
			}
			comboBox->setCanCloseListBox(false);
			break;

		}
	}
	return CallWindowProc(oldWndProc, hWnd, nMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CCJFlatComboBox

CCJFlatComboBox::CCJFlatComboBox()
{
	m_bLBtnDown	= FALSE;
	m_bPainted	= FALSE;
	m_bHasFocus	= FALSE;
	m_bAutoComp = TRUE;

	m_clrBtnHilite  = ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow  = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace    = ::GetSysColor(COLOR_BTNFACE);
	m_nOffset		= ::GetSystemMetrics(SM_CXHTHUMB);
	m_hListBox		= NULL;
	oldListBoxProc_ = NULL;
	canCloseListBox_ = false;
	oldSelectedItem_ = -1;
	autoCompleteInterrupted_ = false;
}

CCJFlatComboBox::~CCJFlatComboBox()
{
}

void CCJFlatComboBox::setCanCloseListBox(bool value) const{
	canCloseListBox_ = value;
}
bool CCJFlatComboBox::getCanCloaseListBox() const{
	return canCloseListBox_;
}

IMPLEMENT_DYNAMIC(CCJFlatComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CCJFlatComboBox, CComboBox)
	//{{AFX_MSG_MAP(CCJFlatComboBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(CBN_SETFOCUS,   OnSetFocus)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS,  OnKillFocus)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCbnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFlatComboBox message handlers

void CCJFlatComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetTimer(1,10,NULL);
	OnTimer(1);
	CComboBox::OnMouseMove(nFlags, point);
}

void CCJFlatComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = TRUE;
	CComboBox::OnLButtonDown(nFlags, point);
}

void CCJFlatComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = FALSE;
	setCanCloseListBox(true);	
	Invalidate();
	CComboBox::OnLButtonUp(nFlags, point);
}

void CCJFlatComboBox::OnTimer(UINT nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rcItem;
	GetWindowRect(&rcItem);

	// OnLButtonDown, show pressed.
	if (m_bLBtnDown==TRUE) {
		KillTimer (1);
		if (m_bPainted == TRUE) {
			DrawCombo( pressed, m_clrBtnShadow, m_clrBtnHilite );
			m_bPainted = FALSE;
		}
		return;
	}

	// If mouse leaves, show flat.
	if (!rcItem.PtInRect(pt) && !m_bHasFocus) {
		KillTimer (1);
		if (m_bPainted == TRUE) {
			DrawCombo( normal, m_clrBtnFace, m_clrBtnFace );
			m_bPainted = FALSE;
		}
		return;
	}

	// On mouse over, show raised.
	else {
		if (m_bPainted == TRUE)
			return;
		else {
			m_bPainted = TRUE;
			DrawCombo( raised, m_clrBtnShadow, m_clrBtnHilite );
		}
	}
	
	CComboBox::OnTimer(nIDEvent);
}

void CCJFlatComboBox::OnPaint() 
{
	ModifyStyleEx (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE,
		0, SWP_FRAMECHANGED);
	
	Default();

	CPoint pt;
	GetCursorPos(&pt);

	CRect rcItem;
	GetWindowRect(&rcItem);

	if ((rcItem.PtInRect(pt)) || m_bHasFocus)
		DrawCombo( raised, m_clrBtnShadow, m_clrBtnHilite );
	else
		DrawCombo( normal, m_clrBtnFace, m_clrBtnFace );
}

void CCJFlatComboBox::DrawCombo(STATE eState, 
								COLORREF clrTopLeft, 
								COLORREF clrBottomRight)
{
	CRect rcItem;
	GetClientRect(&rcItem);
	CDC* pDC = GetDC();

	// Cover up dark 3D shadow.
	pDC->Draw3dRect(rcItem, clrTopLeft, clrBottomRight);
	rcItem.DeflateRect(1,1);
	
	if (!IsWindowEnabled()) {
		pDC->Draw3dRect( rcItem, m_clrBtnHilite, m_clrBtnHilite );
	}
	
	else {
		pDC->Draw3dRect( rcItem, m_clrBtnFace, m_clrBtnFace );
	}

	// Cover up dark 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	rcItem.left = rcItem.right-m_nOffset;
	pDC->Draw3dRect( rcItem, m_clrBtnFace, m_clrBtnFace );
	
	// Cover up normal 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	pDC->Draw3dRect( rcItem, m_clrBtnFace, m_clrBtnFace );
	
	if (!IsWindowEnabled()) {
		return;
	}

	switch (eState)
	{
	case normal:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		pDC->Draw3dRect( rcItem, m_clrBtnHilite, m_clrBtnHilite );
		rcItem.left -= 1;
		pDC->Draw3dRect( rcItem, m_clrBtnHilite, m_clrBtnHilite );
		break;

	case raised:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		pDC->Draw3dRect( rcItem, m_clrBtnHilite, m_clrBtnShadow);
		break;

	case pressed:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		rcItem.OffsetRect(1,1);
		pDC->Draw3dRect( rcItem, m_clrBtnShadow, m_clrBtnHilite );
		break;
	}

	ReleaseDC(pDC);
}

void CCJFlatComboBox::OnSetFocus()
{
	m_bHasFocus = TRUE;
	DrawCombo( raised, m_clrBtnShadow, m_clrBtnHilite );
}

void CCJFlatComboBox::OnKillFocus() 
{
	m_bHasFocus = FALSE;
	DrawCombo( normal, m_clrBtnFace, m_clrBtnFace );
}

BOOL CCJFlatComboBox::PreTranslateMessage(MSG* pMsg) 
{
	// Catch the Alt key so we don't choke if focus 
	// is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;

	if (pMsg->message == WM_KEYDOWN)
	{
		autoCompleteInterrupted_ = false;

		int nVirtKey = (int) pMsg->wParam;
		if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
			autoCompleteInterrupted_ = true;
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

void CCJFlatComboBox::OnEditUpdate() 
{
	// if we are not to auto update the text, get outta here
	if (!m_bAutoComp||autoCompleteInterrupted_) 
		return;
	
	// Get the text in the edit box
	CString str;
	GetWindowText(str);
	int nLength = str.GetLength();
	
	// Currently selected range
	DWORD dwCurSel = GetEditSel();
	WORD dStart = LOWORD(dwCurSel);
	WORD dEnd   = HIWORD(dwCurSel);
	
	// Search for, and select in, and string in the combo box that is prefixed
	// by the text in the edit box
	if (SelectString(-1, str) == CB_ERR)
	{
		SetWindowText(str);		// No text selected, so restore what was there before
		if (dwCurSel != CB_ERR)
			SetEditSel(dStart, dEnd);	//restore cursor postion
	}
	
	// Set the text selection as the additional text that we have added
	if (dEnd < nLength && dwCurSel != CB_ERR)
		SetEditSel(dStart, dEnd);
	else
		SetEditSel(nLength, -1);
}

void CCJFlatComboBox::OnSysColorChange() 
{
	CComboBox::OnSysColorChange();

	// re-initialize system colors.
	m_clrBtnHilite  = ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow  = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace    = ::GetSysColor(COLOR_BTNFACE);
}

//! Устанавливает ширину лист-бокса для контрола
void CCJFlatComboBox::SetDroppedWidth(UINT nWidth)
{
	if (nWidth == DROPPED_WIDTH_AUTO_FIT)
		fitDropedWidthToContent();
	else
		CComboBox::SetDroppedWidth(nWidth);
}

void CCJFlatComboBox::fitDropedWidthToContent()
{
	CDC*      pDC = GetDC();
	CFont*      pFont = GetFont();
	
	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	TEXTMETRIC   tm;
	pDC->GetTextMetrics(&tm);
	
	CString str;
	CSize sz;
	int dx = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		GetLBText(i, str);
		sz = pDC->GetTextExtent(str);
		
		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
	
	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);
	
	// Set the width of the list box so that every item is completely visible.
	CComboBox::SetDroppedWidth(dx);
}

LRESULT CCJFlatComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam) 
{
	// If the listbox hasn't been subclassed yet, do so...
	if (m_hListBox == 0) {
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) {
			// Save the listbox handle
			m_hListBox = hWnd;

			// Do the subclassing
			oldListBoxProc_ = (WNDPROC)GetWindowLong(m_hListBox, GWL_WNDPROC);
			SetWindowLong(m_hListBox, GWL_WNDPROC, (LONG)ComboBoxListBoxProc);
			SetWindowLong(m_hListBox, GWL_USERDATA, reinterpret_cast<LONG>(this));
		}
	}

	
	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

void CCJFlatComboBox::OnDropdown() 
{
	setCanCloseListBox(false);	
	oldSelectedItem_ = GetCurSel();
}

// this member function is called by the combo box whenever a paint
BOOL CCJFlatComboBox::Create(DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID )
{
	int maxHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	CRect rc(rect);
	rc.bottom = rc.top + maxHeight;

	if ((dwStyle&CBS_OWNERDRAWVARIABLE)||dwStyle&CBS_OWNERDRAWFIXED)
		dwStyle |= CBS_HASSTRINGS;
	// Make sure to use the CBS_OWNERDRAWVARIABLE style
//	dwStyle |= CBS_OWNERDRAWVARIABLE;

	// Use default strings. We need the itemdata to store checkmarks
//	dwStyle |= CBS_HASSTRINGS;

	return CComboBox::Create(dwStyle, rc, pParentWnd, nID);
}

void CCJFlatComboBox::PreSubclassWindow()
{
	int maxHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	CRect rc;
	::GetWindowRect(m_hWnd, rc);
	rc.bottom = rc.top + maxHeight;
	::SetWindowPos(m_hWnd, NULL, 0, 0, rc.Width(), maxHeight, SWP_NOZORDER|SWP_NOMOVE);
	CComboBox::PreSubclassWindow();
}


void CCJFlatComboBox::OnCbnCloseup()
{
	Invalidate();
	oldSelectedItem_ = -1;
}



void CCJFlatComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
   ASSERT(lpMeasureItemStruct->CtlType == ODT_COMBOBOX);

   if (lpMeasureItemStruct->itemID != (UINT) -1)
   {
      LPCTSTR lpszText = (LPCTSTR) lpMeasureItemStruct->itemData;
      ASSERT(lpszText != NULL);
      CSize   sz;
      CDC*    pDC = GetDC();

      sz = pDC->GetTextExtent(lpszText);

      ReleaseDC(pDC);

      lpMeasureItemStruct->itemHeight = sz.cy-2;
   }
	
}

void CCJFlatComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (lpDrawItemStruct->itemID == static_cast<UINT_PTR>(-1))
		return;
	CString strText;
	// Check if we are drawing the static portion of the combobox
	GetLBText(lpDrawItemStruct->itemID, strText);

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	
	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();
	int oldBkMode = dc.SetBkMode(TRANSPARENT);
	
	// If this item is selected, set the background color 
	// and the text color to appropriate values. Erase
	// the rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState  & ODS_SELECTED))
	{
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
		dc.DrawFocusRect(&lpDrawItemStruct->rcItem);
	}
	else if (lpDrawItemStruct->itemID == oldSelectedItem_)
	{
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, 
			::GetSysColor(COLOR_INACTIVECAPTION));
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
	
	lpDrawItemStruct->rcItem.left += 2;
	// Draw the text.
	dc.DrawText(strText,-1,
		&lpDrawItemStruct->rcItem,
		DT_SINGLELINE|DT_VCENTER);
	
	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkMode(oldBkMode);
	dc.SetBkColor(crOldBkColor);
	
	dc.Detach();
}


