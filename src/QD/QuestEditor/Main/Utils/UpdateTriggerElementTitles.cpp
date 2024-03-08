#include "stdafx.h"
#include ".\updatetriggerelementtitles.h"

#include "qd_trigger_chain.h"
#include "qd_game_object_animated.h"
#include "qd_grid_zone.h"

#include <boost/rtl.hpp>


//! Функтор для поиска элемента цепочки триггеров, содержащего объект
class is_object_in_te
{
	qdNamedObject const* pno_;
public:
	is_object_in_te(qdNamedObject const* pno):pno_(pno){}
	inline bool operator()(qdTriggerElementConstPtr const& pe)
	{
		return (pe->object() == pno_);
	}
};

UpdateTriggerElementTitles::UpdateTriggerElementTitles(void)
{
}

UpdateTriggerElementTitles::~UpdateTriggerElementTitles(void)
{
}

void UpdateTriggerElementTitles::update(qdTriggerChainList const& chainList, 
										qdNamedObject const* obj)
{
	boost::rtl::crange<qdTriggerChainList const> r(chainList);
	for(; r; ++r)
	{
		qdTriggerChain* pchain = *r;
		boost::rtl::crange<
			qdTriggerElementList const
		> res = boost::rtl::rng::find_if(pchain->elements_list(), is_object_in_te(obj));
		if (res)
			(*res)->update_object_reference();
	}
}

void UpdateTriggerElementTitles::updateForAnimated(qdTriggerChainList const& chainList, 
							  qdGameObjectAnimated const* obj)
{
	boost::rtl::crange<qdGameObjectStateVector const> v(obj->state_vector());
	for(; v; ++v)
		update(chainList, *v);
}

void UpdateTriggerElementTitles::updateForZone(qdTriggerChainList const& chainList, 
											   qdGridZone* zone)
{
	qdGridZoneState* state = 
		zone->get_state(qdGridZoneState::ZONE_STATE_ON_NAME);
	if (state->is_in_triggers())
	{
		update(chainList, state);
	}
	state = zone->get_state(qdGridZoneState::ZONE_STATE_OFF_NAME);
	if (state->is_in_triggers())
	{
		update(chainList, state);
	}
}
