#include "stdafx.h"
#include "activetobeginforalltriggers.h"
#include "qd_trigger_chain.h"

ActiveToBeginForAllTriggers::MakeRootActive::MakeRootActive(bool bActive)
:m_bActive(bActive)
{
}
/**
	Пробегается по всем элементам цепочки и убирает у них признак активности.
	Ставит признак на корневой элемент. Реинитит отладочную инфу
 */
void ActiveToBeginForAllTriggers::MakeRootActive::DO(bool bActive, qdTriggerChain *pc)
{
	const qdTriggerElementList& l = pc->elements_list();
	qdTriggerElementList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i)
		(*i)->make_active(false);
	pc->root_element()->make_active(bActive);
	pc->init_debug_check();
}
void ActiveToBeginForAllTriggers::MakeRootActive::operator()(qdTriggerChain *pc) const{
	DO(m_bActive, pc);
}

ActiveToBeginForAllTriggers::ActiveToBeginForAllTriggers(void)
{
}

ActiveToBeginForAllTriggers::~ActiveToBeginForAllTriggers(void)
{
}

/**
	Пробегается по всем цепочкам и применяет к ним MakeRootActive
 */
bool ActiveToBeginForAllTriggers::DO(qdGameDispatcher* ptrDisp)
{
	ASSERT(ptrDisp);
	qdTriggerChainList const& lst = ptrDisp->trigger_chain_list();
	std::for_each(lst.begin(), lst.end(), MakeRootActive(true));
	return true;
}