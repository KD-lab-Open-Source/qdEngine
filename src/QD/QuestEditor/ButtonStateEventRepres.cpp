#include "stdafx.h"
#include "resource.h"
#include "buttonstateeventrepres.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"
#include "qd_interface_button.h"

ButtonStateEventRepres::ButtonStateEventRepres(void)
:InterfaceEventWithDataRepres(qdInterfaceEvent::EVENT_BUTTON_STATE,
							  IDS_EVENT_BUTTON_STATE)
{
}

ButtonStateEventRepres::~ButtonStateEventRepres(void)
{
}

LPCTSTR ButtonStateEventRepres::getDefaultValue() const
{
	static std::string str;

	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceScreen* scr = pd->selected_screen();

	if(!scr) return 0;

	const qdInterfaceScreen::element_list_t& lst = scr->element_list();
	qdInterfaceScreen::element_list_t::const_iterator it = lst.begin();

	for(; it != lst.end(); ++it){
		if((*it)->get_element_type() == qdInterfaceElement::EL_BUTTON){
			qdInterfaceButton* btn = static_cast<qdInterfaceButton*>(*it);
			for(int i = 0; i < btn->num_states(); i++){
				const qdInterfaceElementState* st = btn->get_state(i);
				if(btn->name() && st->name()){
					str = btn->name();
					str += "::";
					str += st->name();
					return str.c_str();
				}
			}
		}
	}

	return 0;
}

void ButtonStateEventRepres::loadData(CWnd* pwnd) const
{
	CCJFlatComboBox* pcombo = static_cast<CCJFlatComboBox*>(pwnd);
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	qdInterfaceScreen* scr = pd->selected_screen();

	if(!scr) return;

	const qdInterfaceScreen::element_list_t& lst = scr->element_list();
	qdInterfaceScreen::element_list_t::const_iterator it = lst.begin();

	PutObjectNameWithOwnerToCombo op(*pcombo);

	for(; it != lst.end(); ++it){
		if((*it)->get_element_type() == qdInterfaceElement::EL_BUTTON){
			qdInterfaceButton* btn = static_cast<qdInterfaceButton*>(*it);
			for(int i = 0; i < btn->num_states(); i++){
				const qdInterfaceElementState* st = btn->get_state(i);
				op(btn, st);
			}
		}
	}

	pcombo->SetCurSel(0);
}
