// CJColorPicker.cpp : implementation file
//
// CJColorPicker is a drop-in Color picker control. Check out the 
// header file or the accompanying HTML doc file for details.
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
// Updated 16 May 1998
//         31 May 1998 - added Default text (CJM)
//          9 Jan 1999 - minor vis update

#include "stdafx.h"
#include "CJColorPopup.h"
#include "CJColorPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDX_CJColorPicker(CDataExchange *pDX, int nIDC, COLORREF& crColor)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
#ifdef _DEBUG
	CCJColorPicker* pColorPicker = 
		dynamic_cast<CCJColorPicker*>(CWnd::FromHandle(hWndCtrl));
#else
    CCJColorPicker* pColorPicker = 
		reinterpret_cast<CCJColorPicker*>(CWnd::FromHandle(hWndCtrl));
#endif // _DEBUG
    if (pDX->m_bSaveAndValidate)
    {
        crColor = pColorPicker->GetColor();
    }
    else // initializing
    {
        pColorPicker->SetColor(crColor);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker

CCJColorPicker::CCJColorPicker()
{
    SetBkColor(GetSysColor(COLOR_3DFACE));
    SetTextColor(GetSysColor(COLOR_BTNTEXT));

    m_bTrackSelection = FALSE;
    m_nSelectionMode = CP_MODE_BK;
    m_bActive = FALSE;

////////////////////////////////////////
// 1999-06-11 begin mods Kirk Stowell
////////////////////////////////////////
	m_strDefaultText = "Automatic";
	m_strCustomText  = "Custmize...";
////////////////////////////////////////
// 1999-06-11 end mods Kirk Stowell
////////////////////////////////////////
}

CCJColorPicker::~CCJColorPicker()
{
}

IMPLEMENT_DYNCREATE(CCJColorPicker, CButton)

BEGIN_MESSAGE_MAP(CCJColorPicker, CButton)
    //{{AFX_MSG_MAP(CCJColorPicker)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_CREATE()
	//}}AFX_MSG_MAP
    ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker message handlers

LONG CCJColorPicker::OnSelEndOK(UINT lParam, LONG /*wParam*/)
{
    COLORREF crNewColor = (COLORREF) lParam;
    m_bActive = FALSE;
    SetColor(crNewColor);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDOK, lParam, (WPARAM) GetDlgCtrlID());
    }

    if (crNewColor != GetColor())
        if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

LONG CCJColorPicker::OnSelEndCancel(UINT lParam, LONG /*wParam*/)
{
    m_bActive = FALSE;
    SetColor((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LONG CCJColorPicker::OnSelChange(UINT lParam, LONG /*wParam*/)
{
    if (m_bTrackSelection) SetColor((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

int CCJColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    SetWindowSize();    // resize appropriately
    return 0;
}

// On mouse click, create and show a CJColorPopup window for Color selection
BOOL CCJColorPicker::OnClicked()
{
    m_bActive = TRUE;

    CRect rect;
    GetWindowRect(rect);
    new CCJColorPopup(CPoint(rect.left, rect.bottom),    // Point to display popup
                     GetColor(),                       // Selected Color
                     this,                              // parent
                     m_strDefaultText,                  // "Default" text area
                     m_strCustomText);                  // Custom Text

    CWnd *pParent = GetParent();
    if (pParent)
        pParent->SendMessage(CPN_DROPDOWN, (LPARAM)GetColor(), (WPARAM) GetDlgCtrlID());

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}

////////////////////////////////////////
// 1999-06-11 begin mods Kirk Stowell
////////////////////////////////////////
void CCJColorPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC*	pDC			= CDC::FromHandle( lpDrawItemStruct->hDC );
    UINT	itemState	= lpDrawItemStruct->itemState;
	CRect	rcItem		= lpDrawItemStruct->rcItem;
	DWORD	dwState		= EDGE_RAISED;
	DWORD	dwArrow		= DFCS_SCROLLDOWN;
	
	// erase everything....
	pDC->FillRect( rcItem, CBrush::FromHandle(::GetSysColorBrush( COLOR_3DFACE )));
	
	// set display flags based on state.
	if((itemState & ODS_SELECTED) || (m_bActive == TRUE)) {
		dwState = EDGE_SUNKEN;
		dwArrow = DFCS_SCROLLDOWN|DFCS_PUSHED;
	}
	if(itemState & ODS_DISABLED) {
		dwArrow = DFCS_SCROLLDOWN|DFCS_INACTIVE;
	}
	
	// Draw the drop arrow.
	CRect rcArrow( rcItem );
	rcArrow.left = rcArrow.Width()-::GetSystemMetrics( SM_CXHTHUMB );
    pDC->DrawFrameControl( &rcArrow, DFC_SCROLL, dwArrow );
	pDC->Draw3dRect( rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
	rcArrow.DeflateRect( 1,1 );
	pDC->Draw3dRect( rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
	
	if((itemState & ODS_SELECTED) || (m_bActive == TRUE))
		rcArrow.OffsetRect( 1,1 );
	
	// draw the seperator line.
	CPen penShadow( PS_SOLID, 1, ::GetSysColor( COLOR_3DSHADOW  ));
	pDC->SelectObject( &penShadow );
	pDC->MoveTo( rcArrow.left-1, rcArrow.top+2 );
	pDC->LineTo( rcArrow.left-1, rcArrow.bottom-2 );

	CPen penHilite( PS_SOLID, 1, ::GetSysColor( COLOR_3DHILIGHT ));
	pDC->SelectObject( &penHilite );
	pDC->MoveTo( rcArrow.left, rcArrow.top+2 );
	pDC->LineTo( rcArrow.left, rcArrow.bottom-2 );
	
	// draw the control border.
    pDC->DrawEdge( rcItem, dwState, BF_RECT );
	
	// draw the focus rect.
	rcItem.DeflateRect( 2, 2 );
	pDC->Draw3dRect( rcItem, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ) );
	if( itemState & ODS_FOCUS ) {
		pDC->DrawFocusRect( rcItem );
	}
	
	// draw the color box.
	if((itemState & ODS_SELECTED) || (m_bActive == TRUE))
		rcItem.OffsetRect( 1,1 );
	
	rcItem.DeflateRect( 2, 2 );
	rcItem.right = rcArrow.left-4;
	
    CBrush brush((( itemState & ODS_DISABLED ) || m_crColorBk == CLR_DEFAULT ) ? 
		::GetSysColor( COLOR_3DFACE ) : m_crColorBk );
    CBrush* pOldBrush = ( CBrush* )pDC->SelectObject( &brush );

	pDC->SelectStockObject(( itemState & ODS_DISABLED )?WHITE_PEN:BLACK_PEN );
	pDC->Rectangle( rcItem );
    pDC->SelectObject( pOldBrush );
	
    // Draw the window text (if any)
	CString strText;
    GetWindowText(strText);
    if (strText.GetLength())
    {
        pDC->SetBkMode(TRANSPARENT);
        if (itemState & ODS_DISABLED)
        {
            rcItem.OffsetRect(1,1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
            pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
            rcItem.OffsetRect(-1,-1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
            pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
        else
        {
            pDC->SetTextColor((m_crColorText == CLR_DEFAULT)? 0 : m_crColorText);
            pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
    }
}
////////////////////////////////////////
// 1999-06-11 end mods Kirk Stowell
////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker overrides

void CCJColorPicker::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
    SetWindowSize();                     // resize appropriately
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker attributes

COLORREF CCJColorPicker::GetColor() const
{ 
    return (m_nSelectionMode == CP_MODE_TEXT)? 
        GetTextColor(): GetBkColor(); 
}

void CCJColorPicker::SetColor(COLORREF crColor)
{ 
    (m_nSelectionMode == CP_MODE_TEXT)? 
        SetTextColor(crColor): SetBkColor(crColor); 
}

void CCJColorPicker::SetBkColor(COLORREF crColorBk)
{
    m_crColorBk = crColorBk;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CCJColorPicker::SetTextColor(COLORREF crColorText)
{
    m_crColorText = crColorText;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CCJColorPicker::SetDefaultText(LPCTSTR szDefaultText)
{
    m_strDefaultText = (szDefaultText)? szDefaultText : _T("");
}

void CCJColorPicker::SetCustomText(LPCTSTR szCustomText)
{
    m_strCustomText = (szCustomText)? szCustomText : _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker implementation

void CCJColorPicker::SetWindowSize()
{
    // Get size dimensions of edges
    CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    // Get size of dropdown arrow
    int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
    int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
    CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

    // Get window size
    CRect rect;
    GetWindowRect(rect);

    CWnd* pParent = GetParent();
    if (pParent)
        pParent->ScreenToClient(rect);

    // Set window size at least as wide as 2 arrows, and as high as arrow + margins
    int nWidth = max(rect.Width(), 2*ArrowSize.cx + 2*MarginSize.cx);
	int nHeight = max(rect.Height(), ArrowSize.cy+2*MarginSize.cy);
    MoveWindow(rect.left, rect.top, nWidth, nHeight, TRUE);

    // Get the new coords of this window
    GetWindowRect(rect);
    ScreenToClient(rect);

    // Get the rect where the arrow goes, and convert to client coords.
    m_ArrowRect.SetRect(rect.right - ArrowSize.cx - MarginSize.cx, 
                        rect.top + MarginSize.cy, rect.right - MarginSize.cx,
                        rect.bottom - MarginSize.cy);
}
