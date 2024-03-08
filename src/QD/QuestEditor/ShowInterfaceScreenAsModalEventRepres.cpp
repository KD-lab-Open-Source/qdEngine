#include "stdafx.h"
#include "resource.h"

#include "ShowInterfaceScreenAsModalEventRepres.h"
#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"

#include <boost/bind.hpp>

ShowInterfaceScreenAsModalEventRepres::ShowInterfaceScreenAsModalEventRepres(void)
:InterfaceEventWithDataRepres(qdInterfaceEvent::EVENT_SHOW_INTERFACE_SCREEN_AS_MODAL, 
							  IDS_EVENT_SHOW_INTERFACE_SCREEN_AS_MODAL)
{
}

ShowInterfaceScreenAsModalEventRepres::~ShowInterfaceScreenAsModalEventRepres(void)
{
}

LPCTSTR ShowInterfaceScreenAsModalEventRepres::getDefaultValue() const{
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();
	return pdisp->screen_list().front()->name();
}

void ShowInterfaceScreenAsModalEventRepres::loadData(CWnd* pwnd) const{
	CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pwnd);
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceDispatcher::screen_list_t const&lst = pd->screen_list();
	std::for_each(lst.begin(), lst.end(), PutObjectNameToCombo(*pcombo));
	pcombo->SetCurSel(0);
}
