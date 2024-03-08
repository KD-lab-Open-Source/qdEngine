#include "stdafx.h"
#include "qd_trigger_chain.h"
#include "deleteelement.h"

#include "TEBaseWorkMode.h"
#include "TEWnd.h"
#include "ptree_api.h"

DeleteElement::DeleteElement(TEWnd* pwnd, 
							 qdTriggerChain* pchain,
							 qdTriggerElementPtr pele):
  m_pWnd(pwnd)
, m_pChain(pchain)
, m_pEle(pele)
{
}

DeleteElement::~DeleteElement(void)
{
}

bool DeleteElement::operator()()
{
	return run(m_pWnd, m_pChain, m_pEle);
}

/**
	Удаляет элемнт.
	Уведомляет дерево о том, что элемент был удален
	Пересчитывает bounding box цепочки
 */
bool DeleteElement::run(TEWnd* pwnd, 
				qdTriggerChain* pchain,
				qdTriggerElementPtr pele)
{
	qdNamedObject* const object = pele->object();
	if (!pchain->remove_element(pele))
		return false;

	if (object)
		ptree_api::ptOnRemoveObjFromTrigger(object);
	pwnd->RemakeChainRect(pchain);
	return true;
}
