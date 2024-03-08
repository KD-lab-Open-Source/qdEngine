#include "stdafx.h"
#include "te_consts.h"
#include "qd_trigger_chain.h"
#include "tebaseworkmodedrawer.h"
#include "TEWnd.h"
#include "te_helpers2.h"
#include "TEBaseWorkMode.h"

TEBaseWorkModeDrawer::TEBaseWorkModeDrawer(TEBaseWorkMode* ptrWorkMode)
:m_ptrWorkMode(ptrWorkMode)
, m_linkOutPen(PS_SOLID, 2, LINK_OUT_COLOR)
, m_linkInPen(PS_SOLID, 2, LINK_IN_COLOR)
, m_otherLinksPen(PS_SOLID, 2, OTHER_LINKS_COLOR)
, m_linkOutBrush(LINK_OUT_COLOR)
, m_linkInBrush(LINK_IN_COLOR)
, m_otherLinksBrush(OTHER_LINKS_COLOR)
{
}

TEBaseWorkModeDrawer::~TEBaseWorkModeDrawer(void)
{
}

BOOL TEBaseWorkModeDrawer::setPenCant(HPEN hpen){
	return m_penCant.Attach(hpen);
}

BOOL TEBaseWorkModeDrawer::setLinkOutPen(HPEN hpen){
	return m_linkOutPen.Attach(hpen);
}
BOOL TEBaseWorkModeDrawer::setLinkInPen(HPEN hpen){
	return m_linkInPen.Attach(hpen);
}
BOOL TEBaseWorkModeDrawer::setOtherLinksPen(HPEN hpen){
	return m_otherLinksPen.Attach(hpen);
}

BOOL TEBaseWorkModeDrawer::setLinkOutBrush(HPEN hbrush){
	return m_linkOutBrush.Attach(hbrush);
}
BOOL TEBaseWorkModeDrawer::setLinkInBrush(HPEN hbrush){
	return m_linkInBrush.Attach(hbrush);
}
BOOL TEBaseWorkModeDrawer::setOtherLinksBrush(HPEN hbrush){
	return m_otherLinksBrush.Attach(hbrush);
}

/// ќтрисовывает сетку, а потом цепочку
void TEBaseWorkModeDrawer::draw(CDC* pdc, TEWnd* pwnd, 
								IDrawData* pdd, CRect const& rcPaint) const
{
	//дл€ прорисовки сетки
	BaseTEDrawer::draw(pdc, pwnd, pdd, rcPaint);

	std::pair<qdTriggerChain const* const*, int> drawData = pdd->getDrawedChains();
	for(int i = 0; i < drawData.second; ++i)
		DrawChain(pwnd, pdc, rcPaint, drawData.first[i]);
}

/**
	ќтрисовывает €кор€, а потом линк в режиме R2_NOTXORPEN
 */ 
void TEBaseWorkModeDrawer::drawAnchoredLink(TEWnd* pwnd, HDC dc, 
											qdTriggerElementConstPtr const& link_owner, 
											qdTriggerLink const* link) const
{
	CPoint beg(te_h::get_link_begin(link_owner, link));
	CPoint end(te_h::get_link_end(link));
	drawLinkAnchors(pwnd, dc, link_owner, link);

	int imode = SetROP2(dc, R2_NOTXORPEN);
	DrawLink(pwnd, dc, beg, end,  link);
	SetROP2(dc, imode);
}

/**
	¬ыбирате карандаш в зависимости от типа линка и рисует линию из pt_from до pt_to
 */
VOID TEBaseWorkModeDrawer::drawDragedLink(TEWnd* pwnd, HDC dc, const CPoint& pt_from, 
							const CPoint& pt_to, int type) const
{
	int iMode = SetROP2(dc, R2_NOTXORPEN);
	HGDIOBJ old_p = NULL;

	if (type == -1)
		old_p = SelectObject(dc, m_penCant);
	else
		old_p = SelectObject(dc, getLinkTypePen(type));

	CPoint p(pt_from);
	pwnd->log2scr(&p);

	MoveToEx(dc, p.x, p.y, NULL);
	p = pt_to;
	pwnd->log2scr(&p);
	LineTo(dc, p.x, p.y);
	SelectObject(dc, old_p);

	SetROP2(dc, iMode);
}

/**
	ќтрисовывает текст в спец. режиме. ј потом то,что получилось на экран в R2_NOT
 */
void TEBaseWorkModeDrawer::drawDragedTitle(TEWnd* pwnd, 
								std::string const& strTitle, CPoint pos) const
{
	pwnd->log2scr(&pos);
	HDC dc = pwnd->getDC();

	VERIFY(BeginPath(dc));
	TextOut(dc, pos.x, pos.y, strTitle.c_str(), strTitle.size());
	VERIFY(EndPath(dc));

	int iMode = SetROP2(dc, R2_NOT);
	VERIFY(StrokePath(dc));
	SetROP2(dc, iMode);
}


/**
	»з link_owner берет начало линка. »з link получает конец. » рисует квадратики
 */
void TEBaseWorkModeDrawer::drawLinkAnchors(TEWnd* pwnd, HDC dc, 
											qdTriggerElementConstPtr const& link_owner, 
											qdTriggerLink const* link) const
{
	int iMode = SetROP2(dc, R2_NOTXORPEN);
	HGDIOBJ old = SelectObject(dc, CreateSolidBrush(ANCHOR_COLOR));

	CPoint p = te_h::get_link_begin(link_owner, link);

	pwnd->log2scr(&p);
	CRect r(p, p);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);

	Rectangle(dc, r.left, r.top, r.right, r.bottom);

	p = te_h::get_link_end(link);
	pwnd->log2scr(&p);
	r.SetRect(p.x, p.y, p.x, p.y);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);

	Rectangle(dc, r.left, r.top, r.right, r.bottom);

	DeleteObject(SelectObject(dc, old));
	SetROP2(dc, iMode);
}

/**
	≈сли выбран элемент дл€ подсветки, то все линки идущие к нему и от него 
	отрисовываютс€ кист€ми, соответствующими типу. ќстальные m_otherLinksBrush.
	≈сли не выбран элемент дл€ подсветки, то все ресуютс€ кист€ми, соответствующими типу
 */
HBRUSH	TEBaseWorkModeDrawer::SelectLinkBrColor(qdTriggerLink const* pl) const
{
	if (qdTriggerElementConstPtr const& pele = m_ptrWorkMode->getHilitedLinksElement())
	{
		if (getDrawedLinkOwner() == pele||pele == pl->element())
			return TEBaseWorkModeDrawerBase::SelectLinkBrColor(pl);
		else
			return m_otherLinksBrush;
	}
	else
		return TEBaseWorkModeDrawerBase::SelectLinkBrColor(pl);
}

/**
	≈сли выбран элемент дл€ подсветки, то все линки идущие к нему и от него 
	отрисовываютс€ карандашами, соответствующими типу. ќстальные m_otherLinksBrush.
	≈сли не выбран элемент дл€ подсветки, то все ресуютс€ карандашами, 
	соответствующими типу
 */
HPEN	TEBaseWorkModeDrawer::SelectLinkPenColor(qdTriggerLink const* pl) const
{
	if (qdTriggerElementConstPtr const pele = m_ptrWorkMode->getHilitedLinksElement())
	{
		if (getDrawedLinkOwner() == pele||pele == pl->element())
			return TEBaseWorkModeDrawerBase::SelectLinkPenColor(pl);
		else
			return m_otherLinksPen;
	}
	else
		return TEBaseWorkModeDrawerBase::SelectLinkPenColor(pl);
}
