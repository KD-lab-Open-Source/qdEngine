// ChangeAutoRestart.cpp: implementation of the ChangeAutoRestart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ChangeAutoRestart.h"

#include "qd_trigger_chain.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ChangeAutoRestart::ChangeAutoRestart(qdTriggerLink* ptrLink, bool autoRestart):
  ptrLink_(ptrLink)
, autoRestart_(autoRestart)
{

}

ChangeAutoRestart::~ChangeAutoRestart()
{

}

bool ChangeAutoRestart::operator()()
{
	return run(ptrLink_, autoRestart_);
}

bool ChangeAutoRestart::run(qdTriggerLink* ptrLink, bool autoRestart)
{
	ptrLink->toggle_auto_restart(autoRestart);

	return true;
}