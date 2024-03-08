// TEWnd.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "resource.h"
#include <memory>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>

#include "settingDefs.h"
#include "TEWnd.h"
#include "SplitterClient.h"
#include "TriggerJoiner.h"

#include "qd_named_object.h"
#include "qd_game_object_animated.h"

#include "qd_trigger_chain.h"
#include "qd_game_object_state.h"

#include "object_eraser.h"

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/rtl.hpp>

#include "te_helpers2.h"
#include "ptree_api.h"
#include "ptree_hlprs.hpp"
#include "qd_grid_zone.h"
#include "scenelist.h"
#include "tesettings.h"

#include "ITriggerProfList.h"

#include "TEBaseWorkMode.h"
#include "TEBaseWorkModeDrawer.h"

#include "TEAllChainWorkMode.h"
#include "TEAllChainWorkModeDrawer.h"

#include "TEProfilerWorkMode.h"
#include "TEStatusColoredEleDrawer.h"

#include "memorydc.h"

#include "activetobeginforalltriggers.h"
#include ".\tewnd.h"

#include "Main\MiniMapWnd.h"

#include "serialize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/* Глобальные функции                                                   */
/************************************************************************/
using te_h::get_center;
using te_h::IsKeyPressed;
using te_h::find_in_tc;
using te_h::to_cell_begin;
using te_h::get_cell_begin;
using te_h::get_cell_idx;
using te_h::get_active_ele;
using te_h::get_scene_name;

class offset_ele{
	mutable CRect r_;
	CSize offset_;
	int fullCellSX_;
	int fullCellSY_;
public:
	offset_ele(CSize offset, int fSX, int fSY):
	  offset_(offset),fullCellSX_(fSX),fullCellSY_(fSY){}

	void operator()(qdTriggerElementPtr ptrEle) const
	{
		operator()(ptrEle, offset_);
	}

	VOID operator()(qdTriggerElementPtr ptrEle, const CSize &offset) const
	{
		r_ = ptrEle->bounding_rect();
		r_.OffsetRect(offset);
		ptrEle->set_bounding_rect(r_);
		ptrEle->set_cell_number(get_cell_idx(r_.left, fullCellSX_),
								get_cell_idx(r_.top, fullCellSY_));
	}
};

class bounding_rect {
	CRect r_;
public:
	bounding_rect():r_(0, 0, 0, 0){}
	CRect& getRect(){
		return r_;
	}
	void operator()(qdTriggerElementConstPtr pele) {
		r_.UnionRect(&r_, &pele->bounding_rect());
	}
};
using ITriggerView::eSelectAction;
using ITriggerView::eSelectMode;

class select_te
{
	ITriggerView::eSelectAction sa_;
	ITriggerView::eSelectMode sm_;
	const te_filter_base& f;
	SelectionManager& selMngr_;
public:
	select_te(SelectionManager& selMngr, const te_filter_base& _f, 
		ITriggerView::eSelectAction sa, ITriggerView::eSelectMode sm):f(_f), 
		sa_(sa),sm_(sm), selMngr_(selMngr){}
	void operator()(qdTriggerElementPtr pe) const{
		switch(sm_) {
		case ITriggerView::SM_EXCLUSIVE:
			selMngr_.deselect(pe);
			if (f(pe)&&sa_==ITriggerView::SA_SELECT) 
				selMngr_.select(pe);
			break;
		case ITriggerView::SM_UNION:
			if (!selMngr_.isSelected(pe)&&f(pe)&&sa_==ITriggerView::SA_SELECT) 
				selMngr_.select(pe);
			break;
		case ITriggerView::SM_INTERSECT:
			if (selMngr_.isSelected(pe))
			{
				if(f(pe)) 
				{
					if (sa_==ITriggerView::SA_SELECT)
						selMngr_.select(pe);
				}
				else
				{
					if (sa_!=ITriggerView::SA_SELECT)
						selMngr_.select(pe);
				}
			}
			break;
		}
	}
};

class ExistChain{
	qdGameDispatcher *disp_;
	CString allChainsName_;
public:
	ExistChain(qdGameDispatcher *disp):disp_(disp)
		,allChainsName_((LPTSTR)IDS_ALL_TRIGGER_CHAINS_NAME){}
	bool operator()(std::pair<std::string, CRect> const& vt) const{
		if (vt.first == static_cast<LPCTSTR>(allChainsName_))
			return true;
		qdTriggerChainList const& lst = disp_->trigger_chain_list();
		return (ptree_helpers::findTriggerChainByName(lst, vt.first.c_str()) != NULL);
	}
};
/************************************************************************/
/* Драг и дроп                                                          */
/************************************************************************/
BOOL TEWnd::TEDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pData, 
								 DROPEFFECT dropEffect, CPoint point)
{
	UINT uiDataFormat = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	ASSERT (pData->IsDataAvailable(uiDataFormat)); 
	
	HGLOBAL hData = pData->GetGlobalData(uiDataFormat);
	VOID* pdata = GlobalLock(hData);
	GlobalUnlock(hData);
	
	return static_cast<TEWnd*>(pWnd)->OnDropData(pdata, point);
}

DROPEFFECT TEWnd::TEDropTarget::OnDragEnter( CWnd* pWnd, 
											COleDataObject* pData, 
											DWORD dwKeyState, CPoint point )
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	
	if (pData->IsDataAvailable(uiId))
	{
		HGLOBAL hData = pData->GetGlobalData(uiId);
		VOID* pdata = GlobalLock(hData);
		GlobalUnlock(hData);
		if(static_cast<TEWnd*>(pWnd)->CanDrop(point, pdata)) 
			return DROPEFFECT_COPY;
	}
	return DROPEFFECT_NONE;
}

DROPEFFECT TEWnd::TEDropTarget::OnDragOver( CWnd* pWnd, 
										   COleDataObject* pData, 
										   DWORD dwKeyState, CPoint point )
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	
	
	if (pData->IsDataAvailable(uiId))
	{
		HGLOBAL hData = pData->GetGlobalData(uiId);
		VOID* pdata = GlobalLock(hData);
		GlobalUnlock(hData);
		if((static_cast<TEWnd*>(pWnd)->CanDrop(point, pdata))) 
			return DROPEFFECT_COPY;
	}
	return DROPEFFECT_NONE;
}

DROPEFFECT TEWnd::TEDropTarget::OnDragScroll(CWnd* pWnd, 
											 DWORD dwKeyState, CPoint point)
{
	CRect rectClient, rect;
	pWnd->GetClientRect(rectClient);
	rect = rectClient;

	rect.InflateRect(-10, -10);
	if (rectClient.PtInRect(point) && !rect.PtInRect(point))
	{
		// determine which way to scroll along both X & Y axis
		UINT nTimerID = MAKEWORD(-1, -1);
		if (point.x < rect.left)
			nTimerID = MAKEWORD(SB_LINEUP, HIBYTE(nTimerID));
		else if (point.x >= rect.right)
			nTimerID = MAKEWORD(SB_LINEDOWN, HIBYTE(nTimerID));
		if (point.y < rect.top)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEUP);
		else if (point.y >= rect.bottom)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEDOWN);
		ASSERT(nTimerID != MAKEWORD(-1, -1));

		static_cast<TEWnd*>(pWnd)->OnScroll(nTimerID);
		return DROPEFFECT_SCROLL;
	}
	return DROPEFFECT_COPY;
}
/////////////////////////////////////////////////////////////////////////////
// TEWnd

TEWnd::TEWnd():
	m_ptViewportOrg(0, 0)
	, m_ptrCurrendWorkMode(NULL)
	, m_bVisibleRect4ChainSaved(false)
	, m_ptrGameDisp(NULL)
	, m_hFont(NULL)
	, splitterClient_(NULL)
	, miniMap_(NULL)
{
	m_ptrDT = std::auto_ptr<TEDropTarget>(new TEDropTarget);
	m_iNetCellWidth = DEFAULT_VALUE(TE_NET_CELL_SX);
		//GetMyApp()->GetIntSetting(SETTING_NAME(TE_NET_CELL_SX));
	m_iNetCellHeight = DEFAULT_VALUE(TE_NET_CELL_SY);
		//GetMyApp()->GetIntSetting(SETTING_NAME(TE_NET_CELL_SY));

	m_iNetAmongCellW = DEFAULT_VALUE(TE_NET_AMONG_CELL_SX);
		//GetMyApp()->GetIntSetting(SETTING_NAME(TE_NET_AMONG_CELL_SX));

	m_iNetAmongCellH = DEFAULT_VALUE(TE_NET_AMONG_CELL_SY);
		//GetMyApp()->GetIntSetting(SETTING_NAME(TE_NET_AMONG_CELL_SY));

	InitScale();

	m_iMaxScrollLimX = m_iMinScrollLimX = 0;
	m_iMaxScrollLimY = m_iMinScrollLimY = 0;

	m_bShowAll = false;

	m_fMinScale = m_fMaxScale = 1.f;
	m_hToolTipWnd		= NULL;
	m_eAreaExtending	= AE_NONE;
	m_ptrIScaleMgr		= NULL;
	m_ptrBaseWorkMode.reset(new TEBaseWorkMode(selectionManager_));
	m_ptrBaseWorkModeDrawer.reset(new TEBaseWorkModeDrawer(m_ptrBaseWorkMode.get()));
	m_ptrBaseWorkMode->setDrawer(m_ptrBaseWorkModeDrawer.get());
	setCurrendMode(m_ptrBaseWorkMode.get());
	m_ptrAllChainWorkMode.reset(new TEAllChainWorkMode(qd_get_game_dispatcher()));
	m_ptrAllChainWorkModeDrawer.reset(new TEAllChainWorkModeDrawer);
	m_ptrAllChainWorkMode->setDrawer(m_ptrAllChainWorkModeDrawer.get());

	m_ptrProfilerWorkMode.reset(new TEProfilerWorkMode(selectionManager_));
	m_ptrProfilerWorkModeDrawer.reset(new TEStatusColoredEleDrawer);
	m_ptrProfilerWorkMode->setDrawer(m_ptrProfilerWorkModeDrawer.get());
}

void TEWnd::setSplitterClient(CSplitterClient* client){
	splitterClient_ = client;
}
CSplitterClient* TEWnd::getSplitterClient() const{
	return splitterClient_;
}

void TEWnd::setGameDisp(qdGameDispatcher* ptrDisp){
	m_ptrGameDisp = ptrDisp;
}

void TEWnd::setCurrendMode(ITEWorkMode* pwm)
{
	if (m_ptrCurrendWorkMode)
		m_ptrCurrendWorkMode->OnEnd();
	m_ptrCurrendWorkMode = pwm;
	if (m_ptrCurrendWorkMode)
		m_ptrCurrendWorkMode->OnStart();
}

SelectionManager& TEWnd::getSelectionManager(){
	return selectionManager_;
}

void TEWnd::reset()
{
	m_bShowAll = false;
	m_vecDrwList.clear();
	m_ptrGameDisp = NULL;

	activeChainName_.Empty();
	lastEditedChainName_.Empty();
	chainToRect_.clear();
	m_bVisibleRect4ChainSaved = true;

	InitScale();
	InitViewportOrg();
	SetScale(SA_RESET);
	Invalidate();

	setCurrendMode(NULL);
	selectionManager_.clear();
	m_ptrBaseWorkMode.reset(new TEBaseWorkMode(selectionManager_));
	m_ptrBaseWorkModeDrawer.reset(new TEBaseWorkModeDrawer(m_ptrBaseWorkMode.get()));
	m_ptrBaseWorkMode->setDrawer(m_ptrBaseWorkModeDrawer.get());
	setCurrendMode(m_ptrBaseWorkMode.get());

//	m_ptrProfilerWorkMode.reset(new TEProfilerWorkMode(selectionManager_));
//	m_ptrProfilerWorkModeDrawer.reset(new TEStatusColoredEleDrawer);
//	m_ptrProfilerWorkMode->setDrawer(m_ptrProfilerWorkModeDrawer.get());
}

VOID TEWnd::InitScale()
{
	m_fMinScale = m_fMaxScale = 1.f;
	m_fScale = 1.f;
}

VOID TEWnd::InitViewportOrg()
{
	m_ptViewportOrg = CPoint(0, 0);
}

VOID TEWnd::CrtFont()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));

	lf.lfHeight = scale_y(m_iNetCellHeight - (RECT_INCREASE<<1));
	_tcscpy(lf.lfFaceName, _T("Arial Cyr"));

	m_hFont.reset(CreateFontIndirect(&lf));
}

TEWnd::~TEWnd()
{
}

BEGIN_MESSAGE_MAP(TEWnd, CWnd)
//{{AFX_MSG_MAP(TEWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_GETDISPINFO, 0, OnTTNGetDispInfo)
	ON_COMMAND(ID_TBTN_RUN_TRIGGER_TEST, OnRunTriggerTest)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_COMMAND(ID_TBTN_PROFILER_ON_OFF, OnProfilerOnOff)
	ON_WM_DESTROY()
	ON_COMMAND(IDC_TBTN_DEACTIVATE_TRIGGERS, OnDeactivateAllTriggers)
	ON_COMMAND(IDC_TBTN_ACTIVE_TO_BEGIN_FOR_ALL_TRGRS, OnActiveToBeginForAllTrgrs)
	ON_UPDATE_COMMAND_UI(ID_TBTN_PROFILER_ON_OFF, OnUpdateCommnadUI)
	ON_UPDATE_COMMAND_UI(ID_TBTN_RUN_TRIGGER_TEST, OnUpdateCommnadUI)
	ON_UPDATE_COMMAND_UI(ID_TBTN_ALL_STATES_2_EDITOR, OnUpdateCommnadUI)
	ON_UPDATE_COMMAND_UI(ID_TBTN_CLEAR_STARTS, OnUpdateCommnadUI)
	ON_UPDATE_COMMAND_UI(IDC_TBTN_DEACTIVATE_TRIGGERS, OnUpdateCommnadUI)
	ON_UPDATE_COMMAND_UI(IDC_TBTN_ACTIVE_TO_BEGIN_FOR_ALL_TRGRS, OnUpdateCommnadUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateCommnadUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateCommnadUI)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TEWnd message handlers


BOOL TEWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = AfxRegisterWndClass(	CS_OWNDC|CS_DBLCLKS|
										CS_VREDRAW|CS_HREDRAW,
									GetMyApp()->LoadStandardCursor(IDC_ARROW));

	return CWnd::PreCreateWindow(cs);
}

int TEWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//зарегистрировали драг и доп
	m_ptrDT->Register(this);	
	
	m_hDC = ::GetDC(m_hWnd);

	CrtFont();
	CreateTT();

	return 0;
}

std::string TEWnd::make_te_title(const qdNamedObject* ptrObj)
{
	std::string res;
	return res;
}

const RECT TEWnd::make_te_rect(const CPoint& p, const std::string& title)
{
	return CRect(p.x, p.y, p.x + m_iNetCellWidth, p.y + m_iNetCellHeight);
}
/************************************************************************/
/* обработка движения и бросания                                        */
/************************************************************************/
BOOL TEWnd::OnDropData(VOID* pData, const CPoint& point)
{	
	return m_ptrCurrendWorkMode->OnDropData(this, pData, point);
}

//возвращает истину, если в текущий момент можно добавить елемент в цепочку
bool TEWnd::CanDrop(const CPoint& p, VOID* pData)
{
	return m_ptrCurrendWorkMode->CanDrop(this, pData, p);
}
//////////////////////////////////////////////////////////////////////////


VOID TEWnd::OnSize(UINT nType, int cx, int cy) 
{
	UpdateWndWorkArea();

	CWnd::OnSize(nType, cx, cy);
}

qdTriggerElementPtr TEWnd::find_zone_ele(const qdNamedObject* obj, int status)
{
	ASSERT(m_vecDrwList.size()==1);
	ASSERT(obj->named_object_type()==QD_NAMED_OBJECT_GRID_ZONE);

	qdTriggerChain* pc = m_vecDrwList.front();
	const qdTriggerElementList& l = pc->elements_list();
	qdTriggerElementList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i){
		qdTriggerElementPtr const pe = *i;
		if (pe->object()==obj)
			return pe;
	}
	return NULL;
}

qdTriggerElementPtr TEWnd::find_te(const CPoint& p)
{
	TgrChainVect_t::iterator _itr = m_vecDrwList.begin(), 
		_end = m_vecDrwList.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerChain* pc = *_itr;
		if (PtInRect(&pc->bounding_rect(), p)) {

			if (PtInRect(&pc->root_element()->bounding_rect(), p)) 
				return pc->root_element();

			if (qdTriggerElementPtr ptr = find_in_tc(pc->elements_list(), p)) {
				return ptr;
			}
		}
	}
	return qdTriggerElementPtr(NULL);
}

qdTriggerLink* TEWnd::find_link_in_eles(const CPoint& p, 
								 const qdTriggerElementList& lst, 
								 qdTriggerElementPtr* ppe)
{
	qdTriggerElementList::const_iterator _itr = lst.begin(), _end = lst.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerElementPtr const ptr = *_itr;
		if(qdTriggerLink* plk = find_link_in_ele(p, ptr))
		{
			ASSERT(ppe);
			*ppe = ptr;
			return plk;
		}
	}
	return NULL;
}

qdTriggerLink* TEWnd::find_link_in_ele(const CPoint& p, qdTriggerElementPtr pele)
{
	CPoint pt_from(get_center(pele->bounding_rect()));
	qdTriggerLinkList& lst = pele->children();
	qdTriggerLinkList::iterator _itr = lst.begin(), _end = lst.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerElementConstPtr ptr = _itr->element();

		te_h::TRIANGLE linkTrg;
		te_h::get_link_triangle(pt_from + _itr->get_owner_offset(),
			get_center(ptr->bounding_rect()) + _itr->get_child_offset(), linkTrg);

		if (te_h::PointInTrg(p, linkTrg)) 
			return &*_itr;
	}
	return NULL;
}

qdTriggerLink* TEWnd::find_link(const CPoint &p, qdTriggerElementPtr* ppe)
{
	TgrChainVect_t::iterator _itr = m_vecDrwList.begin(), 
		_end = m_vecDrwList.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerChain* ptrChain = *_itr;
		if (PtInRect(&ptrChain->bounding_rect(), p)) {
			
			//сначала проверяем стартовый элемент
			qdTriggerLink* ptr = 
				find_link_in_ele(p, ptrChain->root_element());
			if (ptr) {
				*ppe = ptrChain->root_element();
				return ptr;
			}
			
			if (ptr = find_link_in_eles( p,	ptrChain->elements_list(), ppe)) {
				return ptr;
			}
		}
	}
	return NULL;
}

qdTriggerChain* TEWnd::WillIntersect(const CRect&r, qdTriggerChain* exclude)
{
	TgrChainVect_t::iterator _itr = m_vecDrwList.begin(), 
		_end = m_vecDrwList.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerChain* ptrChain = *_itr;
		CRect cr(ptrChain->gen_layout_rect());
		if (cr.IntersectRect(r, cr)) {
			if(exclude != ptrChain)
				return ptrChain;
		}
	}
	return NULL;
}

qdTriggerChain* TEWnd::WillIntrsBefMe(const CRect&r, qdTriggerChain* me)
{
	TgrChainVect_t::iterator _itr = m_vecDrwList.begin(), 
		_end = m_vecDrwList.end();
	for(; _itr != _end && *_itr != me; ++_itr)
	{
		qdTriggerChain* ptrChain = *_itr;
		CRect cr(ptrChain->gen_layout_rect());
		if (cr.IntersectRect(r, cr)) {
				return ptrChain;
		}
	}
	return NULL;
}

qdTriggerChain* TEWnd::find_chain(const CPoint& p)
{
	TgrChainVect_t::iterator _itr = m_vecDrwList.begin(), 
		_end = m_vecDrwList.end();
	for(; _itr != _end; ++_itr)
	{
		qdTriggerChain* ptrChain = *_itr;
		CRect r(ptrChain->gen_layout_rect());
		if (r.PtInRect(p)) {
			return ptrChain;
		}
	}
	return NULL;
}

VOID TEWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (this != GetFocus())
		SetFocus();
	m_ptrCurrendWorkMode->OnMouseMove(this, nFlags, point);
	CWnd::OnMouseMove(nFlags, point);
}

VOID TEWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ptrCurrendWorkMode->OnLButtonDown(this, nFlags, point);
	CWnd::OnLButtonDown(nFlags, point);
}

VOID TEWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_ptrCurrendWorkMode->OnLButtonUp(this, nFlags, point);
	CWnd::OnLButtonUp(nFlags, point);
}


/*
*	ITriggerView
*/
BOOL TEWnd::AddState(CPoint& offset, 
					 qdGameObjectState*const* ptrState, int count)
{
	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcUpdate(0, 0, 0, 0);

	ASSERT(!m_vecDrwList.empty()
		&&(!m_bShowAll));
	
	ASSERT(m_vecDrwList.size() == 1);
	qdTriggerChain* ptrActiveChain = m_vecDrwList.front();

	CPoint ptStart(offset.x, offset.y);
	
	if (ptStart.x == -1) {
		CRect rcWA = ptrActiveChain->work_area();
		CRect rcBR = ptrActiveChain->bounding_rect();

		//страхуемся на случай нового триггера с пустым боундом
		ptStart.x = to_cell_begin(rcBR.right + m_iNetAmongCellW, 
			m_iNetCellWidth, m_iNetAmongCellW);
		ptStart.y = rcWA.top;
	}
	
	int iFullH = m_iNetAmongCellH + m_iNetCellHeight;

	for(int i = 0; i < count; ++i)
	{
		qdTriggerElementPtr const ptrEle = ptrActiveChain->
			add_element(static_cast<qdNamedObject*>(ptrState[i]));
		
		if (ptrEle) {
			ptrEle->set_title(
				make_te_title(static_cast<qdNamedObject*>(ptrState[i])));
			ptrEle->update_object_reference();

			ptrEle->set_bounding_rect(make_te_rect(ptStart, ptrEle->title()));
			
			const RECT& r = ptrEle->bounding_rect();
			rcUpdate.UnionRect(&rcUpdate, &r);
			ptStart.y += iFullH;
		}
	}
	
	CRect rcChain = ptrActiveChain->bounding_rect();
	rcChain.UnionRect(&rcChain, &rcUpdate);
	ptrActiveChain->set_bounding_rect(rcChain);

	RemakeChainRect(ptrActiveChain);
	
	UpdateChainWorkArea(ptrActiveChain);
	UpdateWndWorkArea();

	EnsureVisibleBR(ptrActiveChain);

	offset = ptStart;
	
	InvalidateRect(NULL);
	return TRUE;
}

BOOL TEWnd::SetViewState(TVIEW_STATE estate)
{
	return FALSE;
}

CRect TEWnd::GetVisibleRect()
{
	CRect rect;
	GetClientRect(rect);
	scr2log(&rect);
	return rect;
}

void TEWnd::SaveChainVisibleRect(LPCTSTR chainName)
{
	chainToRect_[chainName] = GetVisibleRect();
	m_bVisibleRect4ChainSaved = true;
}

void TEWnd::ReadChainVisibleRect(LPCTSTR chainName)
{
	CRect const& rcChain = chainToRect_[chainName];
	if (!rcChain.IsRectEmpty()&&IsWindowVisible())
	{
		EnsureVisibleRect(rcChain);
	}
	m_bVisibleRect4ChainSaved = false;
}

BOOL TEWnd::ShowTriggers(class qdTriggerChain** ptrTCs, int count)
{
	CWaitCursor wait;
	if (!activeChainName_.IsEmpty())
		SaveChainVisibleRect(activeChainName_);

	qdTriggerChain* pShowed = ShowedTrigger();
	if (*ptrTCs == TE_SHOW_ALL) {
		if (m_bShowAll)
			return TRUE;
		m_bShowAll = true;
		++ptrTCs;
		--count;
		m_rcWorkArea.SetRectEmpty();
	}
	else
		m_bShowAll = false;

	if (m_bShowAll||pShowed!= *ptrTCs)
	{
		m_vecDrwList.clear();
		std::copy_n(ptrTCs, count, std::back_inserter(m_vecDrwList));
		
		TgrChainVect_t::iterator	i = m_vecDrwList.begin(), 
									e = m_vecDrwList.end();
		for(; i != e; ++i){
			RemakeElesAndChainRect(*i);
		}

		if (!m_bShowAll) {
			UpdateChainWorkArea(m_vecDrwList.front());
		}
		else
			CheckLayout();

		UpdateWndWorkArea();
		SetupViewportOrg();
		UpdateScrollers();

		if (m_bShowAll)
		{
			m_ptrAllChainWorkMode->setGameDisp(qd_get_game_dispatcher());
			setCurrendMode(m_ptrAllChainWorkMode.get());
			activeChainName_.LoadString(IDS_ALL_TRIGGER_CHAINS_NAME);
		}
		else 
		{
			m_ptrBaseWorkMode->setChain(m_vecDrwList.front());
			setCurrendMode(m_ptrBaseWorkMode.get());
			activeChainName_ = m_vecDrwList.front()->name();
		}
		if (IsWindowVisible())
		{
			ReadChainVisibleRect(activeChainName_);
			m_bVisibleRect4ChainSaved = false;
		}
		lastEditedChainName_ = activeChainName_;
	}
	InvalidateRect(NULL);
	return TRUE;
}

BOOL TEWnd::Redraw()
{
	InvalidateRect(NULL);
	UpdateWindow();
	return TRUE;
}

BOOL TEWnd::SelectByFilter(const te_filter_base& f, 
						   eSelectAction sa, eSelectMode mode)
{
	if (m_vecDrwList.empty()) {
		return FALSE;
	}

	TgrChainVect_t::iterator i = m_vecDrwList.begin(), e = m_vecDrwList.end();
	for(; i != e; ++i)
	{
		const qdTriggerElementList& lst = (*i)->elements_list();
		std::for_each(lst.begin(), lst.end(),select_te(selectionManager_,f, sa, mode));
	}
	return TRUE;
}

BOOL TEWnd::ShowLTOfSelected(const te_filter_base& f)
{
	if (m_vecDrwList.empty()||m_bShowAll) 
		return FALSE;

	typedef boost::function1<bool, qdTriggerElementConstPtr> FilterWrapp;
	boost::rtl::filtered_range<qdTriggerElementList const, 
		FilterWrapp> range(m_vecDrwList.front()->elements_list(),
								FilterWrapp(boost::ref(f)));


	bounding_rect br = boost::rtl::rng::for_each(range, bounding_rect());
	if (br.getRect().IsRectEmpty()) 
		return TRUE;

	CRect rcClient;
	GetClientRect(rcClient);
	scr2log(rcClient);
	
	CRect rcToShow(br.getRect());
	rcToShow.right = rcToShow.left + rcClient.Width();
	rcToShow.bottom = rcToShow.top + rcClient.Height();
	EnsureVisibleRect(rcToShow);
	return TRUE;
}

BOOL TEWnd::MergeTriggers(qdTriggerChain const* from,qdTriggerChain* to)
{
	CWaitCursor wait;
	qdTriggerElementList lst;
	BOOL res = TriggerJoiner().join(from, to, lst);
	if (res)
	{
		RECT const& r_t = to->bounding_rect();
		bounding_rect br = std::for_each(from->elements_list().begin(),
									from->elements_list().end(), bounding_rect());
		RECT const& r_f = br.getRect();
		CSize const offset(-((r_f.right - r_t.right + r_f.left - r_t.left)>>1),
							m_iNetAmongCellH - r_f.top + r_t.bottom);
		std::for_each(lst.begin(), lst.end(), offset_ele(offset,
			m_iNetCellWidth + m_iNetAmongCellW, m_iNetCellHeight + m_iNetAmongCellH));
		RemakeElesAndChainRect(to);
		UpdateChainWorkArea(to);
		Invalidate();
		SetProjChange();
	}
	return res;
}

BOOL TEWnd::IsVisible()
{
	return (IsWindow(m_hWnd)&&IsWindowVisible());
}

void TEWnd::EnsureVisible(qdTriggerElementPtr const*eles, int count)
{
	bounding_rect br = std::for_each(eles, eles + count, bounding_rect());
	if (br.getRect().IsRectEmpty())
		return;
	EnsureVisibleRect(br.getRect());
	Invalidate();
}

void TEWnd::ShowTrigger(qdNamedObject const* object)
{
	if(m_vecDrwList.empty())
		return;

	qdTriggerChain* const chain = m_vecDrwList.front();
	qdTriggerElementPtr const& ptr = chain->search_element(object);
	if (!ptr)
		return;
	CRect clientRect;
	GetClientRect(clientRect);
	scr2log(clientRect);
	CRect inters;
	CRect bound = ptr->bounding_rect();
	inters.IntersectRect(clientRect, bound);
	if (inters.IsRectEmpty()
		||
		(bound.Width()>>1)>(inters.Width()>>1)
		||(bound.Height()>>1)>(inters.Height()>>1))
	{
		CPoint center = clientRect.CenterPoint();
		CPoint topLeft = bound.TopLeft();
		clientRect.OffsetRect(topLeft - center);
		EnsureVisibleRect(clientRect);
	}
	Invalidate();
}

void TEWnd::SetScaleMgr(IScaleMgr* psmgr)
{
	m_ptrIScaleMgr = psmgr;
}

void TEWnd::UpdateScaleInfo()
{
	if (m_ptrIScaleMgr)
		m_ptrIScaleMgr->UpdateScaleInfo();
}
void TEWnd::IncreaseScale()
{
	SetScale(SA_PLUS);
	Invalidate();
}

void TEWnd::DecreaseScale()
{
	SetScale(SA_MINUS);
	Invalidate();
}

float TEWnd::CurrentScale()
{
	return m_fScale;
}

void TEWnd::SetScale(float fscale)
{
	SetScale(m_fScale, fscale);
	Invalidate();
}

const qdTriggerChain* TEWnd::ConstShowedTrigger() const
{
	if (m_bShowAll)
		return ALL_SHOWED;
	else if (m_vecDrwList.empty())
		return NULL;
	else 
		return m_vecDrwList.front();
}

qdTriggerChain* TEWnd::ShowedTrigger() 
{
	// Изменяем const модификатор на основании того, что 
	// ShowTrigger() вызывается для не const объекта this
	return const_cast<qdTriggerChain*>(ConstShowedTrigger());
}

CString const TEWnd::GetLastEditedTrigger()
{
	return lastEditedChainName_;
}

void TEWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_ptrCurrendWorkMode->OnRButtonDown(this, nFlags, point);
	CWnd::OnRButtonDown(nFlags, point);
}

void TEWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_ptrCurrendWorkMode->OnRButtonUp(this, nFlags, point);
	CWnd::OnRButtonUp(nFlags, point);
}

void TEWnd::OffsetChainsGL(const CSize& sz)
{
	TgrChainVect_t::iterator i = m_vecDrwList.begin(), e = m_vecDrwList.end();
	for(; i != e; ++i)
	{
		qdTriggerChain* ptc = *i;
		CSize gl(ptc->gen_layout());
		gl += sz;
		ptc->set_gen_layout(gl);
	}
}

VOID TEWnd::OffsetChain(qdTriggerChain* ptrChain, const CPoint& new_lt)
{
	CSize sz = new_lt;
	ptrChain->set_gen_layout(sz);
}

VOID TEWnd::OffsetChain(qdTriggerChain* ptrChain, const CSize &sz)
{
	CRect rc(ptrChain->bounding_rect());
	rc.OffsetRect(sz);
	ptrChain->set_bounding_rect(rc);
	const qdTriggerElementList& lst = ptrChain->elements_list();
	qdTriggerElementList::const_iterator i = lst.begin(), e = lst.end();
	for(;i != e; ++i)
	{
		qdTriggerElementPtr const& pe = *i;
		rc = pe->bounding_rect();
		rc.OffsetRect(sz);
		pe->set_bounding_rect(rc);
		RemakeCellIdx(pe, te_h::get_lt(pe->bounding_rect()));
	}
	rc = ptrChain->root_element()->bounding_rect();
	rc.OffsetRect(sz);
	ptrChain->root_element()->set_bounding_rect(rc);
	RemakeCellIdx(ptrChain->root_element(), 
		te_h::get_lt(ptrChain->root_element()->bounding_rect()));
}

VOID TEWnd::RemakeChainRect(qdTriggerChain* ptrChain)
{
	qdTriggerElementList::const_iterator i = ptrChain->elements_list().begin(),
		e = ptrChain->elements_list().end();

	bounding_rect br = std::for_each(i, e, bounding_rect());
	br.getRect().UnionRect(&br.getRect(), &ptrChain->root_element()->bounding_rect());
	ptrChain->set_bounding_rect(br.getRect());
}

VOID TEWnd::RemakeCellIdx(qdTriggerElementPtr pe, const CPoint& p)
{
	te_h::recalc_cell_number(pe, p, 
							m_iNetCellWidth + m_iNetAmongCellW,
							m_iNetCellHeight + m_iNetAmongCellH);
}

bool TEWnd::RemakeEleRect(const POINT&v, CRect& rcE)
{
	if(v.x == -1||v.y == -1)
		return false;
	rcE.left = get_cell_begin(v.x, m_iNetCellWidth + m_iNetAmongCellW);
	rcE.top = get_cell_begin(v.y, m_iNetCellHeight + m_iNetAmongCellH);
	rcE.right = rcE.left + m_iNetCellWidth;
	rcE.bottom = rcE.top + m_iNetCellHeight;
	return true;
}

VOID TEWnd::RemakeElesAndChainRect(qdTriggerChain* ptrChain)
{
	const qdTriggerElementList& tel = ptrChain->elements_list();
	CRect rcChain(0, 0, 0, 0);

	qdTriggerElementList::const_iterator i = tel.begin(), e = tel.end();
	CRect rcE(0, 0, 0, 0);
	for(; i != e; ++i)
	{
		qdTriggerElementPtr const& pe = *i;
		if(RemakeEleRect(pe->cell_number(), rcE))
			pe->set_bounding_rect(rcE);
		else
			rcE = pe->bounding_rect();
		rcChain.UnionRect(rcChain, rcE);
	}

	if (ptrChain->elements_list().empty()) 
	{
		CDC *pdc = GetDC();
		HGDIOBJ hOldFont = pdc->SelectObject(m_hFont.get());

		CSize sz (pdc->GetTextExtent(ptrChain->name(), _tcslen(ptrChain->name())));

		TEXTMETRIC   tm;
		pdc->GetTextMetrics(&tm);

		rcE.right = sz.cx + tm.tmAveCharWidth;
		rcE.bottom = sz.cy;

		pdc->SelectObject(hOldFont);
		ReleaseDC(pdc);
	}
	else{
		if(RemakeEleRect(ptrChain->root_element()->cell_number(), rcE))
			ptrChain->root_element()->set_bounding_rect(rcE);
		else
			rcE = ptrChain->root_element()->bounding_rect();
	}
	rcChain.UnionRect(rcChain, rcE);
	ptrChain->set_bounding_rect(rcChain);
}

CRect TEWnd::GetRootNodeRect(qdTriggerElementPtr ptrRootEle, 
							 const RECT& rcFirstEle)
{
	CRect res = make_te_rect(CPoint(rcFirstEle.left, rcFirstEle.top), 
		ptrRootEle->title());
	
	res.OffsetRect(0, - m_iNetCellHeight - m_iNetAmongCellH);
	
	if (m_rcWorkArea.top > res.top) {
		res.OffsetRect(m_iNetCellWidth + m_iNetAmongCellW, 
			m_rcWorkArea.top - res.top);
	}
	return res;
}

VOID TEWnd::to_cell_lt(POINT* p)
{
	p->x = to_cell_begin(p->x, m_iNetCellWidth, m_iNetAmongCellW);
	p->y = to_cell_begin(p->y, m_iNetCellHeight, m_iNetAmongCellH);
}

bool TEWnd::InTrigger(qdNamedObject* pobj, qdTriggerChain* pc)
{
	return pc->is_element_in_list(pobj);
}

VOID TEWnd::UpdateWndWorkArea()
{
	if (m_bShowAll) 
	{
		CRect rcClient = GetVisibleRect();
		rcClient.OffsetRect(-rcClient.left,-rcClient.top);
		TgrChainVect_t::iterator i = m_vecDrwList.begin(), e = m_vecDrwList.end();
		for(; i != e; ++i)
			rcClient.UnionRect(&(*i)->gen_layout_rect(), rcClient);
		m_rcWorkArea.UnionRect(rcClient, m_rcWorkArea);
	}
	else if (!m_vecDrwList.empty()) 
	{
		m_rcWorkArea = m_vecDrwList.front()->work_area();
	}
	else
	{
		GetClientRect(m_rcWorkArea);
		scr2log(m_rcWorkArea);
	}
	SetScaleRanges();
	SetScrollRanges();
	UpdateScrollers();
}

//пересчитываем размеры рабочей области
//если надо увеличиваем
VOID TEWnd::UpdateChainWorkArea(qdTriggerChain* ptrChain)
{
	CRect rcClient(0, 0, 8000, 8000);
	
	if (ptrChain->elements_list().empty()) {
		ptrChain->set_work_area(rcClient);
	}
	else
	{
		CRect rcWA = ptrChain->bounding_rect();

		rcWA.left -= get_wa_x_add();
		rcWA.right += get_wa_x_add();

		rcWA.top -= get_wa_y_add();
		rcWA.bottom += get_wa_y_add();

		//если окно полностью накрывает увеличенный прямоугольник цепочки,
		//то в качестве рабочей области назначаем оконный прямоугольник

		rcWA.left = (std::min)(rcWA.left, rcClient.left);
		rcWA.top = (std::min)(rcWA.top, rcClient.top);
		rcWA.right = (std::max)(rcWA.right, rcClient.right);
		rcWA.bottom = (std::max)(rcWA.bottom, rcClient.bottom);

		CSize sz(-(std::min)(0l, rcWA.left), -(std::min)(0l, rcWA.top));
		if (sz.cx || sz.cy) {
			rcWA.OffsetRect(sz);
			//сдвигаем каждый элемент
			OffsetChain(ptrChain, sz);
		}
		
		ptrChain->set_work_area(rcWA);
	}
}

VOID TEWnd::CheckLayout()
{
	const CSize INVALID_LAYOUT(-1, -1);

	CRect rcUnion(10, 10, 11, 11);
	CRect _tmp;
	TgrChainVect_t::iterator i = m_vecDrwList.begin(), e = m_vecDrwList.end();
	for(; i != e; ++i)
	{
		qdTriggerChain* pc = *i;
		CSize szGenOff(pc->gen_layout());
		if (szGenOff == INVALID_LAYOUT) {
			szGenOff.cx = rcUnion.right + 20;
			szGenOff.cy = rcUnion.top;
			pc->set_gen_layout(szGenOff);
		}
		else if (_tmp.IntersectRect(rcUnion, &pc->gen_layout_rect()))
		{
			CPoint lt = te_h::get_lt(pc->bounding_rect());
			szGenOff.cx = rcUnion.right + 20;
			szGenOff.cy = rcUnion.top;
			pc->set_gen_layout(szGenOff);
		}
		rcUnion.UnionRect(rcUnion, &pc->gen_layout_rect());
	}
}

void TEWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_ptrCurrendWorkMode->OnLButtonDblClk(this, nFlags, point);
	CWnd::OnLButtonDblClk(nFlags, point);
}

CSize TEWnd::GetLinkOffset(const CPoint& ptFrom, const CPoint& ptTo, 
										const RECT& bound)
{
	CPoint pts[4];
	pts[0] = CPoint(bound.left, bound.top);
	pts[1] = CPoint(bound.left, bound.bottom);
	pts[2] = CPoint(bound.right, bound.bottom);
	pts[3] = CPoint(bound.right, bound.top);

	float kx = 0.f;

	BYTE bSide = te_h::check_inters(ptFrom, ptTo,pts, &kx, NULL);
	ASSERT(0.f<= kx&&kx<=1.f);

	CSize dir = ptTo - ptFrom;
	return CPoint(ptFrom.x + kx*dir.cx, ptFrom.y + kx*dir.cy) - get_center(bound);
}

VOID TEWnd::CalcLinkOffsets(qdTriggerElementPtr parent, qdTriggerElementPtr child,
						CPoint ptFrom, CPoint ptTo)
{
	ASSERT(parent);
	ASSERT(child);
	qdTriggerLink *link = parent->find_child_link(child);
	ASSERT(link);

	link->set_owner_offset(
		GetLinkOffset(ptFrom, ptTo, parent->bounding_rect()));

	link->set_child_offset(GetLinkOffset(ptTo, ptFrom, child->bounding_rect()));
}

CSize TEWnd::GetGLLeftTop()
{
	ASSERT(!m_vecDrwList.empty());
	TgrChainVect_t::iterator i = m_vecDrwList.begin(), e = m_vecDrwList.end();
	CSize sz = (*i)->gen_layout();
	++i;
	for(; i != e; ++i){
		if (sz.cx > (*i)->gen_layout().cx)
			sz.cx = (*i)->gen_layout().cx;
		if (sz.cy > (*i)->gen_layout().cy)
			sz.cy = (*i)->gen_layout().cy;
	}
	return sz;
}

VOID TEWnd::SetupViewportOrg()
{
//	ASSERT(round(m_fScale) == 1);
	m_ptViewportOrg.x = m_ptViewportOrg.y = 0;
	if (!m_bShowAll) {
		qdTriggerChain* ptrChain = m_vecDrwList.front();
		ASSERT(ptrChain);
		CPoint lt = te_h::get_lt( ptrChain->bounding_rect());
		log2scr(&lt, 1);
		m_ptViewportOrg = -lt;
	}
	else
	{
		if (m_vecDrwList.empty())
			return;
		CSize lt(GetGLLeftTop());
		log2scr(&lt);
		m_ptViewportOrg.x = -lt.cx;
		m_ptViewportOrg.y = -lt.cy;
	}
}

HWND TEWnd::CreateTT()
{
    // Create the ToolTip control.
    m_hToolTipWnd = CreateWindow(TOOLTIPS_CLASS, TEXT(""),
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, AfxGetApp()->m_hInstance,
		NULL);
	
    TOOLINFO    ti;
    // Prepare TOOLINFO structure for use as tracking ToolTip.
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd   = m_hWnd;
    ti.uId    = reinterpret_cast<UINT_PTR>(m_hWnd);
    ti.hinst  = AfxGetApp()->m_hInstance;
    ti.lpszText  = LPSTR_TEXTCALLBACK;
    ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 
    
    // Add the tool to the control, displaying an error if needed.
    if(!::SendMessage(m_hToolTipWnd,TTM_ADDTOOL,0,(LPARAM)&ti)){
        MessageBox(_T("Couldn't create the ToolTip control."),
			_T("Error"),MB_OK);
        return NULL;
    }
		
    return(m_hToolTipWnd);    
}

void TEWnd::trackToolTip(CPoint const& pos)
{
	::SendMessage(m_hToolTipWnd, TTM_TRACKPOSITION, 
			0, (LPARAM)MAKELPARAM(pos.x + 15,pos.y + 15));
}

void TEWnd::setToolTipText(CString const& str)
{
	if (m_strToolTipText != str){ 
		TOOLINFO    ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
		ti.hwnd   = m_hWnd;
		ti.uId    = reinterpret_cast<UINT_PTR>(m_hWnd);
		ti.hinst  = AfxGetApp()->m_hInstance;
		ti.lpszText  = LPSTR_TEXTCALLBACK;
		ti.rect.top = ti.rect.bottom = ti.rect.left = ti.rect.right = 0; 

		ti.lParam = NULL;
		// Activate (display) the tracking ToolTip. Then, set a global
		// flag value to indicate that the ToolTip is active, so other
		// functions can check to see if it's visible.
		m_strToolTipText = str;
		::SendMessage(m_hToolTipWnd,TTM_TRACKACTIVATE,
			(WPARAM)(!str.IsEmpty()),(LPARAM)&ti);
	}
}

BOOL TEWnd::OnTTNGetDispInfo(UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	NMTTDISPINFO* pdi = reinterpret_cast<NMTTDISPINFO *>(pTTTStruct);
	if (reinterpret_cast<HWND>(pdi->hdr.idFrom) != *this)
		return FALSE;
	pdi->lpszText = const_cast<TCHAR*>((LPCTSTR)getToolTipText());
	pdi->hinst = NULL;
	return TRUE;
}

VOID TEWnd::EnsureVisibleBR(qdTriggerChain* ptrChain){
	EnsureVisibleRect(ptrChain->bounding_rect());
}

//! вычисляем какую сцену запустить для данного элемента
const CString TEWnd::get_scene_name(qdGameDispatcher* gameDisp,
									qdTriggerElementConstPtr ptrEle, 
									qdTriggerChain* ptrChain)
{
	if (ptrEle == ptrChain->root_element()) 
	{
		//элемент "Старт" неактивный, значит мы искали
		//по первому предыдущему и им оказался "Старт"
		//поиск не удался

		//для стартового элемента определям запускаемую сцену по первому
		//следующему за ним
		//если триггер пустой, то ничего - неопределенность

		if (ptrEle->is_active()&&!ptrEle->children().empty())
			return get_scene_name(gameDisp, 
						ptrEle->children().front().element(), 
						ptrChain);
	}
	else {
		switch(ptrEle->object()->named_object_type()) {
			case QD_NAMED_OBJECT_SCENE:
				return CString(ptrEle->object()->name());
			case QD_NAMED_OBJECT_GRID_ZONE_STATE:
				return CString(ptrEle->object()->owner()->owner()->name());
			case QD_NAMED_OBJECT_MINIGAME:
				{
					//если активынй элемент содержит миниигру, то берем
					//сцену из первого предыдущего

					//если нет предыдущих, то не можем определиться
					if (ptrEle->parents().empty())
						break;
					return get_scene_name(gameDisp,
								ptrEle->parents().front().element(), ptrChain);
				}
			case QD_NAMED_OBJECT_OBJ_STATE:
				{
					//получаем имя сцены из состояния
					CString strName = te_h::get_scene_name(ptrEle->object());

					if (!strName.IsEmpty()) 
						return strName;

					//если не смогли, значит это глобальное состояние

					//если нет предыдущих, то не можем определиться
					if (ptrEle->parents().empty())
						break;
					
					if (ptrEle->parents().front().element() == ptrChain->root_element()) 
						break;
					//берем первый предыдущий элемент
					return get_scene_name(gameDisp,
							ptrEle->parents().front().element(), ptrChain);
				}
		}
	}
	SceneList sl;
	sl.set_data(gameDisp);
	sl.set_multi_select(false);
	sl.set_show_crt_new(false);
	if (sl.DoModal() == IDOK){
		qdGameScene*ps = sl.get_scene();
		if (ps)
			return CString(ps->name());
	}
	return CString();
}

void TEWnd::OnRunTriggerTest()
{
	qdTriggerChain* ptrWorkChain = m_vecDrwList.front();
	qdTriggerElementConstPtr ptrActive = get_active_ele(ptrWorkChain);
	if (!ptrActive)
		ptrActive = ptrWorkChain->root_element();

	CString strSceneName = get_scene_name(m_ptrGameDisp, ptrActive, ptrWorkChain);
	if (!strSceneName.IsEmpty()) 
		ptree_api::RunScene(strSceneName, ptree_api::RM_TRIGGER_TEST);
}

void TEWnd::clear_active(qdTriggerChain* pc)
{
	ActiveToBeginForAllTriggers::MakeRootActive::DO(false, pc);
}

void TEWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_ptrCurrendWorkMode->OnKeyDown(this, nChar, nRepCnt, nFlags);
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void TEWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_ptrCurrendWorkMode->OnKeyUp(this, nChar, nRepCnt, nFlags);
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL TEWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return m_ptrCurrendWorkMode->OnSetCursor(this, pWnd, nHitTest, message);
}

void TEWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	m_ptrCurrendWorkMode->OnShowWindow(this, bShow, nStatus);

//	if (!bShow&&!m_vecDrwList.empty())
//		SaveChainVisibleRect(activeChainName_);
//	else
//	{
//		if (!IsWindowVisible())
//			ShowWindow(SW_SHOWNORMAL );
//		ReadChainVisibleRect(activeChainName_);
//	}
}

void TEWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
	m_ptrCurrendWorkMode->OnDrawItem(nIDCtl, lpdis);
	__super::OnDrawItem(nIDCtl, lpdis);
}

void TEWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
	m_ptrCurrendWorkMode->OnMeasureItem(nIDCtl, lpmis);
	__super::OnMeasureItem(nIDCtl, lpmis);
}

void TEWnd::OnProfilerOnOff()
{
	if (m_ptrCurrendWorkMode != m_ptrBaseWorkMode.get())
		setCurrendMode(m_ptrBaseWorkMode.get());
	else 
	{
		m_ptrProfilerWorkMode->setChain(m_vecDrwList.front());
		setCurrendMode(m_ptrProfilerWorkMode.get());
	}
	Redraw();
}

void TEWnd::SetTriggerProfiler(ITriggerProfList* ptrITPList)
{
	m_ptrProfilerWorkMode->setTriggerProfList(ptrITPList);
}

DWORD TEWnd::PtInScrollArea(const CPoint& p)
{
	const int OFFSET = 12;
	CRect rcClt;
	GetClientRect(rcClt);

	scr2log(&rcClt);
	
	DWORD dwRes = 0;
	
	if (p.x <= rcClt.left + OFFSET)//левый край
		dwRes |= SAP_LEFT;
	else if (p.x >= rcClt.right - OFFSET)//правый край
		dwRes |= SAP_RIGHT;
	
	if (p.y <= rcClt.top + OFFSET)//верхний
		dwRes |= SAP_TOP;
	else if (p.y >= rcClt.bottom - OFFSET)//нижний 
		dwRes |= SAP_BOTTOM;
	return dwRes;
}

void TEWnd::OffsetIfEmpty(qdTriggerElementPtr pele, int offsetx, int offsety)
{
	CRect r = pele->bounding_rect();
	r.OffsetRect(offsetx, offsety);
	if (!find_te(r.TopLeft())) {
		pele->set_bounding_rect(r);
		RemakeCellIdx(pele, r.TopLeft());
	}
}
boost::shared_ptr<AppSettingsBasePage> TEWnd::getPage()
{
	boost::shared_ptr<TESettings> ptrPage(new TESettings);
	ptrPage->AddListener(this);
	return ptrPage;
}

void TEWnd::SettingsChanged()
{
	if (m_ptrCurrendWorkMode)
		m_ptrCurrendWorkMode->OnSettingsChange();
}

BOOL TEWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, 
					  AFX_CMDHANDLERINFO* pHandlerInfo)
{
	BOOL res = CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	if (!res&&nCode == CN_COMMAND)
	{
//		if (m_ptrCurrendWorkMode&&m_ptrCurrendWorkMode->OnCommand(this, nID, 0))
//			return TRUE;
//		else
//			return FALSE;
	}

	return res;
}
BOOL TEWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_ptrCurrendWorkMode&&m_ptrCurrendWorkMode->OnCommand(this, wParam, lParam))
		return TRUE;
	return __super::OnCommand(wParam, lParam);
}

void TEWnd::OnDestroy()
{
	SelectObject(m_hDC, *GetFont());
	__super::OnDestroy();
	::ReleaseDC(m_hWnd, m_hDC);
}

VOID TEWnd::PaintRect(const RECT* lpRc)
{
	CRect r;
	GetClientRect(r);

	SelectClipRgn(m_hDC, NULL);

	CDC* pdc = CDC::FromHandle(m_hDC);
	axl::CMemoryDC __mem_dc__(pdc,&CSize(r.Width(), r.Height()));


	CDC* pMemDC = __mem_dc__.GetDC();

	pMemDC->SetViewportOrg(pdc->GetViewportOrg());

	pMemDC->FillSolidRect(&r,RGB(255, 255, 255));

	HGDIOBJ holdf = pMemDC->SelectObject(m_hFont.get());

	ASSERT(lpRc);
	CRect rc = r;
	CRect rc2(r);
	scr2log(rc);
	m_ptrCurrendWorkMode->OnPaint(this, pMemDC, rc);
	pMemDC->SelectObject(holdf);

	BitBlt(m_hDC, rc2.left, rc2.top, 
		rc2.Width(), rc2.Height(), 
		pMemDC->m_hDC, 
		rc2.left, rc2.top, SRCCOPY);
}

VOID TEWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect r(dc.m_ps.rcPaint);
	PaintRect(r);
	// При перерисовке перерисовываем и миникарту
	if (miniMap_)
	{
		miniMap_->InvalidateRect(NULL); // Всю миникарту нужно обновить Windows
		miniMap_->OnPaint();
	}
}

void TEWnd::OnDeactivateAllTriggers()
{
	ASSERT(m_ptrGameDisp);
	qdTriggerChainList const& lst = m_ptrGameDisp->trigger_chain_list();
	qdTriggerChainList::const_iterator i = lst.begin(), e = lst.end();
	for(; i != e; ++i)
	{
		qdTriggerChain* pc = *i;
		qdTriggerElementList const& lst = pc->elements_list();
		qdTriggerElementList::const_iterator itr = lst.begin(), end = lst.end();
		for(; itr != end; ++itr)
			(*itr)->make_active(false);
		pc->root_element()->make_active(false);
		pc->init_debug_check();
	}
	SetProjChange();
	InvalidateRect(NULL);
}

void TEWnd::OnActiveToBeginForAllTrgrs()
{
	ASSERT(m_ptrGameDisp);
	ActiveToBeginForAllTriggers::DO(m_ptrGameDisp);
	SetProjChange();
	InvalidateRect(NULL);
}

void TEWnd::OnUpdateTbtnRunTriggerTest(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_vecDrwList.empty()&&!m_bShowAll&&IsWindowVisible());
}

void TEWnd::OnUpdateProfilerOnOff(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsWindowVisible()
		&&(m_ptrCurrendWorkMode == m_ptrBaseWorkMode.get()
		||m_ptrCurrendWorkMode == m_ptrProfilerWorkMode.get()));
	pCmdUI->SetCheck(m_ptrCurrendWorkMode == m_ptrProfilerWorkMode.get());
}

void TEWnd::OnUpdateTbtnClearStarts(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsWindowVisible()&&m_ptrCurrendWorkMode == m_ptrBaseWorkMode.get());
}

void TEWnd::OnUpdateTbtnAllStates2Editor(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);	
}

void TEWnd::OnUpdateDeactivateAllTriggers(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsWindowVisible()&&m_ptrGameDisp != NULL);
}

void TEWnd::OnUpdateActiveToBeginForAllTrgrs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsWindowVisible()&&m_ptrGameDisp != NULL);
}

void TEWnd::OnUpdateCommnadUI(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case ID_TBTN_PROFILER_ON_OFF:
		OnUpdateProfilerOnOff(pCmdUI);
		break;
	case ID_TBTN_RUN_TRIGGER_TEST:
		OnUpdateTbtnRunTriggerTest(pCmdUI);
		break;
	case ID_TBTN_ALL_STATES_2_EDITOR:
		OnUpdateTbtnAllStates2Editor(pCmdUI);
		break;
	case ID_TBTN_CLEAR_STARTS:
		OnUpdateTbtnClearStarts(pCmdUI);
		break;
	case IDC_TBTN_DEACTIVATE_TRIGGERS:
		OnUpdateDeactivateAllTriggers(pCmdUI);
		break;
	case IDC_TBTN_ACTIVE_TO_BEGIN_FOR_ALL_TRGRS:
		OnUpdateActiveToBeginForAllTrgrs(pCmdUI);
		break;
	default:
		{
			if (IsWindowVisible()
				&&m_ptrCurrendWorkMode
				&&!m_ptrCurrendWorkMode->OnCommandUpdateUI(this, pCmdUI))
			{
					pCmdUI->ContinueRouting();
			}
		}
	}
}

void TEWnd::SaveVisibleRect()
{
	CRect rcClient;
	GetClientRect(rcClient);
	scr2log(rcClient);
	m_bVisibleRect4ChainSaved = true;
}

void TEWnd::SaveIniSettings(IProjectSettings* pps)
{
	if (!m_ptrGameDisp)
		return;
	if (!m_bVisibleRect4ChainSaved)
		SaveChainVisibleRect(activeChainName_);

	std::ostringstream output;
	std::ostream_iterator<ChainToRect::value_type> out(output, _T(" "));
	std::copy(
		boost::make_filter_iterator(
									ExistChain(m_ptrGameDisp), 
									chainToRect_.begin(),
									chainToRect_.end()
									), 
		boost::make_filter_iterator(
									ExistChain(m_ptrGameDisp), 
									chainToRect_.end(),
									chainToRect_.end()
									), out);

	CString section;
	section.LoadString(IDS_INI_SEC_NAME_TE);
	CString setting;
	setting.LoadString(IDS_INI_KEY_NAME_VISIBLE_CHAINS_RECTS);
	pps->WriteStrSetting(section, setting, output.str().c_str());
	setting.LoadString(IDS_INI_KEY_NAME_LAST_TRIGGER_CHAIN);
	pps->WriteStrSetting(section, setting, lastEditedChainName_);
}

void TEWnd::ReadIniSettings(IProjectSettings* pps)
{
	CString section;
	section.LoadString(IDS_INI_SEC_NAME_TE);
	CString setting;
	setting.LoadString(IDS_INI_KEY_NAME_VISIBLE_CHAINS_RECTS);
	CString val = pps->GetStrSetting(section, setting, CString());

	std::string buffer(val);
	std::istringstream input(buffer);
	std::istream_iterator<std::pair<std::string, CRect> > in(input);
	std::istream_iterator<std::pair<std::string, CRect> > end;
	for(; in != end; ++in){
		ChainToRect::value_type const& v = *in;
		chainToRect_[v.first] = v.second;
	}
	setting.LoadString(IDS_INI_KEY_NAME_LAST_TRIGGER_CHAIN);
	lastEditedChainName_ = pps->GetStrSetting(section, setting, CString());
}

void TEWnd::OnEndIniReading()
{
	if (IsWindowVisible()&&!m_vecDrwList.empty())
	{
		ReadChainVisibleRect(lastEditedChainName_);
		Invalidate();
	}
}

//! Вызывается до изменения
bool TEWnd::OnVisibilityChanging(eVCAction action)
{
	if (action == VCA_HIDE)
	{
		if (!activeChainName_.IsEmpty())
			SaveChainVisibleRect(activeChainName_);
	}
	return true;
}
//! Вызывается после изменения
void TEWnd::OnVisibilityChange(eVCAction action)
{
	if (action == VCA_SHOW)
	{
		ReadChainVisibleRect(activeChainName_);
		Invalidate();
	}
}

void TEWnd::activate()
{
	getSplitterClient()->SetResizedChild(this);
	ShowWindow(SW_SHOW);
}

void TEWnd::deactivate()
{
	getSplitterClient()->SetResizedChild(NULL);
	ShowWindow(SW_HIDE);
}

void TEWnd::clearUndobuffer()
{
	m_ptrCurrendWorkMode->clearUndobuffer();
}
