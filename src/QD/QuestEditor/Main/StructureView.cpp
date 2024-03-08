// Main\StructureView.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "Main\StructureView.h"
#include ".\structureview.h"
#include "Project.h"
#include "ptree_consts.h"

namespace {
	bool LoadBitmaps(CImageList* ptrIL)
	{
		ptrIL->Create(16, 16, ILC_MASK|ILC_COLOR32, 55, 4);
		CBitmap bitmap;

		for (UINT nID = 180; nID <= 199; nID++)  
		{
			bitmap.LoadBitmap(nID);
			ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
			bitmap.DeleteObject();
		}

		bitmap.LoadBitmap(IDB_INVENTORY_STATE_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_DIALOG_STATE_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_MINIGAME_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_MINIGAMESTATE_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_MINIGAMES_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_MUSIC_TRACKS_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_MUSIC_TRACK_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_GZONE_STATE_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_INTERFACE_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_INTERFACE_SCREEN_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_INTERFACE_ELEMENT_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_FOR_DYN_OBJ);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_FOR_ZONES);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_FOR_DYN_OBJ_AND_ZONES);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_GZONE_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_VIDEOS_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_VIDEO_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_INVENTORY_OBJS_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_INVENTORY_OBJ_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_DISABLE_MOUSE_ANIMATED_OBJ_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_NOT_MOV_STATIC_OBJ_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_INTERFACE_ELEMENT_STATE_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_FOR_ALL_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_GZONE_NEED_CUR_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_STATE4INVENTORY_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_FOR_MAIN_MENU);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_FOR_INGAME_INTERF);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_GAME_END_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_GAME_ENDS_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_COUNTER_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_COUNTERS_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_DIALOGS_FRAME_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_FONTS_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_FONT_NODE);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		bitmap.LoadBitmap(IDB_CURSOR_FOR_OBJ_IN_INVENTORY);
		ptrIL->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();

		return true;
	}
}
// StructureView

IMPLEMENT_DYNCREATE(StructureView, CView)

StructureView::StructureView()
{
}

StructureView::~StructureView()
{
}

QETreeListCtrl& StructureView::getTreeListCtrl(){
	return treeList_;
}

bool StructureView::LoadStateBitmaps(CImageList* pil)
{
	pil->Create(16, 16, ILC_MASK, 0, 4);

	CBitmap bitmap;

	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_STATE_IN_TGR);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_STATE_IN_NOT_ACTIVE_TGR);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_BMP_ITEM_STATE_DEF_STATE);
	pil->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	return true;
}

void StructureView::setProject(Project* project){
	m_pDocument = project;
}
Project* StructureView::getProject() const{
	return reinterpret_cast<Project*>(GetDocument());
}

BEGIN_MESSAGE_MAP(StructureView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MENUCOMMAND, OnMenuCommand)
END_MESSAGE_MAP()


// StructureView drawing

void StructureView::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
}


// StructureView diagnostics

#ifdef _DEBUG
void StructureView::AssertValid() const
{
	CView::AssertValid();
}

void StructureView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// StructureView message handlers

int StructureView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);

	treeList_.Create(WS_CHILD|WS_VISIBLE, rect, this, 0);
	
	treeList_.SetStyle(0
		| TLC_TREELIST								// TreeList or List
//		| TLC_BKGNDIMAGE							// image background
//		| TLC_BKGNDCOLOR							// colored background ( not client area )
//		| TLC_DOUBLECOLOR							// double color background

//		| TLC_MULTIPLESELECT						// single or multiple select
//		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELALWAYS							// show selected item always
		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode

//		| TLC_HEADER								// show header
//		| TLC_HGRID									// show horizonal grid lines
//		| TLC_VGRID									// show vertical grid lines
//		| TLC_TGRID									// show tree horizonal grid lines ( when HGRID & VGRID )
//		| TLC_HGRID_EXT								// show extention horizonal grid lines
//		| TLC_VGRID_EXT								// show extention vertical grid lines
//		| TLC_HGRID_FULL							// show full horizonal grid lines

		| TLC_TREELINE								// show tree line
		| TLC_ROOTLINE								// show root line
		| TLC_BUTTON								// show expand/collapse button [+]
		| TLC_CHECKBOX								// show check box
		| TLC_IMAGE									// show image
		| TLC_HOTTRACK								// show hover text 

		| TLC_DRAG									// drag support
		| TLC_DROP									// drop support
		| TLC_DROPTHIS								// drop self support
//		| TLC_DROPROOT								// drop on root support

		| TLC_NOAUTOCHECK							// do NOT auto set checkbox of parent & child
		);

	treeList_.setAutoFocus(true);
	treeList_.SetBkColor(GetSysColor(COLOR_WINDOW) );
	treeList_.InsertColumn(NULL, TLF_DEFAULT_LEFT, rect.Width()-1);

	LoadBitmaps(&nodeImages_);
	treeList_.SetImageList(&nodeImages_);

	LoadStateBitmaps(&stateImages_);
	treeList_.SetStateImageList(&stateImages_);

	CFont *pf = CFont::FromHandle(
		reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));
	LOGFONT lf;
	pf->GetLogFont(&lf);
	_tcscpy(lf.lfFaceName, _T("Tahoma"));
	CFont font;
	font.CreateFontIndirect(&lf);
	treeList_.SetFont(font, false);
	return 0;
}

void StructureView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	CRect r;
	GetClientRect(&r);
	treeList_.SetColumnWidth(0, r.Width()-2);
	treeList_.MoveWindow(&r, FALSE);
}

BOOL StructureView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void StructureView::OnTimer(UINT nIDEvent)
{
	getProject()->OnTimer(nIDEvent);
	CView::OnTimer(nIDEvent);
}

LRESULT StructureView::OnMenuCommand(WPARAM wParam, LPARAM lParam)
{
	return getProject()->OnMenuCommand(wParam, lParam);
}
