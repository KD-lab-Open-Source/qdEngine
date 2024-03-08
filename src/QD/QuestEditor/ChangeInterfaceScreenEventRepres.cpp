#include "stdafx.h"
#include "resource.h"

#include "changeinterfacescreeneventrepres.h"
#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"

#include <boost/bind.hpp>

ChangeInterfaceScreenEventRepres::ChangeInterfaceScreenEventRepres(void)
:InterfaceEventWithDataRepres(qdInterfaceEvent::EVENT_CHANGE_INTERFACE_SCREEN, 
							  IDS_EVENT_CHANGE_INTERFACE_SCREEN)
{
}

ChangeInterfaceScreenEventRepres::~ChangeInterfaceScreenEventRepres(void)
{
}

LPCTSTR ChangeInterfaceScreenEventRepres::getDefaultValue() const{
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	return pdisp->screen_list().front()->name();
}

void ChangeInterfaceScreenEventRepres::loadData(CWnd* pwnd) const{
	CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pwnd);
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceDispatcher::screen_list_t const&lst = pd->screen_list();
	std::for_each(lst.begin(), lst.end(), PutObjectNameToCombo(*pcombo));
	pcombo->SetCurSel(0);
}
