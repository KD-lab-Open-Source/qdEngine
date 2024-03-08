#include "stdafx.h"
#include "qd_trigger_chain.h"
#include "deletelink.h"

DeleteLink::DeleteLink(qdTriggerChain* pchain, 
					 qdTriggerElementPtr pOwner, 
					 qdTriggerElementPtr pChild):
  m_pChain(pchain)
, m_pOwner(pOwner)
, m_pChild(pChild)
{
}

DeleteLink::~DeleteLink(void)
{}

bool DeleteLink::operator()(){
	return run(m_pChain, m_pOwner, m_pChild);
}

bool DeleteLink::run(qdTriggerChain* pchain, 
					 qdTriggerElementPtr pOwner, 
					 qdTriggerElementPtr pChild)
{
	return pchain->remove_link(pOwner, pChild);
}
