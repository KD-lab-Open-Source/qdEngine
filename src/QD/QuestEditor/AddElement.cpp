#include "stdafx.h"
#include "qd_trigger_chain.h"
#include "qd_trigger_element.h"
#include "addelement.h"
#include "tewnd.h"
#include "ptree_api.h"

AddElement::AddElement(TEWnd* pwnd, 
					   CPoint const& basePoint,
					   qdTriggerChain* pchain, 
					   qdTriggerElementPtr pElement):
  m_pWnd(pwnd)
, m_pChain(pchain)
, m_pElement(pElement)
, m_basePoint(basePoint)
{
	assert(m_pElement->object());
	m_pElementObject = m_pElement->object();
}

AddElement::~AddElement(void)
{
}

bool AddElement::operator()()
{
	return run(m_pWnd, m_basePoint, m_pChain, m_pElement, m_pElementObject);
}

/**
	Добавляет элемент в цепочку. 
	Пересчитывает рект элемента.
	Пересчитывает рект цепочки. Если цепочка была пустая, то к её ректу добавляется
	еще и рект корневого элемента
	Уведомляет дерево проекта о добавлении элемента
 */
bool AddElement::run(TEWnd* pwnd, 
				CPoint const& basePoint,
				qdTriggerChain* pchain, 
				qdTriggerElementPtr ptrEle,
				qdNamedObject*	ptrElementObject)
{
	if (!pchain->add_element(ptrEle))
	{
		return false;
	}
	ptrEle->update_title();
	ptrEle->set_bounding_rect(pwnd->make_te_rect(basePoint, ptrEle->title()));

	pwnd->RemakeCellIdx(ptrEle, basePoint);

	CRect rcUpdate = ptrEle->bounding_rect();

	//прямоугольники охватывающий цепочку
	CRect rcChain(pchain->bounding_rect());
	rcChain.UnionRect(&rcChain, rcUpdate);

	CRect r = pchain->root_element()->bounding_rect();
	if (r.IsRectEmpty()) 
	{
		r = pwnd->GetRootNodeRect(pchain->root_element(), ptrEle->bounding_rect());
		pchain->root_element()->set_bounding_rect(r);
		pwnd->RemakeCellIdx(pchain->root_element(), CPoint(r.left, r.top));
		rcChain.UnionRect(&rcChain, r);
	}
	pchain->set_bounding_rect(rcChain);
	ptree_api::OnAddObjToTrigger(ptrEle->object());
	return true;
}
