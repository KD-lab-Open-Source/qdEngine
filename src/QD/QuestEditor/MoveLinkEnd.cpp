#include "stdafx.h"
#include "movelinkend.h"
#include "qd_trigger_element.h"

MoveLinkEnd::MoveLinkEnd(qdTriggerLink* plink, CSize const& offset, MLESelectEnd eval):
  m_pLink(plink)
, m_szOffset(offset)
, m_eMLESelectEnd(eval)
{
}

MoveLinkEnd::~MoveLinkEnd(void)
{
}

bool MoveLinkEnd::operator()()
{
	return run(m_pLink, m_szOffset, m_eMLESelectEnd);
}

bool MoveLinkEnd::run(qdTriggerLink* plink, 
					   CSize const& offset, 
					   MLESelectEnd eSelectEnd)
{
	if (eSelectEnd == MLE_CHILD_OFFSET) 
		plink->set_child_offset(offset);
	else
		plink->set_owner_offset(offset);
	return true;
}
