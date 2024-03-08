#include "stdafx.h"
#include "qd_trigger_chain.h"
#include "teallchainworkmodedrawer.h"
#include "TEWnd.h"
#include "IDrawData.h"
#include "te_helpers.h"

TEAllChainWorkModeDrawer::TEAllChainWorkModeDrawer(void)
{
}

TEAllChainWorkModeDrawer::~TEAllChainWorkModeDrawer(void)
{
}

void TEAllChainWorkModeDrawer::draw(CDC* pdc, TEWnd* pwnd, 
									IDrawData* pdd, CRect const& rcPaint) const
{
	drawAllChains(pdc, pwnd, pdd, rcPaint);
}

/**
	Перебирает все цепочки и отрисовывает их одна за другой
 */
void TEAllChainWorkModeDrawer::drawAllChains(CDC* pdc, TEWnd* pwnd, 
									IDrawData* pdd, CRect const& rcPaint) const
{
	std::pair<qdTriggerChain const* const*, int> data = 
		pdd->getDrawedChains();

	for(int i = 0; i < data.second; ++i)
		drawChain(pdc, pwnd, rcPaint, data.first[i]);
}

/**
	Назначает смещение триггера, чтобы при отрисовке все элементы сместились
 */
void TEAllChainWorkModeDrawer::drawChain(CDC* pdc, TEWnd* pwnd, const CRect& rcPaint, 
					  qdTriggerChain  const* ptrChain) const
{
	CRect intersect;
	if (!intersect.IntersectRect(&rcPaint, &ptrChain->gen_layout_rect())) 
		return;
	intersect.IntersectRect(&rcPaint, &ptrChain->gen_layout_rect());
	setTriggerOffset(ptrChain->gen_layout());
	setTriggerOffset(getTriggerOffset()-CSize(te_h::get_lt(ptrChain->bounding_rect())));

	CRect r(ptrChain->gen_layout_rect());

	pwnd->log2scr(r);
	HGDIOBJ br = pdc->SelectObject(GetStockObject(NULL_BRUSH));
	pdc->Rectangle(r);
	pdc->SelectObject(br);

	if (ptrChain->elements_list().empty()) {
		int old = pdc->SetBkMode(TRANSPARENT);
		pdc->TextOut(r.left, r.top, ptrChain->name(), _tcslen(ptrChain->name()));
		pdc->SetBkMode(old);
		return;
	}

	setCurrentDrawedChain(ptrChain);
	DrawElements(pwnd, pdc, intersect, ptrChain);
	DrawLinks(pwnd, pdc, ptrChain);
	setCurrentDrawedChain(NULL);
}
