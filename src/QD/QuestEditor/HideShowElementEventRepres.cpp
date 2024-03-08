#include "stdafx.h"
#include "resource.h"
#include "hideshowelementeventrepres.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_element.h"

HideShowElementEventRepres::HideShowElementEventRepres(qdInterfaceEvent::event_t id, 
													   UINT_PTR descr_id)
:InterfaceEventWithDataRepres(id, descr_id)
{
}

HideShowElementEventRepres::~HideShowElementEventRepres(void)
{
}

LPCTSTR HideShowElementEventRepres::getDefaultValue() const{
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceScreen const* pscr = pd->selected_screen();
	if (!pscr)
		return NULL;
	return pscr->element_list().front()->name();
}
void HideShowElementEventRepres::loadData(CWnd* pwnd) const{
	CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pwnd);
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceScreen* const pscr = pd->selected_screen();
	qdInterfaceScreen::element_list_t const& ele_list = pscr->element_list();
	std::for_each(ele_list.begin(), ele_list.end(), PutObjectNameToCombo(*pcombo));
	pcombo->SetCurSel(0);
}
