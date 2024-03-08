#include "stdafx.h"
#include "qd_trigger_chain.h"
#include "addlink.h"
#include "TEBaseWorkMode.h"


AddLink::AddLink(
					qdTriggerChain* pchain,
					qdTriggerElementPtr pOwner, 
					qdTriggerElementPtr pChild,
					CSize const& szChildOffset,
					CSize const& szOwnerOffset,
					int iLinkType):
 m_pChain(pchain)
, m_pOwner(pOwner)
, m_pChild(pChild)
, m_szChildOffset(szChildOffset)
, m_szOwnerOffset(szOwnerOffset)
, m_iLinkType(iLinkType)
{
}

AddLink::~AddLink(void)
{
}

bool AddLink::operator()()
{
	return run(m_pChain, m_pOwner, m_pChild, 
				m_szChildOffset, m_szOwnerOffset, m_iLinkType);
}

/**
	Добавляет связь элементам
	Назначает отступы от центра
 */
bool AddLink::run(
				  qdTriggerChain* pchain,
				  qdTriggerElementPtr pOwner, 
				  qdTriggerElementPtr pChild,
				  CSize const& szChildOffset,
				  CSize const& szOwnerOffset,
				  int iLinkType)
{
	if (!pchain->add_link(pOwner, pChild, iLinkType))
		return false;
	qdTriggerLink *link = pOwner->find_child_link(pChild);
	link->set_owner_offset(szOwnerOffset);
	link->set_child_offset(szChildOffset);
	return true;
}