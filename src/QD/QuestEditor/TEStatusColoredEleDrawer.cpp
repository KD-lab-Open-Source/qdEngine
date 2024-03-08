#include "stdafx.h"
#include "te_consts.h"
#include "qd_trigger_element.h"
#include "testatuscoloredeledrawer.h"

TEStatusColoredEleDrawer::TEStatusColoredEleDrawer(void)
{
}

TEStatusColoredEleDrawer::~TEStatusColoredEleDrawer(void)
{
}

HBRUSH	TEStatusColoredEleDrawer::SelectEleBackBrush(
						qdTriggerElementConstPtr const& pele) const
{
	switch(pele->status()) 
	{
		case qdTriggerElement::TRIGGER_EL_INACTIVE:
			{
				static CBrush s_brInactiveEle(INACTIVE_ELE_CLR);
				return s_brInactiveEle;
			}
			break;
		case qdTriggerElement::TRIGGER_EL_WAITING:
			{
				static CBrush s_brWaiting(WAITING_ELE_CLR);
				return s_brWaiting;
			}
			break;
		case qdTriggerElement::TRIGGER_EL_WORKING:
			{
				static CBrush s_br(WORKING_ELE_CLR);
				return s_br;
			}
			break;
		case qdTriggerElement::TRIGGER_EL_DONE:
			{
				static CBrush s_br(DONED_ELE_CLR);
				return s_br;
			}
			break;
	}
	ASSERT(0);
	return NULL;
}

HBRUSH	TEStatusColoredEleDrawer::SelectLinkBrColor(qdTriggerLink const* pl) const
{
	switch (pl->status())
	{
	case qdTriggerLink::LINK_ACTIVE:
		{
			static CBrush s_brAcitve(ACTIVE_LINK_CLR);
			return s_brAcitve;
		}
	case qdTriggerLink::LINK_INACTIVE:
		{
			static CBrush s_brInactive(INACTIVE_LINK_CLR);
			return s_brInactive;
		}
	case qdTriggerLink::LINK_DONE:
		{
			static CBrush s_brDoned(DONED_LINK_CLR);
			return s_brDoned;
		}
	}
	ASSERT(0);
	return NULL;
}

HPEN	TEStatusColoredEleDrawer::SelectLinkPenColor(qdTriggerLink const* pl) const
{
	switch (pl->status())
	{
	case qdTriggerLink::LINK_ACTIVE:
		{
			static CPen s_penAcitve(PS_SOLID, 2, ACTIVE_LINK_CLR);
			return s_penAcitve;
		}
	case qdTriggerLink::LINK_INACTIVE:
		{
			static CPen s_penInactive(PS_SOLID, 2, INACTIVE_LINK_CLR);
			return s_penInactive;
		}
	case qdTriggerLink::LINK_DONE:
		{
			static CPen s_penDoned(PS_SOLID, 2, DONED_LINK_CLR);
			return s_penDoned;
		}
	}
	ASSERT(0);
	return NULL;
}

void TEStatusColoredEleDrawer::draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, 
									CRect const& rcPaint) const
{
	__super::draw(pdc, pwnd, pdd, rcPaint);
	DrawChain(pwnd, pdc, rcPaint, *pdd->getDrawedChains().first);
}