#include "stdafx.h"
#include "resource.h"
#include "interf_events.h"
#include "changeinterfacescreeneventrepres.h"
#include "hideshowelementeventrepres.h"
#include "activatepersonageeventrepres.h"
#include "prevnextelementstateeventrepres.h"
#include "playvideoeventrepres.h"
#include "buttonstateeventrepres.h"
#include "LoadSceneEventrepres.h"
#include "ShowInterfaceScreenAsModalEventRepres.h"

#include <boost/iterator/indirect_iterator.hpp>
#include <Additional/itr_traits_spec.hpp>

typedef boost::shared_ptr<IInterfaceEventRepres> Ptr;
BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION( Ptr )

InterfEventRep::InterfEventRep(void)
{
	events_.reserve(20);
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
			(qdInterfaceEvent::EVENT_EXIT, IDS_EVENT_EXIT)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
			(qdInterfaceEvent::EVENT_NEW_GAME, IDS_EVENT_NEW_GAME)));

	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
			(qdInterfaceEvent::EVENT_RESUME_GAME, IDS_EVENT_RESUME_GAME)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
			(qdInterfaceEvent::EVENT_SET_SAVE_MODE, IDS_EVENT_SET_SAVE_MODE)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
			(qdInterfaceEvent::EVENT_SET_LOAD_MODE, IDS_EVENT_SET_LOAD_MODE)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
			(qdInterfaceEvent::EVENT_CLEAR_MOUSE,IDS_EVENT_CLEAR_MOUSE)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_CHANGE_PERSONAGE, IDS_EVENT_CHANGE_PERSONAGE)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_MAIN_MENU,IDS_EVENT_MAIN_MENU)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_SCROLL_LEFT,IDS_EVENT_SCROLL_LEFT)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_SCROLL_RIGHT,IDS_EVENT_SCROLL_RIGHT)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_SCROLL_UP,IDS_EVENT_SCROLL_UP)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_SCROLL_DOWN,IDS_EVENT_SCROLL_DOWN)));

	events_.push_back(
		Ptr( new HideShowElementEventRepres
		(qdInterfaceEvent::EVENT_TMP_HIDE_ELEMENT, IDS_EVENT_TMP_HIDE_ELEMENT)));

	events_.push_back(
		Ptr( new HideShowElementEventRepres(qdInterfaceEvent::EVENT_HIDE_ELEMENT, 
		IDS_EVENT_HIDE_ELEMENT)));
	events_.push_back(
		Ptr( new HideShowElementEventRepres(qdInterfaceEvent::EVENT_SHOW_ELEMENT, 
		IDS_EVENT_SHOW_ELEMENT)));

	events_.push_back(Ptr( new ActivatePersonageEventRepres));

	events_.push_back(
		Ptr( new PrevNextElementStateEventRepres
			(qdInterfaceEvent::EVENT_PREV_ELEMENT_STATE,IDS_EVENT_PREV_ELEMENT_STATE)));
	events_.push_back(
		Ptr( new PrevNextElementStateEventRepres
			(qdInterfaceEvent::EVENT_NEXT_ELEMENT_STATE, IDS_EVENT_NEXT_ELEMENT_STATE)));

	events_.push_back(
		Ptr( new LoadSceneEventRepres(IDS_EVENT_NEXT_ELEMENT_STATE)));


	events_.push_back(Ptr( new PlayVideoEventRepres));
	events_.push_back(Ptr(new ChangeInterfaceScreenEventRepres));

	events_.push_back(Ptr( new ButtonStateEventRepres));
	
	// Модальные запросы
	events_.push_back(Ptr(new ShowInterfaceScreenAsModalEventRepres));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_MODAL_OK,IDS_EVENT_MODAL_OK)));
	events_.push_back(
		Ptr( new InterfaceEventWithoutDataRepres
		(qdInterfaceEvent::EVENT_MODAL_CANCEL,IDS_EVENT_MODAL_CANCEL)));
}

InterfEventRep::~InterfEventRep()
{
}


boost::shared_ptr<IInterfaceEventRepres> InterfEventRep::find
													(qdInterfaceEvent::event_t type)
{
	typedef boost::indirect_iterator<Events::iterator> Iter;

	Iter res = std::find(Iter(events_.begin()), 
					Iter(events_.end()), type);
	ASSERT(res != Iter(events_.end()));
	return *res.base();
}

boost::shared_ptr<IInterfaceEventRepres> InterfEventRep::find(
	qdInterfaceEvent::event_t type) const
{
	typedef boost::indirect_iterator<Events::const_iterator> Iter;
	Iter res = std::find(Iter(events_.begin()), 
		Iter(events_.end()), type);
	ASSERT(res != Iter(events_.end()));
	return *res.base();
}

size_t InterfEventRep::events_count() const
{
	return events_.size();
}

InterfEventRep::Events const& InterfEventRep::events() const{
	return events_;
}

IInterfaceEventRepres const* InterfEventRep::operator[](int i) const
{
	return events_[i].get();
}
IInterfaceEventRepres const* InterfEventRep::operator[]
				(const qdInterfaceEvent::event_t& type) const
{
	return find(type).get();
}
