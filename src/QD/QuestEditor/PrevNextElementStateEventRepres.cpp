#include "stdafx.h"
#include "resource.h"
#include "prevnextelementstateeventrepres.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_element.h"

PrevNextElementStateEventRepres::
	PrevNextElementStateEventRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id)
: InterfaceEventWithDataRepres(id, descr_id)
{
}

PrevNextElementStateEventRepres::~PrevNextElementStateEventRepres(void)
{
}

LPCTSTR PrevNextElementStateEventRepres::getDefaultValue() const
{
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceScreen* const pscr = pd->selected_screen();
	const qdInterfaceScreen::element_list_t& e_lst = pscr->element_list();
	qdInterfaceScreen::element_list_t::const_iterator ix = e_lst.begin(),
		ex = e_lst.end();
	for(; ix != ex; ++ix)
	{
		if ((*ix)->get_element_type() == qdInterfaceElement::EL_BUTTON)
			return (*ix)->name();
	}
	return NULL;
}

void PrevNextElementStateEventRepres::loadData(CWnd* pwnd) const
{
	CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pwnd);
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceScreen* pscr = pd->selected_screen();
	ASSERT(pscr);
	const qdInterfaceScreen::element_list_t& lst = pscr->element_list();
	qdInterfaceScreen::element_list_t::const_iterator ix = lst.begin(),
		ex = lst.end();
	for(; ix != ex; ++ix)
		if ((*ix)->get_element_type() == qdInterfaceElement::EL_BUTTON)
			pcombo->AddString((*ix)->name());
	pcombo->SetCurSel(0);
}
