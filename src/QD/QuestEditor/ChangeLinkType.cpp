// ChangeLinkType.cpp: implementation of the ChangeLinkType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ChangeLinkType.h"

#include "qd_trigger_element.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ChangeLinkType::ChangeLinkType(qdTriggerLink* ptrLinkPar2Child,
								qdTriggerLink* ptrLinkChild2Par,
								int type):
  ptrLinkPar2Child_(ptrLinkPar2Child)
, ptrLinkChild2Par_(ptrLinkChild2Par)
, type_(type)
{

}

ChangeLinkType::~ChangeLinkType()
{

}

bool ChangeLinkType::operator()()
{
	return run(ptrLinkPar2Child_, ptrLinkChild2Par_, type_);
}
bool ChangeLinkType::run(qdTriggerLink* ptrLinkPar2Child,
						 qdTriggerLink* ptrLinkChild2Par,
						 int type)
{
	ptrLinkChild2Par->set_type(type);
	ptrLinkPar2Child->set_type(type);
	return true;
}
