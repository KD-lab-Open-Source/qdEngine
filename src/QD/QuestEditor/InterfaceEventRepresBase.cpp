#include "stdafx.h"
#include "interfaceeventrepresbase.h"

IInterfaceEventRepres::~IInterfaceEventRepres(){}

InterfaceEventRepresBase::InterfaceEventRepresBase(qdInterfaceEvent::event_t id, 
												   UINT_PTR descr_id)
:  m_eEventId(id)
,  m_strDescription((LPTSTR)descr_id)
{
}

InterfaceEventRepresBase::~InterfaceEventRepresBase(void)
{
}


InterfaceEventWithoutDataRepres::InterfaceEventWithoutDataRepres
				(qdInterfaceEvent::event_t id, UINT_PTR descr_id)
:InterfaceEventRepresBase(id, descr_id)
{}

LPCTSTR	InterfaceEventWithoutDataRepres::getDefaultValue() const{
	ASSERT(0);
	return NULL;
}

void InterfaceEventWithoutDataRepres::loadData(CWnd* pwnd) const{
	ASSERT(0);
}

InterfaceEventWithDataRepres::InterfaceEventWithDataRepres
				(qdInterfaceEvent::event_t id, UINT_PTR descr_id)
:InterfaceEventRepresBase(id, descr_id)
{}
