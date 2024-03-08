#include "stdafx.h"
#include "teallchainworkmode.h"
#include "TEWnd.h"
#include "te_consts.h"
#include "te_helpers.h"
#include <algorithm>

#include "ITEDrawer.h"
#include "qd_game_dispatcher.h"
#include "qd_trigger_chain.h"
#include "qd_trigger_element.h"

void SetProjChange();

TEAllChainWorkMode::DrawDataKeeper::DrawDataKeeper(){
}
TEAllChainWorkMode::DrawDataKeeper::~DrawDataKeeper(){
}
void TEAllChainWorkMode::DrawDataKeeper::setDrawedChains(qdTriggerChainList const* lst)
{
	m_data.clear();
	std::copy(lst->begin(), lst->end(), std::back_inserter(m_data));
}

TEAllChainWorkMode::TEAllChainWorkMode(qdGameDispatcher* ptrDisp)
:
  m_hRectPen(reinterpret_cast<HPEN>(GetStockObject(BLACK_PEN)))
, m_ptrActiveChain(NULL)
, m_redPen(PS_SOLID, 1, TE_CANNT_COLOR)
, m_ptrGameDisp(ptrDisp)
, m_ptrDrawDataKeeper(new DrawDataKeeper)
{
}

TEAllChainWorkMode::~TEAllChainWorkMode(void)
{
}

void TEAllChainWorkMode::setGameDisp(qdGameDispatcher* ptrDisp){
	m_ptrGameDisp = ptrDisp;
	m_ptrDrawDataKeeper->setDrawedChains(&m_ptrGameDisp->trigger_chain_list());
}
bool TEAllChainWorkMode::setDrawer(ITEDrawer* ptrDrawer){
	if (!ptrDrawer)
		return false;
	m_ptrDrawer = ptrDrawer;
	return true;
}
void TEAllChainWorkMode::setLTPos(CPoint const& p){
	m_ptLTPos = p;
}
void TEAllChainWorkMode::setActiveChain(qdTriggerChain* pc){
	m_ptrActiveChain = pc;
}
void TEAllChainWorkMode::setRectPen(HPEN hPen){
	m_hRectPen = hPen;
}
void TEAllChainWorkMode::setWasMouseMoved(bool bval){
	m_bWasMouseMoved = bval;
}

/**
	≈сли можно скроллировать, будем скроллировать
	≈сли попали по цепочке, значит будем ее тащить.
 */
void TEAllChainWorkMode::OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		TEWorkModeBase::OnLButtonDown(pwnd, nFlags, point);
		return;
	}
	CPoint p(point);
	pwnd->scr2log(&p);
	setBasePoint(p);
	qdTriggerChain* ptrChain = pwnd->find_chain(p);
	if (ptrChain) {
		pwnd->SetCapture();
		setActiveChain(ptrChain);
		setLTPos(te_h::get_lt(getActiveChain()->gen_layout_rect()));
	}
}
/**
	если скроллировали, то прекращаем.
	≈сли перетскивали, то, если цепочка не пересекаетс€ с другой, передвигаем ее
 */
void TEAllChainWorkMode::OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		TEWorkModeBase::OnLButtonUp(pwnd, nFlags, point);
		return;
	}
	if (getActiveChain()) {
		ReleaseCapture();
		CRect r(getActiveChain()->gen_layout_rect());
		r.OffsetRect(getLTPos() - r.TopLeft());
		pwnd->log2scr(r);
		m_ptrDrawer->drawDraggedRect(pwnd->getDC(), &r, getRectPen());

		CRect rcCh(getActiveChain()->gen_layout_rect());
		rcCh.OffsetRect(getLTPos() - rcCh.TopLeft());

		qdTriggerChain* pc = pwnd->WillIntersect(rcCh, getActiveChain());
		if (!pc)
		{
			SetProjChange();

			pwnd->OffsetChain(getActiveChain(), getLTPos());
			pwnd->UpdateWndWorkArea();
		}

		pwnd->Invalidate();
		setActiveChain(NULL);
	}
}
/**
	—кроллируем или
	ѕередвигаем цепочку. —мотрим, если она пересекает какую-то другую, то
	рисуем еЄ красным
 */
void TEAllChainWorkMode::OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		TEWorkModeBase::OnMouseMove(pwnd, nFlags, point);
		return;
	}
	if (getActiveChain()&&nFlags&MK_LBUTTON) 
	{
		pwnd->scr2log(&point);

		CRect rcCh(m_ptrActiveChain->gen_layout_rect());
		rcCh.OffsetRect(getLTPos() - rcCh.TopLeft());

		pwnd->log2scr(rcCh);

		m_ptrDrawer->drawDraggedRect(pwnd->getDC(), &rcCh, getRectPen());

		setLTPos(getLTPos() + point - getBasePoint());
		
		rcCh = getActiveChain()->gen_layout_rect();
		rcCh.OffsetRect(getLTPos() - rcCh.TopLeft());

		setRectPen(reinterpret_cast<HPEN>(GetStockObject(BLACK_PEN)));
		if (qdTriggerChain* pc = pwnd->WillIntersect(rcCh, m_ptrActiveChain)) {
			setRectPen(m_redPen);
		}
		setBasePoint(point);
		pwnd->log2scr(rcCh);
		m_ptrDrawer->drawDraggedRect(pwnd->getDC(), rcCh, getRectPen());
	}
}

void TEAllChainWorkMode::OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint)
{
	m_ptrDrawer->draw(pdc, pwnd, m_ptrDrawDataKeeper.get(), rcPaint);
}
