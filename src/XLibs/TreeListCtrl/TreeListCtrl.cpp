// TreeListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListDC.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TLCDragWnd.h"
#include "TLCDropWnd.h"
#include "TreeListCtrl.h"
#include "TreeListItem.h"
#include "TreeListColumnInfo.h"

#include "Null Listeners/NullCustomDrawNotifyListener.h"
#include "Null Listeners/NullChangeItemLabelNotifyListener.h"
#include "Null Listeners/NullItemDragdropNotifyListener.h"
#include "Null Listeners/NullItemChangeNotifyListener.h"
#include "Null Listeners/NullItemOperationNotifyListener.h"
#include "Null Listeners/NullTreeListNotifyListener.h"
#include "Null Listeners/NullGeneralNotifyListener.h"

#include <utility>
#include <memory>

#include "res/Singletons.h"

#include "ExtBitmap.h"

#include <cassert>
#include "resource.h"

// TODO: Проверить как будет работать custom-draw с TLC_BKGNDIMAGE и TLC_BKGNDCOLOR
//! Закрывает код,котроый шлет нотификацию через сообщения

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SwapArrayElements(CPtrArray& arr, INT_PTR iFirst, INT_PTR iSecond)
{
	ASSERT(iFirst != -1);
	ASSERT(iSecond != -1);
	void * t = arr[iFirst];
	arr[iFirst] = arr[iSecond];
	arr[iSecond] = t;
}

UINT const CTreeListCtrl::DRAG_DELAY = INT_MAX;//через какое время drag and drop начнется 
int const SHOW_TIP_TIMER_ID = 3;
int const BEGIN_MODIFY_TIMER = 4;
int const BEGIN_MODIFY_TIMER_TIME = 300;
							//даже без движения
namespace {
	RECT GetImageListImageRect(CImageList const* imageList){
		IMAGEINFO ii;
		imageList->GetImageInfo(0, &ii);
		return ii.rcImage;
	}

	bool IsKeyPressed(int dwKey){
		return (GetAsyncKeyState(dwKey)>>15 != 0);
	}
	ICustomDrawNotifyListener* getNullCustomDrawNotifyListener()
	{
		static NullCustomDrawNotifyListener listener;
		return &listener;
	}
	IChangeItemLabelNotifyListener* getNullChangeItemLabelNotifyListener()
	{
		static NullChangeItemLabelNotifyListener listener;
		return &listener;
	}

	IItemDragdropNotifyListener* getNullItemDragdropNotifyListener(){
		static NullItemDragdropNotifyListener listener;
		return &listener;
	}

	IItemChangeNotifyListener* getNullItemChangeNotifyListener(){
		static NullItemChangeNotifyListener listener;
		return &listener;
	}
	IItemOperationNotifyListener* getNullItemOperationListener(){
		static NullItemOperationNotifyListener listener;
		return &listener;
	}
	ITreeListNotifyListener* getNullTreeListNotifyListener(){
		static NullTreeListNotifyListener listener;
		return &listener;
	}
	IGeneralNotifyListener* getNullGeneralNotifyListener(){
		static NullGeneralNotifyListener listener;
		return &listener;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl
IMPLEMENT_DYNCREATE(CTreeListCtrl, CWnd)

CTreeListCtrl::CTreeListCtrl() : 
	m_nItemHeight( 0 ),
	m_dwStyle( 0 ),
	m_dwState( TLS_OFFSCREENBUFFER ),
	m_pRootItem( NULL ),
	m_iSelCol( -1 ),
	m_pFocusItem( NULL ),
	m_pTargetItem( NULL ),
	m_pHoverItem( NULL ),
	m_iHoverItem( -1 ),
	m_iModifyCol( -1 ),
	m_pModifyItem( NULL ),
	m_hButton( NULL ),
	m_hCheck( NULL ),
	m_hLock( NULL ),
//	m_hDrop( NULL ),
//	m_hStop( NULL ),
	m_pImageList( NULL ),
	stateImageList_(NULL),
	m_pDragWnd( NULL ),
	m_pDropWnd( NULL ),
	m_pTargetWnd( NULL ),
	m_ptrCustomEditControl(NULL),
	m_dwRequestedCtrlType(TLM_STATIC),
	m_pShiftSelectionBase(NULL)
	, m_ptrCustomDrawNotifyListener(getNullCustomDrawNotifyListener())
	, m_ptrControlCustomDrawInfo(NULL)
	, m_ptrItemCustomDrawInfo(NULL)
	, m_ptrSubitemCustomDrawInfo(NULL)
	, changeItemLabelNotifyListener_(getNullChangeItemLabelNotifyListener())
	, itemDragdropNotifyListener_(getNullItemDragdropNotifyListener())
	, itemChangeNotifyListener_(getNullItemChangeNotifyListener())
	, itemOperationNotifyListener_(getNullItemOperationListener())
	, treeListNotifyListener_(getNullTreeListNotifyListener())
	, generalNotifyListener_(getNullGeneralNotifyListener())
	, readyToEditItem_(NULL, -1)
	, m_bmpBkBitmap(NULL)
	, m_bmpBkDisable(NULL)
	, m_wndStatic(new CTreeListStaticCtrl)
	, m_wndEdit(new CTreeListEditCtrl)
	, m_wndCombo(new CTreeListComboCtrl)
	, m_wndTip(new CTreeListTipCtrl)
	, focusWasLost_(false)
{
	// construct

	// create windows class
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if( !::GetClassInfo(hInst, TREELISTCTRL_CLASSNAME, &wndclass) )
	{
        wndclass.style			= CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
		wndclass.lpfnWndProc	= ::DefWindowProc;
		wndclass.cbClsExtra		= 0;
		wndclass.cbWndExtra		= 0;
		wndclass.hInstance		= hInst;
		wndclass.hIcon			= NULL;
		wndclass.hCursor		= AfxGetApp()->LoadStandardCursor( IDC_ARROW);
		wndclass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		wndclass.lpszMenuName	= NULL;
		wndclass.lpszClassName	= TREELISTCTRL_CLASSNAME;

		if (!AfxRegisterClass(&wndclass))
			AfxThrowResourceException();
	}

	// back ground colors of normal status
	m_crBkColorNonClientArea			= 0xFFFFE0;				
	m_crBkColorSingleColorTree			= 0xFCFCFC;				
	m_crBkColorSingleColor				= 0xF0F0F0;				
	m_crBkColorDoubleColor				= 0xF8F8F8;				

	// back ground colors of disable status
	m_crBkColorDisabledNonClientArea	= 0xC0C080;				
	m_crBkColorDisabledSingleColorTree	= 0xCCCCCC;				
	m_crBkColorDisabledSingleColorTree	= 0xC0C0C0;				
	m_crBkColorDisabledDoubleColor		= 0xC8C8C8;				

	// back ground colors of selected item
	m_crBkColorSeletedItem			= 0xC08080;				
	m_crBkColorSelectedRow		= 0xA00000;				
	m_crBkColorSelectedColumn	= 0xD00000;				
	m_crBkColorSelectedItemNoFocus	= 0xD0A0A0;				

	// text colors
	m_SelectedTextColor		= 0xE8E8E8;				
	m_SelectedRowTextColor		= 0xF0F0F0;				
	m_SelectedColumnTextColor	= 0xFFFFFF;				
	m_SelectedTextNoFocusColor	= 0xE0E0E0;				

	// normal text color
	m_NormalTextColor				= 0x000000;				
	m_HoverTextColor			= 0x00FF00;				

	// grid color
	m_GridColor				= 0xD0D0D0;				
	m_TreeLineColor			= 0x808080;				

	m_Palette.CreateStockObject( DEFAULT_PALETTE );
	m_Font.CreateStockObject( SYSTEM_FIXED_FONT );

//	m_hButton	= AfxGetApp()->LoadCursor( _T("IDC_CURSOR_BUTTON") );
//	m_hCheck	= AfxGetApp()->LoadCursor( _T("IDC_CURSOR_CHECK") );
//	m_hLock		= AfxGetApp()->LoadCursor( _T("IDC_CURSOR_LOCK") );

//	m_hDrop		= AfxGetApp()->LoadCursor( _T("IDC_CURSOR_DROP") );
//	m_hStop		= AfxGetApp()->LoadCursor( _T("IDC_CURSOR_STOP") );
	m_hArrow	= AfxGetApp()->LoadStandardCursor( IDC_ARROW );

	m_pRootItem = new CTreeListItem( 1 );
	m_pRootItem->Expand();
}

CTreeListCtrl::~CTreeListCtrl()
{
	// deconstruct
	m_wndHeader.DestroyWindow();
	m_wndTip->DestroyWindow();

	if( m_Palette.m_hObject != NULL )
		m_Palette.DeleteObject();

	if( m_Font.m_hObject != NULL )
		m_Font.DeleteObject();

//	if( m_hButton != NULL )
//		DestroyCursor( m_hButton );
//
//	if( m_hCheck != NULL )
//		DestroyCursor( m_hCheck );
//
//	if( m_hLock != NULL )
//		DestroyCursor( m_hLock );

	if( m_hArrow != NULL )
		DestroyCursor( m_hArrow );

	while( m_arColumns.GetSize() > 0 )
	{
		INT_PTR nIndex = m_arColumns.GetUpperBound();
		CTreeListColumnInfo* pColumnInfo = 
			static_cast<CTreeListColumnInfo*>(m_arColumns[nIndex]);

		m_arColumns.RemoveAt( nIndex );
		delete pColumnInfo;
	}

//	m_bmpBkBitmap.DeleteObject();
//	m_bmpBkDisable.DeleteObject();

	m_imgCheck.DeleteImageList();
	m_imgLock.DeleteImageList();

	delete m_pRootItem;
}

// background color ( none client area )
COLORREF CTreeListCtrl::getBkColorNonClientArea() const{
	return m_crBkColorNonClientArea;
}
// background color ( single color tree )
COLORREF CTreeListCtrl::getBkColorSingleColorTree() const{
	return m_crBkColorSingleColorTree;
}
COLORREF CTreeListCtrl::getBkColorSingleColor() const{
	return m_crBkColorSingleColor;
}
COLORREF CTreeListCtrl::getBkColorDoubleColor() const{
	return m_crBkColorDoubleColor;
}
// back ground colors of disable status
// background color ( none client area )
COLORREF CTreeListCtrl::getBkColorDisabledNonClientArea() const{
	return m_crBkColorDisabledNonClientArea;
}
// background color ( single color tree )
COLORREF CTreeListCtrl::getBkColorDisabledSingleColorTree() const{
	return m_crBkColorDisabledSingleColorTree;
}
// background color ( single color )
COLORREF CTreeListCtrl::getBkColorDisabledSingleColor() const{
	return m_crBkColorDisabledSingleColor;
}
// background color ( double color )
COLORREF CTreeListCtrl::getBkColorDisabledDoubleColor() const{
	return m_crBkColorDisabledDoubleColor;
}

// back ground colors of selected item
// background color of selected
COLORREF CTreeListCtrl::getBkColorSelectedItem() const{
	return m_crBkColorSeletedItem;
}
COLORREF CTreeListCtrl::getBkColorSelectedRow() const{
	return m_crBkColorSelectedRow;
}
// background color of current selected column
COLORREF CTreeListCtrl::getBkColorSelectedColumn() const{
	return m_crBkColorSelectedColumn;
}
// background color of no focus selected row
COLORREF CTreeListCtrl::getBkColorSelectedItemNoFocus() const{
	return m_crBkColorSelectedItemNoFocus;
}

// text colors
//! color of selected text
COLORREF CTreeListCtrl::getSelectedTextColor() const{
	return m_SelectedTextColor;
}
//! color of current selected row text
COLORREF CTreeListCtrl::getSelectedRowTextColor() const{
	return m_SelectedRowTextColor;
}
//! color of current selected column text
COLORREF CTreeListCtrl::getSelectedColumnTextColor() const{
	return m_SelectedColumnTextColor;
}
//! color of no focus selected text
COLORREF CTreeListCtrl::getSelectedTextNoFocusColor() const{
	return m_SelectedTextNoFocusColor;
}
//! color of text
COLORREF CTreeListCtrl::getNormalTextColor() const{
	return m_NormalTextColor;
}
//! color of hover text
COLORREF CTreeListCtrl::getHoverTextColor() const{
	return m_HoverTextColor;
}

ICustomDrawNotifyListener* CTreeListCtrl::SetCustomDrawNotifyListener(
											ICustomDrawNotifyListener* pcdnl)
{
	ICustomDrawNotifyListener* const old = GetCustomDrawNotifyListener();
	m_ptrCustomDrawNotifyListener = pcdnl;
	if (!m_ptrCustomDrawNotifyListener) 
		m_ptrCustomDrawNotifyListener = getNullCustomDrawNotifyListener();
	return old;
}

ICustomDrawNotifyListener* CTreeListCtrl::GetCustomDrawNotifyListener() const{
	return m_ptrCustomDrawNotifyListener;
}
void CTreeListCtrl::SetControlCustomDrawInfo(CONTROL_CUSTOM_DRAW_INFO* pccdi) const{
	m_ptrControlCustomDrawInfo = pccdi;
}

void CTreeListCtrl::setChangeItemLabelNotifyListener(
									IChangeItemLabelNotifyListener* plistener)
{
	if (plistener)
		changeItemLabelNotifyListener_ = plistener;
	else
		changeItemLabelNotifyListener_ = getNullChangeItemLabelNotifyListener();
}

void CTreeListCtrl::setItemChangeNotifyListener(IItemChangeNotifyListener* listener){
	itemChangeNotifyListener_ = listener;
	if (!itemChangeNotifyListener_)
		itemChangeNotifyListener_ = getNullItemChangeNotifyListener();
}

IItemChangeNotifyListener* CTreeListCtrl::getItemChangeNotifyListener() const{
	return itemChangeNotifyListener_;
}

void CTreeListCtrl::setItemOperationNotifyListener(
										IItemOperationNotifyListener* listener){
	itemOperationNotifyListener_ = listener;
	if (!itemOperationNotifyListener_)
		itemOperationNotifyListener_ = getNullItemOperationListener();
}

IItemOperationNotifyListener* CTreeListCtrl::getItemOperationNotifyListener() const{
	return itemOperationNotifyListener_;
}

IChangeItemLabelNotifyListener* CTreeListCtrl::getChangeItemLabelNotifiListener() const{
	return changeItemLabelNotifyListener_;
}

void CTreeListCtrl::setItemDragdropNotifyListener(IItemDragdropNotifyListener* listener){
	itemDragdropNotifyListener_ = listener;
	if (!itemDragdropNotifyListener_)
		itemDragdropNotifyListener_ = getNullItemDragdropNotifyListener();
}

IItemDragdropNotifyListener* CTreeListCtrl::getItemDragdropNotifyListener() const{
	return itemDragdropNotifyListener_;
}

void CTreeListCtrl::setTreeListNotifyListener(ITreeListNotifyListener* listener)
{
	if (listener)
		treeListNotifyListener_ = listener;
	else
		treeListNotifyListener_ = getNullTreeListNotifyListener();
}

ITreeListNotifyListener* CTreeListCtrl::getTreeListNotifyListener() const{
	return treeListNotifyListener_;
}

void CTreeListCtrl::setGeneralNotifyListener(IGeneralNotifyListener* listener){
	if (listener)
		generalNotifyListener_ = listener;
	else
		generalNotifyListener_ = getNullGeneralNotifyListener();
}
IGeneralNotifyListener* CTreeListCtrl::getGeneralNotifyListener() const{
	return generalNotifyListener_;
}


CONTROL_CUSTOM_DRAW_INFO const* CTreeListCtrl::GetControlCustomDrawInfo() const{
	return m_ptrControlCustomDrawInfo;
}
void CTreeListCtrl::SetItemCustomDrawInfo(ITEM_CUSTOM_DRAW_INFO* picdi) const{
	m_ptrItemCustomDrawInfo = picdi;
}
ITEM_CUSTOM_DRAW_INFO const* CTreeListCtrl::GetItemCustomDrawInfo() const{
	return m_ptrItemCustomDrawInfo;
}
void CTreeListCtrl::SetSubitemCustomDrawInfo(SUBITEM_CUSTOM_DRAW_INFO* pscdi) const{
	m_ptrSubitemCustomDrawInfo = pscdi;
}
SUBITEM_CUSTOM_DRAW_INFO const* CTreeListCtrl::GetSubitemCustomDrawInfo() const{
	return m_ptrSubitemCustomDrawInfo;
}

// grid colors
// color of grid
COLORREF CTreeListCtrl::getGridColor() const{
	return m_GridColor;
}
// color of tree line
COLORREF CTreeListCtrl::getTreeLineColor() const{
	return m_TreeLineColor;
}

void	CTreeListCtrl::SetBackBitmap(CBitmap* bitmap){
	if (bitmap)
		m_bmpBkBitmap = bitmap;
	else
		m_bmpBkBitmap = treelist_res::GetBackgroundBitmap();
}
CBitmap const* CTreeListCtrl::GetBackBitmap() const{
	return m_bmpBkBitmap;
}

void	CTreeListCtrl::SetDisabledBackBitmap(CBitmap* bitmap){
	if (bitmap)
		m_bmpBkDisable = bitmap;
	else
		m_bmpBkDisable = treelist_res::GetBackgroundBitmap2();
}

CBitmap const* CTreeListCtrl::GetDisabledBackBitmap() const{
	return m_bmpBkDisable;
}

BEGIN_MESSAGE_MAP(CTreeListCtrl, CWnd)
	//{{AFX_MSG_MAP(CTreeListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CANCELMODE()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl message handler
bool CTreeListCtrl::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	// create control
	SetState( TLS_SUBCLASSFROMCREATE );
	DWORD dwExStyle = 0;

	if( dwStyle&WS_BORDER )
	{
		dwStyle &= ~WS_BORDER;
		dwExStyle |= WS_EX_CLIENTEDGE;
	}
	if( !CWnd::CreateEx( dwExStyle, TREELISTCTRL_CLASSNAME, NULL, 
						dwStyle, rect, pParentWnd, nID ) )
		return FALSE;

	if( !Create() )
		return FALSE;

	return TRUE;
}

bool CTreeListCtrl::Create()
{
	// create control
	CRect Rect( 0, 0, 0, 0);

	if( !m_wndHeader.Create( 0, Rect, this, 0 ) )
		return FALSE;

	if( !m_wndTip->Create( this ) )
		return FALSE;

	DWORD dwStyle;

	dwStyle = WS_BORDER | WS_CHILD | SS_NOTIFY | SS_CENTERIMAGE;
	if( !m_wndStatic->Create( _T(""), dwStyle, Rect, this, 0 ) )
		return FALSE;

	dwStyle = WS_BORDER | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL;

	if( !m_wndEdit->Create( dwStyle, Rect, this, 0 ) )
		return FALSE;

	dwStyle = WS_CHILD | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT;
	dwStyle |= WS_VSCROLL;

	if( !m_wndCombo->Create( dwStyle, Rect, this, 0 ) )
		return FALSE;


	m_bmpBkBitmap = treelist_res::GetBackgroundBitmap();
	m_bmpBkDisable = treelist_res::GetBackgroundBitmap2();

	if( !m_imgCheck.Create(16, 16, ILC_MASK|ILC_COLOR24, 3, 3))
		return FALSE;

	if (m_imgCheck.Add(treelist_res::GetCheckboxBitmap(), 0xFF00FF) == -1)
		return FALSE;

	ASSERT(m_imgCheck.GetImageCount() == 3);

	if( !m_imgLock.Create( 8, 16, ILC_MASK|ILC_COLOR24, 0, 3 ) )
		return FALSE;

	if (m_imgLock.Add(treelist_res::GetLockboxBitmap(), 0xFF00FF) == -1)
		return FALSE;
	ASSERT(m_imgLock.GetImageCount() == 3);


	SetFont();
	SetItemHeight();
	
	m_wndHeader.SetHeaderFont();
	m_wndHeader.SetHeaderHeight();
	m_wndTip->SetFont();
	
	m_wndStatic->SetFont( GetFont() );
	m_wndEdit->SetFont( GetFont() );
	m_wndCombo->SetFont( GetFont() );

	Layout();

	return TRUE;
}

void CTreeListCtrl::PreSubclassWindow() 
{
	// pre subclass window
	CWnd::PreSubclassWindow();

	if( !GetState( TLS_SUBCLASSFROMCREATE ) )
	{
		if( !Create() )
		{
			AfxThrowMemoryException();
			return;
		}
	}

	return;
}

void CTreeListCtrl::OnDestroy() 
{
	CWnd::OnDestroy();
	
	m_arShows.RemoveAll();
	m_arSorts.RemoveAll();

	DeleteItem( TLI_ROOT );
}

void CTreeListCtrl::SetAllScroll()
{
	// set all scroll bars
	CRect rcPreClient;
	GetWindowRect( &rcPreClient );
	rcPreClient.DeflateRect( 1, 1 );

	int cx = ::GetSystemMetrics( SM_CXVSCROLL );
	int cy = ::GetSystemMetrics( SM_CYHSCROLL );

	SCROLLINFO siv;
	SCROLLINFO sih;

	GetScrollInfo( SB_VERT, &siv, SIF_POS | SIF_RANGE );
	GetScrollInfo( SB_HORZ, &sih, SIF_POS | SIF_RANGE );

	bool bHorzScroll = FALSE;
	bool bVertScroll = FALSE;

	if( rcPreClient.Width() < GetWidth() )
	{
		bHorzScroll = TRUE;

		if( rcPreClient.Height() - cy < m_wndHeader.GetHeaderHeight() + GetVisibleCount() * GetItemHeight() )
			bVertScroll = TRUE;
	}
	else if( rcPreClient.Height() < m_wndHeader.GetHeaderHeight() + GetVisibleCount() * GetItemHeight() )
	{
		bVertScroll = TRUE;

		if( rcPreClient.Width() - cx < GetWidth() )
			bHorzScroll = TRUE;
	}

	if( sih.nPos != sih.nMin )
		bHorzScroll = TRUE;

	if( siv.nPos != siv.nMin )
		bVertScroll = TRUE;

	if( bHorzScroll )
	{
		if( rcPreClient.Width() > cx )
			rcPreClient.right -= cx;
		else
			rcPreClient.right = rcPreClient.left;
	}

	if( bVertScroll )
	{
		if( rcPreClient.Height() > cy )
			rcPreClient.bottom -= cy;
		else
			rcPreClient.bottom = rcPreClient.top;
	}

	SetHorzScroll( &rcPreClient );
	SetVertScroll( &rcPreClient );
}

void CTreeListCtrl::SetHorzScroll( CRect* pPreClient )
{
	// set horizonal scroll
	SCROLLINFO si;
	GetScrollInfo( SB_HORZ, &si, SIF_ALL );

	if( pPreClient->Width() < GetWidth() )
	{
		si.cbSize	= sizeof( SCROLLINFO );
		si.nMin		= 0;
		si.nMax		= GetWidth() - 1;
		si.nPage	= pPreClient->Width();
		si.nPos		= si.nPos;
		si.fMask	=  SIF_PAGE | SIF_RANGE | SIF_POS;
		if( si.nPos > si.nMax - (int)si.nPage )
			si.nPos = si.nMax - (int)si.nPage;
	}
	else
	{
		si.cbSize	= sizeof( SCROLLINFO );
		si.nMin		= 0;
		si.nMax		= 0;
		si.nPage	= 0;
		si.nPos		= 0;
		si.fMask	=  SIF_PAGE | SIF_RANGE | SIF_POS;
	}
	
	SetScrollInfo( SB_HORZ, &si, TRUE );
}

void CTreeListCtrl::SetVertScroll( CRect* pPreClient )
{
	// set vertical scroll
	SCROLLINFO si;
	GetScrollInfo( SB_VERT, &si, SIF_ALL );

	if( pPreClient->Height() < m_wndHeader.GetHeaderHeight() 
								+ GetVisibleCount() * GetItemHeight() 
		|| si.nPos != si.nMin )
	{
		int nShowRows	=	m_rcTreeList.Height() / GetItemHeight();

		si.cbSize	= sizeof( SCROLLINFO );
		si.nMin		= 0;
		si.nMax		= GetVisibleCount() - 1;
		si.nPos		= si.nPos;
		si.nPage	= max( nShowRows, 1 ); // min( pPreClient->Height()/m_nRowHeight, si.nMax -si.nPos );
		si.fMask	=  SIF_PAGE | SIF_RANGE | SIF_POS;
		if( si.nPos > si.nMax )
			si.nPos = si.nMax;
	}
	else
	{
		si.cbSize	= sizeof( SCROLLINFO );
		si.nMin		= 0;
		si.nMax		= 0;
		si.nPage	= 0;
		si.nPos		= 0;
		si.fMask	=  SIF_PAGE | SIF_RANGE | SIF_POS;
	}

	SetScrollInfo( SB_VERT, &si, TRUE );
}

void CTreeListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// on horizonal scroll
	SCROLLINFO si;
	GetScrollInfo( SB_HORZ, &si, SIF_ALL );

	switch( nSBCode )
	{
	case SB_LINEUP:
		if( si.nPos > si.nMin )
			si.nPos--;
		break;

	case SB_LINEDOWN:
		if( si.nPos <= si.nMax - (int)si.nPage )
			si.nPos++;
		break;

	case SB_PAGEUP:
		si.nPos -= si.nPage;
		if( si.nPos < si.nMin )
			si.nPos = si.nMin;
		break;

	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		if( si.nPos > si.nMax )
			si.nPos = si.nMax;
		break;

	case SB_TOP:
		si.nPos = si.nMin;
		break;

	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;

	default:
		break;
	}
	
	SetScrollPos( SB_HORZ, si.nPos );
	m_wndHeader.RedrawWindow();
	RedrawWindow();
}

void CTreeListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// on vertival scroll
	SCROLLINFO si;
	GetScrollInfo( SB_VERT, &si, SIF_ALL );

	switch( nSBCode )
	{
	case SB_LINEUP:
		if( si.nPos > si.nMin )
			si.nPos--;
		break;

	case SB_LINEDOWN:
		if( si.nPos <= si.nMax - (int)si.nPage )
			si.nPos++;;
		break;

	case SB_PAGEUP:
		si.nPos -= si.nPage;
		if( si.nPos < si.nMin )
			si.nPos = si.nMin;
		break;

	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		if( si.nPos > si.nMax )
			si.nPos = si.nMax;
		break;

	case SB_TOP:
		si.nPos = si.nMin;
		break;

	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;

	default:
		break;
	}

	if( si.nPos == si.nMin )
	{
		if(  si.nMax-si.nMin+1 <= m_rcClient.Height()/m_nItemHeight )
		{
			Layout();
		}
	}

	SetScrollPos( SB_VERT, si.nPos );

	Invalidate();
}

void CTreeListCtrl::Layout()
{
	// layout size
	if( !IsWindow( m_wndHeader.GetSafeHwnd() ) )
		return;

	if( !IsWindow( m_wndTip->GetSafeHwnd() ) )
		return;

	m_wndTip->Hide();

	SetAllScroll();

	GetClientRect( &m_rcClient );
	m_rcHeader		= m_rcClient;
	m_rcTreeList	= m_rcClient;

	m_wndHeader.SendMessage( HDM_LAYOUT, (WPARAM)0, (LPARAM)&m_rcHeader );

	if( m_rcHeader.Height() == 0 )
	{
		m_rcTreeList = m_rcClient;
	}
	else if( m_rcHeader.Height() == m_rcClient.Height() )
	{
		m_rcTreeList.top	= m_rcHeader.bottom;
		m_rcTreeList.bottom = m_rcHeader.bottom;
	}
	else
	{
		m_rcTreeList.top	= m_rcHeader.bottom;
	}

	SetAllScroll();

	return;
}
int CTreeListCtrl::GetWidth() const
{
	// retrieve total width
	int nWidth = 0;

	for( int iShow=0; iShow<m_arShows.GetSize(); iShow++ )
	{
		CTreeListColumnInfo* pColumnInfo;
		int iCol;

		iCol = m_arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];

		nWidth += pColumnInfo->m_nWidth;
	}

	return nWidth;
}

int CTreeListCtrl::GetHeight() const
{
	// retrieve total height
	return GetItemHeight() * GetVisibleCount();
}

int CTreeListCtrl::GetColumns() const
{
	// retrieve total number of columns
	return static_cast<int>(m_arColumns.GetSize());
}

int CTreeListCtrl::GetCount() const
{
	// retrieve a count of the items in treelist control
	return m_pRootItem->m_nChild - 1;
}

CImageList* CTreeListCtrl::GetImageList()
{
	// retrieve global point of image list
	return m_pImageList;
}

void CTreeListCtrl::SetImageList( CImageList* pImageList )
{
	// set global point of image list
	ASSERT_VALID( pImageList );
	m_pImageList = pImageList;
	SetItemHeight();
	Invalidate();
}

CImageList*	CTreeListCtrl::GetStateImageList() const{
	return stateImageList_;
}
void CTreeListCtrl::SetStateImageList(CImageList* pImageList)
{
	ASSERT_VALID( pImageList );
	stateImageList_ = pImageList;

	Invalidate();
}

CTreeListItem* CTreeListCtrl::GetNextItem( CTreeListItem const* pItem, UINT nCode ) const
{
	// retrieve next item specified by pItem
	CTreeListItem* pRetItem = NULL;

	switch( nCode )
	{
	case TLGN_ROOT:				pRetItem = GetRootItem();					break;
	case TLGN_NEXT:				pRetItem = GetNextSiblingItem( pItem );		break;
	case TLGN_PREVIOUS:			pRetItem = GetPrevSiblingItem( pItem );		break;
	case TLGN_PARENT:			pRetItem = GetParentItem( pItem );			break;
	case TLGN_CHILD:			pRetItem = GetChildItem( pItem );			break;
	case TLGN_FIRSTVISIBLE:		pRetItem = GetFirstVisibleItem();			break;
	case TLGN_NEXTVISIBLE:		pRetItem = GetNextVisibleItem( pItem );		break;
	case TLGN_PREVIOUSVISIBLE:	pRetItem = GetPrevVisibleItem( pItem );		break;
	case TLGN_DROPHILITE:		pRetItem = NULL;							break;
	case TLGN_CARET:			pRetItem = GetSelectedItem();				break;
	case TLGN_NEXTSELECT:		pRetItem = GetNextSelectedItem( pItem );	break;
	default:					pRetItem = NULL;							break;
	}

	return pRetItem;
}

bool CTreeListCtrl::ItemHasChildren( CTreeListItem const* pItem ) const
{
	// determine whether the treelist item specified by pItem has a child item
	ASSERT( pItem != NULL );

	if( pItem->m_pChild == NULL )
		return FALSE;

	return TRUE;
}

CTreeListItem* CTreeListCtrl::GetChildItem( CTreeListItem const* pItem ) const
{
	// retrieve first child item specified by pItem
	ASSERT( pItem != NULL );
	if (pItem == TLI_ROOT)
		return m_pRootItem->m_pChild;
	return pItem->m_pChild;
}

CTreeListItem* CTreeListCtrl::GetNextSiblingItem( CTreeListItem const* pItem ) const
{
	// retrieve next sibling item specified by pItem
	ASSERT( pItem != NULL );
	return pItem->m_pNext;
}

CTreeListItem* CTreeListCtrl::GetPrevSiblingItem( CTreeListItem const* pItem ) const
{
	// retrieve prev sibling item specified by pItem
	ASSERT( pItem != NULL );
	return pItem->m_pPrev;
}

CTreeListItem* CTreeListCtrl::GetParentItem( CTreeListItem const* pItem ) const
{
	// retrieve parent item specified by pItem
	ASSERT( pItem != NULL );
	if( pItem->m_pParent == m_pRootItem )
		return NULL;
	else
		return pItem->m_pParent;
}

CTreeListItem* CTreeListCtrl::GetFirstVisibleItem() const
{
	// retrieve first visible item of treelist
	if( m_pRootItem->m_pChild != NULL )
		return m_pRootItem->m_pChild;

	return NULL;
}

CTreeListItem* CTreeListCtrl::GetNextVisibleItem( CTreeListItem const* pItem ) const
{
	// retrieve next visible item of treelist
	ASSERT( pItem != NULL );

	return GetNextVisibleItem( pItem, TRUE );
}

CTreeListItem* CTreeListCtrl::GetNextVisibleItem( CTreeListItem const* pItem, 
												 bool bChild ) const
{
	// retrieve next visible item of treelist
	ASSERT( pItem != NULL );

	if( pItem->GetState()&TLIS_EXPANDED )
		if( bChild && pItem->m_pChild != NULL )
			return pItem->m_pChild;

	if( pItem->m_pNext != NULL )
		return pItem->m_pNext;

	if( pItem != m_pRootItem )
	{
		pItem = pItem->m_pParent;
		if( pItem != m_pRootItem )
			return GetNextVisibleItem( pItem, FALSE );
	}

	return NULL;
}

CTreeListItem* CTreeListCtrl::GetPrevVisibleItem( CTreeListItem const* pItem ) const
{
	// retrieve previous visible item of treelist
	ASSERT( pItem != NULL );

	return GetPrevVisibleItem( pItem, TRUE );
}

CTreeListItem* CTreeListCtrl::GetPrevVisibleItem( CTreeListItem const* pItem, 
												 bool bChild ) const
{
	// retrieve previous visible item of treelist
	ASSERT( pItem != NULL );
	CTreeListItem *result = NULL;
	if( pItem->m_pPrev != NULL )
	{
		result = pItem->m_pPrev;
		if( result->GetState()&TLIS_EXPANDED && result->m_pChild != NULL )
		{
			result = GetLastVisibleItem( result );
		}
		return result;
	}

	result = pItem->m_pParent;
	if( result != m_pRootItem )
		return result;

	return NULL;
}

CTreeListItem* CTreeListCtrl::GetLastVisibleItem( CTreeListItem const* pItem ) const
{
	// retrieve last visible child item of item specified by pItem
	ASSERT( pItem != NULL && pItem->m_pChild != NULL );

	CTreeListItem* pChild = pItem->m_pChild;

	while( pChild->m_pNext != NULL )
	{
		pChild = pChild->m_pNext;
	}

	if( pChild->GetState()&TLIS_EXPANDED && pChild->m_pChild != NULL  )
		return GetLastVisibleItem( pChild );
	else
		return pChild;
}


CTreeListItem* CTreeListCtrl::GetSelectedItem() const
{
	// always retrieve first selected item
	POSITION pos = GetFirstSelectedItemPosition();
	return GetNextSelectedItem( pos );
}

CTreeListItem* CTreeListCtrl::GetNextSelectedItem( CTreeListItem const* pItem ) const
{
	// retrieve next selected item specified by pItem
	if( pItem == NULL )
		return NULL;

	if( pItem == m_pRootItem && m_arSelects.GetSize() > 0 )
		return (CTreeListItem*)m_arSelects[0];

	for( int iSel = 0; iSel<m_arSelects.GetSize()-1; iSel++ )
	{
		if( pItem == (CTreeListItem*)m_arSelects[iSel] )
			return (CTreeListItem*)m_arSelects[iSel+1];
	}

	return NULL;
}

CTreeListItem* CTreeListCtrl::GetRootItem() const
{
	// retrieve root item of treelist control
	return m_pRootItem->m_pChild;
}

DWORD CTreeListCtrl::GetItemState( CTreeListItem const* pItem, 
								  DWORD nStateMask ) const
{
	// retrieve status of item specified by pItem
	ASSERT( pItem != NULL );
	return pItem->GetState( nStateMask );
}

void CTreeListCtrl::SetItemState( CTreeListItem* pItem, 
								 DWORD dwAddState, 
								 DWORD dwRemoveState )
{
	// set status of item specified by pItem
	ASSERT( pItem != NULL );
	pItem->SetState( dwAddState, dwRemoveState );
	Invalidate();
}

DWORD CTreeListCtrl::GetItemStateImage(CTreeListItem const* item) const{
	return item->GetStateImage();
}

void CTreeListCtrl::SetItemStateImage(CTreeListItem* item, DWORD index){
	item->SetStateImage(index);
}

void CTreeListCtrl::GetItemImage( CTreeListItem const* pItem, 
								 int& nImage, 
								 int& nExpandImage, 
								 int& nSelectedImage, 
								 int& nExpandSelectedImage ) const
{
	// retrieve images of item specified by pItem
	ASSERT( pItem != NULL );
	pItem->GetImage( nImage, nExpandImage, nSelectedImage, nExpandSelectedImage );
}

void CTreeListCtrl::SetItemImage( CTreeListItem* pItem, 
								 int nImage, 
								 int nExpandImage, 
								 int nSelectedImage, 
								 int nExpandSelectedImage ) 
{
	// set images of item specified by pItem
	ASSERT( pItem != NULL );
	pItem->SetState( TLIS_SHOWTREEIMAGE );
	pItem->SetImage( nImage, nExpandImage, nSelectedImage, nExpandSelectedImage );
	Invalidate();
}	

int CTreeListCtrl::GetItemImage( CTreeListItem const* pItem) const
{
	// retrieve images of item specified by pItem
	ASSERT( pItem != NULL );
	return pItem->GetImage();
}

void CTreeListCtrl::SetItemImage( CTreeListItem* pItem, int nImage) 
{
	// set images of item specified by pItem
	ASSERT( pItem != NULL );
	if (nImage != -1)
		pItem->SetState( TLIS_SHOWTREEIMAGE );
	else
		pItem->SetState( 0, TLIS_SHOWTREEIMAGE );
	pItem->SetImage( nImage);
	Invalidate();
}	

LPCTSTR	CTreeListCtrl::GetItemText( CTreeListItem const* pItem, int nSubItem ) const
{
	// retrieve text of item specified by pItem
	ASSERT( pItem != NULL );
	return pItem->GetText( nSubItem );
}

bool CTreeListCtrl::SetItemText( CTreeListItem* pItem, 
								int nSubItem, 
								LPCTSTR lpszText) 
{
	// set text of item specified by pItem
	ASSERT( pItem != NULL );
	bool bRet = pItem->SetText( lpszText, nSubItem );
	Invalidate();
	return bRet;
}

DWORD_PTR CTreeListCtrl::GetItemData( CTreeListItem const* pItem ) const
{
	// retrieve 32-bit data of item specified by pItem
	ASSERT( pItem != NULL );
	return pItem->GetData();
}

void CTreeListCtrl::SetItemData( CTreeListItem* pItem, DWORD_PTR dwData )
{
	// set 32-bit data of item specified by pItem
	ASSERT( pItem != NULL );
	pItem->SetData( dwData );
}

int CTreeListCtrl::GetVisibleCount() const
{
	// retrieve the  number of visible items in the treelist control
	return GetVisibleCount( m_pRootItem );
}

int CTreeListCtrl::GetItemHeight() const
{
	// retrieve the height of the item
	return m_nItemHeight;
}

int CTreeListCtrl::SetItemHeight( int cyHeight )
{
	// set the height of the item
	int nOldHeight = m_nItemHeight;

	if( cyHeight == -1 )
	{
		ASSERT( m_Font.m_hObject != NULL );

		LOGFONT lf;
		m_Font.GetLogFont( &lf );
	
		if( lf.lfHeight < 0 )
		{
			m_nItemHeight = -lf.lfHeight + 5;
		}
		else if( lf.lfHeight == 0 )
		{
			m_nItemHeight = DEFAULT_HEIGHT;
		}
		else
		{
			m_nItemHeight = lf.lfHeight + 5;
		}
		if (m_pImageList)
		{
			CRect const r(GetImageListImageRect(m_pImageList));
			if (m_nItemHeight <= r.Height())
				m_nItemHeight = r.Height()+1;
		}
	}
	else
	{
		m_nItemHeight = cyHeight;
	}

	if( m_nItemHeight % 2 == 1 )
		m_nItemHeight ++;

	Invalidate();

	return nOldHeight;
}

COLORREF CTreeListCtrl::GetBkColor( int nColor )
{
	// set background color of treelist control
	COLORREF clr;

	switch( nColor )
	{
	case 0:		clr = getBkColorSingleColorTree();	break;
	case 1:		clr = getBkColorSingleColor();		break;
	case 2:		clr = getBkColorDoubleColor();		break;
	default:	clr = getBkColorNonClientArea();	break;
	}

	return clr;
}

COLORREF CTreeListCtrl::SetBkColor( COLORREF clr, int nColor )
{
	// retrieve background color of treelist control
	COLORREF oldclr = 0;

	switch( nColor )
	{
	case 0:		
		oldclr = getBkColorSingleColorTree();		
		m_crBkColorSingleColorTree = clr;		
		break;
	case 1:		
		oldclr = getBkColorSingleColor();
		m_crBkColorSingleColor = clr;		
		break;
	case 2:		
		oldclr = getBkColorDoubleColor();		
		m_crBkColorDoubleColor = clr;		
		break;
	default:	
		oldclr = getBkColorNonClientArea();
		m_crBkColorNonClientArea = clr;		
		break;
	}

	Invalidate();
	return oldclr;
}

COLORREF CTreeListCtrl::GetTextColor()
{
	// retrieve the color of the text
	return m_NormalTextColor;
}

COLORREF CTreeListCtrl::SetTextColor( COLORREF clr )
{
	// set the color of the text
	COLORREF oldclr;
	oldclr = getNormalTextColor();
	m_NormalTextColor = clr;

	Invalidate();

	return oldclr;
}

bool CTreeListCtrl::IsItemImageVisible(CTreeListItem const* pItem) const{
	return pItem->IsShowTreeImage();
}

void CTreeListCtrl::ShowItemImage(CTreeListItem *pItem, bool bShow /*= true*/) {
	pItem->ShowTreeImage(bShow);
	Invalidate();
}

bool CTreeListCtrl::IsItemCheckBoxVisible(CTreeListItem const* pItem) const{
	return pItem->IsShowCheckBox();
}

void CTreeListCtrl::ShowItemCheckBox(CTreeListItem *pItem, bool bShow /*= true*/){
	pItem->ShowCheckBox(bShow);
	Invalidate();
}

bool CTreeListCtrl::IsItemLockBoxVisible(CTreeListItem const* pItem) const{
	return pItem->IsShowLockBox();
}

void CTreeListCtrl::ShowItemLockBox(CTreeListItem *pItem, bool bShow /*= true*/)
{
	pItem->ShowLockBox(bShow);
	Invalidate();
}


bool CTreeListCtrl::GetItemRect( CTreeListItem const* pItem, LPRECT lpRect ) const
{
	// retrieve rect of an item
	int nFirstRow	=	GetScrollPos( SB_VERT );
	int	nShowRows	=	m_rcTreeList.Height() / GetItemHeight() + 1;

	CRect rcItem( 0, 0, GetWidth(), GetItemHeight() );
	rcItem.OffsetRect( m_rcTreeList.left, m_rcTreeList.top );
	rcItem.OffsetRect( -GetScrollPos( SB_HORZ ), 0 );

	for( int i = nFirstRow; 
		i<= nFirstRow + nShowRows && i < GetVisibleCount(); 
		i++ )
	{
		CTreeListItem* pShowItem = GetFirstShowItem( i );
		if( pShowItem == pItem )
		{
			lpRect->left	= rcItem.left;
			lpRect->right	= rcItem.right;
			lpRect->top		= rcItem.top;
			lpRect->bottom	= rcItem.bottom;

//			lpRect->top		+= 1;
//			lpRect->bottom	-= 1;

			return TRUE;
		}

		rcItem.OffsetRect( 0, GetItemHeight() );
	}
	return FALSE;
}

bool CTreeListCtrl::GetItemRect( CTreeListItem const* pItem, 
								int iSubItem, 
								LPRECT lpRect, bool bTextOnly ) const
{
	// retrieve rect of a column of an item
	CRect rcItem;
	
	if( GetItemRect( pItem, rcItem ) )
	{
		int nColPos = 0;
		CTreeListColumnInfo* pColumnInfo = NULL;

		for( int iShow = 0; 
			iShow < m_arShows.GetSize(); 
			iShow++, nColPos += pColumnInfo->GetWidth() )
		{
			int iCol = m_arShows[iShow];
			pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];

			if( iCol == iSubItem )
			{
				CRect rcColumn( 0, rcItem.top, pColumnInfo->GetWidth(), rcItem.bottom );
				rcColumn.OffsetRect( nColPos, 0 );
				rcColumn.OffsetRect( rcItem.left, 0 );

				lpRect->left	= rcColumn.left;
				lpRect->right	= rcColumn.right;
				lpRect->top		= rcColumn.top;
				lpRect->bottom	= rcColumn.bottom;

				if( iCol == 0 && bTextOnly )
				{
					if ( m_dwStyle&TLC_TREELIST )
						return GetItemRectTree( pItem, lpRect );
					else
						return GetItemRectMain( pItem, lpRect );

				}

//				lpRect->left	+= 1;
//				lpRect->right	-= 1;

				return TRUE;
			}
		}
	}
	
	return FALSE;
}	

bool CTreeListCtrl::GetItemBkgrndRect(CTreeListItem const* pItem, 
									  CRect const& rcItem, 
									  LPRECT lpRect) const
{
	SetRect(lpRect, 0, rcItem.top, GetWidth(), rcItem.bottom );
	OffsetRect(lpRect, -GetScrollPos( SB_HORZ ), 0 );

	if( GetStyle()&TLC_HGRID )
		lpRect->bottom -= 1;

	if( GetStyle()&TLC_VGRID )
		lpRect->right -= 1;

	return true;
}

bool CTreeListCtrl::GetItemRectTree( CTreeListItem const* pItem, LPRECT lpRect ) const
{
	int nPerfix = 0;

	// skip perfix and tree line
	nPerfix += TLL_BORDER;
	nPerfix += (pItem->m_nLevel-1) * TLL_WIDTH;

	// skip tree button
	if( m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
		nPerfix += TLL_WIDTH;

	// skip tree check box
	if( m_dwStyle&TLC_CHECKBOX )
		if( pItem->GetState()&TLIS_SHOWCHECKBOX )
			nPerfix += TLL_WIDTH;

	if (GetStateImageList()&&pItem->GetStateImage())
	{
		CRect rc = GetImageListImageRect(GetStateImageList());
		nPerfix += rc.Width();
	}

	// skip tree lock box
	if( m_dwStyle&TLC_LOCKBOX )
		if( pItem->GetState()&TLIS_SHOWLOCKBOX )
			nPerfix += TLL_WIDTH/2;

	// skip tree image
	if( m_dwStyle&TLC_IMAGE )
		if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
			nPerfix += TLL_WIDTH;

	// skip gaps between text and image/loc
	if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	if( lpRect->left + nPerfix + 2 <= lpRect->right )
	{
		lpRect->left	+= nPerfix;
//		lpRect->left	+= 1;
//		lpRect->right	-= 1;
		return TRUE;
	}
	
	lpRect->left	+= nPerfix;
	return FALSE;
}

bool CTreeListCtrl::GetItemRectMain( CTreeListItem const* pItem, LPRECT lpRect ) const
{
	int nPerfix = 0;

	// skip perfix
	nPerfix += TLL_BORDER;

	// skip tree button
	if( m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
		nPerfix += TLL_WIDTH;

	// skip tree check box
	if( m_dwStyle&TLC_CHECKBOX )
		if( pItem->GetState()&TLIS_SHOWCHECKBOX )
			nPerfix += TLL_WIDTH;

	if (GetStateImageList()&&pItem->GetStateImage())
	{
		CRect const rc = GetImageListImageRect(GetStateImageList());
		nPerfix += rc.Width();
	}
	// skip tree lock box
	if( m_dwStyle&TLC_LOCKBOX )
		if( pItem->GetState()&TLIS_SHOWLOCKBOX )
			nPerfix += TLL_WIDTH/2;

	// skip tree image
	if( m_dwStyle&TLC_IMAGE )
		if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
			nPerfix += TLL_WIDTH;

	// skip gaps between text and image/loc
	if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	if( lpRect->left + nPerfix + 2 <= lpRect->right )
	{
		lpRect->left	+= nPerfix;
//		lpRect->left	+= 1;
//		lpRect->right	-= 1;
		return TRUE;
	}
	
	lpRect->left += nPerfix;
	return FALSE;
}

bool CTreeListCtrl::EnsureVisible( CTreeListItem const* pItem, int iSubItem )
{
	if (GetStyle()&TLC_TREELIST)
	{
		parentsExpanding(pItem);
	}
	// ensure an item is visible that specified by pItem
	int nItem		=	GetTreeListItem( pItem );
	int nFirstRow	=	GetScrollPos( SB_VERT );
	int nShowRows	=	m_rcTreeList.Height() / GetItemHeight();

	if( nShowRows > 0 )
		nShowRows = nShowRows - 1;

	if( nItem < nFirstRow )
	{
		SetScrollPos( SB_VERT, nItem );
	}
	else
	{
		if( nItem > nFirstRow + nShowRows )
			SetScrollPos( SB_VERT, nItem - nShowRows );
	}

	CRect rcClient;
	GetClientRect( rcClient );
	int	nCurrent	=	GetScrollPos( SB_HORZ );
	int nLeft		=	0;
	int nRight		=	0;
	for( int iShow = 0; iShow < m_arShows.GetSize(); iShow ++ )
	{
		CTreeListColumnInfo* pColumnInfo;
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[m_arShows[iShow]];

		if( m_arShows[iShow] == iSubItem )
		{
			nRight = pColumnInfo->GetWidth();
			break;
		}
		nLeft += pColumnInfo->GetWidth();
	}
	nRight = nLeft + nRight;

	if( iSubItem == 0 )
	{
		int iLevel		= pItem->m_nLevel - 1;
		CTreeListColumnInfo* pColumnInfo;
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[0];
		nLeft += TLL_WIDTH * iLevel;
	}

	if( nLeft <= nCurrent )
	{
		SetScrollPos( SB_HORZ, nLeft );
	}
	else
	{
		if( nCurrent + rcClient.Width() < nRight )
		{
			if( nRight-nLeft <= rcClient.Width() )
			{
				SetScrollPos( SB_HORZ, nRight - rcClient.Width() );
			}
			else
			{
				SetScrollPos( SB_HORZ, nLeft );
			}
		}
	}

	Invalidate();
	return TRUE;
}

int	CTreeListCtrl::GetItemCount() const
{
	// retrieve count of all items.
	return m_pRootItem->m_nChild - 1;
}

int CTreeListCtrl::GetNextItem( int nItem, int nFlags ) const
{
	// retrieve next item specified by nItem
	CTreeListItem* pItem = NULL;
	pItem = GetTreeListItem( nItem );

	if( pItem == NULL )
		return -1;

	if( nFlags&TLNI_ABOVE )
		return GetTreeListItem( GetPrevSiblingItem( pItem ) );

	if( nFlags&TLNI_BELOW )
		return GetTreeListItem( GetNextSiblingItem( pItem ) );

	return -1;
}

POSITION CTreeListCtrl::GetFirstSelectedItemPosition() const
{
	// retrieve position of selected item
	if( m_arSelects.GetSize() == 0 )
		return NULL;

	return reinterpret_cast<POSITION>(m_pRootItem);
}

CTreeListItem* CTreeListCtrl::GetNextSelectedItem( POSITION& pos ) const
{
	// retrieve next selected item specified by pItem
	CTreeListItem * res = GetNextSelectedItem( (CTreeListItem*)pos );
	pos = reinterpret_cast<POSITION>(res);
	return res;
}

int CTreeListCtrl::GetStringWidth( LPCTSTR lpsz )
{
	// retrieve width of string in pixel
	CPaintDC dc(this);

	CFont* pOldFont = dc.SelectObject( GetFont() );
	CSize sz = dc.GetTextExtent( lpsz, 
		static_cast<int>(_tcslen(lpsz)) );
	dc.SelectObject( pOldFont );

	return sz.cx + 6;
}

void CTreeListCtrl::CopyItemText(CTreeListItem* target, CTreeListItem const* source)
{
	const INT_PTR columnsCount = m_arColumns.GetSize();
	for(int i = 0; i < columnsCount; ++i)
		target->SetText(source->GetText(i), i);
}

DWORD CTreeListCtrl::SetColumnFormat( int nCol, DWORD dwAdd, DWORD dwRemove )
{
	// set column fromat
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	DWORD dwFormat = pColumnInfo->SetFormat( dwAdd, dwRemove );

	Invalidate();

	return dwFormat;
}

DWORD CTreeListCtrl::GetColumnFormat( int nCol, DWORD dwMask )
{
	// retrieve column format
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->GetFormat();
}

DWORD CTreeListCtrl::SetColumnModify( int nCol, DWORD dwModify )
{
	// retrieve column modify
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->SetModify( dwModify );
}

DWORD CTreeListCtrl::GetColumnModify( int nCol )
{
	// retrieve column modify
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->GetModify();
}

int CTreeListCtrl::SetColumnWidth( int nCol, int nWidth, int nMin, int nMax )
{
	// set column width
	if( nWidth == TLSCW_AUTOSIZE || nWidth == TLSCW_AUTOSIZE_USEHEADER )
	{
		// AutoSetWidth();		// all show items
		if( nWidth == TLSCW_AUTOSIZE )
		{
			nWidth = nMin;
		}
		else
		{
			nWidth = GetStringWidth( GetColumnText( nCol ) ) + 4;
		}

		CTreeListItem* pShowItem = GetFirstShowItem( 0 );
		while( pShowItem != NULL )
		{
			ASSERT( pShowItem != NULL );

			int x = 0;
			if( nCol == 0 )
			{
				if( m_dwStyle&TLC_TREELIST )
					x += (pShowItem->m_nLevel-1) * TLL_WIDTH;

				if( m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
					x += TLL_WIDTH;

				if( m_dwStyle&TLC_CHECKBOX )
					if( pShowItem->GetState()&TLIS_SHOWCHECKBOX )
						x += TLL_WIDTH;

				if( m_dwStyle&TLC_LOCKBOX )
					if( pShowItem->GetState()&TLIS_SHOWLOCKBOX )
						x += TLL_WIDTH/2;

				if (GetStateImageList()&&pShowItem->GetStateImage())
				{
					CRect const rc = GetImageListImageRect(GetStateImageList());
					x += rc.Width();
				}
				
				if( m_dwStyle&TLC_IMAGE )
					if( pShowItem->GetState()&TLIS_SHOWTREEIMAGE )
						x += TLL_WIDTH;

				if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
					x += 2;
			}

			x += GetStringWidth( pShowItem->GetText( nCol ) );
			x += 2;

			if( nWidth < x )
				nWidth = x;

			pShowItem = GetNextShowItem( pShowItem );
		}
		return SetColumnWidth( nCol, nWidth, nMin, nMax );
	}
	else if( nWidth == TLSCW_AUTOSIZEV || nWidth == TLSCW_AUTOSIZEV_USEHEADER )
	{
		// AutoSetWidth();		// all visible items
		if( nWidth == TLSCW_AUTOSIZEV )
		{
			nWidth = nMin;
		}
		else
		{
			nWidth = GetStringWidth( GetColumnText( nCol ) ) + 4;
		}

		CTreeListItem* pVisibleItem = GetFirstVisibleItem();
		while( pVisibleItem != NULL )
		{
			ASSERT( pVisibleItem != NULL );
			int x = 0;
			if( nCol == 0 )
			{
				if( m_dwStyle&TLC_TREELIST )
					x += (pVisibleItem->m_nLevel-1) * TLL_WIDTH;

				if( m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
					x += TLL_WIDTH;

				if( m_dwStyle&TLC_CHECKBOX )
					if( pVisibleItem->GetState()&TLIS_SHOWCHECKBOX )
						x += TLL_WIDTH;

				if( m_dwStyle&TLC_LOCKBOX )
					if( pVisibleItem->GetState()&TLIS_SHOWLOCKBOX )
						x += TLL_WIDTH/2;

				if (GetStateImageList()&&pVisibleItem->GetStateImage())
				{
					CRect const rc = GetImageListImageRect(GetStateImageList());
					x += rc.Width();
				}

				if( m_dwStyle&TLC_IMAGE )
					if( pVisibleItem->GetState()&TLIS_SHOWTREEIMAGE )
						x += TLL_WIDTH;

				if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
					x += 2;
			}

			x += GetStringWidth( pVisibleItem->GetText( nCol ) );
			x += 2;

			if( nWidth < x )
				nWidth = x;

			pVisibleItem = GetNextShowItem( pVisibleItem );
		}
		return SetColumnWidth( nCol, nWidth, nMin, nMax );
	}
	else
	{
		// SetWidth()
		ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

		CTreeListColumnInfo* pColumnInfo;
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

		int iRet = pColumnInfo->SetWidth( nWidth, nMin, nMax );
		Layout();
		Invalidate();
		return iRet;
	}
}

int CTreeListCtrl::GetColumnWidth( int nCol )
{
	// retrieve column width
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->GetWidth();
}

int CTreeListCtrl::SetColumnImage( int nCol, int iImage )
{
	// set index of column image
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	int iRet = pColumnInfo->SetImage( iImage );

	Invalidate();

	return iRet;
}

int CTreeListCtrl::GetColumnImage( int nCol )
{
	// retrieve index of column image
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->GetImage();
}

bool CTreeListCtrl::SetColumnText( int nCol, LPCTSTR lpszText )
{
	// set text of column
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	bool bRet = pColumnInfo->SetText( lpszText );

	Invalidate();

	return bRet;
}

LPCTSTR CTreeListCtrl::GetColumnText( int nCol ) const
{
	// retrieve text of column
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->GetText();
}

bool CTreeListCtrl::SetColumnDefaultText( int nCol, LPCTSTR lpszText )
{
	// set default text of column
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->SetDefaultText( lpszText );
}

LPCTSTR CTreeListCtrl::GetColumnDefaultText( int nCol )
{
	// retrieve default text of column
	ASSERT( 0 <= nCol && nCol < m_arColumns.GetSize() );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[nCol];

	return pColumnInfo->GetDefaultText();
}

DWORD CTreeListCtrl::GetItemState( int nItem, DWORD nStateMask ) const
{
	// retrieve item state ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	return GetItemState( GetTreeListItem(nItem), nStateMask );
}

void CTreeListCtrl::SetItemState( int nItem, DWORD dwAddState, DWORD dwRemoveState )
{
	// set item state ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	SetItemState( GetTreeListItem(nItem), dwAddState, dwRemoveState );
}

void CTreeListCtrl::GetItemImage( int nItem, 
								 int& nImage, 
								 int& nExpandImage, 
								 int& nSelectedImage, 
								 int& nExpandSelectedImage ) const
{
	// retrieve index of item image ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	GetItemImage( GetTreeListItem(nItem), nImage, 
		nExpandImage, nSelectedImage, nExpandSelectedImage );
}

void CTreeListCtrl::SetItemImage( int nItem, 
								 int nImage, 
								 int nExpandImage, 
								 int nSelectedImage, 
								 int nExpandSelectedImage )
{
	// set index of item image ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	SetItemImage( GetTreeListItem(nItem), nImage, 
		nExpandImage, nSelectedImage, nExpandSelectedImage );
}

int CTreeListCtrl::GetItemImage( int nItem ) const
{
	// retrieve index of item image ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	return GetItemImage( GetTreeListItem(nItem));
}

void CTreeListCtrl::SetItemImage( int nItem, int nImage)
{
	// set index of item image ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	SetItemImage( GetTreeListItem(nItem), nImage);
}

LPCTSTR CTreeListCtrl::GetItemText( int nItem, int nSubItem ) const
{
	// retrieve text of sub item ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	return GetItemText( GetTreeListItem(nItem), nSubItem );
}

bool CTreeListCtrl::SetItemText( int nItem, int nSubItem, LPCTSTR lpszText ) 
{
	// set text of sub item ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	return SetItemText( GetTreeListItem(nItem), nSubItem, lpszText );
}

DWORD_PTR CTreeListCtrl::GetItemData( int nItem ) const
{
	// retrieve data of item ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	return GetItemData( GetTreeListItem(nItem) );
}

void CTreeListCtrl::SetItemData( int nItem, DWORD_PTR dwData )
{
	// set data of item ( list )
	ASSERT( 0 <= nItem && nItem < GetCount() );
	SetItemData( GetTreeListItem(nItem), dwData );
}

BOOL CTreeListCtrl::GetViewRect( LPRECT lpRect )
{
	// retrieve list rect ( list )
	lpRect->left	= m_rcTreeList.left;
	lpRect->right	= m_rcTreeList.right;
	lpRect->top		= m_rcTreeList.top;
	lpRect->bottom	= m_rcTreeList.bottom;

	return m_rcTreeList.IsRectEmpty();
}

int CTreeListCtrl::GetTopIndex()
{
	// retrieve index of first visible item ( list )
	return GetScrollPos( SB_VERT );
}

int CTreeListCtrl::GetCountPerPage()
{
	// retrieve rows of visible item of one page ( list )
	return m_rcTreeList.Height() / GetItemHeight() + 1;
}

bool CTreeListCtrl::GetOrigin( LPPOINT lpPoint )
{
	// retrieve origin point of a list ( list )
	lpPoint->x = m_rcTreeList.left;
	lpPoint->y = m_rcTreeList.top;
	return TRUE;
}

UINT CTreeListCtrl::GetSelectedCount()
{
	// retrieve count of selected item ( list )
	return static_cast<UINT>(m_arSelects.GetSize());
}

bool CTreeListCtrl::SetColumnOrderArray( int iCount, LPINT piArray )
{
	// set show order of columns
	m_arShows.RemoveAll();

	for( int iShow=0; iShow<iCount; iShow++ )
		m_arShows.Add( piArray[iShow] );

	Invalidate();

	return TRUE;
}

bool CTreeListCtrl::GetColumnOrderArray( LPINT piArray, int iCount )
{
	// retrieve column show order
	if( iCount <= 0 )
		iCount = static_cast<int>(m_arShows.GetSize());

	for( int iShow=0; iShow<m_arShows.GetSize(); iShow++ )
		piArray[iShow] = m_arShows[iShow];

	Invalidate();

	return TRUE;
}

CTreeListHeaderCtrl* CTreeListCtrl::GetHeaderCtrl()
{
	// retrieve header control
	return &m_wndHeader;
}

int CTreeListCtrl::GetSelectionMark()
{
	// retrieve selection 
	POSITION pos = GetFirstSelectedItemPosition();
	return GetTreeListItem( GetNextSelectedItem( pos ) );
}

int CTreeListCtrl::SetSelectionMark( int iIndex )
{
	// set selection mark of an item
	int nItem = GetSelectionMark();
	SelectItem( GetTreeListItem( iIndex ), 0, SI_SELECT, FALSE );

	Invalidate();

	return nItem;
}

void CTreeListCtrl::SetFont()
{
	// set font with font of parent window
	CWnd* pWnd = GetParent();
	if( pWnd == NULL )
		return;

	CFont* pFont = pWnd->GetFont();
	if( pFont == NULL )
		return;

	LOGFONT lf;
	pFont->GetLogFont(&lf);
	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&lf);

	Invalidate();
}

void CTreeListCtrl::SetFont(CFont& font, bool repaint)
{
	LOGFONT lf;
	font.GetLogFont(&lf);
	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&lf);
	SetItemHeight();

	m_wndHeader.SetHeaderFont();
	m_wndHeader.SetHeaderHeight();
	m_wndTip->SetFont();

	m_wndStatic->SetFont( GetFont() );
	m_wndEdit->SetFont( GetFont() );
	m_wndCombo->SetFont( GetFont() );

	if (repaint)
		Invalidate();
}

CFont* CTreeListCtrl::GetFont()
{
	// retrieve font
	return &m_Font;
}

DWORD CTreeListCtrl::SetStyle( DWORD dwStyle )
{
	// set style of treelist control
	DWORD dwoldStyle = m_dwStyle;
	m_dwStyle = dwStyle;

	if( !(m_dwStyle&TLC_TREELIST ) )
	{
		m_dwStyle &= ~TLC_TREELINE;
		m_dwStyle &= ~TLC_ROOTLINE;
		m_dwStyle &= ~TLC_BUTTON;	
	}

	if( !(m_dwStyle&TLC_HGRID) )
	{
		m_dwStyle &= ~TLC_TGRID;
		m_dwStyle &= ~TLC_HGRID_EXT;
		m_dwStyle &= ~TLC_HGRID_FULL;
	}

	if( !(m_dwStyle&TLC_VGRID) )
	{
		m_dwStyle &= ~TLC_VGRID_EXT;
	}

	if( m_dwStyle&TLC_HGRID && m_dwStyle&TLC_VGRID )
	{
		if( m_dwStyle&TLC_HGRID_EXT && !(m_dwStyle&TLC_VGRID_EXT) )
			m_dwStyle &= ~( TLC_HGRID_EXT | TLC_VGRID_EXT );
		if( !(m_dwStyle&TLC_HGRID_EXT) && m_dwStyle&TLC_VGRID_EXT )
			m_dwStyle &= ~( TLC_HGRID_EXT | TLC_VGRID_EXT );
	}

	if( !(m_dwStyle&TLC_HEADER) )
	{
		m_dwStyle &= ~TLC_HEADDRAGDROP;
		m_dwStyle &= ~TLC_HEADFULLDRAG;
	}

	if( GetStyle()&TLC_HEADDRAGDROP )
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()|TLHS_DRAGDROP );
	else
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()&~TLHS_DRAGDROP );
	
	if( GetStyle()&TLC_HEADFULLDRAG )
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()|TLHS_FULLDRAG );
	else
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()&~TLHS_FULLDRAG );

	if( GetStyle()&TLC_HEADER )
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()|TLHS_SHOWHEAD );
	else
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()&~TLHS_SHOWHEAD );

	if( GetStyle()&TLC_HOTTRACK )
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()|TLHS_HOTTRACK );
	else
		m_wndHeader.SetStyle( m_wndHeader.GetStyle()&~TLHS_HOTTRACK );

	Layout();
	Invalidate();

	return dwoldStyle;
}

DWORD CTreeListCtrl::GetStyle() const
{
	// retrieve treelist control style
	return m_dwStyle;
}

void CTreeListCtrl::SetState( DWORD dwAddState, DWORD dwRemoveState )
{
	// set retrieve state of control
	m_dwState |= dwAddState;
	m_dwState &=~dwRemoveState;
	return; 
}

DWORD CTreeListCtrl::GetState( DWORD nStateMask ) const
{
	// retrieve state of control
	return m_dwState&nStateMask;
}

int CTreeListCtrl::GetSelectColumn()
{
	// retrieve select column
	return m_iSelCol;
}

int CTreeListCtrl::GetColumnCount()
{
	// retrieve column count
	return GetColumns();
}

int CTreeListCtrl::GetVisibleCount( CTreeListItem const* pParent ) const
{
	// retrieve visible rows
	if( pParent == m_pRootItem )
		return pParent->m_nVisibleChild - 1;
	else
		return pParent->m_nVisibleChild;
}

void CTreeListCtrl::StatChildAdd( CTreeListItem* pItem, int nChild, int nVisibleChild )
{
	// stat. number of child & visible child on any level when add some new child
	ASSERT( pItem != NULL );
	
	if( pItem->m_pParent == NULL )
		return;
	
	CTreeListItem* pParent = pItem->m_pParent;

	pParent->m_nChild			+= nChild;

	if( pParent->GetState()&TLIS_EXPANDED )
	{
		pParent->m_nVisibleChild	+= nVisibleChild;
		StatChildAdd( pParent, nChild, nVisibleChild );
	}
	else
	{
		StatChildAdd( pParent, nChild, 0 );
	}
}

void CTreeListCtrl::StatChildDel( CTreeListItem* pItem, int nChild, int nVisibleChild )
{
	// stat. number of child & visible child on any level when add some new child
	ASSERT( pItem != NULL );
	
	if( pItem->m_pParent == NULL )
		return;
	
	CTreeListItem* pParent = pItem->m_pParent;
	
	pParent->m_nChild -= nChild;
	
	if( pParent->GetState()&TLIS_EXPANDED )
	{
		pParent->m_nVisibleChild	-= nVisibleChild;
		StatChildDel( pParent, nChild, nVisibleChild );
	}
	else
	{
		StatChildDel( pParent, nChild, 0 );
	}
}

void CTreeListCtrl::StatExpand( CTreeListItem* pItem )
{
	// stat. number of child & visible child on any level when expand an item
	ASSERT( pItem != NULL );
	if( pItem->m_pChild == NULL )
		return;

	int nVisibleChild = 0;
	
	CTreeListItem* pChild = pItem->m_pChild;
	while( pChild != NULL )
	{
		nVisibleChild += pChild->m_nVisibleChild;
		pChild = pChild->m_pNext;
	}
	pItem->m_nVisibleChild += nVisibleChild;
	
	CTreeListItem* pParent = pItem->m_pParent;
	while( pParent != NULL&&pParent->GetState(TLIS_EXPANDED)!= 0 )
	{
		pParent->m_nVisibleChild += nVisibleChild;
		pParent = pParent->m_pParent;
	}

	return;
}

void CTreeListCtrl::StatCollapse( CTreeListItem* pItem )
{
	// stat. number of child & visible child on any level when collapse an item
	ASSERT( pItem != NULL );
	if( pItem->m_pChild == NULL )
		return;

	int nVisibleChild = 0;

	CTreeListItem* pChild = pItem->m_pChild;
	while( pChild != NULL )
	{
		nVisibleChild += pChild->m_nVisibleChild;
		pChild = pChild->m_pNext;
	}
	pItem->m_nVisibleChild -= nVisibleChild;

	CTreeListItem* pParent = pItem->m_pParent;
	while( pParent != NULL &&pParent->GetState(TLIS_EXPANDED)!= 0)
	{
		pParent->m_nVisibleChild -= nVisibleChild;
		pParent = pParent->m_pParent;
	}

	return;
}

bool CTreeListCtrl::GetItemCheck(CTreeListItem const* pItem) const{
	return (pItem->GetCheck() == TRUE);
}

bool CTreeListCtrl::SetItemCheck( CTreeListItem* pItem, bool bCheck , bool bAutoCheck)
{
	if (!(GetStyle()&TLC_CHECKBOX))
		return false;
	ASSERT(GetStyle()&TLC_CHECKBOX);
	ITEM_CHANGE_INFO info;
	info.item = pItem;
	info.iOldSubItem = -1;
	info.iNewSubItem = -1;
	if (bAutoCheck)
		info.uNewState = pItem->GetCheck()? TLIS_CHECKED_NULL: TLIS_CHECKED;
	else 
		info.uNewState = bCheck ? TLIS_CHECKED : TLIS_CHECKED_NULL;

//	if (bAutoCheck)
		info.uOldState = pItem->GetCheck()? TLIS_CHECKED : TLIS_CHECKED_NULL;
//	else 
//		info.uOldState = pItem->GetCheck() ? TLIS_CHECKED_NULL : TLIS_CHECKED;

	if (!itemChangeNotifyListener_->onItemCheckChanging(*this, &info))
		return false;

	if( bAutoCheck )
		pItem->SetCheck( !pItem->GetCheck() );
	else
		pItem->SetCheck(bCheck);

	itemChangeNotifyListener_->onItemCheckChanged(*this, &info);

	if( m_dwStyle&TLC_NOAUTOCHECK )
	{
		Invalidate();
		return true;
	}

	if( pItem->GetCheck() )
	{
		SetItemChildStatus( pItem, TLIS_CHECKED, TLIS_CHECKED_NULL,
			&IItemChangeNotifyListener::onItemCheckChanged);
		SetItemParentStatus( pItem, TLIS_CHECKEDMASK,
			&IItemChangeNotifyListener::onItemCheckChanged );
	}
	else
	{
		SetItemChildStatus( pItem, TLIS_CHECKED_NULL, TLIS_CHECKED,
			&IItemChangeNotifyListener::onItemCheckChanged );
		SetItemParentStatus( pItem, TLIS_CHECKEDMASK,
			&IItemChangeNotifyListener::onItemCheckChanged );
	}

	Invalidate();

	return true;
}

bool CTreeListCtrl::GetItemLock( CTreeListItem const* pItem) const{
	return (pItem->GetLock() == TRUE);
}

bool CTreeListCtrl::SetItemLock( CTreeListItem* pItem, bool bLock, bool bAutoLock )
{
	// auto set lock status ( include parent and child )
	if (!(GetStyle()&TLC_LOCKBOX))
		return false;
	ASSERT(GetStyle()&TLC_LOCKBOX);
	ITEM_CHANGE_INFO info;
	info.item = pItem;
	info.iOldSubItem = -1;
	info.iNewSubItem = -1;
	if (bAutoLock)
		info.uNewState = pItem->GetLock()? TLIS_LOCKED_NULL: TLIS_LOCKED;
	else 
		info.uNewState = bLock ? TLIS_LOCKED : TLIS_LOCKED_NULL;

//	if (bAutoLock)
		info.uOldState = pItem->GetLock()? TLIS_LOCKED : TLIS_LOCKED_NULL;
//	else 
//		info.uOldState = bLock ? TLIS_LOCKED_NULL : TLIS_LOCKED;

	if (!itemChangeNotifyListener_->onItemLockChanging(*this, &info))
		return false;

	if( bAutoLock )
		pItem->SetLock( !pItem->GetLock() );
	else
		pItem->SetLock(bLock);

	itemChangeNotifyListener_->onItemLockChanged(*this, &info);

	if( m_dwStyle&TLC_NOAUTOLOCK )
	{
		Invalidate();
		return true;
	}
	if( pItem->GetLock() )
	{
		SetItemChildStatus( pItem, TLIS_LOCKED, TLIS_LOCKED_NULL,
			&IItemChangeNotifyListener::onItemLockChanged );
		SetItemParentStatus( pItem, TLIS_LOCKEDMASK,
			&IItemChangeNotifyListener::onItemLockChanged );
	}
	else
	{
		SetItemChildStatus( pItem, TLIS_LOCKED_NULL, TLIS_LOCKED,
			&IItemChangeNotifyListener::onItemLockChanged );
		SetItemParentStatus( pItem, TLIS_LOCKEDMASK,
			&IItemChangeNotifyListener::onItemLockChanged );
	}

	Invalidate();

	return true;
}

void CTreeListCtrl::SetItemParentStatus( CTreeListItem* pItem, DWORD dwMask,
										 ICN_AFTER after)
{
	// set parent items status;
	CTreeListItem* pParent = pItem->m_pParent;

	if( pParent == NULL || pParent == m_pRootItem)
		return;

	DWORD dwShow;
	ASSERT( dwMask == TLIS_CHECKEDMASK || dwMask == TLIS_LOCKEDMASK );
	if( dwMask == TLIS_CHECKEDMASK )
		dwShow = TLIS_SHOWCHECKBOX;
	else
		dwShow = TLIS_SHOWLOCKBOX;
		

	DWORD dwAdd = 0;
	DWORD dwRemove = 0;
	CTreeListItem* pNext = pItem->m_pParent->m_pChild;
	if( pNext->GetState()&dwShow )
		dwAdd |= pNext->GetState();
	while( pNext != NULL )
	{
		if( pNext->GetState()&dwShow )
			dwAdd |= pNext->GetState();
		pNext = pNext->m_pNext;
	}
	dwAdd		 =  dwAdd;
	dwRemove	 = ~dwAdd;
	dwAdd		&=  dwMask;
	dwRemove	&=  dwMask;

	ITEM_CHANGE_INFO info;
	info.item = pParent;
	info.iOldSubItem = -1;
	info.iNewSubItem = -1;
	info.uNewState = dwAdd;
	info.uOldState = dwRemove;

	pParent->SetState( dwAdd, dwRemove );
	(itemChangeNotifyListener_->*after)(*this, &info);

	SetItemParentStatus( pParent, dwMask, after );

	return;
}


void CTreeListCtrl::SetItemChildStatus( CTreeListItem* pItem, 
									   DWORD dwAdd, DWORD dwRemove, 
									   ICN_AFTER after )
{

	// set child items status
	CTreeListItem* pChild = pItem->m_pChild;

	bool bres = true;
	while( pChild != NULL )
	{

		if( pChild->m_pChild != NULL )
			SetItemChildStatus( pChild, dwAdd, dwRemove, after );

		ITEM_CHANGE_INFO info;
		info.item = pChild;
		info.iOldSubItem = -1;
		info.iNewSubItem = -1;
		info.uNewState = dwAdd;
		info.uOldState = dwRemove;

		pChild->SetState( dwAdd, dwRemove );
		(itemChangeNotifyListener_->*after)(*this, &info);

		pChild = pChild->m_pNext;
	}
}

CTreeListItem* CTreeListCtrl::GetFirstShowItem( int nShowItem ) const
{
	// retrieve first visible item after n visible items
	if( nShowItem < 0 )
		return NULL;

	if( nShowItem >= m_pRootItem->m_nVisibleChild )
		return NULL;
	
	CTreeListItem* pItem = m_pRootItem->m_pChild;
	while( nShowItem != 0 )
	{
		ASSERT( nShowItem > 0 );

		if( nShowItem < pItem->m_nVisibleChild )
		{
			nShowItem --;
			pItem = pItem->m_pChild;
		}
		else
		{
			nShowItem -= pItem->m_nVisibleChild;
			pItem = pItem->m_pNext;
		}
	}

	return pItem;
}

CTreeListItem* CTreeListCtrl::GetNextShowItem( CTreeListItem const* pItem, 
											  bool bChild ) const
{
	// retrieve next visible item
	ASSERT( pItem != NULL );

	if( pItem->GetState()&TLIS_EXPANDED )
		if( bChild && pItem->m_pChild != NULL )
			return pItem->m_pChild;

	if( pItem->m_pNext != NULL )
		return pItem->m_pNext;

	pItem = pItem->m_pParent;
	if( pItem != m_pRootItem )
		return GetNextShowItem( pItem, FALSE );

	return NULL;
}

BOOL CTreeListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// wm_erasebkgnd
	return FALSE;
}

void CTreeListCtrl::OnPaint() 
{
	// wm_paint
	CPaintDC dc(this);
	CPalette* pPalette = NULL;

	if( dc.GetDeviceCaps(RASTERCAPS)&RC_PALETTE )
	{
		pPalette = dc.SelectPalette( &m_Palette, FALSE );
		dc.RealizePalette();
	}

	dc.ExcludeClipRect( m_rcHeader );

	if( GetState( TLS_OFFSCREENBUFFER ) )
	{
		CTreeListDC TreeListDC(&dc);
		DrawCtrl( &TreeListDC );
	}
	else
	{
		DrawCtrl( &dc );
	}

	if( pPalette != NULL && dc.GetDeviceCaps(RASTERCAPS)&RC_PALETTE )
	{
		dc.SelectPalette( pPalette, FALSE );
	}
}

void CTreeListCtrl::PrepareInfoForControlPaintCycle(CONTROL_CUSTOM_DRAW_INFO* pcdi,
													CDC* pDC, CRect const& rcClip)
{
	pcdi->hdc = *pDC;
	pcdi->rc = rcClip;
	if( CWnd::GetStyle()&WS_DISABLED )
	{
		if( m_dwStyle&TLC_BKGNDIMAGE )
		{
			pcdi->hbmpBckgrnd = *m_bmpBkDisable;
		}
		else if( m_dwStyle&TLC_BKGNDCOLOR )
		{
			pcdi->clrBckgrnd = getBkColorDisabledNonClientArea();
		}
		else
		{
			pcdi->clrBckgrnd = getBkColorDisabledSingleColorTree();
		}
	}
	else
	{
		if( m_dwStyle&TLC_BKGNDIMAGE )
		{
			pcdi->hbmpBckgrnd = *m_bmpBkBitmap;
		}
		else if( m_dwStyle&TLC_BKGNDCOLOR )
		{
			pcdi->clrBckgrnd = getBkColorNonClientArea();
		}
		else
		{
			pcdi->clrBckgrnd = getBkColorSingleColorTree();
		}
	}
}

void CTreeListCtrl::DrawCtrl( CDC* pDC )
{
	// draw the control
	CRect rcClip;
	if( pDC->GetClipBox( &rcClip ) == ERROR )
		return;

	CONTROL_CUSTOM_DRAW_INFO ccdi;
	memset(&ccdi, 0, sizeof(CONTROL_CUSTOM_DRAW_INFO));

	PrepareInfoForControlPaintCycle(&ccdi, pDC, rcClip);
	SetControlCustomDrawInfo(&ccdi);
	ICustomDrawNotifyListener* pcdnListener = GetCustomDrawNotifyListener();
	assert(pcdnListener);
	DWORD const res = pcdnListener->onPrepaint(*this, &ccdi);

	if (res == CDRF_SKIPDEFAULT) 
	{
		pcdnListener->onPostpaint(*this, &ccdi);
		SetControlCustomDrawInfo(NULL);
		return;
	}

	DrawBkgnd( pDC, rcClip );
	DrawItems( pDC, rcClip );

	pcdnListener->onPostpaint(*this, &ccdi);
	SetControlCustomDrawInfo(NULL);
}

void CTreeListCtrl::DrawBkgnd( CDC* pDC, CRect const& rcClip )
{
	// draw background
#ifdef _DEBUG
	CRect rectClip;
	if (pDC->GetClipBox(&rectClip) == ERROR)
		return;

	ASSERT(rcClip == rectClip);
#endif // _DEBUG
	CONTROL_CUSTOM_DRAW_INFO const* pccdi = GetControlCustomDrawInfo();
	if( m_dwStyle&TLC_BKGNDIMAGE )
		DrawBkgndBmp( pDC, pccdi->rc, pccdi->hbmpBckgrnd);
	else 
		pDC->FillSolidRect( &pccdi->rc, pccdi->clrBckgrnd );

	return;
}

void CTreeListCtrl::DrawBkgndBmp( CDC* pDC, RECT const& rcClip, HBITMAP hBkgnd )
{
	assert(hBkgnd);
	// draw bitmap background
	CRect rcClient(m_rcTreeList);
	BITMAP bmBkgnd;
	
	GetClientRect( rcClient );
	GetObject(hBkgnd, sizeof(BITMAP), &bmBkgnd);
	
	CDC dc;
	dc.CreateCompatibleDC( pDC );
	HGDIOBJ hOldBmp = dc.SelectObject( hBkgnd );

	pDC->IntersectClipRect( &rcClip );

	int nPerfix = - ( GetScrollPos( SB_HORZ ) % bmBkgnd.bmWidth );
	for( int y = m_rcTreeList.top; y < rcClient.Height(); y += bmBkgnd.bmHeight )
		for( int x = nPerfix; x < rcClient.Width(); x += bmBkgnd.bmWidth )
			pDC->BitBlt( x, y, bmBkgnd.bmWidth, bmBkgnd.bmHeight, &dc, 0, 0, SRCCOPY);

	::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

	dc.SelectObject( hOldBmp );
	dc.DeleteDC();

}

void CTreeListCtrl::GetCellBkColor(int iRow, COLORREF* color) const
{
	//! Рисуем фоны для ячеек
	if( !(CWnd::GetStyle()&WS_DISABLED) )
	{
		if( m_dwStyle&TLC_DOUBLECOLOR )
		{
			if( iRow%2 == 1 )
				*color = getBkColorSingleColor();
			else
				*color = getBkColorDoubleColor();
		}
		else
			*color = getBkColorSingleColorTree();
	}
	else
	{
		if( m_dwStyle&TLC_DOUBLECOLOR )
		{
			if( iRow%2 == 1 )
				*color = getBkColorDisabledSingleColor();
			else
				*color = getBkColorDisabledDoubleColor();
		}
		else
			*color = getBkColorDisabledSingleColorTree();
	}
}

void CTreeListCtrl::DrawGrid( CDC* pDC, CRect rcItem )
{
	// draw all grids
	CTreeListColumnInfo* pColumnInfo = NULL;
	int nColPos = 0;
	for( int iShow = 0; 
		iShow < m_arShows.GetSize(); 
		iShow++, nColPos += pColumnInfo->m_nWidth )
	{
		int iCol = m_arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
		if( pColumnInfo->m_nWidth == 0 )
			continue;

		CRect rcColumn;
		rcColumn.SetRect( 0, rcItem.top, pColumnInfo->m_nWidth, rcItem.bottom );
		rcColumn.OffsetRect( nColPos, 0 );
		rcColumn.OffsetRect( -GetScrollPos( SB_HORZ ), 0 );

		if( iCol == 0 )
		{
			// draw vertical grid line of tree
			if( m_dwStyle&TLC_HGRID_EXT && m_dwStyle&TLC_TGRID )
			{
				pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right-1, rcColumn.bottom - 1);
			}

			if( m_dwStyle&TLC_VGRID_EXT )
			{
				pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
			}
		}
		else
		{
			// draw vertical grid line and horizonal grid lin
			if( m_dwStyle&TLC_HGRID_EXT )
			{
				pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right, rcColumn.bottom - 1);
			}

			if( m_dwStyle&TLC_VGRID_EXT )
			{
				pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
			}
		}
	}
}

void CTreeListCtrl::DrawItems( CDC* pDC, CRect rcClip )
{
	// draw items
	CTreeListItem* pShowItem = GetFirstVisibleItem();
	if( pShowItem == NULL )
		return;

	CPen Pen( PS_SOLID, 1, getGridColor() );
	CPen* pOldPen	= pDC->SelectObject( &Pen );
	CFont* pOldFont	= pDC->SelectObject( &m_Font );
	int nOldMode = pDC->SetBkMode( TRANSPARENT );

	CRect rcClient;
	GetClientRect( rcClient );

	CRect rcItem( 0, 0, GetWidth(), GetItemHeight() );
	rcItem.OffsetRect( m_rcTreeList.left, m_rcTreeList.top );
	rcItem.OffsetRect( -GetScrollPos( SB_HORZ ), 0 );

	int nFirstRow	=	GetScrollPos( SB_VERT );
	int nShowRows	=	m_rcTreeList.Height() / GetItemHeight() + 1;

	pShowItem	=	GetFirstShowItem( nFirstRow );

	for( int iRow=nFirstRow; 
		iRow<= nFirstRow + nShowRows && iRow < GetVisibleCount(); 
		iRow++, pShowItem = GetNextShowItem( pShowItem ) )
	{
		ASSERT( pShowItem != NULL );

		ITEM_CUSTOM_DRAW_INFO icdi;
		memset(&icdi, 0, sizeof(ITEM_CUSTOM_DRAW_INFO));
		SetItemCustomDrawInfo(&icdi);

		PrepareInfoForItemPaintCycle(&icdi, pDC, iRow, pShowItem, rcItem);

		ICustomDrawNotifyListener* pcdnListener = GetCustomDrawNotifyListener();
		assert(pcdnListener);
		DWORD res = pcdnListener->onItemPrepaint(*this, &icdi);
		if (res == CDRF_SKIPDEFAULT) 
		{
			pcdnListener->onItemPostpaint(*this, &icdi);
			SetItemCustomDrawInfo(NULL);
			rcItem.OffsetRect( 0, m_nItemHeight );
			continue;
		}

		//! Рисуем фоны для ячеек
		if( !(m_dwStyle&TLC_BKGNDIMAGE) )
		{
			COLORREF color;
			GetCellBkColor(iRow, &color);
			pDC->FillSolidRect( rcItem, color );
		}

		DrawItem( pDC, rcItem, pShowItem ); 
		
		if( m_dwStyle&TLC_HGRID_FULL && rcItem.right<rcClient.right )
		{
			pDC->MoveTo( rcItem.right, rcItem.bottom - 1);
			pDC->LineTo( rcClient.right, rcItem.bottom - 1);
		}

		if (pcdnListener) 
		{
			pcdnListener->onItemPostpaint(*this, &icdi);
			SetItemCustomDrawInfo(NULL);
		}
		rcItem.OffsetRect( 0, m_nItemHeight );
	}

	if( GetVisibleCount() > 0 )
	{
		if( m_dwStyle&TLC_HGRID 
			&& !(m_dwStyle&TLC_HGRID_EXT) 
			&& !(m_dwStyle&TLC_VGRID_EXT) )
		{
			pDC->MoveTo( rcItem.left, rcItem.top - 1);
			pDC->LineTo( rcItem.right-1, rcItem.top - 1);
		}
	}
// Дорисовываем сетку до конца окна, после того как кончились узлы
	for( int iGrid=GetVisibleCount(); 
		iGrid<= nFirstRow + nShowRows; 
		++iGrid )
	{
		if( !(m_dwStyle&TLC_BKGNDIMAGE) )
		{
			COLORREF color = 0;
			GetCellBkColor(iGrid, &color);
			pDC->FillSolidRect( rcItem, color );
		}

		DrawGrid( pDC, rcItem );

		if( m_dwStyle&TLC_HGRID_EXT 
			&& m_dwStyle&TLC_HGRID_FULL 
			&& rcItem.right<rcClient.right )
		{
			pDC->MoveTo( rcItem.right, rcItem.bottom - 1 );
			pDC->LineTo( rcClient.right, rcItem.bottom - 1 );
		}

		rcItem.OffsetRect( 0, m_nItemHeight );
	}

	pDC->SetBkMode( nOldMode );
	pDC->SelectObject( pOldFont );
	pDC->SelectObject( pOldPen );
}


void CTreeListCtrl::PrepareInfoForItemPaintCycle(ITEM_CUSTOM_DRAW_INFO* pcdi,
												 CDC* pDC, 
												 int iRow, 
												 CTreeListItem const* pItem,
												 CRect const& rcItem)
{
	//! Рисуем фоны для ячеек
	if( !(m_dwStyle&TLC_BKGNDIMAGE) )
	{
		pcdi->useBackgrndClr = false;
		GetCellBkColor(iRow, &pcdi->clrTextBk);
	}
	else
		pcdi->useBackgrndClr = false;

	pcdi->clrText = getNormalTextColor();
	GetItemBkgrndRect(pItem, rcItem, &pcdi->rc);
	pcdi->hdc = *pDC;
	pcdi->item = const_cast<CTreeListItem*>(pItem);
	pcdi->iRow = iRow;
}

void CTreeListCtrl::PrepareInfoForSubItemPaintCycle(SUBITEM_CUSTOM_DRAW_INFO* pscdi,
												 CDC* pDC, 
												 CTreeListItem const* pItem,
												 int iCol)
{
	pscdi->useBackgrndClr = GetItemCustomDrawInfo()->useBackgrndClr;
	pscdi->clrTextBk = GetItemCustomDrawInfo()->clrTextBk;
	pscdi->clrText = GetItemCustomDrawInfo()->clrText;
	CTreeListColumnInfo* const pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
	pscdi->item = const_cast<CTreeListItem*>(pItem);
	pscdi->iSubItem = iCol;
	pscdi->uTextFormat = pColumnInfo->GetTextFormat();
	pscdi->hdc = *pDC;
	GetItemRect(pItem, iCol, &pscdi->rc, false);
	GetItemRect(pItem, iCol, &pscdi->rcText, true);
}

void CTreeListCtrl::DrawItem( CDC* pDC, CRect rcItem, CTreeListItem* pItem )
{
	if (!m_arShows.GetSize())
		return;

	SUBITEM_CUSTOM_DRAW_INFO scdi;
	memset(&scdi, 0, sizeof(SUBITEM_CUSTOM_DRAW_INFO));
	SetSubitemCustomDrawInfo(&scdi);
	// draw all columns
	CTreeListColumnInfo* pColumnInfo = NULL;
	int nColPos = 0;
	for( int iShow = 0; 
		iShow < m_arShows.GetSize(); 
		iShow++, nColPos += pColumnInfo->m_nWidth )
	{
		// do NOT draw zero column;
		int iCol = m_arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
		if( pColumnInfo->m_nWidth == 0 )
			continue;


		CRect rcColumn( 0, rcItem.top, pColumnInfo->m_nWidth, rcItem.bottom );
		rcColumn.OffsetRect( nColPos - GetScrollPos( SB_HORZ ), 0 );
		
		if (nColPos == 0)
		{
			// draw tree column of treelist control
			DrawItemBkgnd(pDC, rcColumn, pItem);
		}

		PrepareInfoForSubItemPaintCycle(&scdi, pDC, pItem, iCol);
		ICustomDrawNotifyListener* pcdnListener = GetCustomDrawNotifyListener();
		assert(pcdnListener);
		DWORD res = pcdnListener->onSubitemPrepaint(*this, &scdi);
		if (res == CDRF_SKIPDEFAULT)
		{
			pcdnListener->onSubitemPostpaint(*this, &scdi);
			continue;
		}
		if( iCol == 0 )
		{
			if ( m_dwStyle&TLC_TREELIST )
				DrawItemTree( pDC, rcColumn, pItem, iCol );
			else
				DrawItemMain( pDC, rcColumn, pItem, iCol );
		}
		else
		{
			DrawItemList( pDC, rcColumn, pItem, iCol );
		}
		pcdnListener->onSubitemPostpaint(*this, &scdi);
	}

	if( pItem == GetFocusedItem() )
	{
		CPen pen;
		pen.CreatePen( PS_SOLID, 1, (COLORREF)0x000000 );
		CPen* ppen = pDC->SelectObject( &pen );

		CRect rcFocus( 0, rcItem.top, GetWidth(), rcItem.bottom );
		rcFocus.OffsetRect( -GetScrollPos( SB_HORZ ), 0 );

		if( GetStyle()&TLC_HGRID )
			rcFocus.DeflateRect( 0, 0, 0, 1 );

		if( GetStyle()&TLC_VGRID )
			rcFocus.DeflateRect( 0, 0, 1, 0 );

		COLORREF crBkgnd, croldBkgnd;
		if( CWnd::GetStyle()&WS_DISABLED )
		{
			if( m_dwStyle&TLC_BKGNDIMAGE )
				crBkgnd = getBkColorDisabledNonClientArea();
			else if( m_dwStyle&TLC_BKGNDCOLOR )
				crBkgnd = getBkColorDisabledNonClientArea();
			else
				crBkgnd = getBkColorDisabledSingleColorTree();
		}
		else
		{
			if( m_dwStyle&TLC_BKGNDIMAGE )
				crBkgnd = getBkColorNonClientArea();
			else if( m_dwStyle&TLC_BKGNDCOLOR )
				crBkgnd = getBkColorNonClientArea();
			else
				crBkgnd = getBkColorSingleColorTree();
		}

		croldBkgnd = pDC->SetBkColor( crBkgnd );
		pDC->SetTextColor( getNormalTextColor() );
		pDC->DrawFocusRect( rcFocus );
		pDC->SetBkColor( croldBkgnd );

		pDC->SelectObject( ppen );
	}
	SetSubitemCustomDrawInfo(NULL);
	return;
}

void CTreeListCtrl::DrawItemBkgnd( CDC* pDC, 
								  CRect const& rcItem, 
								  CTreeListItem const* pItem )
{
	// draw selected background
	if( pItem == GetDropTargetItem() )
	{
		CRect rcBkgnd;
		GetItemBkgrndRect(pItem, rcItem, &rcBkgnd);

		COLORREF const crBkgnd = getBkColorSelectedColumn();

		DrawItemExclude( pDC, rcItem, pItem );
		pDC->FillSolidRect( rcBkgnd, crBkgnd );
		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
	}
	else if( pItem->GetSelected() && GetStyle()&TLC_SHOWSELFULLROWS )
	{
		CRect const rcBkgnd(rcItem);

		COLORREF crBkgnd;
		if( GetState( TLS_FOCUS ) )
		{
			if( !(m_dwStyle&TLC_MULTIPLESELECT) )
			{
				// single select
				crBkgnd = getBkColorSelectedRow();
			}
			else
			{
				// multiple select
				assert(m_arSelects.GetSize());
				if(pItem == GetFocusedItem())
					crBkgnd = getBkColorSelectedRow();
				else
					crBkgnd = getBkColorSelectedItem();
			}
			DrawItemExclude( pDC, rcItem, pItem );
			pDC->FillSolidRect( rcBkgnd, crBkgnd );
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
		}
		else
		{
			if( m_dwStyle&TLC_SHOWSELALWAYS )
			{
				crBkgnd = getBkColorSelectedItemNoFocus();
				DrawItemExclude( pDC, rcItem, pItem );
				pDC->FillSolidRect( rcBkgnd, crBkgnd );
				::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
			}
		}
	}
}

void CTreeListCtrl::DrawItemExclude( CDC* pDC, 
									CRect const& rcItem, 
									CTreeListItem const* pItem )
{
	if( !pItem->GetSelected() || !(GetStyle()&TLC_SHOWSELFULLROWS) )
		return;

	// exclude rects of selected item
	CTreeListColumnInfo* pColumnInfo = NULL;
	int nColPos = 0;


	for( int iShow = 0; 
		iShow < m_arShows.GetSize(); 
		iShow++, nColPos += pColumnInfo->m_nWidth 
		)
	{
		int iCol = m_arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
		
		CRect rcColumn( 0, rcItem.top, pColumnInfo->m_nWidth, rcItem.bottom );
		rcColumn.OffsetRect( nColPos - GetScrollPos( SB_HORZ ), 0 );
// TODO: При стиле TLC_SHOWSELFULLROWS, если первый столбец сделать вторым 
//		не прорисовывается фон под кнопками, чеками, линиями и т.д.
		 
		if( iCol == 0 )
		{
			CRect rcExclude;
			int nPerfix = TLL_BORDER;
			if ( m_dwStyle&TLC_TREELIST )
			{
				nPerfix += TLL_WIDTH * ( pItem->m_nLevel - 1 );

				if( m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
					nPerfix += TLL_WIDTH;
			}

			if( m_dwStyle&TLC_CHECKBOX )
			{
				if( pItem->GetState()&TLIS_SHOWCHECKBOX )
				{
					CRect rcCheckBox( 0, 0, TLL_WIDTH, TLL_WIDTH );
					rcCheckBox.OffsetRect( rcColumn.left + nPerfix, 
						rcColumn.top + ( rcColumn.Height() - TLL_HEIGHT)/2 );
					rcExclude = rcColumn & rcCheckBox;

					if( !rcExclude.IsRectEmpty() )
						pDC->ExcludeClipRect( rcExclude );

					nPerfix += TLL_HEIGHT;
				}
			}

			if (GetStateImageList()&&pItem->GetStateImage())
			{
				CRect rcImage( 0, 0, TLL_WIDTH, TLL_WIDTH );
				rcImage.OffsetRect( rcColumn.left + nPerfix, 
					rcColumn.top + ( rcColumn.Height() - TLL_HEIGHT)/2 );
				rcExclude = rcColumn & rcImage;

				if( !rcExclude.IsRectEmpty() )
					pDC->ExcludeClipRect( rcExclude );

				nPerfix += TLL_HEIGHT;
			}

			if( m_dwStyle&TLC_LOCKBOX )
			{
				if( pItem->GetState()&TLIS_SHOWLOCKBOX )
				{
					CRect rcLockBox( 0, 0, TLL_WIDTH / 2, TLL_WIDTH );
					rcLockBox.OffsetRect( rcColumn.left + nPerfix, 
						rcColumn.top + ( rcColumn.Height() - TLL_HEIGHT)/2 );
					rcExclude = rcColumn & rcLockBox;

					if( !rcExclude.IsRectEmpty() )
						pDC->ExcludeClipRect( rcExclude );

					nPerfix += TLL_HEIGHT / 2;
				}
			}
		
			if( m_dwStyle&TLC_IMAGE )
			{
				if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
				{
					CRect rcImage( 0, 0, TLL_WIDTH, TLL_WIDTH );
					rcImage.OffsetRect( rcColumn.left + nPerfix, 
						rcColumn.top + ( rcColumn.Height() - TLL_HEIGHT)/2 );
					rcExclude = rcColumn & rcImage;

					if( !rcExclude.IsRectEmpty() )
						pDC->ExcludeClipRect( rcExclude );

					nPerfix += TLL_HEIGHT;
				}
			}

			break;
		}
	}
}

int	CTreeListCtrl::DrawStateImage(CDC* pDC, CRect const& rcColumn, CRect const& rcGraph,
					   CTreeListItem const* pItem, int iCol, int nPrefix)  const
{
	int const stateImage = pItem->GetStateImage();
	if (!GetStateImageList()||!stateImage)
		return 0;

	CRect rcImage(rcGraph);
	rcImage.left = rcColumn.left + nPrefix;
	rcImage.right = rcImage.left + TLL_WIDTH;

	CPoint pt(rcImage.left, rcImage.top + (rcImage.Height() - TLL_HEIGHT)/2);

	pDC->IntersectClipRect( rcGraph );
	pDC->IntersectClipRect( rcImage );

	UINT nStyle = ILD_TRANSPARENT;
	if( pItem == m_pTargetItem || pItem == m_pHoverItem )
		nStyle |= ILD_FOCUS;

	GetStateImageList()->Draw( pDC, stateImage-1, pt, nStyle );
	::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

	return TLL_WIDTH;
}

int CTreeListCtrl::DrawCheckBox(CDC* pDC, 
								CRect const& rcColumn, 
								CRect const& rcGraph, 
								CTreeListItem const* pItem, 
								int iCol, int nPerfix)
{
	// draw check box
	if( m_dwStyle&TLC_CHECKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWCHECKBOX )
		{
			CRect rcCheckBox(rcGraph);
			rcCheckBox.left = rcColumn.left + nPerfix;
			rcCheckBox.right += TLL_WIDTH;

			CPoint pt(rcCheckBox.left, rcCheckBox.top 
				+ (rcCheckBox.Height() - TLL_HEIGHT)/2);

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcCheckBox );

			UINT nStyle = ILD_TRANSPARENT;
			if( pItem == m_pTargetItem || pItem == m_pHoverItem )
				nStyle |= ILD_FOCUS;
			m_imgCheck.Draw( pDC, pItem->GetCheck(), pt, nStyle );
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

			return TLL_WIDTH;
		}
	}
	return 0;
}

int CTreeListCtrl::DrawLockBox( CDC* pDC, CRect const& rcColumn, CRect const& rcGraph, 
					CTreeListItem const* pItem, int iCol, int nPerfix )
{
	// draw lock box
	if( m_dwStyle&TLC_LOCKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWLOCKBOX )
		{
			CRect rcLockBox(rcGraph);
			rcLockBox.left = rcColumn.left + nPerfix;
			rcLockBox.right = rcLockBox.left + TLL_WIDTH/2;

			POINT pt;
			pt.x = rcLockBox.left;
			pt.y = rcLockBox.top + (rcLockBox.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcLockBox );

			UINT nStyle = ILD_TRANSPARENT;
			if( pItem == m_pTargetItem || pItem == m_pHoverItem )
				nStyle |= ILD_FOCUS;
			m_imgLock.Draw( pDC, pItem->GetLock(), pt, nStyle );
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
			
			return (TLL_WIDTH>>1);
		}
	}
	return 0;
}

int	CTreeListCtrl::DrawItemImage( CDC* pDC, CRect const& rcColumn, CRect const& rcGraph, 
					  CTreeListItem const* pItem, int iCol, int nPerfix )
{
	// draw image box
	if( m_dwStyle&TLC_IMAGE )
	{
		if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
		{
			CRect rcImage(rcGraph);
			rcImage.left = rcColumn.left + nPerfix;
			rcImage.right = rcImage.left + TLL_WIDTH;

			POINT pt;
			pt.x = rcImage.left;
			pt.y = rcImage.top + (rcImage.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcImage );

			UINT nStyle = ILD_TRANSPARENT;
			if( pItem == m_pTargetItem || pItem == m_pHoverItem )
				nStyle |= ILD_FOCUS;

			if( m_dwStyle&TLC_TREELIST
				&& pItem->GetState()&TLIS_EXPANDED 
				&& pItem->m_pChild != NULL )
			{
				if( pItem == GetFocusedItem() )
					m_pImageList->Draw( pDC, pItem->m_nExpandSelectedImage, pt, nStyle );
				else
					m_pImageList->Draw( pDC, pItem->m_nSelectedImage, pt, nStyle );
			}
			else
			{
				if( pItem->GetSelected() && pItem == GetFocusedItem() )
					m_pImageList->Draw( pDC, pItem->m_nExpandImage, pt, nStyle );
				else
					m_pImageList->Draw( pDC, pItem->m_nImage, pt, nStyle );
			}
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
			return TLL_WIDTH;
		}
	}
	return 0;
}
void CTreeListCtrl::DrawItemTree( CDC* pDC, 
								 CRect rcColumn, 
								 CTreeListItem* pItem, 
								 int iCol )
{
	assert(!iCol);
//	// draw tree column of treelist control
//	DrawItemBkgnd(pDC, rcColumn, pItem);

	CRect rcGraph(rcColumn);
	rcGraph.DeflateRect( TLL_BORDER, 0, TLL_BORDER+1, 0 );
	if( rcGraph.Width() <= 0 )
		return;

	CTreeListColumnInfo* pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
	CRect rcTree(rcGraph);
	rcTree.right = rcTree.left + TLL_WIDTH;

	int nPerfix = TLL_BORDER;
	// draw prefix vertical line
	for( int iLevel=1; 
		iLevel<pItem->m_nLevel; 
		iLevel++, nPerfix += TLL_WIDTH )
	{
		CTreeListItem* pCheckItem = pItem;
		while( pCheckItem->m_nLevel != iLevel )
		{
			pCheckItem = pCheckItem->m_pParent;
		}

		rcTree.left = rcColumn.left + nPerfix;
		rcTree.right = rcTree.left + TLL_WIDTH;

		if( pCheckItem->m_pNext != NULL )
		{
			if( iLevel != 1 || m_dwStyle&TLC_ROOTLINE )
				DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_TOP | TLL_BOTTOM );	// ©§
		}
	}

	// draw cross line
	if( m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
	{
		rcTree.left = rcColumn.left + nPerfix;
		rcTree.right = rcTree.left + TLL_WIDTH;
		if( pItem->m_nLevel == 1 )
		{
			// child item of root item
			if( m_dwStyle&TLC_ROOTLINE )
			{
				if( pItem->m_pPrev == NULL && pItem->m_pNext == NULL )
					DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_RIGHT );						// ©Ґ ( right part )
				else if( pItem->m_pPrev == NULL )
					DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_RIGHT | TLL_BOTTOM );			// ©і
				else if( pItem->m_pNext == NULL )
					DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_RIGHT | TLL_TOP );				// ©»
				else
					DrawItemTreeLine( pDC, rcGraph, rcTree, 
									TLL_RIGHT | TLL_TOP | TLL_BOTTOM );	// ©З
			}
			else
			{
				DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_RIGHT );							// ©Ґ ( right part )
			}
		}
		else
		{
			// child item of other items ( not root item )
			if( pItem->m_pNext == NULL )
				DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_RIGHT | TLL_TOP );					// ©»
			else
				DrawItemTreeLine( pDC, rcGraph, rcTree, 
									TLL_RIGHT | TLL_TOP | TLL_BOTTOM );		// ©З
		}
		
		// draw button of child item of root item
		if( m_dwStyle&TLC_BUTTON )
		{
			if( pItem->m_pChild != NULL )
			{
				if( pItem->GetState()&TLIS_EXPANDED )
					DrawItemTreeButton( pDC, rcGraph, rcTree, TLB_MINUS );
				else
					DrawItemTreeButton( pDC, rcGraph, rcTree, TLB_PLUS );
			}				
		}
		nPerfix += TLL_WIDTH;
	}

	nPerfix += DrawCheckBox(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);
	nPerfix += DrawLockBox(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);
	nPerfix += DrawStateImage(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);
	nPerfix += DrawItemImage(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);

	// add gaps between text and image 
	if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	// draw item text
	CRect rcBkgnd(rcColumn);
	rcBkgnd.left = rcColumn.left + nPerfix;

	DrawItemTreeText( pDC, rcBkgnd, pItem, iCol );

	// draw vertical grid line of tree
	if( m_dwStyle&TLC_TGRID )
	{
		pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.bottom - 1);
	}

	if( m_dwStyle&TLC_VGRID )
	{
		pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
	}

	return;
}

void CTreeListCtrl::DrawItemMain( CDC* pDC, 
								 CRect rcColumn, 
								 CTreeListItem* pItem, 
								 int iCol )
{
//	// draw main column of list
//	DrawItemBkgnd(pDC, rcColumn, pItem);

	CRect rcGraph(rcColumn);
	rcGraph.DeflateRect( TLL_BORDER, 0, TLL_BORDER+1, 0 );
	if( rcGraph.Width() <= 0 )
		return;
	
	CTreeListColumnInfo* pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];

	CRect rcTree(rcGraph);
	rcTree.right = rcTree.left + TLL_WIDTH;

	int nPerfix = TLL_BORDER;

	// draw check box
	nPerfix += DrawCheckBox(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);
	nPerfix += DrawLockBox(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);
	nPerfix += DrawStateImage(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);
	nPerfix += DrawItemImage(pDC, rcColumn, rcGraph, pItem, iCol, nPerfix);

	// add gaps between text and image 
	if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	// draw item text
	CRect rcBkgnd(rcColumn);
	rcBkgnd.left = rcColumn.left + nPerfix;

	DrawItemTreeText( pDC, rcBkgnd, pItem, iCol );

	// draw vertical grid line of tree
	if( m_dwStyle&TLC_TGRID )
	{
		pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.bottom - 1);
	}

	if( m_dwStyle&TLC_VGRID )
	{
		pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
	}
}

void CTreeListCtrl::DrawItemList( CDC* pDC, 
								 CRect const& rcColumn, 
								 CTreeListItem* pItem, 
								 int iCol )
{
	// draw list
	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];

	DrawItemListText( pDC, rcColumn, pItem, iCol );

	// draw vertical grid line and horizonal grid lin
	if( m_dwStyle&TLC_HGRID )
	{
		pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right, rcColumn.bottom - 1);
	}

	if( m_dwStyle&TLC_VGRID )
	{
		pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
	}
}

void CTreeListCtrl::DrawItemTreeLine( CDC* pDC, 
									 CRect const& rcColumn, 
									 CRect const& rcTree, 
									 DWORD dwFormat )
{
	// draw line of tree
	if( rcColumn.Width() == 0 || rcColumn.Height() == 0 )
		return;

	if( !(m_dwStyle&TLC_TREELINE) )
		return;

	ASSERT( rcTree.Width()	% 2 == 0 );
	ASSERT( rcTree.Height() % 2 == 0 );

	// clip column
	pDC->IntersectClipRect( rcColumn );
	pDC->IntersectClipRect( rcTree );

	CPoint ptCenter;
	ptCenter.x = rcTree.left + rcTree.Width() / 2 - 1;
	ptCenter.y = rcTree.top  + rcTree.Height() / 2 - 1;

	// draw center-left
	if( dwFormat&TLL_LEFT )
	{
		for( int ix = ptCenter.x; ix >= rcTree.left; ix -= 2 )
			pDC->SetPixelV( ix, ptCenter.y, getTreeLineColor() );
	}

	// draw center-right
	if( dwFormat&TLL_RIGHT )
	{
		for( int ix = ptCenter.x; ix <= rcTree.right; ix += 2 )
			pDC->SetPixelV( ix, ptCenter.y, getTreeLineColor() );
	}

	// draw center-top
	if( dwFormat&TLL_TOP )
	{
		for( int iy = ptCenter.y; iy >= rcTree.top; iy -= 2 )
			pDC->SetPixelV( ptCenter.x, iy, getTreeLineColor() );
	}

	// draw center-bottom
	if( dwFormat&TLL_BOTTOM )
	{
		for( int iy = ptCenter.y; iy <= rcTree.bottom; iy += 2 )
			pDC->SetPixelV( ptCenter.x, iy, getTreeLineColor() );
	}

	::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
}

void CTreeListCtrl::DrawItemTreeButton( CDC* pDC, 
									   CRect const& rcColumn, 
									   CRect const& rcTree, 
									   DWORD dwFormat )
{
	// draw tree button [+][-][?]
	if( rcColumn.Width() == 0 || rcColumn.Height() == 0 )
		return;

	ASSERT( rcTree.Width()  % 2 == 0 );
	ASSERT( rcTree.Height() % 2 == 0 );

	// clip column
	pDC->IntersectClipRect( rcColumn );
	pDC->IntersectClipRect( rcTree );

	// button center
	CPoint ptCenter;
	ptCenter.x = rcTree.left + rcTree.Width() / 2 - 1;
	ptCenter.y = rcTree.top	+ rcTree.Height() / 2 - 1;

	CRect rcButton;
	rcButton.SetRect( -TLL_BUTTON, -TLL_BUTTON, TLL_BUTTON+1, TLL_BUTTON+1 );
	rcButton.OffsetRect( ptCenter );

	CBrush brButton;
	brButton.CreateSolidBrush( getTreeLineColor() );

	if ( !(CWnd::GetStyle()&WS_DISABLED) )
		pDC->FillSolidRect( rcButton, 0x00FFFFFF );
	else
		pDC->FillSolidRect( rcButton, 0x00D0D0D0 );
	pDC->FrameRect( rcButton, &brButton );

	rcButton.DeflateRect( 2, 2, 2, 2 );
	// draw unknow button [+] in grey
	if( dwFormat&TLB_UNKNOW )
	{
		CPen Pen, *pOldPen;
		Pen.CreatePen( PS_SOLID, 1, getTreeLineColor() );
		pOldPen = pDC->SelectObject( &Pen );
		pDC->MoveTo( rcButton.left, ptCenter.y );
		pDC->LineTo( rcButton.right, ptCenter.y );
		pDC->MoveTo( ptCenter.x, rcButton.top );
		pDC->LineTo( ptCenter.x, rcButton.bottom );
		pDC->SelectObject( pOldPen );
	}

	// draw plus button [+]
	if( dwFormat&TLB_PLUS )
	{
		CPen Pen, *pOldPen;
		Pen.CreatePen( PS_SOLID, 1, (COLORREF)0x000000 );
		pOldPen = pDC->SelectObject( &Pen );
		pDC->MoveTo( rcButton.left, ptCenter.y );
		pDC->LineTo( rcButton.right, ptCenter.y );
		pDC->MoveTo( ptCenter.x, rcButton.top );
		pDC->LineTo( ptCenter.x, rcButton.bottom );
		pDC->SelectObject( pOldPen );
	}

	// draw minus button [-]
	if( dwFormat&TLB_MINUS )
	{
		CPen Pen, *pOldPen;
		Pen.CreatePen( PS_SOLID, 1, (COLORREF)0x000000 );
		pOldPen = pDC->SelectObject( &Pen );
		pDC->MoveTo( rcButton.left, ptCenter.y );
		pDC->LineTo( rcButton.right, ptCenter.y );
		pDC->SelectObject( pOldPen );
	}

	::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
}

void CTreeListCtrl::DrawItemTreeText( CDC* pDC, 
									 CRect rcBkgnd, 
									 CTreeListItem* pItem, 
									 int iCol )
{
	// draw text on tree
	CRect rcText(rcBkgnd);
	rcText.DeflateRect( 2, 0, 3, 0 );
	rcBkgnd.bottom = rcBkgnd.bottom - 1;
	DrawItemText( pDC, rcBkgnd, rcText, pItem, iCol );
}

void CTreeListCtrl::DrawItemListText( CDC* pDC, 
									 CRect rcBkgnd, 
									 CTreeListItem* pItem, 
									 int iCol )
{
	// draw text on list
	CRect rcText(rcBkgnd);
	rcText.DeflateRect( 2, 0, 3, 0 );
	rcBkgnd.bottom = rcBkgnd.bottom - 1;
	DrawItemText( pDC, rcBkgnd, rcText, pItem, iCol );
}

void CTreeListCtrl::DrawItemText( CDC* pDC, 
								 CRect const& rcBkgnd, 
								 CRect rcText, 
								 CTreeListItem* pItem, 
								 int iCol )
{

	// draw selected background
	if( rcBkgnd.Width() <= 0 || rcBkgnd.Height() <= 0 )
		return;
	// draw text
	if( rcText.Width() <= 0 || rcText.Height() <= 0 )
		return;

	COLORREF crBkgrnd = GetSubitemCustomDrawInfo()->clrTextBk;
	COLORREF crText = GetSubitemCustomDrawInfo()->clrText;
	bool drawBack = GetSubitemCustomDrawInfo()->useBackgrndClr;
	if( pItem == m_pTargetItem )
	{
		crText	= getSelectedColumnTextColor();
		crBkgrnd = getBkColorSelectedColumn();
		drawBack = true;
	}
	else if( pItem->GetSelected() )
	{
		if( GetState( TLS_FOCUS ) )
		{
			if( !(m_dwStyle&TLC_MULTIPLESELECT) )
			{
				// single select
				if( iCol == m_iSelCol && m_dwStyle&TLC_SHOWSELACTIVE )
				{
					drawBack = true;
					crBkgrnd = getBkColorSelectedColumn();
					crText	= getSelectedColumnTextColor();
				}
				else
				{
					crText	= getSelectedRowTextColor();
					crBkgrnd = getBkColorSelectedRow();
					drawBack = true;
				}
			}
			else
			{
				// multiple select
				assert(m_arSelects.GetSize() > 0);
				if(pItem == GetFocusedItem())
				{
					if( iCol == m_iSelCol && m_dwStyle&TLC_SHOWSELACTIVE )
					{
						drawBack = true;
						crBkgrnd = getBkColorSelectedColumn();
						crText	= getSelectedColumnTextColor();
					}
					else
					{
						drawBack = true;
						crBkgrnd = getBkColorSelectedRow();
						crText	= getSelectedRowTextColor();
					}
				}
				else
				{
					drawBack = true;
					crBkgrnd = getBkColorSelectedItem();
					crText	= getSelectedTextColor();
				}
			}
		}
		else
		{
			if( m_dwStyle&TLC_SHOWSELALWAYS )
			{
				drawBack = true;
				crBkgrnd = getBkColorSelectedItemNoFocus();
				crText = getSelectedTextNoFocusColor();
			}
		}
	}
	if (drawBack)
		pDC->FillSolidRect( rcBkgnd, crBkgrnd);
	COLORREF crOldText = pDC->SetTextColor( crText );

	UINT dwFormat = GetSubitemCustomDrawInfo()->uTextFormat;

	pDC->DrawText( pItem->GetText(iCol), rcText, dwFormat );

	pDC->SetTextColor( crOldText );
}

void CTreeListCtrl::OnEnable(BOOL bEnable) 
{
	// enable or disable control
	CWnd::OnEnable(bEnable);
	
	m_wndHeader.EnableWindow( bEnable );
	m_wndTip->EnableWindow( bEnable );

	Invalidate();

	return;
}

void CTreeListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// calculate size of not client area
	if( !(CWnd::GetExStyle()&WS_EX_CLIENTEDGE) )
	{
		LPRECT pRECT = &lpncsp->rgrc[0];
		::InflateRect( pRECT, -1, -1 );
	}

	Default();
	return;
}

void CTreeListCtrl::OnNcPaint() 
{
	// draw 3d frame rect
	Default();

	if( !(CWnd::GetExStyle()&WS_EX_CLIENTEDGE) )
	{
		CWindowDC dc(this);
		CRect rcBorder;
		GetWindowRect( &rcBorder );
		ScreenToClient( &rcBorder );
		rcBorder.OffsetRect( -rcBorder.left, -rcBorder.top );
		dc.Draw3dRect( &rcBorder, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHILIGHT ) );
	}

	return;
}

void CTreeListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	// resize tree list control
	CWnd::OnSize(nType, cx, cy);
	
	Layout();

	return;
}

BOOL CTreeListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// translate mouse wheel message
	int nPos = zDelta / 120;

	if( IsWindow(m_wndTip->GetSafeHwnd()) && m_wndTip->IsWindowVisible() )
		m_wndTip->Hide();

	if( nPos > 0 )
	{
		PostMessage( WM_VSCROLL, (WPARAM)SB_PAGEUP );
	}
	else
	{
		PostMessage( WM_VSCROLL, (WPARAM)SB_PAGEDOWN );
	}
	return TRUE;
}


bool CTreeListCtrl::InsertColumn( LPCTSTR lpszColumnHeading, DWORD dwFormat, int nWidth, int iImage, int nMin, int nMax)
{
	// insert a column
	ASSERT( m_pRootItem->m_pChild == NULL );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = new CTreeListColumnInfo;

	pColumnInfo->m_strCaption	= lpszColumnHeading;
	pColumnInfo->m_dwFormat		= dwFormat;
	pColumnInfo->m_nWidth		= nWidth;
	pColumnInfo->m_iImage		= iImage;
	pColumnInfo->m_nMin			= nMin;
	pColumnInfo->m_nMax			= nMax;

	int nCol = static_cast<int>(m_arColumns.Add( (void*)pColumnInfo ));
	
	if( (pColumnInfo->m_dwFormat&TLF_HIDDEN) == 0 )
		m_arShows.Add( nCol );

	if( pColumnInfo->m_dwFormat&TLF_CAPTION_SORT )
		if( pColumnInfo->m_dwFormat&TLF_SORT_MASK )
			m_arSorts.Add( nCol );

	Layout();
	Invalidate();

	return TRUE;
}

bool CTreeListCtrl::DeleteColumn( int iCol )
{
	// delete a column
	ASSERT( m_pRootItem->m_pChild == NULL );

	if( iCol < 0  || m_arColumns.GetSize() <= iCol )
		return FALSE;

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
	m_arColumns.RemoveAt( iCol );

	for( int iShow=0; iShow<m_arShows.GetSize(); iShow++ )
	{
		if( m_arShows[iShow] == iCol )
		{
			m_arShows.RemoveAt( iCol );
		}

		if( iShow<m_arShows.GetSize() )
		{
			if( m_arShows[iShow] > iCol )
				m_arShows[iShow] -= 1;
		}
	}

	for( int iSort=0; iSort<m_arSorts.GetSize(); iSort++ )
	{
		if( m_arSorts[iSort] == iCol )
			m_arSorts.RemoveAt( iCol );

		if( iSort<m_arSorts.GetSize() )
			if( m_arSorts[iSort] > iCol )
				m_arSorts[iSort] -= 1;
	}

	Layout();
	Invalidate();

	return TRUE;
}

int CTreeListCtrl::InsertItem( int nItem, LPCTSTR lpszItem )
{
	// insert an new item ( list )
	ASSERT( !(m_dwStyle&TLC_TREELIST) );
	CTreeListItem* pItem = NULL;
	int iItem = -1;
	if( m_pRootItem->m_pChild == NULL )
	{
		pItem = InsertItemNew( lpszItem, m_pRootItem );
		if (!pItem)
			return -1;
		iItem = 0;
	}
	else if( nItem == 0 )
	{
		pItem = InsertItemFirst( lpszItem, m_pRootItem );
		if (!pItem)
			return -1;
		iItem = 0;
	}
	else if( nItem >= GetCount() )
	{
		pItem = InsertItemAux( lpszItem, TLI_ROOT, TLI_LAST );
		if (!pItem)
			return -1;
		iItem = GetTreeListItem( pItem );
	}
	else
	{
		pItem = GetTreeListItem( nItem );
		pItem = InsertItemAux( lpszItem, TLI_ROOT, pItem );
		if (!pItem)
			return -1;
		iItem = GetTreeListItem( pItem );
	}
		ITEM_OPERATION_INFO info;
		info.item = pItem;
		itemOperationNotifyListener_->onInsertItem(*this, &info);
	return iItem;
}

CTreeListItem* CTreeListCtrl::InsertItemAux(LPCTSTR lpszItem, 
											CTreeListItem* pParent, 
											CTreeListItem* pInsertAfter )
{
	// insert an new item
	ASSERT( m_dwStyle&TLC_TREELIST || pParent == TLI_ROOT || pParent == m_pRootItem ) ;

	// add first level item
	if( pParent == TLI_ROOT )
	{
		return InsertItemAux( lpszItem, m_pRootItem, pInsertAfter );
	}

	// no child item
	if( pParent->m_pChild == NULL )
	{
		return InsertItemNew( lpszItem, pParent );
	}

	// add befor first child item
	if( pInsertAfter == TLI_FIRST )
	{
		return InsertItemFirst( lpszItem, pParent );
	}

	// add befor or after a child item by sorting
	if( pInsertAfter == TLI_SORT )
	{
		CTreeListItem* pPrevItem = GetSortChildItem( pParent );
		if( pPrevItem == NULL )
			return InsertItemFirst( lpszItem, pParent );
		else
			return InsertItemNext( lpszItem, pParent, pPrevItem );
	}

	// add after a child item
	if( pInsertAfter == TLI_LAST )
	{
		CTreeListItem* pPrevItem = GetLastChildItem( pParent );
		return InsertItemNext( lpszItem, pParent, pPrevItem );
	}

	// add after some child item
	{
		CTreeListItem* pPrevItem = GetValidChildItem( pParent, pInsertAfter );
		if( pPrevItem == NULL )
			return InsertItemFirst( lpszItem, pParent );
		else
			return InsertItemNext( lpszItem, pParent, pPrevItem );
	}

	return NULL;
}

CTreeListItem* CTreeListCtrl::InsertItem( LPCTSTR lpszItem, 
										 CTreeListItem* pParent, 
										 CTreeListItem* pInsertAfter )
{
	CTreeListItem* pItem = InsertItemAux(lpszItem, pParent, pInsertAfter);
	if (!pItem)
		return NULL;
	ITEM_OPERATION_INFO info;
	info.item = pItem;
	itemOperationNotifyListener_->onInsertItem(*this, &info);

	return pItem;
}

bool CTreeListCtrl::DeleteItem( int nItem )
{
	// delete an item ( list )
	ASSERT( !(m_dwStyle&TLC_TREELIST) );
	CTreeListItem* pItem;

	pItem = GetTreeListItem( nItem );

	if( pItem == NULL )
		return FALSE;

	return DeleteItem( pItem );
}


bool CTreeListCtrl::DeleteItem( CTreeListItem* pItem )
{
	ASSERT(pItem);
	// delete an item
	if( pItem == TLI_ROOT )
	{
		CTreeListItem* pChild = m_pRootItem->m_pChild;
		while( pChild != NULL )
		{
			CTreeListItem * const pDelete = pChild;
			pChild = pChild->m_pNext;
			DeleteItemX( pDelete, TLCC_NOTIFY );
		}
	}
	else
	{
		DeleteItemX( pItem, TLCC_NOTIFY );
	}

	if( IsWindow( GetSafeHwnd() ) )
	{
		Layout();
		Invalidate();
	}

	return true;
}

bool CTreeListCtrl::DeleteAllItems()
{
	// delete all items of treelist control
	BOOL bRedraw = IsWindowVisible();
	
	if( bRedraw )
		SetRedraw( FALSE );

	bool bDelte = true;
	if (m_dwStyle&TLC_TREELIST) 
		DeleteItem( TLI_ROOT );
	else{//! если дерево работает как список
		DeleteAllListItems();
	}

	if( bRedraw )
		SetRedraw( TRUE );
	Invalidate();

	return true;
}

bool CTreeListCtrl::Expand( CTreeListItem* pItem, int nCode )
{
	// expand an item
	return Expand( pItem, nCode, m_iSelCol );
}

bool CTreeListCtrl::Expand( CTreeListItem* pItem, int nCode, int iSubItem )
{
	// expand an item or collapse an item
	ASSERT( m_dwStyle&TLC_TREELIST );

	if( pItem == NULL )
		return FALSE;

	if( pItem->m_pChild == NULL )
		return true;

	if( nCode == TLE_TOGGLE )
	{
		if( pItem->GetState()&TLIS_EXPANDED )
			nCode = TLE_COLLAPSE;
		else
			nCode = TLE_EXPAND;
	}

	switch( nCode )
	{
	case TLE_EXPAND:
		if( pItem->GetState()&TLIS_EXPANDED )
			return true;
		break;

	case TLE_COLLAPSE:
	case TLE_COLLAPSERESET:
		if( !(pItem->GetState()&TLIS_EXPANDED ) )
			return true;
		break;

	default:
		return false;
		break;
	}

	ITEM_CHANGE_INFO info;
	info.item = pItem;
	info.iOldSubItem = -1;
	info.iNewSubItem = iSubItem;
	info.uNewState = 0;
	info.uOldState = 0;
	switch(nCode) {
	case TLE_EXPAND:
		info.uNewState = TLIS_EXPANDED;
		if (!(pItem->GetState()&TLIS_EXPANDEDONCE))
			info.uNewState |= TLIS_EXPANDEDONCE;
		break;

	case TLE_COLLAPSERESET:
		info.uOldState |= (pItem->GetState()&TLIS_EXPANDEDONCE);
		break;
	}
	info.uOldState |= pItem->GetState()&TLIS_EXPANDED;
	if (!itemChangeNotifyListener_->onItemExpanding(*this, &info))
		return false;

	BOOL bRedraw = IsWindowVisible();
	
	if( bRedraw )
		SetRedraw( FALSE );

	bool bExpand = FALSE;

	switch( nCode )
	{
	case TLE_EXPAND:
		pItem->Expand();
		StatExpand( pItem );
		break;

	case TLE_COLLAPSE:
		pItem->Collapse();
		StatCollapse( pItem );
		break;

	case TLE_COLLAPSERESET:
		pItem->Collapse();
		while( pItem->m_pChild != NULL )
			DeleteItem( pItem->m_pChild );
		StatCollapse( pItem );
		break;

	default:
		ASSERT( FALSE );
		break;
	}

	//SelectItem( pItem, iSubItem );
	if( bRedraw )
		SetRedraw( TRUE );

	Layout();
	Invalidate();

	itemChangeNotifyListener_->onItemExpanded(*this, &info);

	return bExpand;
}

bool CTreeListCtrl::Select( CTreeListItem* pItem, int nCode )
{
	// select an item
	SelectItem( pItem, 0, SI_SELECT, FALSE );
	return TRUE;
}

CTreeListItem* CTreeListCtrl::InsertItemNew( LPCTSTR lpszItem, CTreeListItem* pParent )
{
	// insert a new item
	ASSERT( pParent != NULL );

	CTreeListItem* pNewItem;
	pNewItem = new CTreeListItem(GetColumns());

	pNewItem->SetText( lpszItem );

	pNewItem->m_pParent	= pParent;
	pNewItem->m_nLevel	= pParent->m_nLevel + 1;


	pParent->m_pChild	= pNewItem;
	pParent->GetState( TLIS_EXPANDEDONCE );

	return InsertItemX( pParent, pNewItem );
}

CTreeListItem* CTreeListCtrl::InsertItemFirst( LPCTSTR lpszItem, CTreeListItem* pParent )
{
	// insert a child item before first child item
	ASSERT( pParent != NULL );

	CTreeListItem* pNewItem = new CTreeListItem(GetColumns());

	pNewItem->SetText( lpszItem );

	CTreeListItem* pFirstItem;
	pFirstItem	= pParent->m_pChild;

	pNewItem->m_pParent = pFirstItem->m_pParent;
	pNewItem->m_pNext	= pFirstItem;
	pNewItem->m_nLevel	= pFirstItem->m_nLevel;

	pFirstItem->m_pPrev	= pNewItem;
	pParent->m_pChild	= pNewItem;

	return InsertItemX( pParent, pNewItem );
}

CTreeListItem* CTreeListCtrl::InsertItemNext( LPCTSTR lpszItem, 
											 CTreeListItem* pParent, 
											 CTreeListItem* pInsertAfter )
{
	// insert a child item after some item
	ASSERT( pParent != NULL && pInsertAfter != NULL );

	CTreeListItem* pNewItem = new CTreeListItem(GetColumns());

	pNewItem->SetText( lpszItem );

	pNewItem->m_pParent = pInsertAfter->m_pParent;
	pNewItem->m_pNext	= pInsertAfter->m_pNext;
	pNewItem->m_pPrev	= pInsertAfter;
	pNewItem->m_nLevel	= pInsertAfter->m_nLevel;

	pInsertAfter->m_pNext	= pNewItem;

	if( pNewItem->m_pNext != NULL )
		pNewItem->m_pNext->m_pPrev = pNewItem;

	return InsertItemX( pParent, pNewItem );
}

CTreeListItem* CTreeListCtrl::InsertItemX( CTreeListItem* pParent, 
										  CTreeListItem* pNewItem )
{
	// insert an item
	ASSERT( pParent != NULL );
	ASSERT( pNewItem != NULL );

	if ((GetStyle()&TLC_CHECKBOX)&&!(GetStyle()&TLC_NOAUTOCHECK))
	{
		ShowItemCheckBox(pNewItem, true);
		SetItemCheck(pNewItem, pParent->GetCheck(), FALSE);
	}

	if ((GetStyle()&TLC_LOCKBOX)&&!(GetStyle()&TLC_NOAUTOLOCK))
	{
		ShowItemLockBox(pNewItem, true);
		SetItemCheck(pNewItem, pParent->GetCheck(), FALSE);
	}

	StatChildAdd( pNewItem, pNewItem->m_nChild, pNewItem->m_nVisibleChild );

//	pNewItem->SetImage( 0, 1, 2, 3 );

	Layout();
	Invalidate();

	return pNewItem;
}

bool CTreeListCtrl::DeleteAllListItems()
{
	ITEM_OPERATION_INFO info;
	info.item = NULL;
	bool const bNotify = !itemOperationNotifyListener_->onDeleteAllItems(*this, &info);

	CTreeListItem* pChild = m_pRootItem->m_pChild;
	CTreeListItem* pDelete;
	while( pChild != NULL )
	{
		pDelete = pChild;
		pChild = pChild->m_pNext;
		DeleteItemX( pDelete, bNotify? TLCC_NOTIFY : TLCC_NONOTIFY );
	}
	return true;
}

bool CTreeListCtrl::DeleteItemX( CTreeListItem* pItem, int flag )
{
	// delete an item
	if( pItem->m_pPrev == NULL && pItem->m_pNext == NULL )
	{
		// no brother
		pItem->m_pParent->m_pChild = NULL;
	}
	else if( pItem->m_pPrev == NULL )
	{
		// first item of parent
		pItem->m_pParent->m_pChild = pItem->m_pNext;
		pItem->m_pNext->m_pPrev = NULL;
	}
	else if( pItem->m_pNext == NULL )
	{
		// last item of parent
		pItem->m_pPrev->m_pNext = NULL;
	}
	else
	{
		// middle item of parent
		pItem->m_pPrev->m_pNext = pItem->m_pNext;
		pItem->m_pNext->m_pPrev = pItem->m_pPrev;
	}

	StatChildDel( pItem, pItem->m_nChild, pItem->m_nVisibleChild );

	DeleteItemFast( pItem, flag );

	return true;
}

bool CTreeListCtrl::DeleteItemFast( CTreeListItem* pItem, int flag )
{
	// fast delete child items of pItem
	CTreeListItem* pChild = pItem->m_pChild;
	while( pChild != NULL )
	{
		CTreeListItem* pDelete = pChild;
		pChild = pChild->m_pNext;
		DeleteItemFast( pDelete, flag );
	}

	ITEM_CHANGE_INFO info;
	info.uOldState	= 0;

	ASSERT(pItem);
	if (pItem->GetSelected())
	{
		INT_PTR iItem = FindInSelected(pItem);
		info.uOldState	|= TLIS_SELECTED;
		assert(iItem != -1);
		pItem->SetSelected(false);
		m_arSelects.RemoveAt(iItem);
	}

	if( pItem == GetFocusedItem() )
	{
		m_pFocusItem = NULL;
		info.uOldState |= TLIS_FOCUSED;

		if (m_arSelects.GetSize()) 
			SetFocusedItem(reinterpret_cast<CTreeListItem*>(m_arSelects[0]));
	}

	if (pItem == GetShiftSelectionBase())
		SetShiftSelectionBase(GetFirstVisibleItem());
	if (pItem == GetReadyToEditItem().first)
		SetReadyToEditItem(NULL, -1);
	if (flag != TLCC_NONOTIFY) 
	{
		info.item		= pItem;
		info.iOldSubItem = -1;
		info.iNewSubItem = -1;
		info.uNewState	= 0;
		itemChangeNotifyListener_->onItemStateChanged(*this, &info);

		ITEM_OPERATION_INFO info2;
		info2.item = pItem;
		itemOperationNotifyListener_->onDeleteItem(*this, &info2);
	}
	delete pItem;

	return true;
}

void CTreeListCtrl::SetTargetItem( CTreeListItem* pItem )
{
	// set drop target item
	m_pTargetItem = pItem;
	Invalidate();
}

void CTreeListCtrl::SetHoverItem( CTreeListItem* pItem )
{
	// set hover target item
	m_pHoverItem = pItem;
	m_iHoverItem = GetTreeListItem( pItem );
	Invalidate();
}

CTreeListItem* CTreeListCtrl::GetModifiedItem() const{
	return m_pModifyItem;
}
void CTreeListCtrl::SetModifiedItem(CTreeListItem* pItem){
	m_pModifyItem = pItem;
}

CTreeListItem * CTreeListCtrl::GetFocusedItem() const{
	return m_pFocusItem;
}

CTreeListItem* CTreeListCtrl::GetDropTargetItem() const{
	return m_pTargetItem;
}

void CTreeListCtrl::SetReadyToEditItem(CTreeListItem* item, int iSubItem){
	readyToEditItem_.first = item;
	readyToEditItem_.second = iSubItem;
}
std::pair<CTreeListItem*, int> const&  CTreeListCtrl::GetReadyToEditItem() const{
	return readyToEditItem_;
}

void CTreeListCtrl::SetFocusedItem(CTreeListItem* pFocusItem){
	if (m_pFocusItem == pFocusItem)
		return;
	if (m_pFocusItem)
	{
		ITEM_CHANGE_INFO info;
		info.item		= m_pFocusItem;
		info.iOldSubItem = -1;
		info.iNewSubItem = -1;
		info.uNewState	= 0;
		info.uOldState	= TLIS_FOCUSED;
		if (!itemChangeNotifyListener_->onItemStateChanging(*this, &info))
			return;
		m_pFocusItem->SetFocused(false);
		itemChangeNotifyListener_->onItemStateChanged(*this, &info);
	}
	m_pFocusItem = pFocusItem;
	if (pFocusItem)
	{
		ITEM_CHANGE_INFO info;
		info.item		= pFocusItem;
		info.iOldSubItem = -1;
		info.iNewSubItem = -1;
		info.uNewState	= TLIS_FOCUSED;
		info.uOldState	= pFocusItem->GetState(TLIS_FOCUSED);
		if (!itemChangeNotifyListener_->onItemStateChanging(*this, &info))
			return ;

		m_pFocusItem->SetFocused(true);
		itemChangeNotifyListener_->onItemStateChanged(*this, &info);
	}
}

CTreeListItem* CTreeListCtrl::GetShiftSelectionBase() const{
	return m_pShiftSelectionBase;
}
void CTreeListCtrl::SetShiftSelectionBase(CTreeListItem* pitem){
	m_pShiftSelectionBase = pitem;
}

INT_PTR	CTreeListCtrl::FindInSelected(CTreeListItem const* pItem) const
{
	for(INT_PTR iItem=0; iItem<m_arSelects.GetSize(); iItem++ )
	{
		if( (void*)pItem == m_arSelects[iItem] )
			return iItem;
	}
	return -1;
}

INT_PTR	CTreeListCtrl::FindInShowedColumns(int iCol) const{
	for( int iShow=0; iShow<m_arShows.GetSize(); iShow++ )
	{
		if( m_arShows[iShow] == iCol )
			return iShow;
	}
	return -1;
}

bool CTreeListCtrl::IsColumnVisible(int iCol) const{
	return FindInShowedColumns(iCol) != -1;
}

void CTreeListCtrl::DeselectAll(CTreeListItem const* pItem)
{
	if (pItem)
	{
		INT_PTR iIdx = FindInSelected(pItem);
		if (iIdx != -1)
			m_arSelects.RemoveAt(iIdx);
	}
	CPtrArray arr;
	arr.Copy(m_arSelects);

	m_arSelects.RemoveAll();
	if (pItem)
		m_arSelects.Add(const_cast<CTreeListItem*>(pItem));

	for(INT_PTR iItem = arr.GetSize(); --iItem >= 0; )
	{
		CTreeListItem* pi = reinterpret_cast<CTreeListItem*>(arr[iItem]);
		if (!SelectItemAux(pi, 0, SI_DESELECT))
			m_arSelects.Add(pi);//У некоторых могли откзаться снимать выделение
	}
}

void CTreeListCtrl::SelectItemOnKeyPressing(CTreeListItem* pItem, int iSubItem)
{
	if ((GetStyle()&TLC_MULTIPLESELECT))
	{
		bool const bControlPressed = IsKeyPressed(VK_CONTROL);
		bool const bShiftPressed = IsKeyPressed(VK_SHIFT);
		if (bControlPressed&&!bShiftPressed){
			SetFocusedItem(pItem);
			Invalidate();
		}
		else if (bShiftPressed&&!bControlPressed) 
				ShiftSelection(pItem, m_iSelCol);
		else if (bShiftPressed&&bControlPressed)
				ShiftSelection(pItem, m_iSelCol, false);
		else
			SelectItem(pItem, m_iSelCol, SI_SELECT, TRUE);
	}
	else
		SelectItem(pItem, m_iSelCol, SI_SELECT, TRUE);
}
void CTreeListCtrl::ShiftSelection(CTreeListItem* pItem, 
								   int iSubItem, 
								   bool bClearSelection/* = true*/)
{
	if (!(m_dwStyle&TLC_MULTIPLESELECT))
	{
		SelectItem(pItem, iSubItem, SI_SELECT, TRUE);
		return;
	}
	if (!GetShiftSelectionBase())
		SetShiftSelectionBase(GetFirstVisibleItem());

	CTreeListItem *selectionBase = GetShiftSelectionBase();
	if (m_arSelects.GetSize() == 1&&selectionBase == pItem) 
	{
		//если выделен только один и кликнули на него
		//ничего не делаем
		return;
	}
	if (bClearSelection) 
		DeselectAll(NULL);
	eItemLayout const layout = IsItemAbove(selectionBase, pItem);
	int const nextItemDirection = (layout == EIL_ABOVE)? TLGN_PREVIOUSVISIBLE: 
														 TLGN_NEXTVISIBLE;
	CTreeListItem* nextItem = selectionBase;
	do {
		SelectItemAux(nextItem, iSubItem, SI_SELECT);
		if (nextItem == pItem)
			break;
		nextItem = GetNextItem(nextItem, nextItemDirection);
	} while(nextItem);
	SetFocusedItem(pItem);
	Invalidate();
}

void CTreeListCtrl::SelectItem( CTreeListItem* pItem, int iSubItem, 
							    eSelectAction eAction, bool bClearLastSelected )
{
	if (eAction == SI_SELECT)
	{
		// select a new item
		if( pItem == NULL )
		{
			DeselectAll(NULL);
			Invalidate();
			return;
		}

		// select a new item
		if( !(m_dwStyle&TLC_MULTIPLESELECT) )
		{
			// single select
			ASSERT( m_arSelects.GetSize() <= 1 );

			if( m_arSelects.GetSize() == 1 )
			{
				if (m_arSelects[0] == pItem)
				{
					if (m_iSelCol != iSubItem)
					{
						SelectItemAux(pItem, iSubItem, SI_SELECT);
						SetFocusedItem(pItem);
					}
				}
				else if (SelectItemAux(reinterpret_cast<CTreeListItem*>(m_arSelects[0]), 
										0, SI_DESELECT))
				{
					if (SelectItemAux(pItem, iSubItem, SI_SELECT))
						SetFocusedItem(pItem);
				}
			}
			else if (SelectItemAux(pItem, iSubItem, SI_SELECT))
					SetFocusedItem(pItem);
		}
		else
		{
			// multi select
			if ( bClearLastSelected )
				DeselectAll((pItem->GetSelected()? pItem : NULL));

			if (!pItem->GetSelected()||m_iSelCol != iSubItem)
			{
				if (SelectItemAux(pItem, iSubItem, SI_SELECT))
					SetFocusedItem(pItem);
			}
			else
				SetFocusedItem(pItem);
		}
		SetShiftSelectionBase(pItem);
	}
	else if (eAction == SI_DESELECT)
		SelectItemAux(pItem, 0, SI_DESELECT);

	Invalidate();
	return;
}

bool CTreeListCtrl::SelectItemAux(CTreeListItem* pItem, 
							   int iSubItem /* = 0 */, 
							   eSelectAction eAction /* = SI_SELECT */)
{

	switch(eAction) {
	case SI_SELECT:
		{
			ITEM_CHANGE_INFO info;
			info.item = pItem;
			info.iOldSubItem = -1;
			info.iNewSubItem = -1;
			if (GetStyle()&TLC_SHOWSELACTIVE)
			{
				info.iOldSubItem = m_iSelCol;
				info.iNewSubItem = iSubItem;
			}
			info.uOldState = pItem->GetState(TLIS_SELECTED);
			info.uNewState = TLIS_SELECTED;
			if (!itemChangeNotifyListener_->onItemStateChanging(*this, &info))
				return false;
			if (!pItem->GetSelected())
			{
				m_arSelects.Add(pItem);
				pItem->SetSelected(TRUE);
				if (m_arSelects.GetSize() > 1)
					SwapArrayElements(m_arSelects, 0, m_arSelects.GetSize()-1);
			}
			else
				SwapArrayElements(m_arSelects, 0, FindInSelected(pItem));


			m_iSelCol = iSubItem;
			itemChangeNotifyListener_->onItemStateChanged(*this, &info);
		}
		break;
	case SI_DESELECT:
		{
			ITEM_CHANGE_INFO info;
			info.item = pItem;
			info.iOldSubItem = -1;
			info.iNewSubItem = -1;

			info.uOldState = pItem->GetState(TLIS_SELECTED);
			info.uNewState = 0;

			bool const notify = pItem->GetSelected();
			if (notify&&!itemChangeNotifyListener_->onItemStateChanging(*this, &info))
				return false;
			INT_PTR iItem = FindInSelected(pItem);
			ASSERT(pItem);
			if (iItem != -1)
				m_arSelects.RemoveAt(iItem);
			pItem->SetSelected(FALSE);
			if (notify)
				itemChangeNotifyListener_->onItemStateChanged(*this, &info);
		}
		break;
	}
	return true;
}

CTreeListCtrl::eItemLayout CTreeListCtrl::IsItemAbove(CTreeListItem* pBaseItem, 
													  CTreeListItem* pTestItem)
{
	CRect rc1;
	GetItemRect(pBaseItem, &rc1);
	int const baseTop = rc1.top;
	GetItemRect(pTestItem, &rc1);
	if (baseTop < rc1.top)
		return EIL_BELOW;
	return EIL_ABOVE;
}

/*
void CTreeListCtrl::SetImageList( CTreeListItem* pItem, CImageList* pImageList )
{
	// 
	ASSERT( FALSE );

	pItem->SetImageList( pImageList );

	return;
}
*/

CTreeListItem* CTreeListCtrl::GetLastChildItem( CTreeListItem* pParent )
{
	// retrieve last child item
	if( pParent == TLI_ROOT )
		return GetLastChildItem( m_pRootItem );

	if( pParent->m_pChild == NULL )
		return NULL;

	CTreeListItem* pChildItem;
	pChildItem = pParent->m_pChild;

	while( pChildItem->m_pNext != NULL )
	{
		pChildItem = pChildItem->m_pNext;
	}

	return pChildItem;
}

CTreeListItem* CTreeListCtrl::GetSortChildItem( CTreeListItem* pParent )
{
	// retrieve sort child item
	if( pParent == TLI_ROOT )
		return GetSortChildItem( m_pRootItem );

	return GetLastChildItem( pParent );
}

CTreeListItem* CTreeListCtrl::GetValidChildItem( CTreeListItem* pParent, CTreeListItem* pChild )
{
	// is it a valid child item
	ASSERT( pChild != NULL );

	if( pParent == TLI_ROOT )
	{
		return GetValidChildItem( m_pRootItem, pChild );
	}

	if( pParent->m_pChild == NULL )
	{
		return NULL;
	}

	if( pChild == NULL )
	{
		return NULL;
	}

	bool bValid = FALSE;
	CTreeListItem* pChildItem;
	pChildItem = pParent->m_pChild;

	while( pChildItem != NULL )
	{
		if( pChildItem == pChild )
		{
			bValid = TRUE;
			break;
		}
		pChildItem = pChildItem->m_pNext;
	}
	
	if( !bValid )
	{
		return NULL;
	}

	return pChild;
}

void CTreeListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( GetState( TLS_DRAG ) )
	{
		ASSERT( GetState( TLS_CAPTURE ) );
		DoDraging( point );
	}
	else
	{
		// on mouse move
		int nFlag = 0, iSubItem = 0;
		CRect rcText;

		CTreeListItem* pItem = HitTest( point, &nFlag, &iSubItem, &rcText );
		if( pItem != NULL )
		{
			if( !(nFlags&(MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) )
			{
				SetTimer(SHOW_TIP_TIMER_ID, 300, NULL);
			}
			else
			{
				if( IsWindow( m_wndTip->GetSafeHwnd() ) && m_wndTip->IsWindowVisible() )
					m_wndTip->Hide();
			}

//			if( nFlag&TLHT_ONITEMBUTTON )
//			{
//				SetCursor( m_hButton );
//			} 
//
//			if( nFlag&TLHT_ONITEMCHECKBOX )
//			{
//				SetCursor( m_hCheck );
//			}
//
//			if( nFlag&TLHT_ONITEMLOCKBOX )
//			{
//				SetCursor( m_hLock );
//			}
		}
		else
		{
			if( IsWindow( m_wndTip->GetSafeHwnd() ) )
				m_wndTip->Hide();
		}

	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CTreeListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// left button was down
	SetFocus();

	CWnd::OnLButtonDown( nFlags, point );

	m_wndTip->Hide();

	if (GetModifiedItem())
		EndLabelEdit(true);

	focusWasLost_ = false;

	int nFlag = 0, iSubItem = 0;
	CRect rcText;

	CTreeListItem* pItem = HitTest( point, &nFlag, &iSubItem, &rcText );

	if( pItem != NULL )
	{
		if( m_dwStyle&TLC_TREELIST )
		{
			if( nFlag&TLHT_ONITEMBUTTON )
			{
				if( pItem->GetState() & TLIS_EXPANDED )
				{
					Expand( pItem, TLE_COLLAPSE, iSubItem );
				}
				else
				{
					Expand( pItem, TLE_EXPAND, iSubItem );
				}
//				SetCursor( m_hButton );
			}
		}

		if( nFlag&TLHT_ONITEMCHECKBOX )
		{
			SetItemCheck( pItem, false, true );
//			SetCursor( m_hCheck );
		}

		if( nFlag&TLHT_ONITEMLOCKBOX )
		{
			SetItemLock( pItem, false, true);
//			SetCursor( m_hLock );
		}

		if( nFlag&TLHT_ONITEM 
			&& !(nFlag&TLHT_ONITEMBUTTON) 
			&& !(nFlag&TLHT_ONITEMCHECKBOX) 
			&& !(nFlag&TLHT_ONITEMLOCKBOX)
			&& !GetState( TLS_MODIFY ))
		{
			bool bDraging = FALSE;
			bool const wasSelected = pItem->GetSelected();
			int const iOldColumn = m_iSelCol;
			if ((GetStyle()&TLC_MULTIPLESELECT))
			{
				if (!pItem->GetSelected())
				{
					if (nFlags&MK_CONTROL&&!(nFlags&MK_SHIFT))
					{
						SelectItem(
								pItem, 
								iSubItem, 
								(pItem->GetSelected()?SI_DESELECT:SI_SELECT), 
								FALSE
								);
					}
					else if (nFlags&MK_SHIFT&&!(nFlags&MK_CONTROL))
						ShiftSelection(pItem, iSubItem);
					else if (nFlags&MK_SHIFT&&(nFlags&MK_CONTROL))
					{
						ShiftSelection(pItem, iSubItem, false);
					}
					else
						SelectItem(pItem, iSubItem, SI_SELECT, true);
				}
			}
			else
				SelectItem(pItem, iSubItem, SI_SELECT, TRUE);
			if (wasSelected
				&&pItem->GetSelected()
				&&pItem->GetFocused()
				&&iOldColumn == iSubItem)
			{
				SetReadyToEditItem(pItem, iSubItem);
			}

			Layout();
			UpdateWindow();

			if( pItem->GetSelected() )
				bDraging = !focusWasLost_&&BeginDraging( point, DB_LBUTTON );
			if( bDraging )
			{
				ASSERT( !GetState( TLS_CAPTURE ) );
				SetState( TLS_CAPTURE );
				SetCapture();
				DoDraging(point);
			}
			else
			{
				if ((GetStyle()&TLC_MULTIPLESELECT))
				{
					if (wasSelected)
					{
						if (nFlags&MK_CONTROL&&!(nFlags&MK_SHIFT))
						{
							if (m_iSelCol!=iSubItem)
								SelectItem(pItem, iSubItem, SI_SELECT, FALSE);
							else
							{
								SelectItem(pItem, iSubItem, SI_DESELECT, FALSE);
								SetFocusedItem(pItem);
							}
						}
						else if (nFlags&MK_SHIFT&&!(nFlags&MK_CONTROL)) 
							ShiftSelection(pItem, iSubItem);
						else if (nFlags&MK_SHIFT&&(nFlags&MK_CONTROL)) 
							ShiftSelection(pItem, iSubItem, false);
						else
							SelectItem(pItem, iSubItem, SI_SELECT, TRUE);
						Invalidate();
					}
				}
			}
		}

	}
	else
	{
		if (!(nFlags&MK_CONTROL)&&!GetState(TLS_MODIFY))
			SelectItem( NULL );
		Layout();
		Invalidate();
	}
}

void CTreeListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// left button was up

	if( GetState( TLS_DRAG ) )
	{
		ReleaseCapture();
		SetState( 0, TLS_CAPTURE );

		EndDraging( point );
		Invalidate();
	}
	else
	{
		int nFlag, iSubItem;
		CTreeListItem* pItem = HitTest( point, &nFlag, &iSubItem );

		GENERAL_NOTIFY_INFO info;
		info.item = pItem;
		info.iSubItem = iSubItem;
		info.nFlags = nFlag;
		generalNotifyListener_->onClick(*this, &info);

		if( pItem != NULL )
		{
//			if( nFlag&TLHT_ONITEMBUTTON )
//			{
//				SetCursor( m_hButton );
//			}
//
//			if( nFlag&TLHT_ONITEMCHECKBOX )
//			{
//				SetCursor( m_hCheck );
//			}
//
//			if( nFlag&TLHT_ONITEMLOCKBOX )
//			{
//				SetCursor( m_hLock );
//			}
			if( nFlag&TLHT_ONITEMTEXT)
			{
				if (pItem == GetReadyToEditItem().first
					&&iSubItem == GetReadyToEditItem().second
					&&!(nFlags&MK_CONTROL)&&!(nFlags&MK_SHIFT))
				{
					SetTimer(BEGIN_MODIFY_TIMER, BEGIN_MODIFY_TIMER_TIME, NULL);
				}
			}
		}
	}
	
	ASSERT( !GetState( TLS_CAPTURE ) );

	CWnd::OnLButtonUp(nFlags, point);
}

void CTreeListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// left button double click
	CWnd::OnLButtonDblClk( nFlags, point );

	// Для того чтобы предотвратить редактирование элемента, по которому кликнули 2 раза
	SetReadyToEditItem(NULL, -1);
	KillTimer(BEGIN_MODIFY_TIMER);

	int nFlag = 0, iSubItem = -1;
	CTreeListItem* pItem = HitTest( point, &nFlag, &iSubItem );
	GENERAL_NOTIFY_INFO info;
	info.item = pItem;
	info.iSubItem = iSubItem;
	info.nFlags = nFlag;
	if (!generalNotifyListener_->onDBLClick(*this, &info))
		return;
	if( pItem != NULL )
	{
//		if( nFlag&TLHT_ONITEMBUTTON )
//		{
//			SetCursor( m_hButton );
//		}
//
//		if( nFlag&TLHT_ONITEMCHECKBOX )
//		{
//			SetCursor( m_hCheck );
//		}
//
//		if( nFlag&TLHT_ONITEMLOCKBOX )
//		{
//			SetCursor( m_hLock );
//		}

		if( m_dwStyle&TLC_TREELIST )
		{
			if ( nFlag&TLHT_ONITEMIMAGE || nFlag&TLHT_ONITEMTEXT )
			{
				//SelectItem( pItem, iSubItem );
				Expand( pItem, TLE_TOGGLE, iSubItem );
			}
		}

		Layout();
		Invalidate();
	}

}

void CTreeListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// right button was down
	SetFocus();


	CWnd::OnRButtonDown(nFlags, point);
//посутпаем как в CListCtrl
	if (GetModifiedItem())
		EndLabelEdit(true);

	focusWasLost_ = false;

	int nFlag, iSubItem;
	CRect rcText;
	CTreeListItem* pItem = HitTest( point, &nFlag, &iSubItem, &rcText );

	if( pItem != NULL )
	{
		if (GetStyle()&TLC_MULTIPLESELECT)
		{
			if( nFlag&TLHT_ONITEM && !(nFlag&TLHT_ONITEMBUTTON) )
			{
				if( !(nFlags&MK_CONTROL)&&!(nFlags&MK_SHIFT) )
				{
					if (pItem->GetSelected()) 
						SelectItem( pItem, iSubItem, SI_SELECT, FALSE);
					else
						SelectItem( pItem, iSubItem, SI_SELECT, TRUE);
				}
			}
			Layout();
			RedrawWindow();

			if (GetSelectedCount())
			{
				bool bDraging = !focusWasLost_&&BeginDraging( point, DB_RBUTTON );

				if( bDraging )
				{
					if( !pItem->GetSelected()&&((nFlags&MK_CONTROL)||(nFlags&MK_SHIFT)) )
					{
						SelectItem( pItem, iSubItem, SI_SELECT,FALSE);
					}

					ASSERT( !GetState( TLS_CAPTURE ) );
					SetState( TLS_CAPTURE );
					SetCapture();
					DoDraging(point);
				}
			}
		}
		else
		{
			if( nFlag&TLHT_ONITEM && !(nFlag&TLHT_ONITEMBUTTON) )
			{
				if (pItem->GetSelected()) 
					SelectItem( pItem, iSubItem, SI_SELECT, FALSE);
				else
					SelectItem( pItem, iSubItem, SI_SELECT, TRUE);
			}
			Layout();
			RedrawWindow();

			bool bDraging = !focusWasLost_&&BeginDraging( point, DB_RBUTTON );

			if( bDraging )
			{
				if( !pItem->GetSelected()&&((nFlags&MK_CONTROL)||(nFlags&MK_SHIFT)) )
				{
					SelectItem( pItem, iSubItem, SI_SELECT,FALSE);
				}

				ASSERT( !GetState( TLS_CAPTURE ) );
				SetState( TLS_CAPTURE );
				SetCapture();
				DoDraging(point);
			}
		}
	}
	else
	{
		SelectItem( NULL );
	}
	Layout();
	Invalidate();
}

void CTreeListCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( GetState( TLS_DRAG ) )
	{
		ReleaseCapture();
		SetState( 0, TLS_CAPTURE );

		EndDraging( point );
		Invalidate();
	}
	else{
		// right button was up
		int nFlag, iSubItem = -1;
		CTreeListItem* pItem = HitTest( point, &nFlag, &iSubItem, NULL );
		GENERAL_NOTIFY_INFO info;
		info.item = pItem;
		info.iSubItem = iSubItem;
		info.nFlags = nFlag;
		generalNotifyListener_->onRClick(*this, &info);

	}
	CWnd::OnRButtonUp(nFlags, point);
}

void CTreeListCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	int nFlag, iSubItem = -1;
	CTreeListItem* pItem = HitTest( point, &nFlag, &iSubItem, NULL );
	GENERAL_NOTIFY_INFO info;
	info.item = pItem;
	info.iSubItem = iSubItem;
	info.nFlags = nFlag;
	generalNotifyListener_->onRDBLClick(*this, &info);

	CWnd::OnRButtonDblClk(nFlags,point);
}


CTreeListItem* CTreeListCtrl::HitTest( CPoint pt, 
									  int* pFlag, 
									  int* pSubItem, 
									  CRect* prcText )
{
	// hittest of all items
	int nFlag, nSubItem;
	CRect rcText;

	if( pFlag == NULL )
		pFlag = &nFlag;

	if( pSubItem == NULL )
		pSubItem = &nSubItem;

	if( prcText == NULL )
		prcText = &rcText;

	*pFlag		= 0;
	*pSubItem	= 0;
	prcText->SetRect( 0, 0, 0, 0 );

	int nFirstRow	=	GetScrollPos( SB_VERT );
	int	nShowRows	=	m_rcTreeList.Height() / GetItemHeight() + 1;

	CRect rcItem;
	rcItem.SetRect( 0, 0, GetWidth(), GetItemHeight() );
	rcItem.OffsetRect( m_rcTreeList.left, m_rcTreeList.top );
	rcItem.OffsetRect( -GetScrollPos( SB_HORZ ), 0 );

	for( int i = nFirstRow; i <= nFirstRow + nShowRows && i < GetVisibleCount(); i++ )
	{
		if( rcItem.PtInRect( pt ) )
		{
			CTreeListItem* pItem = GetFirstShowItem( i );
			ASSERT( pItem != NULL );

			return HitTest( pt, pFlag, pSubItem, prcText, rcItem, pItem );
		}
		rcItem.OffsetRect( 0, m_nItemHeight );
	}
	return NULL;
}

CTreeListItem* CTreeListCtrl::HitTest( CPoint pt, int* pFlag, int* pSubItem, CRect* prcText, CRect rcItem, CTreeListItem* pItem )
{
	// hittest of one item
	ASSERT( pFlag != NULL && pSubItem != NULL );

	int nColPos = 0;

	CTreeListColumnInfo* pColumnInfo = NULL;

	for( int iShow = 0; iShow < m_arShows.GetSize(); iShow++, nColPos += pColumnInfo->m_nWidth )
	{
		// do NOT test zero column;
		int iCol = m_arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
		if( pColumnInfo->m_nWidth == 0 )
			continue;

		CRect rcColumn;
		rcColumn.SetRect( 0, rcItem.top, pColumnInfo->m_nWidth, rcItem.bottom );
		rcColumn.OffsetRect( nColPos, 0 );
		rcColumn.OffsetRect( rcItem.left, 0 );

		if( rcColumn.PtInRect( pt ) )
		{
			*pSubItem = iCol;
			if( iCol == 0 )
			{
				if ( m_dwStyle&TLC_TREELIST )
					return HitTestTree( pt, pFlag, prcText, rcColumn, pItem );
				else
					return HitTestMain( pt, pFlag, prcText, rcColumn, pItem );
			}
			else
			{
				*prcText = rcColumn;
				return HitTestList( pt, pFlag, prcText, rcColumn, pItem );
			}
		}
	}

	return NULL;
}

CTreeListItem* CTreeListCtrl::HitTestTree( CPoint pt, int* pFlag, CRect* prcText, CRect rcColumn, CTreeListItem* pItem )
{
	// hittest of tree column of treelist control
	ASSERT( pFlag != NULL );
	ASSERT( pItem != NULL );

	int nPerfix = 0;
	CRect rcGraph;
	
	// skip perfix and tree line
	nPerfix += TLL_BORDER;
	nPerfix += (pItem->m_nLevel-1) * TLL_WIDTH;

	// check tree button
	if( m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
	{
		if( /*!(pItem->GetState()&TLIS_EXPANDEDONCE) || */pItem->m_pChild != NULL )
		{
			rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH, rcColumn.bottom );
			rcGraph.OffsetRect( nPerfix, 0 );
			rcGraph.OffsetRect( rcColumn.left, 0 );

			CPoint ptCenter;
			ptCenter.x = rcGraph.left + rcGraph.Width() / 2 - 1;
			ptCenter.y = rcGraph.top + rcGraph.Height() / 2 - 1;

			CRect rcButton;
			rcButton.SetRect( -TLL_BUTTON, -TLL_BUTTON, TLL_BUTTON+1, TLL_BUTTON+1 );
			rcButton.OffsetRect( ptCenter );

			if( rcButton.PtInRect( pt ) )
				*pFlag |= TLHT_ONITEMBUTTON;
		}
		nPerfix += TLL_WIDTH;
	}

	// check tree check box;
	if( m_dwStyle&TLC_CHECKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWCHECKBOX )
		{
			rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH, rcColumn.bottom );
			rcGraph.OffsetRect( nPerfix, 0 );
			rcGraph.OffsetRect( rcColumn.left, 0 );

			if( rcGraph.PtInRect( pt ) )
				*pFlag |= TLHT_ONITEMCHECKBOX;

			nPerfix += TLL_WIDTH;
		}
	}

	// check tree lock box;
	if( m_dwStyle&TLC_LOCKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWLOCKBOX )
		{
			rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH/2, rcColumn.bottom );
			rcGraph.OffsetRect( nPerfix, 0 );
			rcGraph.OffsetRect( rcColumn.left, 0 );

			if( rcGraph.PtInRect( pt ) )
				*pFlag |= TLHT_ONITEMLOCKBOX;

			nPerfix += TLL_WIDTH/2;
		}
	}

	// check tree image
	if( m_dwStyle&TLC_IMAGE )
	{
		if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
		{
			rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH, rcColumn.bottom );
			rcGraph.OffsetRect( nPerfix, 0 );
			rcGraph.OffsetRect( rcColumn.left, 0 );
	
			if( rcGraph.PtInRect( pt ) )
				*pFlag |= TLHT_ONITEMIMAGE;

			nPerfix += TLL_WIDTH;
		}
	}

	// add gaps between text and image/lock 
	if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	// check tree text
	CRect rcText;
	rcText = rcColumn;
	rcText.left = rcText.left + nPerfix;

	if( rcText.Width() > 0 )
	{
		if( rcText.PtInRect( pt ) )
		{
			*pFlag |= TLHT_ONITEMTEXT;
			*prcText = rcText;
		}
	}

	if( *pFlag == 0 )
		*pFlag |= TLHT_ONITEMSPACE;

	return pItem;
}

CTreeListItem* CTreeListCtrl::HitTestMain( CPoint pt, int* pFlag, 
										  CRect* prcText, CRect rcColumn, 
										  CTreeListItem* pItem )
{
	// hittest of main column of list control
	ASSERT( pFlag != NULL );
	ASSERT( pItem != NULL );

	int nPerfix = 0;
	CRect rcGraph;
	
	// skip perfix and tree line
	nPerfix += TLL_BORDER;

	// check tree check box;
	if( m_dwStyle&TLC_CHECKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWCHECKBOX )
		{
			rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH, rcColumn.bottom );
			rcGraph.OffsetRect( nPerfix, 0 );
			rcGraph.OffsetRect( rcColumn.left, 0 );

			if( rcGraph.PtInRect( pt ) )
				*pFlag |= TLHT_ONITEMCHECKBOX;

			nPerfix += TLL_WIDTH;
		}
	}

	// check tree lock box;
	if( m_dwStyle&TLC_LOCKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWLOCKBOX )
		{
			rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH/2, rcColumn.bottom );
			rcGraph.OffsetRect( nPerfix, 0 );
			rcGraph.OffsetRect( rcColumn.left, 0 );

			if( rcGraph.PtInRect( pt ) )
				*pFlag |= TLHT_ONITEMLOCKBOX;

			nPerfix += TLL_WIDTH/2;
		}
	}

	if (GetStateImageList()&&pItem->GetStateImage())
	{
		rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH/2, rcColumn.bottom );
		rcGraph.OffsetRect( nPerfix+rcColumn.left, 0 );

		if( rcGraph.PtInRect( pt ) )
			*pFlag |= TLHT_ONITEMSTATEIMAGE;

		nPerfix += TLL_WIDTH/2;

	}
	// check tree image
	if( m_dwStyle&TLC_IMAGE )
	{
		if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
		{
			rcGraph.SetRect( 0, rcColumn.top, TLL_WIDTH, rcColumn.bottom );
			rcGraph.OffsetRect( nPerfix, 0 );
			rcGraph.OffsetRect( rcColumn.left, 0 );
	
			if( rcGraph.PtInRect( pt ) )
				*pFlag |= TLHT_ONITEMIMAGE;

			nPerfix += TLL_WIDTH;
		}
	}

	// add gaps between text and image/lock 
	if( m_dwStyle&TLC_LOCKBOX || m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	// check tree text
	CRect rcText;
	rcText = rcColumn;
	rcText.left = rcText.left + nPerfix;

	if( rcText.Width() > 0 )
	{
		if( rcText.PtInRect( pt ) )
		{
			*pFlag |= TLHT_ONITEMTEXT;
			*prcText = rcText;
		}
	}

	if( *pFlag == 0 )
		*pFlag |= TLHT_ONITEMSPACE;

	return pItem;}

CTreeListItem* CTreeListCtrl::HitTestList( CPoint pt, int* pFlag, CRect* prcText, CRect rcColumn, CTreeListItem* pItem )
{
	// hittest of other columns of treelist control
	ASSERT( pFlag != NULL );
	ASSERT( pItem != NULL );

	*pFlag = TLHT_ONITEMTEXT;
	*prcText = rcColumn;

	return pItem;
}

void CTreeListCtrl::OnSetFocus( CWnd* pOldWnd ) 
{
	// set focus
	CWnd::OnSetFocus( pOldWnd );
	
	SetCtrlFocus( pOldWnd, TRUE );
	Invalidate();

	generalNotifyListener_->onSetFocus(*this);

	return;
}

void CTreeListCtrl::OnKillFocus( CWnd* pNewWnd ) 
{
	// lost focus
	CWnd::OnKillFocus( pNewWnd );
	focusWasLost_ = true;
	SetCtrlFocus( pNewWnd, FALSE );
	Invalidate();

	generalNotifyListener_->onKillFocus(*this);

	return;
}

BOOL CTreeListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// pre translate some messages
	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:
		// cancel tips
		{
			POINTS points;
			CPoint pt;

			points = MAKEPOINTS( pMsg->lParam );
			pt.x = points.x;
			pt.y = points.y;

			if( m_rcHeader.PtInRect( pt ) )
			{
				if( IsWindow( m_wndTip->GetSafeHwnd() ) && m_wndTip->IsWindowVisible() )
					m_wndTip->Hide();
			}
		}
		break;

	case WM_LBUTTONDOWN:
		// cancel midified controls
		if( GetState( TLS_MODIFY ) )
		{
			POINTS points;
			CPoint pt;

			points = MAKEPOINTS( pMsg->lParam );
			pt.x = points.x;
			pt.y = points.y;

			if( pMsg->hwnd == m_wndHeader.GetSafeHwnd() && m_rcHeader.PtInRect( pt ) )
			{
				EndLabelEdit(true);
				return TRUE;
			}

			int nFlag;
			int iSubItem;
			CRect rcText;
			CTreeListItem* pItem = HitTest( pt, &nFlag, &iSubItem, &rcText );
			if( pMsg->hwnd == GetSafeHwnd() && pItem != NULL && pItem != m_pModifyItem )
			{
				EndLabelEdit(true);
				return TRUE;
			}
		}
		break;

	case WM_LBUTTONDBLCLK:
		// filter dbclk message on button & chanbox & lockbox of treelist control
		{
			CTreeListItem* pItem = NULL;
			POINTS points;
			CPoint pt;
			int nFlag;
			int iSubItem;

			points = MAKEPOINTS( pMsg->lParam );
			pt.x = points.x;
			pt.y = points.y;

			pItem = HitTest( pt, &nFlag, &iSubItem );
			if( pItem != NULL && nFlag&TLHT_ONITEMBUTTON )
			{
				pMsg->message = WM_LBUTTONDOWN;
				return PreTranslateMessage( pMsg );
			}
			if( pItem != NULL && nFlag&TLHT_ONITEMCHECKBOX )
			{
				pMsg->message = WM_LBUTTONDOWN;
				return PreTranslateMessage( pMsg );
			}

			if( pItem != NULL && nFlag&TLHT_ONITEMLOCKBOX )
			{
				pMsg->message = WM_LBUTTONDOWN;
				return PreTranslateMessage( pMsg );
			}
		}
		break;

	case WM_RBUTTONDOWN:
		// dragging...
		if( GetState( TLS_DRAG ) )
		{
			return TRUE;
		}

		// cancel modified control
		if( GetState( TLS_MODIFY ) )
		{
			POINTS points;
			CPoint pt;

			points = MAKEPOINTS( pMsg->lParam );
			pt.x = points.x;
			pt.y = points.y;

			if( pMsg->hwnd == m_wndHeader.GetSafeHwnd() && m_rcHeader.PtInRect( pt ) )
			{
				AbortLabelEdit();
				return TRUE;
			}
			
			int nFlag;
			int iSubItem;
			CRect rcText;
			CTreeListItem* pItem = HitTest( pt, &nFlag, &iSubItem, &rcText );
			if( pMsg->hwnd == GetSafeHwnd() && m_rcTreeList.PtInRect( pt ) )
			{
				if( pItem != m_pModifyItem )
				{
					AbortLabelEdit();
					return TRUE;
				}
				else
				{
					return TRUE;
				}
			}
		}
		break;

	case WM_RBUTTONDBLCLK:
		// filter dbclk message on button & chanbox & lockbox of treelist control
		if( pMsg->hwnd == GetSafeHwnd() )
		{
			pMsg->message = WM_RBUTTONDOWN;
			return PreTranslateMessage( pMsg );
		}
		break;

	case WM_NCLBUTTONDOWN:
		// cancel modified controls
		if( GetState( TLS_MODIFY ) )
		{
			AbortLabelEdit();
			PostMessage( WM_NCLBUTTONDOWN, pMsg->wParam, pMsg->lParam );
			PostMessage( WM_NCLBUTTONUP,   pMsg->wParam, pMsg->lParam );
			return TRUE;
		}
		break;

	case WM_NCRBUTTONDOWN:
		// cancel modified controls
		if( GetState( TLS_MODIFY ) )
		{
			AbortLabelEdit();
			PostMessage( WM_NCRBUTTONDOWN, pMsg->wParam, pMsg->lParam );
			PostMessage( WM_NCRBUTTONUP,   pMsg->wParam, pMsg->lParam );
			return TRUE;
		}
		break;

	case WM_KEYDOWN:
		// filter keydown mesages by status
		{
//			ASSERT( GetState( TLS_FOCUS ) );

			if( GetState( TLS_DRAG ) )
			{
				return TRUE;
			}

			if( !GetState( TLS_MODIFY ) )
			{
//				if( PreTranslateKeyDownMessage( pMsg ) )
//					return TRUE;
				;
			}
			else
			{
				if( PreTranslateKeyDownMessage2( pMsg ) )
					return TRUE;
			}
		}
		break;

	case WM_CHAR:
	case WM_SYSCHAR:
	case WM_DEADCHAR:
	case WM_SYSDEADCHAR:

	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:

	case WM_COMMAND:
	case WM_SYSCOMMAND:
//	case WM_MENUCOMMAND:
		// filter keyup mesages by status
		{
			if( GetState( TLS_DRAG ) )
			{
				return TRUE;
			}
		}

	default:
		break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

bool	CTreeListCtrl::KeyDownHandler(UINT_PTR nChar, UINT_PTR nRepCnt, UINT_PTR nFlags)
{
	if( m_arShows.GetSize() == 0 )
		return true;

	CTreeListItem* pItem = NULL;
	switch( nChar )
	{
	case VK_ADD:
		{
			if( GetStyle()&TLC_TREELIST )
			{
				Expand( GetFocusedItem(), TLE_EXPAND );
				return true;
			}
		}
		break;

	case VK_SUBTRACT:
		{
			if( GetStyle()&TLC_TREELIST )
			{
				Expand( GetFocusedItem(), TLE_COLLAPSE );
				return true;
			}
		}
		break;

	case VK_SPACE:
		{
			if( GetStyle()&TLC_TREELIST )
			{
				Expand( GetFocusedItem(), TLE_TOGGLE );
				return true;
			}
		}
		break;

	case VK_LEFT:
		{
			if( GetKeyState( VK_SHIFT ) < 0 && GetStyle()&TLC_TREELIST )
			{
				Expand( GetFocusedItem(), TLE_COLLAPSE );
				return true;
			}

			INT_PTR iShow = FindInShowedColumns(m_iSelCol);
			if( iShow == -1 )
				m_iSelCol = m_arShows[0];
			else
			{
				iShow = iShow - 1;
				if( iShow < 0 )
					iShow = 0;
				m_iSelCol = m_arShows[iShow];
			}

			if( GetFocusedItem() != NULL )
				pItem = GetFocusedItem();
			else
				pItem = GetNextVisibleItem( m_pRootItem );

			if( pItem != NULL )
				SelectItem( pItem, m_iSelCol );

			return true;
		}
		break;

	case VK_RIGHT:
		{
			if( GetKeyState( VK_SHIFT ) < 0 && GetStyle()&TLC_TREELIST )
			{
				Expand( GetFocusedItem(), TLE_EXPAND );
				return true;
			}

			INT_PTR iShow = FindInShowedColumns(m_iSelCol);
			if( iShow == -1 )
				m_iSelCol = m_arShows[0];
			else
			{
				iShow = iShow + 1;
				if( iShow >= m_arShows.GetSize() )
					iShow = m_arShows.GetUpperBound();
				m_iSelCol = m_arShows[iShow];
			}


			if( GetFocusedItem() != NULL )
				pItem = GetFocusedItem();
			else
				pItem = GetNextVisibleItem( m_pRootItem );

			if( pItem != NULL )
				SelectItem( pItem, m_iSelCol );

			return true;
		}
		break;
// TODO: При перемещении по дереву скролировать за выбранным элементом
	case VK_UP:
		{
			if( GetFocusedItem() != NULL )
				pItem = GetPrevVisibleItem( GetFocusedItem() );
			else
				pItem = GetNextVisibleItem( m_pRootItem );

			if (!IsColumnVisible(m_iSelCol))
				m_iSelCol = 0;

			if( pItem != NULL )
				SelectItemOnKeyPressing(pItem, m_iSelCol);

			return true;
		}
		break;

	case VK_DOWN:
		{
			if( GetFocusedItem() != NULL )
				pItem = GetNextVisibleItem( GetFocusedItem() );
			else
				pItem = GetNextVisibleItem( m_pRootItem );

			if (!IsColumnVisible(m_iSelCol))
				m_iSelCol = 0;

			if( pItem != NULL )
				SelectItemOnKeyPressing(pItem, m_iSelCol);

			return true;
		}
		break;

	case VK_PRIOR:	// VK_PAGE_UP
		{
			int nFirstItem;
			int nLastItem;
			int nCount;
			CTreeListItem* pFirstItem = NULL;

			nCount		= m_rcTreeList.Height() / GetItemHeight();
			if( m_rcTreeList.Height() >= GetItemHeight() )
				nCount -= 1;

			nFirstItem	= GetScrollPos( SB_VERT );
			nLastItem	= nFirstItem + nCount;

			pFirstItem = GetTreeListItem( nFirstItem );

			if (!IsColumnVisible(m_iSelCol))
				m_iSelCol = 0;

			if( GetFocusedItem() != NULL )
			{
				int nFocusItem = GetTreeListItem( GetFocusedItem() );
				if( nFirstItem > nFocusItem )
				{
					EnsureVisible( GetFocusedItem(), m_iSelCol );
					return true;
				}
			}

			if( pFirstItem != NULL )
			{
				if( pFirstItem != GetFocusedItem() )
					//SelectItem( pFirstItem, m_iSelCol );
					SelectItemOnKeyPressing(pFirstItem, m_iSelCol);
				else
				{
					nFirstItem = nFirstItem - nCount - 1;
					if( nFirstItem < 0 )
						nFirstItem = 0;

					nLastItem = nLastItem - nCount - 1;
					if( nLastItem < 0 )
						nLastItem = 0;

					pFirstItem = GetTreeListItem( nFirstItem );
					SetScrollPos( SB_VERT, nFirstItem );
//					SelectItem( pFirstItem, m_iSelCol );
					SelectItemOnKeyPressing(pFirstItem, m_iSelCol);
				}
			}

			return true;

		}
	case VK_NEXT:	// VK_PAGE_DOWN
		{
			int nFirstItem;
			int nLastItem;
			int nCount;
			CTreeListItem* pLastItem = NULL;
			
			nCount		= m_rcTreeList.Height() / GetItemHeight();
			if( m_rcTreeList.Height() >= GetItemHeight() )
				nCount -= 1;

			nFirstItem	= GetScrollPos( SB_VERT );
			nLastItem	= nFirstItem + nCount;

			if( nLastItem >= GetVisibleCount() )
				nLastItem  = GetVisibleCount() - 1;

			pLastItem = GetTreeListItem( nLastItem );

			if (!IsColumnVisible(m_iSelCol))
				m_iSelCol = 0;

			if( GetFocusedItem() != NULL )
			{
				int nFocusItem = GetTreeListItem( GetFocusedItem() );
				if( nLastItem < nFocusItem ){
					EnsureVisible( GetFocusedItem(), m_iSelCol );
					return true;
				}
			}
			
			if( pLastItem != NULL )
			{
				if( pLastItem != GetFocusedItem() )
				{
//					SelectItem( pLastItem, m_iSelCol );
					SelectItemOnKeyPressing(pLastItem, m_iSelCol);
				}
				else
				{
					if( nFirstItem < GetVisibleCount() - nCount - 1 )
					{
						if( nFirstItem + nCount + 1 < GetVisibleCount() - nCount - 1 )
						{
							nFirstItem = nFirstItem + nCount + 1;
						}
					}
					
					nLastItem	= nLastItem + nCount + 1;
					if( nLastItem >= GetVisibleCount() )
					{
						nLastItem = GetVisibleCount() - 1;
					}

					pLastItem = GetTreeListItem( nLastItem );
					SetScrollPos( SB_VERT, nFirstItem );
//					SelectItem( pLastItem, m_iSelCol );
					SelectItemOnKeyPressing(pLastItem, m_iSelCol);
				}
			}

			return true;
		}
		break;

	case VK_HOME:
		{
			pItem = GetNextVisibleItem( m_pRootItem );

			if (!IsColumnVisible(m_iSelCol))
				m_iSelCol = 0;

			if( pItem != NULL )
				SelectItemOnKeyPressing(pItem, m_iSelCol);

			return true;
		}
		break;

	case VK_END:
		{
			CTreeListItem* pNextItem;
			pItem = GetNextVisibleItem( m_pRootItem );
			pNextItem = pItem;
			while( pNextItem != NULL ){
				pItem = pNextItem;
				pNextItem = GetNextVisibleItem( pNextItem );
			}

			if (!IsColumnVisible(m_iSelCol))
				m_iSelCol = 0;

			if( pItem != NULL )
				SelectItemOnKeyPressing(pItem, m_iSelCol);

			return true;
		}

		break;


	default:
		break;

	}
	return false;
}

bool CTreeListCtrl::PreTranslateKeyDownMessage( MSG* pMsg )
{
	// pre translate key down messages
	ASSERT( pMsg->message == WM_KEYDOWN );

	return KeyDownHandler(pMsg->wParam, pMsg->lParam&0xFF, pMsg->lParam>>16);
}

bool CTreeListCtrl::PreTranslateKeyDownMessage2( MSG* pMsg )
{
	// pre translate key down messages
	ASSERT( pMsg->message == WM_KEYDOWN );
	if( m_arShows.GetSize() == 0 )
		return TRUE;

	switch( pMsg->wParam )
	{
	case VK_ESCAPE:
		if( GetState( TLS_MODIFY ) )
		{
			AbortLabelEdit();
			return TRUE;
		}
		break;

	case VK_RETURN:
		{
			if (!(GetAsyncKeyState(VK_CONTROL)>>15)) //if Cotrol not pressed
			{
				EndLabelEdit(true);
				return TRUE;
			}
			return FALSE;
		}
		break;

	default:
		break;
	}

	return FALSE;
}

CTreeListItem* CTreeListCtrl::GetTreeListItem( int nItem ) const
{
	// retrieve pointer of item by index of item
	if( GetCount() == 0 )
		return NULL;

	if( nItem < 0 || GetCount() <= nItem )
		return NULL;

	CTreeListItem* pItem = m_pRootItem->m_pChild;
	while( nItem != 0 )
	{
		ASSERT( nItem > 0 );

		if( nItem < pItem->m_nVisibleChild )
		{
			nItem --;
			pItem = pItem->m_pChild;
		}
		else
		{
			nItem -= pItem->m_nVisibleChild;
			pItem = pItem->m_pNext;
		}
	}

	return pItem;
}

int CTreeListCtrl::GetTreeListItem( CTreeListItem const* pItem ) const
{
	// retrieve index of item by pointer of item
	if( GetCount() == 0 )
		return -1;

	if( pItem == NULL )
		return -1;

	if( pItem == m_pRootItem )
		return -1;

	int iItem = 1;
	while( pItem->m_pPrev != NULL )
	{
		pItem = pItem->m_pPrev;
		iItem = iItem + pItem->m_nVisibleChild;
	}

	pItem = pItem->m_pParent;
	iItem = iItem + GetTreeListItem( pItem );
	return iItem;
}

bool CTreeListCtrl::SetCtrlFocus( CWnd* pWnd, bool bFocus )
{
	// set focus

	// hidden tooltips
	if( IsWindow( m_wndTip->GetSafeHwnd() ) )
		m_wndTip->Hide();

	if( bFocus )
	{
		CWnd* pOldWnd = pWnd;
		if( pOldWnd == this )
			return false;

		if( IsChild( pOldWnd ) )
			return false;

		SetState( TLS_FOCUS );
	}
	else
	{
		CWnd* pNewWnd = pWnd;
		if( pNewWnd == this )
			return false;

		if( IsChild( pNewWnd ) )
			return false;

		if( GetState( TLS_MODIFY ) )
			EndLabelEdit(true);

		SetState( 0, TLS_FOCUS );
	}
	
	Invalidate();

	return true;
}

void  CTreeListCtrl::AbortLabelEdit()
{
	EndLabelEdit(false);
}

bool  CTreeListCtrl::EndLabelEdit(bool save)
{
	if( !GetState( TLS_MODIFY ) )
		return true;

	ASSERT( GetState( TLS_MODIFY ) );
	ASSERT( m_pModifyItem != NULL && m_iModifyCol != -1 );

	CString strText;
	ASSERT(m_ptrCustomEditControl);
	if (save)
		m_ptrCustomEditControl->GetWindowText( strText );

	CancelControl();

	SetState( 0, TLS_MODIFY );

	CHANGE_LABEL_NOTIFY_INFO st;
	st.iCol = m_iModifyCol;
	st.pItem = m_pModifyItem;
	st.pEditControl = NULL;

	// если сохраняем и если текст изменили
	save = save&&(m_pModifyItem->GetText(m_iModifyCol) != strText);

	if (save)
		st.text = strText;

	CTreeListItem* const item = m_pModifyItem;
	int const column = m_iModifyCol;
	m_pModifyItem = NULL;
	m_iModifyCol  = -1;

	if (changeItemLabelNotifyListener_->onEndLabelEdit(*this, &st)&&save)
		item->SetText(st.text, column);
	Invalidate();
	return true;
}

CWnd* CTreeListCtrl::EditLabel(CTreeListItem* pItem, int iCol)
{
	ASSERT( pItem != NULL && 0 <= iCol && iCol < GetColumns() );

	if( (GetStyle()&TLC_READONLY) )
		return NULL;

	// update modified columns
	if( GetState( TLS_MODIFY ) )
	{

		EndLabelEdit(true);
	}

	CHANGE_LABEL_NOTIFY_INFO st;
	st.iCol = iCol;
	st.pItem = pItem;
	st.pEditControl = NULL;
	if (!changeItemLabelNotifyListener_->onBeginLabelEdit(*this, &st))
		return NULL;
		

	// save values in m_pUpdateItem
	ASSERT( m_pModifyItem == NULL && m_iModifyCol == -1 );

	m_pModifyItem	= pItem;
	m_iModifyCol	= iCol;

	// begin modify a new column
	ASSERT( !GetState( TLS_MODIFY ) );

	SetState( TLS_MODIFY );
	if (BeginControl( pItem, iCol ))
	{
		EnsureVisible(pItem, iCol);
		// выравниваем положение контрола
		// get text rect
		CRect rcText;
		GetItemRect( pItem, iCol, &rcText, true );
		if (m_ptrCustomEditControl)
			m_ptrCustomEditControl->MoveWindow(rcText);

		Invalidate();

		return NULL;
	}

	SetState( 0, TLS_MODIFY );
	SetModifiedItem(NULL);
	m_iModifyCol = -1;

	return NULL;
}

CWnd* CTreeListCtrl::GetControl( CTreeListItem* pItem, int iCol )
{
	// get pointer of modified control

	// check parameter
	if( pItem == NULL )
		return NULL;

	if( iCol < 0 ||  m_arColumns.GetSize() <= iCol )
		return NULL;

	// check state of control
	if( !GetState( TLS_MODIFY ) )
		return NULL;

	CWnd* pWnd;
	// restore a control
	switch( m_dwRequestedCtrlType )
	{
	case TLM_STATIC:
		pWnd = (CWnd*)&m_wndStatic;
		break;

	case TLM_EDIT:
		pWnd = (CWnd*)&m_wndEdit;
		break;

	case TLM_COMBO:
		pWnd = (CWnd*)&m_wndCombo;
		break;
	case TLM_CUSTOM:
		pWnd = m_ptrCustomEditControl;
		break;
	default:
		break;
	}

	return pWnd;
}

int CTreeListCtrl::GetPrevModifyCol()
{
	// retrieve previous modify column
	ASSERT( GetState( TLS_MODIFY ) );

	int  iShow;
	int  iShowCol	= -1;
	int  iPrevCol	= -1;
	bool bFound		= FALSE;

	for( iShow = 0; iShow < m_arShows.GetSize(); iShow++ )
	{
		if( m_arShows[iShow] == m_iModifyCol )
		{
			iShowCol = iShow;
			bFound = TRUE;
			break;
		}
	}

	ASSERT( bFound == TRUE );

	for( iShow = iShowCol - 1; iShow >= 0; iShow-- )
	{
		if( GetColumnModify( m_arShows[iShow] ) )
		{
			iPrevCol = m_arShows[iShow];
			break;
		}
	}

	return iPrevCol;
}

int CTreeListCtrl::GetNextModifyCol()
{
	// retrieve next modify column
	ASSERT( GetState( TLS_MODIFY ) );

	int  iShow;
	int  iShowCol	= -1;
	int  iNextCol	= -1;
	bool bFound		= FALSE;

	for( iShow = 0; iShow < m_arShows.GetSize(); iShow++ )
	{
		if( m_arShows[iShow] == m_iModifyCol )
		{
			iShowCol = iShow;
			bFound	 = TRUE;
			break;
		}
	}

	ASSERT( bFound == TRUE );

	for( iShow = iShowCol + 1; iShow < m_arShows.GetSize(); iShow++ )
	{
		if( GetColumnModify( m_arShows[iShow] ) )
		{
			iNextCol = m_arShows[iShow];
			break;
		}
	}

	return iNextCol;
}

bool CTreeListCtrl::BeginControl( CTreeListItem* pItem, int iCol )
{
	// begin a modify control
	ASSERT( GetState( TLS_MODIFY ) );

	// clean first
	m_wndStatic->SetWindowText( _T("") );
	m_wndEdit->SetWindowText( _T("") );
	m_wndCombo->ResetContent();

	DWORD dwCtrlType = GetColumnModify( iCol );
	LRESULT res = 0;
	if (dwCtrlType == TLM_REQUEST)
	{
		CHANGE_LABEL_NOTIFY_INFO st;
		st.iCol = iCol;
		st.pItem = pItem;
		st.pEditControl = NULL;
		dwCtrlType = changeItemLabelNotifyListener_->onRequestCtrl(*this, &st);
		ASSERT(dwCtrlType != TLM_REQUEST);
	}
	m_dwRequestedCtrlType = dwCtrlType;

	CHANGE_LABEL_NOTIFY_INFO st;
	st.iCol = iCol;
	st.pItem = m_pModifyItem;
	st.pEditControl = NULL;
	// get text rect
	CRect rcText;
	GetItemRect( pItem, iCol, &rcText, true );
	// begin a control
	switch( dwCtrlType )
	{
	case TLM_STATIC:
		BeginStatic( pItem, iCol, rcText );

		st.pEditControl = m_wndStatic.get();
		changeItemLabelNotifyListener_->onBeginControl(*this, &st);
		break;

	case TLM_EDIT:
		BeginEdit( pItem, iCol, rcText );

		st.pEditControl = m_wndEdit.get();
		changeItemLabelNotifyListener_->onBeginControl(*this, &st);
		break;

	case TLM_COMBO:
		rcText.OffsetRect( 0, -1 );
		BeginCombo( pItem, iCol, rcText );

		st.pEditControl = m_wndCombo.get();
		changeItemLabelNotifyListener_->onBeginControl(*this, &st);

		break;
	case TLM_CUSTOM:

		st.pEditControl = NULL;
		changeItemLabelNotifyListener_->onBeginControl(*this, &st);
		break;
	}

	m_ptrCustomEditControl = st.pEditControl;

	if (m_ptrCustomEditControl)
		ShowCtrl(m_ptrCustomEditControl);
	else
		return false;
	return true;
}

bool CTreeListCtrl::CancelControl()
{
	// end modify a column of an item
	ASSERT( GetState( TLS_MODIFY ) );

	switch( m_dwRequestedCtrlType )
	{
	case TLM_COMBO:
		CancelCombo();
		break;
	default:
		m_ptrCustomEditControl->SetWindowText(_T(""));
		CancelCtrl(m_ptrCustomEditControl);
		break;
	}

	// notify
	CHANGE_LABEL_NOTIFY_INFO st;
	st.iCol = m_iModifyCol;
	st.pItem = m_pModifyItem;
	st.pEditControl = m_ptrCustomEditControl;
	changeItemLabelNotifyListener_->onEndControl(*this, &st);

	m_ptrCustomEditControl = NULL;
	m_dwRequestedCtrlType = TLM_STATIC;
	return true;
}

bool CTreeListCtrl::RestoreControl( CTreeListItem* pItem, int iCol )
{
	// restore a modify control
	ASSERT( GetState( TLS_MODIFY ) );

	// get text rect
	CRect rcText;
	GetItemRect( pItem, iCol, &rcText, TRUE );

	// restore a control
//	switch( GetColumnModify( iCol ) )
	switch( m_dwRequestedCtrlType )
	{
	case TLM_STATIC:
		RestoreStatic( pItem, iCol, rcText );
		break;

	case TLM_EDIT:
		RestoreEdit( pItem, iCol, rcText );
		break;

	case TLM_COMBO:
		RestoreCombo( pItem, iCol, rcText );
		break;
	case TLM_CUSTOM:
		ASSERT(0);
		break;
	}

	return true;
}

bool CTreeListCtrl::CancelCtrl(CWnd* pCtrl)
{
	if (!pCtrl)
		return true;

	pCtrl->ShowWindow( SW_HIDE );
	pCtrl->EnableWindow( FALSE );
	return true;
}

void CTreeListCtrl::RestoreCtrl( CWnd* pCtrl, CTreeListItem* pItem, 
								int iCol, CRect const& rcText)
{
	// restore a combo control
	ASSERT( m_pModifyItem	== pItem );
	ASSERT( m_iModifyCol	== iCol  );

	SetFocus();
	pCtrl->MoveWindow( rcText );
	ShowCtrl(pCtrl);
}

void CTreeListCtrl::ShowCtrl(CWnd* pwnd)
{
	pwnd->EnableWindow( TRUE );
	pwnd->ShowWindow( SW_SHOW );
	pwnd->SetFocus();
}


bool CTreeListCtrl::BeginStatic( CTreeListItem* pItem, int iCol, CRect rcText )
{
	// begin static a column of an item
	ASSERT( 0 <= iCol && iCol <= m_arColumns.GetSize() );

	if( rcText.IsRectEmpty() )
		return FALSE;

	CTreeListColumnInfo* pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
	int nMin = GetStringWidth( pItem->GetText( iCol ) );
	DWORD dwStyle = 0;

	if( rcText.Width() < pColumnInfo->GetMinWidth() )
		rcText.right = rcText.left + pColumnInfo->GetWidth();

	if( rcText.Width() < nMin )
		rcText.right = rcText.left + pColumnInfo->GetWidth();

	if( GetStyle()&TLC_HGRID )
		rcText.top--;

	if( GetStyle()&TLC_VGRID )
		rcText.left--;

	if( pColumnInfo->GetFormat()&TLF_TEXTALIGN_LEFT )
		dwStyle |= SS_LEFT;
	else if( pColumnInfo->GetFormat()&TLF_TEXTALIGN_RIGHT )
		dwStyle |= SS_RIGHT;
	else if( pColumnInfo->GetFormat()&TLF_TEXTALIGN_CENTER )
		dwStyle |= SS_CENTER;
	else
		dwStyle |= SS_LEFT;

	m_wndStatic->ModifyStyle(SS_LEFT|SS_RIGHT|SS_CENTER, 0, SS_LEFT|SS_RIGHT|SS_CENTER);
	m_wndStatic->ModifyStyle( 0, dwStyle, dwStyle );
	m_wndStatic->MoveWindow( rcText );
	m_wndStatic->SetWindowText( GetItemText( pItem, iCol ) );
	return true;
}

bool CTreeListCtrl::CancelStatic()
{
	// cancel an static control
	m_wndStatic->ShowWindow( SW_HIDE );
	m_wndStatic->EnableWindow( FALSE );

	return true;
}

bool CTreeListCtrl::RestoreStatic( CTreeListItem* pItem, int iCol, CRect rcText )
{
	RestoreCtrl(m_wndStatic.get(), pItem, iCol, rcText);
	return true;
}

bool CTreeListCtrl::BeginEdit( CTreeListItem* pItem, int iCol, CRect rcText )
{
	// begin edit a column of an item
	ASSERT( 0 <= iCol && iCol <= m_arColumns.GetSize() );

	if( rcText.IsRectEmpty() )
		return false;

	CTreeListColumnInfo* pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
	int nMin = GetStringWidth( pItem->GetText( iCol ) );
	DWORD dwStyle = 0;

	if( rcText.Width() < pColumnInfo->GetMinWidth() )
		rcText.right = rcText.left + pColumnInfo->GetWidth();

	if( rcText.Width() < nMin )
		rcText.right = rcText.left + pColumnInfo->GetWidth();

	if( GetStyle()&TLC_HGRID )
		rcText.top--;

	if( GetStyle()&TLC_VGRID )
		rcText.left--;

	if( pColumnInfo->GetFormat()&TLF_TEXTALIGN_LEFT )
		dwStyle |= ES_LEFT;
	else if( pColumnInfo->GetFormat()&TLF_TEXTALIGN_RIGHT )
		dwStyle |= ES_RIGHT;
	else if( pColumnInfo->GetFormat()&TLF_TEXTALIGN_CENTER )
		dwStyle |= ES_CENTER;
	else
		dwStyle |= ES_LEFT;

	m_wndEdit->ModifyStyle(ES_LEFT|ES_RIGHT|ES_CENTER, 0, ES_LEFT|ES_RIGHT|ES_CENTER);
	m_wndEdit->ModifyStyle( 0, dwStyle, dwStyle );
	m_wndEdit->MoveWindow( rcText );
	m_wndEdit->SetWindowText( GetItemText( pItem, iCol ) );
	m_wndEdit->SetSel( 0, -1 );

	return true;
}

bool CTreeListCtrl::CancelEdit()
{
	// cancel an edit control
	return CancelCtrl(m_wndEdit.get());
}

bool CTreeListCtrl::RestoreEdit( CTreeListItem* pItem, int iCol, CRect rcText )
{
	// restore an edit control
	RestoreCtrl(m_wndEdit.get(), pItem, iCol, rcText);
	return true;
}

bool CTreeListCtrl::BeginCombo( CTreeListItem* pItem, int iCol, CRect rcText )
{
	// begin combo a column of an item
	ASSERT( 0 <= iCol && iCol <= m_arColumns.GetSize() );

	if( rcText.IsRectEmpty() )
		return FALSE;

	rcText.bottom += GetItemHeight() * 6;

	CTreeListColumnInfo* pColumnInfo = (CTreeListColumnInfo*)m_arColumns[iCol];
	int nMin = GetStringWidth( pItem->GetText( iCol ) );
	DWORD dwStyle = 0;

	CString strInit = GetColumnDefaultText( iCol );
	CString strItem;

	int nBeg = 0;
	int nEnd = strInit.Find( _T("|"), nBeg );
	while( nEnd != -1 )
	{
		strItem = strInit.Mid( nBeg, nEnd-nBeg );
		m_wndCombo->InsertString( -1, strItem );
		nBeg = nEnd + 1;
		nEnd = strInit.Find( _T("|"), nBeg );
	}
	strItem = strInit.Mid( nBeg );
	m_wndCombo->InsertString( -1, strItem );

	m_wndCombo->MoveWindow( rcText );
	if( m_wndCombo->SelectString( -1, GetItemText( pItem, iCol ) ) == -1 )
	{
		m_wndCombo->InsertString( 0, GetItemText( pItem, iCol ) );
		m_wndCombo->SelectString( -1, GetItemText( pItem, iCol ) );
	}

	return true;
}

bool CTreeListCtrl::CancelCombo()
{
	m_wndCombo->ResetContent();
	return CancelCtrl(m_wndCombo.get());
}

bool CTreeListCtrl::RestoreCombo( CTreeListItem* pItem, int iCol, CRect rcText)
{
	// restore a combo control
	RestoreCtrl(m_wndCombo.get(), pItem, iCol, rcText);
	return true;
}

bool CTreeListCtrl::TestForDrag(CWnd* pWnd, CRect const& rect, UINT nDragDelay)
{
	ASSERT_VALID(pWnd);
	bool bDragStarted = FALSE;

	DWORD dwLastTick = GetTickCount();
	pWnd->SetCapture();

	while (!bDragStarted)
	{
		// some applications steal capture away at random times
		if (CWnd::GetCapture() != pWnd)
			break;

		// peek for next input message
		MSG msg;
		if (::PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE) ||
			::PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) ||
			::PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
		{
			// check for button cancellation (any button down will cancel)
			if (msg.message == WM_LBUTTONUP || msg.message == WM_RBUTTONUP ||
				msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN)
				break;

			// check for keyboard cancellation
			if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
				break;

			// check for drag start transition
			bDragStarted = !rect.PtInRect(msg.pt);
		}

		// if the user sits here long enough, we eventually start the drag
		if (GetTickCount() - dwLastTick > nDragDelay)
			bDragStarted = TRUE;
	}
	ReleaseCapture();

	return bDragStarted;
}

bool CTreeListCtrl::BeginDraging( CPoint point, eDragButton dragButton )
{
	// begin a draging
	ASSERT( GetSelectedItem() != NULL );
	
	if( !(GetStyle()&TLC_DRAG) )
		return false;

	CPoint cursorPos(point);
//	ClientToScreen(&cursorPos);
	GetCursorPos(&cursorPos);

	CRect rectStartDrag(cursorPos.x, cursorPos.y, cursorPos.x, cursorPos.y);
	rectStartDrag.InflateRect(10, 10);
	if (TestForDrag(this, rectStartDrag, DRAG_DELAY))
	{
		TREELISTDRAGDROP data;
		data.hoverItem = NULL;
		data.pSource = this;
		data.hCursor = m_hArrow;
		switch(dragButton) {
		case DB_LBUTTON:
			if (!itemDragdropNotifyListener_->onBeginDrag(*this, &data))
				return false;
			break;
		case DB_RBUTTON:
			if (!itemDragdropNotifyListener_->onBeginRDrag(*this, &data))
				return false;
			break;
		}

		SetState( TLS_DRAG );
		
		m_ptBegin = point;
		m_ptDelta.x = -160;
		m_ptDelta.y = -160;
		return true;
	}
	return false;
}

bool CTreeListCtrl::EndDraging( CPoint point )
{
	// end a draging
	ASSERT( GetStyle()&TLC_DRAG );

	if( m_pDragWnd != NULL )
	{
		m_pDragWnd->Hide();
		m_pDragWnd->DestroyWindow();
		delete m_pDragWnd;
		m_pDragWnd = NULL;
	}

	if( m_pDropWnd != NULL )
	{
		m_pDropWnd->Hide();
		m_pDropWnd->DestroyWindow();
		delete m_pDropWnd;
		m_pDropWnd = NULL;
	}
	
	ClientToScreen( &point );

/*	CWnd* pWnd;
	pWnd = CWnd::WindowFromPoint( point );
	if( pWnd != NULL && pWnd->IsKindOf( RUNTIME_CLASS(CTreeListCtrl) ) )
	{
		CTreeListCtrl* pTargetWnd = static_cast<CTreeListCtrl*>(pWnd);
		pTargetWnd->Drop( this, point );
	}
*/
	if( m_pTargetWnd != NULL )
	{
		ASSERT(m_pTargetWnd->IsKindOf( RUNTIME_CLASS( CTreeListCtrl ) ));
		m_pTargetWnd->Drop( this, point );
	}
	SetState( 0, TLS_DRAG );
	
	return true;
}

bool CTreeListCtrl::EndDraging()
{
	// cancel a draging
	ASSERT( GetStyle()&TLC_DRAG );

	if( m_pDragWnd != NULL )
	{
		m_pDragWnd->Hide();
		m_pDragWnd->DestroyWindow();
		delete m_pDragWnd;
		m_pDragWnd = NULL;
	}

	if( m_pDropWnd != NULL )
	{
		m_pDropWnd->Hide();
		m_pDropWnd->DestroyWindow();
		delete m_pDropWnd;
		m_pDropWnd = NULL;
	}

	if( m_pTargetWnd != NULL && m_pTargetWnd->IsKindOf( RUNTIME_CLASS( CTreeListCtrl ) ) )
	{
		m_pTargetWnd->DragLeave( this );
		m_pTargetWnd = NULL;
	}

	SetState( 0, TLS_DRAG );
	return true;
}

bool CTreeListCtrl::DoDraging( CPoint point )
{
	// draging
	ASSERT( GetStyle()&TLC_DRAG );

	if( m_pDragWnd == NULL )
	{
		CRect rcDragWnd;
		rcDragWnd.SetRect( 0, 0, 320, 320 );
		rcDragWnd.OffsetRect( point );
		rcDragWnd.OffsetRect( m_ptDelta );
		ClientToScreen( rcDragWnd );

		m_pDragWnd = new CTLCDragWnd;
		m_pDragWnd->Create( rcDragWnd, this );
		m_pDragWnd->Show();

		m_pDropWnd = new CTLCDropWnd;
		m_pDropWnd->Create( this );

		DoDraging( point );
	}
	else
	{
		CRect rcDragWnd;
		m_pDragWnd->GetClientRect( &rcDragWnd );
		rcDragWnd.OffsetRect( point );
		if( m_pDragWnd->m_bLayeredWindows )
			rcDragWnd.OffsetRect( m_ptDelta );
		else
			rcDragWnd.OffsetRect( 20, 4 );

		ClientToScreen( rcDragWnd );

		m_pDragWnd->MoveWindow( rcDragWnd );

		CPoint pt;
		pt = point;
		ClientToScreen( &pt );

		CWnd* pWnd;
		pWnd = CWnd::WindowFromPoint( pt );
		if( pWnd != NULL && pWnd->IsKindOf( RUNTIME_CLASS(CTreeListCtrl) ) )
		{
			CTreeListCtrl* pTargetWnd = (CTreeListCtrl*)pWnd;

			if( m_pTargetWnd != NULL && m_pTargetWnd != pTargetWnd )
			{
				m_pTargetWnd->DragLeave( this );
				m_pTargetWnd = NULL;
			}

			if( m_pTargetWnd == NULL && pTargetWnd->DragEnter( this ))
			{
				m_pTargetWnd = pTargetWnd;
			}

			if( m_pTargetWnd != NULL )
			{
				CTreeListItem* pItem = NULL;
				CRect rcItem;
				CRect rcClient;
				int nDrag = m_pTargetWnd->DragOver( this, pt, &pItem );
				if (CTreeListItem * const pTarget = m_pTargetWnd->GetDropTargetItem()) 
				{
					m_pTargetWnd->GetItemRect( pTarget, &rcItem );
					m_pTargetWnd->ClientToScreen( &rcItem );
					m_pTargetWnd->GetClientRect( &rcClient );
					m_pTargetWnd->ClientToScreen( &rcClient );
					m_pDropWnd->Show( CPoint( rcClient.left, 
										( rcItem.top + rcItem.bottom ) / 2 ) );
				}
				else
				{
//					SetCursor( m_hStop );
					m_pDropWnd->Hide();
				}
			}
			else
			{
//				SetCursor( m_hStop );
				m_pDropWnd->Hide();
			}
		}
		else
		{
			if( m_pTargetWnd != NULL )
			{
				m_pTargetWnd->DragLeave( this );
				m_pTargetWnd = NULL;
			}
			m_pDropWnd->Hide();
		}
	}

	return TRUE;
}

void CTreeListCtrl::ExchangeItem( CTreeListItem* pItemA, CTreeListItem* pItemB )
{
	// exchange itemA and itemB ( the same parent )
	ASSERT( pItemA != NULL && pItemB != NULL );

	if( pItemA == pItemB )
		return;

	if( pItemA->m_pParent != pItemB->m_pParent )
		return;

	CTreeListItem* pParentA;
	CTreeListItem* pParentB;

	pParentA = pItemA->m_pParent;
	pParentB = pItemB->m_pParent;

	if( pParentA->m_pChild == pItemA )
	{
		pParentA->m_pChild = pItemB;
	}
	else if( pParentB->m_pChild == pItemB )
	{
		pParentB->m_pChild = pItemA;
	}

	if( pItemA->m_pNext == pItemB )
	{
		pItemA->m_pNext = pItemB->m_pNext;
		pItemB->m_pPrev = pItemA->m_pPrev;

		pItemA->m_pPrev->m_pNext = pItemB;
		pItemB->m_pNext->m_pPrev = pItemA;
		
		pItemA->m_pPrev = pItemB;
		pItemB->m_pNext = pItemA;
	}
	else if( pItemA->m_pPrev == pItemB )
	{
		pItemB->m_pNext	= pItemA->m_pNext;
		pItemA->m_pPrev = pItemB->m_pPrev;

		pItemB->m_pPrev->m_pNext = pItemA;
		pItemA->m_pNext->m_pPrev = pItemB;

		pItemB->m_pPrev = pItemA;
		pItemA->m_pNext	= pItemB;
	}
	else
	{
		CTreeListItem* pPrevA;
		CTreeListItem* pPrevB;

		pPrevA	= pItemA->m_pPrev;
		pPrevB	= pItemB->m_pPrev;

		pItemA->m_pPrev	= pPrevB;
		pItemB->m_pPrev = pPrevA;

		if( pPrevA != NULL )
			pPrevA->m_pNext	= pItemB;

		if( pPrevB != NULL )
			pPrevB->m_pNext	= pItemA;

		CTreeListItem* pNextA;
		CTreeListItem* pNextB;

		pNextA	= pItemA->m_pNext;
		pNextB	= pItemB->m_pNext;

		pItemA->m_pNext	= pNextB;
		pItemB->m_pNext	= pNextA;

		if( pNextA != NULL )
			pNextA->m_pPrev	= pItemB;

		if( pNextB != NULL )
			pNextB->m_pPrev = pItemA;
	}

	Invalidate();

	return;
}

void CTreeListCtrl::MoveAfterItem( CTreeListItem* pItem, CTreeListItem* pAfter )
{
	// move pItemA after pAfter ( the same parent )
	ASSERT( pItem != NULL );

	CTreeListItem* pParent;
	CTreeListItem* pFirst;
	CTreeListItem* pPrev;
	CTreeListItem* pNext;
		 
	pParent = pItem->m_pParent;
	pFirst = pParent->m_pChild;
	pPrev = pItem->m_pPrev;
	pNext = pItem->m_pNext;

	if( pAfter == NULL )
	{
		if( pParent->m_pChild == pItem )
			return;
	}
	else
	{
		if( pItem == pAfter )
			return;
	}

	if( pParent->m_pChild == pItem )
		pParent->m_pChild = pItem->m_pNext;

	if( pPrev != NULL )
		pPrev->m_pNext = pItem->m_pNext;

	if( pNext != NULL )
		pNext->m_pPrev = pItem->m_pPrev;

	if( pAfter == NULL )
	{
		pParent->m_pChild = pItem;

		pItem->m_pPrev = pFirst->m_pPrev;

		pItem->m_pNext = pFirst;
		pFirst->m_pPrev = pItem;
	}
	else
	{
		ASSERT( pParent == pAfter->m_pParent );
		
		pPrev = pAfter;
		pNext = pAfter->m_pNext;

		pItem->m_pPrev = pPrev;
		pPrev->m_pNext = pItem;

		pItem->m_pNext = pNext;
		if( pNext != NULL )
			pNext->m_pPrev = pItem;
	}

	Invalidate();

	return;
}

bool CTreeListCtrl::DragEnter( CTreeListCtrl* pTreeListCtrl )
{
	// drag enter
	if( !(GetStyle()&TLC_DROP) )
		return false;

	TREELISTDRAGDROP data;

	data.pSource = pTreeListCtrl;
	data.hoverItem = NULL;
	data.hCursor = m_hArrow;
	bool const b = itemDragdropNotifyListener_->onDragEnter(*this, &data);
	SetCursor(data.hCursor);
	return b;
}

bool CTreeListCtrl::DragLeave( CTreeListCtrl* pTreeListCtrl )
{
	// drag leave
	TREELISTDRAGDROP data;
	data.pSource = pTreeListCtrl;
	data.hoverItem = NULL;
	data.hCursor = m_hArrow;
	itemDragdropNotifyListener_->onDragLeave(*this, &data);

	//	SetTargetItem( NULL );
	SetHoverItem( NULL );
	return true;
}

int CTreeListCtrl::DragOver( CTreeListCtrl* pTreeListCtrl, 
							CPoint point, 
							CTreeListItem** pp )
{
	// drag over
	ASSERT( pTreeListCtrl != NULL );
	ASSERT_VALID( pTreeListCtrl );

	LRESULT lResult = 0;
	ScreenToClient( &point );
	CTreeListItem* pItem = HitTest( point );

	if( pp != NULL )
		*pp = pItem;

	int nDropEffect = DROP_NONE;

	if( pTreeListCtrl != this )
	{
		if( GetStyle()&TLC_DROP )
		{
			if( pItem != NULL )
			{
				TREELISTDRAGDROP data;
				data.pSource = pTreeListCtrl;
				data.hoverItem = pItem;
				data.hCursor = m_hArrow;

				if (itemDragdropNotifyListener_->onDragOver(*this, &data))
					nDropEffect = DROP_ITEM;
				else
					nDropEffect = DROP_NONE;
				SetCursor(data.hCursor);
			}
			else
			{
				if( GetStyle()&TLC_DROPROOT )
				{
					TREELISTDRAGDROP data;
					data.pSource = pTreeListCtrl;
					data.hoverItem = TLI_ROOT;
					data.hCursor = m_hArrow;

					if (itemDragdropNotifyListener_->onDragOver(*this, &data))
						nDropEffect = DROP_ROOT;
					else
						nDropEffect = DROP_NONE;
					SetCursor(data.hCursor);
				}
				else
					nDropEffect = DROP_NONE;
			}
		}
		else
		{
			nDropEffect = DROP_NONE;
		}
	}
	else
	{
		if( GetStyle()&TLC_DROP && GetStyle()&TLC_DROPTHIS )
		{
			if( pItem != NULL )
			{
				TREELISTDRAGDROP data;
				data.pSource = pTreeListCtrl;
				data.hoverItem = pItem;
				data.hCursor = m_hArrow;

				if (itemDragdropNotifyListener_->onDragOver(*this, &data))
					nDropEffect = DROP_ITEM;
				else
					nDropEffect = DROP_NONE;
				SetCursor(data.hCursor);

			}
			else
			{
				if( GetStyle()&TLC_DROPROOT )
				{
					TREELISTDRAGDROP data;
					data.pSource = this;
					data.hoverItem = TLI_ROOT;
					data.hCursor = m_hArrow;

					if (itemDragdropNotifyListener_->onDragOver(*this, &data))
						nDropEffect = DROP_ROOT;
					else
						nDropEffect = DROP_NONE;
					SetCursor(data.hCursor);
				}
				else //if( GetStyle()&TLC_DROPROOT )
				{
					nDropEffect = DROP_NONE;
				}
			}
		}
		else//if( GetStyle()&TLC_DROP && GetStyle()&TLC_DROPTHIS )
		{
			nDropEffect = DROP_NONE;
		}
	}

	switch( nDropEffect )
	{
	case DROP_NONE:
//		SetTargetItem( NULL );
		SetHoverItem( pItem );
//		SetCursor(m_hStop);
		if( GetStyle()&TLC_TREELIST )
		{
			SetTimer( 2, 100, NULL );
			SetTimer( 1, 500, NULL );
		}
		break;

	case DROP_SELF:
	case DROP_ROOT:
//		SetTargetItem( NULL );
		SetHoverItem( NULL );
		break;

	case DROP_ITEM:
//		SetTargetItem( pItem );
		SetHoverItem( pItem );
		if( GetStyle()&TLC_TREELIST )
		{
			SetTimer( 2, 200, NULL );
			SetTimer( 1,1000, NULL );
		}
		break;

	default:
		ASSERT( FALSE );
		break;
	}

	return nDropEffect;
}

int CTreeListCtrl::Drop( CTreeListCtrl* pTreeListCtrl, CPoint point )
{
	CTreeListItem* pItem = NULL;
	int	nDrag = DragOver( pTreeListCtrl, point, &pItem );
//	DragLeave( pTreeListCtrl );
	switch( nDrag )
	{
	case DROP_NONE:
	case DROP_SELF:
		break;

	case DROP_ITEM:
	case DROP_ROOT:
		{
			TREELISTDRAGDROP data;
			data.pSource = pTreeListCtrl;
			data.hoverItem = pItem;
			data.hCursor = m_hArrow;

			itemDragdropNotifyListener_->onDrop(*this, &data);
		}
		break;

	default:
		ASSERT( FALSE );
		break;
	}
	SetTargetItem( NULL );
	SetHoverItem( NULL );
	return 0;
}

bool CTreeListCtrl::IsInSelectsTree( CTreeListItem const* pItem )
{
	ASSERT( pItem != NULL );
	ASSERT( m_arSelects.GetSize() > 0 );
	
	ASSERT(GetFocusedItem() == (CTreeListItem*)m_arSelects[0]);
	CTreeListItem* pSelItem = GetFocusedItem();

	while( pItem->m_nLevel > pSelItem->m_nLevel )
		pItem = pItem->m_pParent;
	
	for( int iSel = 0; iSel < m_arSelects.GetSize(); iSel++ )
	{
		pSelItem = (CTreeListItem*)m_arSelects[iSel];
		if( pItem == pSelItem )
			return true;
	}

	return false;
}

void CTreeListCtrl::OnTimer(UINT nIDEvent) 
{
	
	int nItem;
	int nPosition;
	int nPage;

	CPoint point;
	::GetCursorPos( &point );
	ScreenToClient( &point );
	int iSubItem = -1;
	int nFlags = 0;
	CTreeListItem* pItem = HitTest( point, &nFlags, &iSubItem );

	switch( nIDEvent )
	{
	case 1:

		if( pItem == m_pHoverItem )
		{
			Expand( pItem, TLE_EXPAND );
		}

		KillTimer( 1 );

		break;

	case 2:
		nPosition	= GetScrollPos( SB_VERT );
		nPage		= GetCountPerPage(); 
		nItem = GetTreeListItem( pItem );
		if( nItem == 0 )
		{
			KillTimer( 2 );
		}
		else if( nItem == nPosition )
		{
			KillTimer( 1 );
			SendMessage( WM_VSCROLL, (WPARAM)SB_LINEUP );
			pItem = HitTest( point );
			SetHoverItem( pItem );

			if( m_pDropWnd != NULL )
				m_pDropWnd->Hide();
		}
		else if( nItem >= nPosition + nPage - 1 )
		{
			KillTimer( 1 );
			SendMessage( WM_VSCROLL, (WPARAM)SB_LINEDOWN );
			pItem = HitTest( point );
			SetHoverItem( pItem );

			if( m_pDropWnd != NULL )
				m_pDropWnd->Hide();
		}
		else
		{
			KillTimer( 2 );
		}

		break;
	case SHOW_TIP_TIMER_ID:
		{
			KillTimer(SHOW_TIP_TIMER_ID);
			if (nFlags&TLHT_ONITEMTEXT&&!(GetState()&TLS_MODIFY))
				ShowTip(pItem, iSubItem);
			break;
		}
	case BEGIN_MODIFY_TIMER:
	{
		KillTimer(BEGIN_MODIFY_TIMER);
		std::pair<CTreeListItem*, int> const& editItem = GetReadyToEditItem();
		if (editItem.first != NULL&& editItem.second != -1)
			EditLabel(editItem.first, editItem.second);
		SetReadyToEditItem(NULL, -1);
		break;
	}
	default:
		break;
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CTreeListCtrl::OnCancelMode() 
{
	// cancel internal event
	if( GetState( TLS_DRAG ) )
	{
		ASSERT( GetState( TLS_CAPTURE ) );
		EndDraging();
	}

	if( GetState( TLS_CAPTURE ) )
	{
		ReleaseCapture();
		SetState( 0, TLS_CAPTURE );
	}

	ASSERT( !GetState( TLS_CAPTURE ) );
	CWnd::OnCancelMode();
}

void CTreeListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	TLKEYDOWN info;
	info.wVKey = nChar;
	info.flags = nFlags;

	bool const res = treeListNotifyListener_->onKeyDown(*this, &info);
	if (nChar == VK_RETURN)
		generalNotifyListener_->onReturn(*this);
	if (!res)
		return;
	KeyDownHandler(nChar, nRepCnt, nFlags);
// TODO: Вставить поддержку поиска по дереву и запрета её,если вернули false
}

UINT CTreeListCtrl::OnGetDlgCode()
{
	UINT res = DLGC_WANTCHARS|DLGC_WANTARROWS;
	if (GetState(TLS_MODIFY))
		res = DLGC_HASSETSEL;
	return res;
}

void CTreeListCtrl::ShowTip(CTreeListItem* pItem, int iSubItem)
{
	CTreeListColumnInfo* pColumnInfo = 
		reinterpret_cast<CTreeListColumnInfo*>(m_arColumns[iSubItem]);

	CRect rcText;
	GetItemRect(pItem, iSubItem, rcText, true);

	m_wndTip->Show( rcText, pItem->GetText( iSubItem ), pColumnInfo->GetTextFormat() );

}

void CTreeListCtrl::parentsExpanding(CTreeListItem const* pItem)
{
	while (CTreeListItem * parent = GetParentItem(pItem))
	{
		if (!parent->GetExpanded())
			Expand(parent, TLE_EXPAND);
		pItem = parent;
	}
}
