#include "stdafx.h"
#include "resource.h"
#include "QuestEditor.h"

#include "TEBaseWorkMode.h"
#include "tewnd.h"
#include <boost/iterator/filter_iterator.hpp>
#include <boost/rtl.hpp>

#include "qd_trigger_chain.h"
#include "qd_trigger_element.h"
#include "te_helpers2.h"
#include "te_consts.h"
#include "ptree_api.h"
#include "TEBaseWorkModeDrawerBase.h"
#include <boost/bind.hpp>
#include "obj_type_detectors.h"
#include "menu_helpers.h"

#include "MoveLinkEnd.h"
#include "MakeLinkActive.h"
#include "MoveTEElementPos.h"

#include "AddLink.h"
#include "DeleteLink.h"

#include "AddElement.h"
#include "DeleteElement.h"

#include "InsertColumns.h"
#include "DeleteColumns.h"

#include "InsertRows.h"
#include "DeleteRows.h"

#include "ChangeLinkType.h"
#include "ChangeAutoRestart.h"

#include "SelectIfIntersect.h"


void SetProjChange();

TEBaseWorkMode::TEBaseWorkMode(SelectionManager& selectionManger)
: TESingleChainWorkMode(selectionManger)
, m_bMouseMoved(false)
, m_ptrActiveLink(NULL)
, m_ptrDataKeeper(new DataKeeper(NULL))
, m_iDrawedLinkType(0)
, m_iActiveLinkType(0)
, m_eLATest(LAT_NONE)
, m_eBetweenPlotsLayout(BPL_NOWERE)
, m_ptrHilitedLinksElement(NULL)
, m_ptrDrawer(NULL)
{
	m_bShowMenuOnLinkCreate = static_cast<bool>(
		GetMyApp()->GetIntSetting(SETTING_NAME(TE_SHOW_MENU_ON_LINK_CREATE)));
}

TEBaseWorkMode::~TEBaseWorkMode(void)
{
}

void TEBaseWorkMode::setMouseMoved(bool bVal){
	m_bMouseMoved = bVal;
}

void TEBaseWorkMode::setChain(qdTriggerChain* pchain){
	if (getChain() != pchain)
	{
		setActiveLink(NULL);
		setLinkOwner(NULL);
		TESingleChainWorkMode::setChain(pchain);
	}
}
void TEBaseWorkMode::setLinkOwner(qdTriggerElementPtr pele){
	m_ptrLinkOwner = pele;
}
void TEBaseWorkMode::setActiveLink(qdTriggerLink* plink){
	m_ptrActiveLink = plink;
}
void TEBaseWorkMode::setDrawedLinkType(int newType){
	m_iDrawedLinkType = newType;
}
void TEBaseWorkMode::setObjPos(CPoint const& point){
	m_ptObjPos = point;
}
void TEBaseWorkMode::setLATest(eLinkAnchorTest eVal){
	m_eLATest = eVal;
}
void TEBaseWorkMode::setActiveLinkType(int iVal){
	m_iActiveLinkType = iVal;
}
void TEBaseWorkMode::setBetweenPlotsLayout(eBetweenPlotsLayout eVal){
	m_eBetweenPlotsLayout = eVal;
}

bool TEBaseWorkMode::setDrawer(TEBaseWorkModeDrawerBase* ptrDrawer){
	if (!ptrDrawer)
		return false;
	m_ptrDrawer = ptrDrawer;
	return true;
}

void TEBaseWorkMode::setHilitedLinksElement(qdTriggerElementConstPtr pele){
	m_ptrHilitedLinksElement = pele;
}

TEUndoManager& TEBaseWorkMode::getUndoManager(qdTriggerChain* pchain){
	assert(pchain);
	return m_undoBuffers[pchain];
}

/**
	���� ����� ������������ ���������� �� � ������������.
	���� ������, �� ����������� ����� ��� ������ � � ����������� �� �� 
	����������� ����������� �������� ������
 */
void TEBaseWorkMode::OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		Base::OnLButtonDown(pwnd, nFlags, point);
		return;
	}
	CPoint p(point);
	pwnd->scr2log(&p);

	setMouseMoved(false);
	setBasePoint(p);
	if (getActiveLink()) {
		//��������� �� ������ �� �� � ����� ��������� �����
		m_eLATest = find_link_anchor(getLinkOwner(), getActiveLink(), p);

		if (m_eLATest != LAT_NONE) {
			
			if (m_eLATest == LAT_BEGIN) {
				/// ��� ���� ������ �� ����
				setObjPos(te_h::get_link_begin(getLinkOwner(), getActiveLink()));/// ��� ���� ������ �� ����
				/// ��������� ������ �����, ����� ����� �������� � undo buffer
				m_undo = MoveLinkEnd(getActiveLink(), 
									getActiveLink()->get_owner_offset(), 
									MoveLinkEnd::MLE_OWNER_OFFSET);
			}
			else if (m_eLATest == LAT_END) {
				
				setObjPos(te_h::get_link_end(getActiveLink()));/// ��� ���� ������ �� ����
				m_undo = MoveLinkEnd(getActiveLink(), 
									getActiveLink()->get_child_offset(), 
									MoveLinkEnd::MLE_CHILD_OFFSET);
			}
			return;
		}
	}

	qdTriggerElementPtr pele = NULL;
	qdTriggerLink* plink = pwnd->find_link(p, &pele);
	activateLink(plink, pele);

	if (getActiveLink()) 
		pwnd->Invalidate();
	else
	{
		bool const shiftPressed = te_h::IsKeyPressed(VK_SHIFT);
		/// ���� �������
		pele = pwnd->find_te(p);	
		setBaseElement(pele);
		if (pele) 
		{
			/// � ����������� �� ���� ������ �� SHIFT, ������ ������� ��� ���
			/// ������ ��������� ������� �����������
			if (!shiftPressed)
			{
				if (!getSelectionManager().isSelected(pele))
				{
					getSelectionManager().deselectAll();
					getSelectionManager().select(pele);
					pwnd->RedrawWindow();
				}
				else
					getSelectionManager().reselect(pele);

				/// ����� ������� ��������
				setObjPos(CPoint(INT_MIN, INT_MIN));
				pwnd->SetCapture();
			}
			else
			{
				SelectionManager& selectionManager = getSelectionManager();
				if (selectionManager.getCount() != 1
					&&selectionManager.isSelected(pele))
					selectionManager.deselect(pele);
				else
					selectionManager.select(pele);
				pwnd->RedrawWindow();
			}
		}
		else{
			CRectTracker rectTracker;
			if (!rectTracker.TrackRubberBand(pwnd, point)) 
			{
				/// ���� �������� ���������� ������, ��
				/// �������� ���������, ������ ��� ��� ��������� ��������.
				/// � ��� �������� ����������� �������
				pwnd->SendMessage(WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y));
			}
			else
			{
				/// ���� ����� ���� ��������
				/// �� �������� ����� ��������� ���������
				if (!te_h::IsKeyPressed(VK_SHIFT))
					getSelectionManager().deselectAll();

				rectTracker.m_rect.NormalizeRect();
				pwnd->scr2log(&rectTracker.m_rect);
				boost::rtl::rng::for_each(getChain()->elements_list(), 
					SelectIfIntersect(rectTracker.m_rect, getSelectionManager()));
			}
			pwnd->Invalidate();
		}
	}
}

/**
	���������� �������������� ��������, ���� ��� ������.
	���������� �������������� ��������� � ������ ������

 */
void TEBaseWorkMode::OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	BaseElementGuard baseElementGuard(this);

	if (getCanDragContent())
	{
		Base::OnLButtonUp(pwnd, nFlags, point);
		return;
	}
	if (getBaseElement()) 
	{
		bool const shiftPressed = te_h::IsKeyPressed(VK_SHIFT);

		if (!shiftPressed)
		{
			ReleaseCapture();
			if (getMouseMoved())/// ����� ���������
			{
				/// �������������� ����������
				drawDraggedTitles(pwnd);
				/// ����������� ��������� ��������
				moveSelection(pwnd, point);
				SetProjChange();
			}
			else
			{
				/// ���� �������, ���� �����, �� �������� ���
				CPoint p(point);
				pwnd->scr2log(&p);
				qdTriggerElementPtr ptrEle = pwnd->find_te(p);	
				getSelectionManager().deselectAll();
				if (ptrEle)
					getSelectionManager().select(ptrEle);
			}
		}
		/// ���� ������ ������ ����, �� ���������� ���� � ������ ���������������
		/// ������� ��������
		SelectionManager& sm = getSelectionManager();
		if (sm.getCount() == 1&&sm.getLast()->object())
			ptree_api::ShowObjectInTree(sm.getLast()->object());

		pwnd->InvalidateRect(NULL);
	}
	else /// �������� � ������
	{
		bool const shiftPressed = te_h::IsKeyPressed(VK_SHIFT);
		if (!shiftPressed)
		{
			getSelectionManager().deselectAll();
			pwnd->Invalidate();
		}
		if (getActiveLink()) 
		{
			pwnd->scr2log(&point);
			if (getMouseMoved()) // ������ ����� �� ������
			{
				TEUndoManager::Action redo;
				if (m_eLATest == LAT_BEGIN) 
				{
					redo = MoveLinkEnd(getActiveLink(), 
						getActiveLink()->get_owner_offset(), 
						MoveLinkEnd::MLE_OWNER_OFFSET);
				}
				else if (m_eLATest == LAT_END) 
				{
					redo = MoveLinkEnd(getActiveLink(), 
						getActiveLink()->get_child_offset(), 
						MoveLinkEnd::MLE_CHILD_OFFSET);
				}
				/// ������ � undo ������
				if (m_undo) 
					getUndoManager(getChain()).addAction(m_undo, redo);
				m_undo = NULL;
			}
			if (m_eLATest != LAT_NONE){
				setLATest(LAT_NONE);
				pwnd->InvalidateRect(NULL);
			}
		}
	}
}

/**
	������� ������� � ���������� ��� ��������
 */
void TEBaseWorkMode::OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint p)
{
	pwnd->scr2log(&p);
	qdTriggerElementPtr const ptrEle = pwnd->find_te(p);	
	if (ptrEle&&ptrEle->object()) 
	{
		if(ptree_api::ShowCondDlg(ptrEle->object()))
			pwnd->Invalidate();
	}
}

/**
	����� � ���� ��������. ���� �������� � ����, �� ������������ ���.
	���� �������� �� ��������, �� �������� ���� - �������� ����� ��������� ����. 
	� ����� ������ ���� ��� ����� ������
 */
void TEBaseWorkMode::OnRButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	CPoint p(point);
	pwnd->scr2log(&p);

	setBaseElement(NULL);

	qdTriggerElementPtr ptrEle = NULL;
	qdTriggerLink* ptrLink = pwnd->find_link(p, &ptrEle);
	setMouseMoved(false);
	if (ptrLink) 
	{
		activateLink(ptrLink, ptrEle);
		setBasePoint(p);
		setObjPos(p);
		pwnd->Invalidate();
	}
	else
	{
		if (getActiveLink())
		{
			activateLink(NULL, NULL);
			pwnd->Invalidate();
		}

		/// ����� �������
		ptrEle = pwnd->find_te(p);
		setBaseElement(ptrEle);
		if (ptrEle) 
		{
			/// �������� ��� 
			if (!getSelectionManager().isSelected(ptrEle))
			{	
				getSelectionManager().deselectAll();
				getSelectionManager().select(ptrEle);
				pwnd->Invalidate();
			}
			else
				getSelectionManager().reselect(ptrEle);

			setBasePoint(p);
			//setCurrentPoint(p);
			setObjPos(p);
			pwnd->SetCapture();
//			m_ptrDrawer->drawDragedLink(pwnd, pwnd->getDC(), 
//										getObjPos(), getBasePoint(),-1);
			setDrawedLinkType(-1);
		}
	}
	Base::OnRButtonDown(pwnd, nFlags, point);
}

/**
	�������� ���������� � OnRButtonDown ���������. 
	��� �� ������ ��� ����� �������� ������
 */
void TEBaseWorkMode::OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	BaseElementGuard baseElementGuard(this);

	CPoint p(point);
	pwnd->scr2log(&p);
	if (!getMouseMoved()) /// ���� �� �������, �.�. ������ ��������
	{
		if (!getBaseElement())
		{
			getSelectionManager().deselectAll();
			pwnd->Invalidate();
		}
		else
		{
			ReleaseCapture();
			trackPopupMenu(IDR_TE_TRIGGER_ELE, pwnd);
			return;
		}

		if (getActiveLink())
		{
			trackPopupMenu(IDR_TE_LINK, pwnd);
			Base::OnRButtonUp(pwnd, nFlags, point);
			return;
		}
		/// ��������� ���� �� ���� �� ������
		else if (TestForBetweenPlots(pwnd, p))
		{
			//���������� ����� �� ��� �����, ������ ��� 
			//������ ����� ������ ������ ���� ��������� ��������� ���.
			setBasePoint(p);
			trackPopupMenu(IDR_BETWEEN_PLOTS_MENU, pwnd);
			Base::OnRButtonUp(pwnd, nFlags, point);
			return;
		}
	}

	if (getBaseElement()) 
	{	//�.�. ��������� ������ ��� ���������
		ReleaseCapture();
		/// ���������� � ��������� ��� �������
		m_ptrDrawer->drawDragedLink(pwnd, pwnd->getDC(), 
			getObjPos(), getBasePoint(),getDrawedLinkType());

		if (getMouseMoved()) {
			/// ����� �������, ����� ��������� ��� ���
			/// � ��������� � ����� �� �� ��������� ��������(������� � ���������) ������

			qdTriggerChain* const ptrChain = getChain();
			qdTriggerElementPtr const& ptrEle = pwnd->find_te(p);	

			qdTriggerElementPtr const& baseElement = getBaseElement();
			if (ptrEle&&CanPlugLink(ptrEle, ptrChain)) {

				/// �������� ��� undo-������
				AddLink redo(ptrChain, 
						baseElement, 
						ptrEle, 
						//������ � ��������� �������� �����
						getLinkOffset(p, getObjPos(), ptrEle->bounding_rect()),
						//������ � �������� ���������� ������
						getLinkOffset(getObjPos(), p, baseElement->bounding_rect()),
						0);
				DeleteLink undo(ptrChain, baseElement, ptrEle);
				/// ��������� ���� ��������
				if (redo())
				{
					getUndoManager(getChain()).addAction(undo, redo);
					activateLink(baseElement->find_child_link(ptrEle), baseElement);
				}

				CRect rcUpdate;
				rcUpdate.UnionRect(&ptrEle->bounding_rect(), 
					&baseElement->bounding_rect());

				pwnd->log2scr(rcUpdate);
				SetProjChange();
				pwnd->InvalidateRect(&rcUpdate);
				/// ���� ����� �� ���������� ���� ��� �����
				if (m_bShowMenuOnLinkCreate&&getActiveLink())
					trackPopupMenu(IDR_TE_LINK, pwnd);
			}
		}
	}
	Base::OnRButtonUp(pwnd, nFlags, point);
}

/**
	��� ��� ���������� ��� �������������� ���� ����������������� �����
 */
void TEBaseWorkMode::OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		/// ������������ 
		Base::OnMouseMove(pwnd, nFlags, point);
		return;
	}
	/// ������ �� ������, ������ ������ ����� ������
	if (!(nFlags&MK_RBUTTON||nFlags&MK_LBUTTON))
		trackToolTip(pwnd, point);

	if (m_eLATest != LAT_NONE)
	{
		/// ������ ����� � ������
		if (nFlags&MK_LBUTTON) {
			// �������� ����� �� ����� ����� ������ ����� ������ ����
			pwnd->scr2log(&point);

			if (!getMouseMoved())
				setMouseMoved(getBasePoint()!=point);

			// ���������� ��� �� ���
			m_ptrDrawer->drawAnchoredLink(pwnd, pwnd->getDC(), 
				getLinkOwner(), getActiveLink());
			/// �����������
			moveLinkEnd(point);
			/// ���������� �� �����
			m_ptrDrawer->drawAnchoredLink(pwnd, pwnd->getDC(), 
				getLinkOwner(), getActiveLink());
		}
	} 
	else  if (!getSelectionManager().empty()) 
	{
		/// ���� ���� ������ ��������� ��������� 
		pwnd->scr2log(&point);

		if (nFlags&MK_RBUTTON) {
			/// ������ ����� ���� �� ������ �������� � �������
			if (!getMouseMoved())
				setMouseMoved(getBasePoint()!=point);
			qdTriggerElementPtr const& ptrEle = pwnd->find_te(point);

			int iType = getActiveLinkType();

			if(ptrEle&&!CanPlugLink(ptrEle, getChain()))
				iType = -1;/// ����������� �������� ���

			m_ptrDrawer->drawDragedLink(pwnd, pwnd->getDC(), 
				getObjPos(), getBasePoint(),getDrawedLinkType());

			if (DWORD dwPlace = pwnd->PtInScrollArea(point))
			{
				/// ���� ����������� �� ����, �� ����� ������ ����, ��� ���
				/// ��� ���������� ��� �� ����
				CONST INT SCROLL_DELTA_FACTOR = 5;
				const int SCROLL_DELTA_BASE = 12;
				/// ���� ������ SHIFT, �� ������ �������
				const int SCROLL_DELTA = 
					te_h::IsKeyPressed(VK_SHIFT)?
								SCROLL_DELTA_FACTOR*SCROLL_DELTA_BASE
								:SCROLL_DELTA_BASE;
				if (dwPlace&TEWnd::SAP_LEFT)
					pwnd->ScrollView(SCROLL_DELTA, 0);
				if (dwPlace&TEWnd::SAP_RIGHT)
					pwnd->ScrollView(-SCROLL_DELTA, 0);
				if (dwPlace&TEWnd::SAP_TOP)
					pwnd->ScrollView(0, SCROLL_DELTA);
				if (dwPlace&TEWnd::SAP_BOTTOM)
					pwnd->ScrollView(0, -SCROLL_DELTA);
			}


			setBasePoint(point);
			setDrawedLinkType(iType);
			m_ptrDrawer->drawDragedLink(pwnd, pwnd->getDC(), 
				getObjPos(), getBasePoint(),getDrawedLinkType());
		}
		else if (nFlags&MK_LBUTTON)
		{
			/// ����� ���������� ������� �� ������
			if (!getMouseMoved())
				setMouseMoved(getBasePoint()!=point);

			/// ����� ������� ��������� ������ �����, ����� ���� ���������� �� 
			/// ������ ������
			CPoint p(point);
			pwnd->to_cell_lt(&p);
			if (p != getObjPos())// ������ �� ������ �� ������ ������
			{
				if (moveDraggedTitles(pwnd, p))
				{
					/// CPoint(INT_MIN, INT_MIN) - ������ ��� ������ ������������
					if (getObjPos() != CPoint(INT_MIN, INT_MIN))
						drawDraggedTitles(pwnd);
					setObjPos(p);
					drawDraggedTitles(pwnd);
				}
			}
			setBasePoint(point);
		}
	}
}

/**
	����������� ����� ����� � �������� �������
	����� ������������� ������ �����, ����� ����� ��������� ������ ��������, ��������
	�������� ����.
	� ���� ������ ����� ��������� ���� ����� ���������� ����
 */
void TEBaseWorkMode::moveLinkEnd(CPoint const& point)
{
	CPoint end;
	if (m_eLATest == LAT_END) {
		if(PtInRect(&getActiveLink()->element()->bounding_rect(), point))
		{
			setBasePoint(point);
			end = te_h::get_link_begin(getLinkOwner(), getActiveLink());
			//��������� �����������
			MoveLinkEnd::run(getActiveLink(),
				getLinkOffset(point, end,
				getActiveLink()->element()->bounding_rect()),
				MoveLinkEnd::MLE_CHILD_OFFSET);

			SetProjChange();
		}
	}
	else if (m_eLATest = LAT_BEGIN) {
		if (PtInRect(&getLinkOwner()->bounding_rect(), point)) {

			setBasePoint(point);
			end = te_h::get_link_end(getActiveLink());
			//��������� �����������
			MoveLinkEnd::run(getActiveLink(),
					getLinkOffset(point, end,
					getLinkOwner()->bounding_rect()),
					MoveLinkEnd::MLE_OWNER_OFFSET);
			SetProjChange();
		}
	}
}

/**
	��������� ��-���� ���������� ��������� � ������������ �� ���������
 */
void TEBaseWorkMode::drawDraggedTitles(TEWnd* source) const
{
	SelectionManager::const_iterator i = getSelectionManager().begin(),
		e = getSelectionManager().end();
	qdTriggerElementPtr const lastSelected = getSelectionManager().getLast();
	CRect const rc(lastSelected->bounding_rect());
	CPoint startMove(rc.TopLeft());
	for(; i != e; ++i)
	{
		qdTriggerElementPtr const& trigger = *i;
		CPoint drawPoint = te_h::get_lt(trigger->bounding_rect());
		drawPoint += getObjPos() - startMove;
		std::string const& title = trigger->title();
		m_ptrDrawer->drawDragedTitle(source, title, drawPoint);
	}
}

/**
	����������� ��-���� ���������� � ��������� ����� �� �� ��������
 */
bool TEBaseWorkMode::moveDraggedTitles(TEWnd* pwnd, CPoint const& point) const
{
	qdTriggerElementPtr const lastSelected = getSelectionManager().getLast();
	CRect const rc(lastSelected->bounding_rect());
	CPoint startMove(rc.TopLeft());

	CSize offset(point - startMove);
	SelectionManager::const_iterator i = getSelectionManager().begin(),
		e = getSelectionManager().end();
	for(; i != e; ++i)
	{
		CRect const rc = (*i)->bounding_rect();
		qdTriggerElementPtr const& trigger = pwnd->find_te(rc.TopLeft() + offset);
		/// ���� � ����� ���� ������� ������ ������� � �� �� ������, 
		///  ������ ���� ������� ������
		if (trigger
		&&trigger != lastSelected
		&&!getSelectionManager().isSelected(trigger))
			return false;
	}
	return true;
}

/**
	�������� ��������� ��������, ���������� ������ �� ���� � undo-������
 */
void TEBaseWorkMode::moveSelection(TEWnd* pwnd, CPoint const& point)
{
	TEUndoManager::Bunch undoBunch, redoBunch;

	CPoint const& endMove = getObjPos();
	qdTriggerElementPtr const lastSelected = getSelectionManager().getLast();
	CRect const rc(lastSelected->bounding_rect());
	/// ����� ��������� ���������, �� ������� ���� �������� ��������,
	/// � �������� ����� ������� �������� ����� ������ ���� ��������, �� ������� ������
	/// ��� ��������� ��������� �� ����� �� ����������
	CPoint startMove(rc.TopLeft());
	SelectionManager::const_iterator i = getSelectionManager().begin(),
		e = getSelectionManager().end();
	for(; i != e; ++i)
	{
		qdTriggerElementPtr const& trigger = *i;
		MoveTEElementPos redo(this, pwnd, trigger, getChain(), endMove - startMove);
		MoveTEElementPos undo(this, pwnd, trigger, getChain(), startMove - endMove);
		if (redo()) 
		{
			SetProjChange();
			undoBunch.addAction(undo);
			redoBunch.addAction(redo);
		}
	}
	if (!redoBunch.empty())
	{
		undoBunch.reverse();
		getUndoManager(getChain()).addAction(undoBunch, redoBunch);
	}
}

/// ������� �������, �� ��� ������ ���� ������ ������
void TEBaseWorkMode::OnKeyDown(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
	{
		ASSERT(0);
		OnTeTgrEleDelete(pwnd);
	}
	Base::OnKeyDown(pwnd, nChar, nRepCnt, nFlags);
}

/// �������� ���������� �������� ������
void TEBaseWorkMode::OnKeyUp(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Base::OnKeyUp(pwnd, nChar, nRepCnt, nFlags);
}

/// ���� ������� ��������� �������� init_debug_check 
bool TEBaseWorkMode::OnStart(){
	if (getChain())
		getChain()->init_debug_check();
	return true;
}
/// ���������� ������������ �������
bool TEBaseWorkMode::OnEnd()
{
	setHilitedLinksElement(NULL);
	return true;
}

/**
	� ����� �� ��, ��� ������� ������ � �������, ��������� ��� ����� ������� �������
 */
BOOL TEBaseWorkMode::OnDropData(TEWnd* pwnd, VOID* pData, const CPoint& point)
{
	//first �������� ������ ��� ����������, second owner() ����� ������
	std::pair<qdNamedObject*, qdNamedObject*> res = getDraggedData(pData);

	CPoint p(point);
	pwnd->scr2log(&p);

	pwnd->to_cell_lt(&p);
	
	qdTriggerChain* pchain = getChain();
	qdTriggerElementPtr const ptrEle = pchain->add_element(res.first);
	if (ptrEle)
	{
		CPoint const& basePoint = p;
		ptrEle->update_title();
		ptrEle->set_bounding_rect(pwnd->make_te_rect(basePoint, ptrEle->title()));

		/// ��������� ������ �����
		pwnd->RemakeCellIdx(ptrEle, basePoint);

		CRect rcUpdate = ptrEle->bounding_rect();

		//�������������� ������������ �������
		CRect rcChain(pchain->bounding_rect());
		rcChain.UnionRect(&rcChain, rcUpdate);

		/// ���� �������� ������ ������� � �������, �� ���������� 
		/// ������� ��� ��������� ��������
		CRect r = pchain->root_element()->bounding_rect();
		if (r.IsRectEmpty()) 
		{
			r = pwnd->GetRootNodeRect(pchain->root_element(), ptrEle->bounding_rect());
			pchain->root_element()->set_bounding_rect(r);
			pwnd->RemakeCellIdx(pchain->root_element(), CPoint(r.left, r.top));
			rcChain.UnionRect(&rcChain, r);
		}

		pchain->set_bounding_rect(rcChain);

		AddElement redo(pwnd, p, pchain, ptrEle);
		DeleteElement undo(pwnd, pchain, ptrEle);
		getUndoManager(pchain).addAction(undo, redo);
		/// ���������� ������ � ���, ��� ������ �������� � �������
		ptree_api::OnAddObjToTrigger(res.first);
		pwnd->InvalidateRect(NULL);
		SetProjChange();
	}
	return TRUE;
}

/// ��������� ����� �� ������ ������ � �������
/// \return false - ���� ������ ������
bool TEBaseWorkMode::CanDrop(TEWnd* pwnd, VOID* pData, const CPoint& point)
{
	std::pair<qdNamedObject*, qdNamedObject*> res = 
		getDraggedData(pData);

	qdNamedObject* const owner = res.second;

	/// ��������� ������ qdConditionalObject
	if (!dynamic_cast<qdConditionalObject const*>(res.first))
		return false;

	//owner - �������� ��������� �� ������, �.�. ������, 
	//��� ����� ��������
	//��������� ������ ������(pobj)
	if (	res.first->named_object_type() == QD_NAMED_OBJECT_OBJ_STATE
		&&	(res.first->ref_owner() != owner/// ���� ��� ����� ����������� ���������
		||	ptree_helpers::is_obj_mouse(owner)))/// ��� ��������� ����
		return false;

	// ����� �� �������� ���� �������
	qdTriggerChain* const pchain = getChain();
	if (!pchain->can_add_element(res.first))
		return false;

	CPoint _p(point);
	pwnd->scr2log(&_p);
	pwnd->to_cell_lt(&_p);
	// ���� ������ ������-��, �� ���� �� �����
	if (pwnd->find_te(_p))
		return false;

	return true;
}

/// �������� �� ������ ��� �� ��� �� ���� ��������
std::pair<qdNamedObject*, qdNamedObject*> TEBaseWorkMode::
							getDraggedData(VOID* pData) const
{
	return std::make_pair(
			*reinterpret_cast<qdNamedObject**>(pData), 
			*(reinterpret_cast<qdNamedObject**>(pData) + 1));
}

/**
	���������� ������� - ������� ����
 */
void TEBaseWorkMode::OnTeLinkDelete(TEWnd* pwnd)
{
	ASSERT(m_ptrLinkOwner);
	ASSERT(m_ptrActiveLink);

	qdTriggerElementPtr const plo(m_ptrLinkOwner);
	qdTriggerLink* plink = m_ptrActiveLink;
	/// ���������� ����������
	activateLink(NULL, NULL);
	TEUndoManager::Action aUndo;
	TEUndoManager::Action aRedo;
	/// �������
	if (removeLink(getChain(), plo, plink, aUndo, aRedo))
		getUndoManager(getChain()).addAction(aUndo, aRedo);

	SetProjChange();

	pwnd->Invalidate();
}

/**
	�������� ������� �� ������� ���������
 */
void TEBaseWorkMode::deleteTrigger(TEWnd* pwnd, qdTriggerChain* chain, 
								   qdTriggerElementPtr const& element, 
								   TEUndoManager::Bunch& undoBunch,
								   TEUndoManager::Bunch& redoBunch
								   )
{
	TEUndoManager::Bunch uBunch, rBunch;
	/// ������� �����
	removeLinks(chain, element, uBunch, rBunch);
	DeleteElement redo(pwnd, chain, element);
	CRect const activeTriggerRect(element->bounding_rect());
	AddElement undo(pwnd, activeTriggerRect.TopLeft(), chain, element);
	/// ������� �������
	if (redo())
	{
		// ���������� �������� � ����� ��� undo �������
		rBunch.addAction(redo);
		uBunch.addAction(undo);
		uBunch.reverse();

		redoBunch.addAction(rBunch);
		undoBunch.addAction(uBunch);
	}
}

/**
	������� ��������� ��������
 */
void TEBaseWorkMode::deleteSelection(TEWnd* pwnd, TEUndoManager::Bunch& undoBunch,
									 TEUndoManager::Bunch& redoBunch)
{
	if (getSelectionManager().getCount() == 0)
		return;

	/// �������� ���� ������ � ��������� ���������, �� �������������� ����
	qdTriggerChain *chain = getChain();
	if (getLinkOwner ()&& getActiveLink())
	{
		if (getSelectionManager().isSelected(getLinkOwner())
			||getSelectionManager().isSelected(getActiveLink()->element()))
			activateLink(NULL, NULL);
	}

	std::vector<qdTriggerElementPtr> sel(getSelectionManager().begin(), 
										getSelectionManager().end());
	getSelectionManager().deselectAll();

	/// ������� ��� ��������� �������� �� ����������� ���������
	std::vector<qdTriggerElementPtr>::const_iterator i = sel.begin(), e = sel.end();
	for(; i != e; ++i){
		if (*i != chain->root_element())
			deleteTrigger(pwnd, chain, *i, undoBunch, redoBunch);
	}
	/// ���� �������� ��� ������, �� ��������������� ��� ��������
	if (sel.back() == chain->root_element())
		getSelectionManager().select(chain->root_element());
}

/**
	������� ��������� ��������.
	������ �������� ��������� � undo-������
 */
void TEBaseWorkMode::deleteSelection(TEWnd* pwnd)
{
	TEUndoManager::Bunch undoBunch, redoBunch;
	deleteSelection(pwnd, undoBunch, redoBunch);

	if (!redoBunch.empty()&&!undoBunch.empty()){
		undoBunch.reverse();
		getUndoManager(getChain()).addAction(undoBunch, redoBunch);
		SetProjChange();
	}
}

/**
	���������� ������� "������� �������"
 */
void TEBaseWorkMode::OnTeTgrEleDelete(TEWnd* pwnd)
{
	CWaitCursor wait;
	deleteSelection(pwnd);
	pwnd->Invalidate();
}

/**
	���������� ������� "������� ������� ��������"
	���� ������� ��� ��� ��������, �� ���������� ���������.
 */
void TEBaseWorkMode::OnTeMakeActive(TEWnd* pwnd)
{
	CWaitCursor __wait__;
	ASSERT(!getSelectionManager().empty());

	qdTriggerChain* pchain = getChain();
	/// ��������� � ������ ��������� ���, �� �������� ��������
	qdTriggerElementPtr const element = getSelectionManager().getLast();
	getSelectionManager().deselectAll();
	getSelectionManager().select(element);
	if (element->is_active())
	{
		element->make_active(false);
		if (pchain->root_element() != element) 
			pchain->root_element()->make_active(true);
	}
	else
	{
		/// �������� ���������� � ������
		clearActive(pchain);
		element->make_active(true);
	}
	pchain->init_debug_check();

	SetProjChange();
	pwnd->InvalidateRect(NULL);
}

/**
	���������� ������� "������ �������� �������"
 */
void TEBaseWorkMode::OnClearStarts(TEWnd* pwnd)
{
	qdTriggerChain* pc = getChain();
	clearActive(pc);
	pc->root_element()->make_active(true);
	pc->init_debug_check();
	pwnd->Invalidate();
}

/**
	���������� ������� "������� ���"
 */
void TEBaseWorkMode::OnTeLinkType(UINT id, TEWnd* pwnd)
{
	if (!m_ptrLinkOwner&&!m_ptrActiveLink)
		return;
//	ASSERT(m_ptrLinkOwner);
//	ASSERT(m_ptrActiveLink);

	/// ���������� ����� ��� ����������
	int type = 0;
	switch(id) {
	case IDC_TE_LINK_TYPE0:
		type = 0;
		break;
	case IDC_TE_LINK_TYPE1:
		type = 1;
		break;
	case IDC_TE_LINK_TYPE2:
		type = 2;
		break;
	case IDC_TE_LINK_TYPE3:
		type = 3;
		break;
	case IDC_TE_LINK_TYPE4:
		type = 4;
		break;
	case IDC_TE_LINK_TYPE5:
		type = 5;
		break;
	case IDC_TE_LINK_TYPE6:
		type = 6;
		break;
	case IDC_TE_LINK_TYPE7:
		type = 7;
		break;
	case IDC_TE_LINK_TYPE8:
		type = 8;
		break;
	case IDC_TE_LINK_TYPE9:
		type = 9;
		break;
	case IDC_TE_LINK_TYPE10:
		type = 10;
		break;
	default:
		ASSERT(0);
	}

	//������� � �������, �� ������� ��������� �������� ����,
	//����, ����������� �� ��������� ��������� �����,
	//� ��� ������������� ��� �� ���
	qdTriggerElementPtr const ptrEle = getActiveLink()->element();
	qdTriggerLink* ptrLink = ptrEle->find_parent_link(m_ptrLinkOwner);

	ASSERT(ptrLink);

	ChangeLinkType redo(m_ptrActiveLink, ptrLink, type);
	ChangeLinkType undo(m_ptrActiveLink, ptrLink, ptrLink->type());
	if (redo())
	{
		getUndoManager(getChain()).addAction(undo, redo);
		SetProjChange();
	}
	pwnd->Invalidate();
}

/**
	������������� ����� ������� "�����������"
 */
void TEBaseWorkMode::OnAutoRestart(TEWnd* pwnd)
{
/*
	ASSERT(getActiveLink());
	getActiveLink()->toggle_auto_restart(!getActiveLink()->auto_restart());
*/
	ChangeAutoRestart redo(getActiveLink(), !getActiveLink()->auto_restart());
	ChangeAutoRestart undo(getActiveLink(), getActiveLink()->auto_restart());
	if (redo())
	{
		getUndoManager(getChain()).addAction(undo, redo);
	}
	pwnd->Invalidate();
}

/**
	��������� ������� ����� ���������� 
 */
void TEBaseWorkMode::OnInsertColumns(TEWnd* pwnd)
{
	InsertColumns redo(1, pwnd, getChain(), getBasePoint());
	if (redo()) 
	{
		DeleteColumns undo(1, pwnd, getChain(), getBasePoint());
		getUndoManager(getChain()).addAction(undo, redo);
		pwnd->Invalidate();
		SetProjChange();
	}
}

/**
	������� ������� ����� ���������� 
 */
void TEBaseWorkMode::OnDeleteColumn(TEWnd* pwnd)
{
	DeleteColumns redo(1, pwnd, getChain(), getBasePoint());
	if (redo())
	{
		InsertColumns undo(1, pwnd, getChain(), getBasePoint());
		getUndoManager(getChain()).addAction(undo, redo);
		pwnd->Invalidate();
		SetProjChange();
	}
}

/**
	��������� ������ ����� ���������� 
*/
void TEBaseWorkMode::OnInsertRows(TEWnd* pwnd)
{
	InsertRows redo(1, pwnd, getChain(), getBasePoint());
	if (redo())
	{
		DeleteRows undo(1, pwnd, getChain(), getBasePoint());
		getUndoManager(getChain()).addAction(undo, redo);
		pwnd->Invalidate();
		SetProjChange();
	}
}

/**
	������� ������ ����� ���������� 
*/
void TEBaseWorkMode::OnDeleteRow(TEWnd* pwnd)
{
	DeleteRows redo(1, pwnd, getChain(), getBasePoint());
	if (redo())
	{
		InsertRows undo(1, pwnd, getChain(), getBasePoint());
		getUndoManager(getChain()).addAction(undo, redo);
		pwnd->Invalidate();
		SetProjChange();
	}
}

/**
	��������� ������� "������������"
 */
void TEBaseWorkMode::OnTEHiliteLinks(TEWnd* pwnd)
{
	qdTriggerElementPtr const element = getSelectionManager().getLast();
	getSelectionManager().deselectAll();
	getSelectionManager().select(element);
	if (element == getHilitedLinksElement())
		setHilitedLinksElement(NULL);
	else
		setHilitedLinksElement(element);
	pwnd->Invalidate();
}

/**
	������� ����� ��� ���� ��������� ���������
 */
void TEBaseWorkMode::OnTELinkSelectedTriggers(TEWnd* pwnd)
{
	if (2 != getSelectionManager().getCount())
	{
		AfxMessageBox(IDS_TWO_TRIGGERS_NEEDED);
		return;
	}
	
	qdTriggerChain* const ptrChain = getChain();
	qdTriggerElementPtr const& ptrChi = getSelectionManager().getLast();	
	qdTriggerElementPtr const& ptrPar = *getSelectionManager().begin();
	/// ��������� �������, ����� ����� ��������� ������
	if (!ptrChain || !ptrPar || !ptrChi || ptrPar == ptrChi || 
		ptrChain->root_element() == ptrChi || ptrChi->is_parent(ptrPar)) 
		return;

	/// �������� ��� undo-������
	CPoint chi_center;
	chi_center.x = (ptrChi->bounding_rect().left + ptrChi->bounding_rect().right)/2;
	chi_center.y = (ptrChi->bounding_rect().top + ptrChi->bounding_rect().bottom)/2;
	CPoint par_center;
	par_center.x = (ptrPar->bounding_rect().left + ptrPar->bounding_rect().right)/2;
	par_center.y = (ptrPar->bounding_rect().top + ptrPar->bounding_rect().bottom)/2;

	AddLink redo(ptrChain, 
			ptrPar, 
			ptrChi, 
			//������ � ��������� �������� �����
			getLinkOffset(chi_center, par_center, ptrChi->bounding_rect()),
			//������ � �������� ���������� ������
			getLinkOffset(par_center, chi_center, ptrPar->bounding_rect()),
			0);
	DeleteLink undo(ptrChain, ptrPar, ptrChi);
	/// ��������� ���� ��������
	if (redo())
	{
		getUndoManager(getChain()).addAction(undo, redo);
		activateLink(ptrPar->find_child_link(ptrChi), ptrPar);
	}

	CRect rcUpdate;
	rcUpdate.UnionRect(&ptrChi->bounding_rect(), 
		&ptrPar->bounding_rect());

	pwnd->log2scr(rcUpdate);
	SetProjChange();
	pwnd->InvalidateRect(&rcUpdate);
	/// ���� ����� �� ���������� ���� ��� �����
	if (m_bShowMenuOnLinkCreate&&getActiveLink())
		trackPopupMenu(IDR_TE_LINK, pwnd);	
}

/**
	���������� ������� �� ������� ���� � ������
 */
BOOL TEBaseWorkMode::OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam)) {
	case IDC_TE_LINK_DELETE:
		OnTeLinkDelete(pwnd);
		break;
	case IDC_TE_TGR_ELE_DELETE:
		OnTeTgrEleDelete(pwnd);
		break;
	case IDC_TE_MAKE_ACTIVE:
		OnTeMakeActive(pwnd);
		break;
	case ID_TBTN_CLEAR_STARTS:
		OnClearStarts(pwnd);
		break;
	case IDC_TE_LINK_TYPE0:
	case IDC_TE_LINK_TYPE1:
	case IDC_TE_LINK_TYPE2:
	case IDC_TE_LINK_TYPE3:
	case IDC_TE_LINK_TYPE4:
	case IDC_TE_LINK_TYPE5:
	case IDC_TE_LINK_TYPE6:
	case IDC_TE_LINK_TYPE7:
	case IDC_TE_LINK_TYPE8:
	case IDC_TE_LINK_TYPE9:
	case IDC_TE_LINK_TYPE10:
		OnTeLinkType(LOWORD(wParam), pwnd);
		break;
	case IDC_AUTO_RESTART:
		OnAutoRestart(pwnd);
		break;
	case IDC_INSERT_COLUMNS:
		OnInsertColumns(pwnd);
		break;
	case IDC_INSERT_ROWS:
		OnInsertRows(pwnd);
		break;
	case IDC_DELETE_COLUMN:
		OnDeleteColumn(pwnd);
		break;
	case IDC_DELETE_ROW:
		OnDeleteRow(pwnd);
		break;
	case IDC_TE_HILITE_LINKS:
		OnTEHiliteLinks(pwnd);
		break;
	case IDC_TE_LINK_SELECTED_TRIGGERS:
		OnTELinkSelectedTriggers(pwnd);
		break;
	case ID_EDIT_UNDO:
		OnEditUndo(pwnd);
		break;
	case ID_EDIT_REDO:
		OnEditRedo(pwnd);
		break;
	default:
		return Base::OnCommand(pwnd, wParam, lParam);
	}
	return TRUE;
}

/**
	���������� �������� "Undo"
 */
void TEBaseWorkMode::OnEditUndo(TEWnd* pwnd)
{
	if (getUndoManager(getChain()).canUNDO())
	{
		getUndoManager(getChain()).undo();
		pwnd->Invalidate();
	}
}

/**
	���������� �������� "Redo"
 */
void TEBaseWorkMode::OnEditRedo(TEWnd* pwnd)
{
	if (getUndoManager(getChain()).canREDO())
	{
		getUndoManager(getChain()).redo();
		pwnd->Invalidate();
	}
}

void TEBaseWorkMode::OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint)
{
	if (!getChain())
		return;
	m_ptrDataKeeper->setTriggerChain(getChain());
	m_ptrDrawer->draw(pdc, pwnd, m_ptrDataKeeper.get(), rcPaint);
	if (getActiveLink())
		m_ptrDrawer->drawLinkAnchors(pwnd, *pdc, getLinkOwner(), getActiveLink());
}

/// ��������� ������ �� ������ bounding rect'a �� ����� ������������� �����
CSize TEBaseWorkMode::getLinkOffset(const CPoint& ptFrom, const CPoint& ptTo, 
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
	return CPoint(ptFrom.x + kx*dir.cx, ptFrom.y + kx*dir.cy) - te_h::get_center(bound);
}

/// \brief ��������� ������ �� ������ bounding rect'a �� ����� ������������� �����
/// ��� ����� ������ �����
VOID TEBaseWorkMode::calcLinkOffsets(qdTriggerElementPtr parent, qdTriggerElementPtr child,
							CPoint const& ptFrom, CPoint const& ptTo)
{
	ASSERT(parent);
	ASSERT(child);
	qdTriggerLink *link = parent->find_child_link(child);
	ASSERT(link);

	link->set_owner_offset(getLinkOffset(ptFrom, ptTo, parent->bounding_rect()));
	link->set_child_offset(getLinkOffset(ptTo, ptFrom, child->bounding_rect()));
}

/**
	���������� � ����� ����� ����� ������
 */
TEBaseWorkMode::eLinkAnchorTest TEBaseWorkMode::find_link_anchor(
						qdTriggerElementPtr powner, qdTriggerLink* plink, const CPoint& p)
{
	CPoint tag(te_h::get_center(powner->bounding_rect()));

	tag.Offset(plink->get_owner_offset());
	CRect r(tag,tag);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);
	if (r.PtInRect(p)) 
		return LAT_BEGIN;

	tag = te_h::get_center(plink->element()->bounding_rect());
	tag.Offset(plink->get_child_offset());

	r.SetRect(tag.x, tag.y, tag.x, tag.y);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);
	if (r.PtInRect(p)) 
		return LAT_END;
	return LAT_NONE;
}

/**
	�������� ���� � � ����������� �� ��� id. ������ ������ �������������� ��� ���
 */
void TEBaseWorkMode::ModifyMenu(CMenu* pmenu, UINT menuResId, TEWnd* pwnd)
{
	if (menuResId == IDR_TE_TRIGGER_ELE) 
	{
		/// ������������ ���� ��� �������� ������� ���������
		if (qdTriggerElementPtr const element = getSelectionManager().getLast()) {
			if(!element->object())
			{
				menu_h::disableMenuItem(pmenu, IDC_TE_SHOW_CONDITION);
				menu_h::disableMenuItem(pmenu, IDC_TE_PROPETIES);
			}
			menu_h::checkMenuItem(pmenu, IDC_TE_MAKE_ACTIVE, element->is_active());
			menu_h::checkMenuItem(pmenu, IDC_TE_HILITE_LINKS, 
				element == getHilitedLinksElement());
		}
		/// ���������� ����� ����, ������ ���� ������� ��� ��������
		if (2 == getSelectionManager().getCount())
			menu_h::enableMenuItem(pmenu, IDC_TE_LINK_SELECTED_TRIGGERS);
		else
			menu_h::disableMenuItem(pmenu, IDC_TE_LINK_SELECTED_TRIGGERS);
	}
	else if (menuResId == IDR_TE_LINK) 
	{
		/// ���� ��� �����
		pmenu->CheckMenuItem(IDC_AUTO_RESTART, getActiveLink()->auto_restart());

		MENUITEMINFO mii;
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_BITMAP|MIIM_DATA;
		mii.hbmpItem = HBMMENU_CALLBACK;
		const int count = pmenu->GetMenuItemCount();
		const int offset = 3;
		for(int i = offset; i < count; ++i){
			mii.dwItemData = i-offset;
			pmenu->SetMenuItemInfo(i,&mii, TRUE);
		}

	}
	else if (menuResId == IDR_BETWEEN_PLOTS_MENU)
	{
		/// ���� �� �����������, �.�. �� �� �����, �� �� ��������
		if (m_eBetweenPlotsLayout == BPL_VERT)
		{
			menu_h::disableMenuItem(pmenu, IDC_INSERT_ROWS);
			menu_h::disableMenuItem(pmenu, IDC_DELETE_ROW);
		}
		else if (m_eBetweenPlotsLayout == BPL_HORZ)
		{
			menu_h::disableMenuItem(pmenu, IDC_INSERT_COLUMNS);
			menu_h::disableMenuItem(pmenu, IDC_DELETE_COLUMN);
		}
	}

}

/**
	������������ ���������� ��� ���� ������ (IDR_TE_LINK)
 */
void TEBaseWorkMode::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
	ASSERT(lpdis->CtlType == ODT_MENU);
	CRect r(lpdis->rcItem);
	r.right = r.left + r.Height();
	::FillRect(lpdis->hDC, &r, m_ptrDrawer->getBrushByLinkType(lpdis->itemData));

	__super::OnDrawItem(nIDCtl, lpdis);
}

/**
	��������� ������
 */
void TEBaseWorkMode::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
	ASSERT(lpmis->CtlType==ODT_MENU);
	lpmis->itemWidth += lpmis->itemHeight;
	__super::OnMeasureItem(nIDCtl, lpmis);
}

/**
	����� �� �������� ����, ������ �� ���������� ���������� � �������� ptrTestEle
 */
bool TEBaseWorkMode::CanPlugLink(qdTriggerElementPtr ptrTestEle, 
								 qdTriggerChain const* pchain) const
{
	if (ptrTestEle == getSelectionManager().getLast()) 
		return false;
	if (pchain->root_element() == ptrTestEle) 
		return false;

	return !ptrTestEle->is_parent(getSelectionManager().getLast());
}

/**
	���������� ��� �� ���� ��������� �������� ������.
	���������� true, ���� ����-������ ������, �.�. ������
 */
bool TEBaseWorkMode::TestForBetweenPlots(TEWnd* pwnd, CPoint p)
{
	int const w = pwnd->GetNetCellWidth() + pwnd->GetNetAmongCellW();
	int const h = pwnd->GetNetCellHeight() + pwnd->GetNetAmongCellH();

	int idx_x = te_h::get_cell_idx(p.x, w);
	int idx_y = te_h::get_cell_idx(p.y, h);

	setBetweenPlotsLayout(BPL_NOWERE);

	if (idx_x*w + pwnd->GetNetCellWidth() < p.x)
		setBetweenPlotsLayout(BPL_VERT);

	if (idx_y*h + pwnd->GetNetCellHeight() < p.y)
	{
		if (m_eBetweenPlotsLayout == BPL_VERT)
			setBetweenPlotsLayout(BPL_VERT_HORZ);
		else
			setBetweenPlotsLayout(BPL_HORZ);
	}
	return m_eBetweenPlotsLayout != BPL_NOWERE;
}

/// ���������� ���� "��������" �� ���� ��������� � ��������� ��� ��������� ��������
void TEBaseWorkMode::clearActive(qdTriggerChain* pc)
{
	const qdTriggerElementList& l = pc->elements_list();
	qdTriggerElementList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i)
		(*i)->make_active(false);
	pc->root_element()->make_active(false);
}

/// ������������ ��������� ����
void TEBaseWorkMode::activateLink(qdTriggerLink* plink, qdTriggerElementPtr pLinkOwner)
{
	if (plink == getActiveLink()) 
		return;

	MakeLinkActive undo(this, getActiveLink() ? getActiveLink()->element() : NULL, 
						getLinkOwner());
	MakeLinkActive redo(this, plink ? plink->element(): NULL, pLinkOwner);
	if (redo()) 
		getUndoManager(getChain()).addAction(undo, redo);
}

/**
	������� ��� �����, ������ �� ��������.
	������� ������� �������� �����.
	����� ������������
 */
void TEBaseWorkMode::removeLinks(qdTriggerChain* pc, qdTriggerElementPtr pe, 
								 TEUndoManager::Bunch& undoBunch, 
								 TEUndoManager::Bunch& redoBunch)
{
	TEUndoManager::Action aUndo, aRedo;
	qdTriggerLinkList& tll = pe->children();
	qdTriggerLinkList::iterator i = tll.begin();
	for(; i != tll.end(); i = tll.begin())
	{
		qdTriggerLink* const pl = &*i;
		if (removeLink(pc, pe, pl, aUndo, aRedo))
		{
			undoBunch.addAction(aUndo);
			redoBunch.addAction(aRedo);
			aUndo = NULL;
			aRedo = NULL;
		}
	}

	qdTriggerLinkList& tll2 = pe->parents();
	for(i = tll2.begin(); i != tll2.end(); i = tll2.begin())
	{
		qdTriggerLink* const pl = &*i;
		qdTriggerElementPtr const pParent = pl->element();
		if (removeLink(pc, pParent, pParent->find_child_link(pe), aUndo, aRedo))
		{
			undoBunch.addAction(aUndo);
			redoBunch.addAction(aRedo);
			aUndo = NULL;
			aRedo = NULL;
		}
	}
}

/**
	������� ���������� ����
 */
bool TEBaseWorkMode::removeLink(qdTriggerChain* pc, qdTriggerElementPtr ptrLinkOwner, 
								qdTriggerLink* pLink, TEUndoManager::Action& aUndo, 
								TEUndoManager::Action& aRedo)
{
	DeleteLink redo(pc, ptrLinkOwner, pLink->element());
	AddLink undo(pc, 
				ptrLinkOwner, 
				pLink->element(),
				pLink->get_child_offset(), 
				pLink->get_owner_offset(),
				pLink->type());
	if (redo())
	{
		aUndo = undo;
		aRedo = redo;
		return true;
	}
	return false;
}

/**
	���������� �����������/������������� ������ undo/redo
 */
bool TEBaseWorkMode::OnCommandUpdateUI(TEWnd *pwnd, CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID) 
	{
	case ID_EDIT_UNDO:
		if (getChain()) 
			pCmdUI->Enable(getUndoManager(getChain()).canUNDO());
		return true;
	case ID_EDIT_REDO:
		if (getChain()) 
			pCmdUI->Enable(getUndoManager(getChain()).canREDO());
		return true;
	}
	return false;
}

/// � ����� �� ��������� �������� ����������, ���������� �������� m_bShowMenuOnLinkCreate
void TEBaseWorkMode::OnSettingsChange()
{
	m_bShowMenuOnLinkCreate = static_cast<bool>(
		GetMyApp()->GetIntSetting(SETTING_NAME(TE_SHOW_MENU_ON_LINK_CREATE)));
}

/// ������� undo �����
void TEBaseWorkMode::clearUndobuffer()
{
	m_undoBuffers.clear();
}
