#include "stdafx.h"
#include "TEBaseWorkMode.h"
#include "makelinkactive.h"

MakeLinkActive::MakeLinkActive(TEBaseWorkMode* pmode, 
							   qdTriggerElementPtr const& pLinkChild, 
							   qdTriggerElementPtr const& pLinkOwner):
  m_pMode(pmode)
, m_pLinkChild(pLinkChild)
, m_pLinkOwner(pLinkOwner)
{
}

MakeLinkActive::~MakeLinkActive(void)
{
}

bool MakeLinkActive::operator()()
{
	return run(m_pMode, m_pLinkChild, m_pLinkOwner);
}

/**
	Находит связь и делает её активной в РР.
 */
bool MakeLinkActive::run(TEBaseWorkMode* pmode, 
								qdTriggerElementPtr const& pLinkChild,
								qdTriggerElementPtr const& pLinkOwner)
{
	if (pLinkChild && pLinkOwner)
	{
		qdTriggerLink *plink = pLinkOwner->find_child_link(pLinkChild);
		if (pmode->getActiveLink() == plink)
			return false;

		pmode->setActiveLink(plink);

	}
	else
		pmode->setActiveLink(NULL);
	pmode->setLinkOwner(pLinkOwner);

	return true;
}
