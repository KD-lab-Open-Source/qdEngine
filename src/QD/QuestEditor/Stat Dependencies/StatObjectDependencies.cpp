#include "stdafx.h"
#include "xutil.h"
#include "resource.h"
#include ".\statobjectdependencies.h"
#include "../obj_type_detectors.h"

#include "qd_game_object_animated.h"
#include "qd_game_object_state.h"
#include "qd_grid_zone.h"
#include "qd_minigame.h"
#include "qd_counter.h"

#include <typeinfo>
#include <boost/rtl.hpp>
#include <boost/bind.hpp>

#include "../IsObjectInCondition.h"
#include "../IsObjectInCounter.h"
#include "../IsObjectCARelativeObject.h"
#include "../IsObjectAttacher.h"


StatObjectDependencies::StatObjectDependencies(void)
{
}

StatObjectDependencies::~StatObjectDependencies(void)
{
}

void StatObjectDependencies::run(qdGameDispatcher* gameDisp,
								 qdNamedObject const* begin, 
								 NamedObjectListIndex* index)
{
	qdGameObjectAnimated::toggle_fast_state_merge(true);
	gameDisp->split_global_objects();

	assert(begin);
	switch(begin->named_object_type()) {
	case QD_NAMED_OBJECT_SCENE:
		stat(gameDisp, static_cast<qdGameScene const*>(begin), index);
		break;
	case QD_NAMED_OBJECT_STATIC_OBJ:
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
	case QD_NAMED_OBJECT_MOVING_OBJ:
		stat(gameDisp, static_cast<qdGameObject const*>(begin), index);
		break;
	case QD_NAMED_OBJECT_GRID_ZONE:
		stat(gameDisp, static_cast<qdGridZone const*>(begin), index);
		break;
	case QD_NAMED_OBJECT_MINIGAME:
		stat(gameDisp, static_cast<qdMiniGame const*>(begin), index);
		break;
	case QD_NAMED_OBJECT_OBJ_STATE:
		stat(gameDisp, static_cast<qdGameObjectState const*>(begin), index);
		break;
	case QD_NAMED_OBJECT_COUNTER:
		stat(gameDisp, static_cast<qdCounter const*>(begin), index);
		break;
	}

	gameDisp->merge_global_objects();
	qdGameObjectAnimated::toggle_fast_state_merge(false);
}

void StatObjectDependencies::stat(qdGameDispatcher const* gameDisp, 
								  qdGameScene const* scene, 
								  NamedObjectListIndex* index)
{
	IsObjectInCondition::run(gameDisp, scene, (*index)[scene]);

	typedef boost::rtl::filtered_range<
		qdGameObjectList const, 
		bool (*)(qdNamedObject const*)> FilteredObjectList;
	FilteredObjectList objs(scene->object_list(), ptree_helpers::is_animated);
	for(; objs; ++objs)
		stat(gameDisp, *objs, index);
	boost::rtl::crange<qdGridZoneList const> zones(scene->grid_zone_list());
	for(; zones; ++zones)
		stat(gameDisp, *zones, index);
}

void StatObjectDependencies::stat(qdGameDispatcher const* gameDisp, 
								  qdGameObject const* object, 
								  NamedObjectListIndex* index)
{
	IsObjectCARelativeObject::run(gameDisp, object, (*index)[object]);
	if (ptree_helpers::is_animated(object))
	{
		IsObjectInCondition::run(gameDisp, object, (*index)[object]);
		qdGameObjectAnimated const* const aobj = 
			static_cast<qdGameObjectAnimated const*>(object);
		boost::rtl::crange<qdGameObjectStateVector const> states(aobj->state_vector());
		for(; states; ++states)
			stat(gameDisp, *states, index);
		if (ptree_helpers::is_obj_moving(object))
			IsObjectAttacher::run(*gameDisp, object, (*index)[object]);
	}
}

void StatObjectDependencies::stat(qdGameDispatcher const* gameDisp, 
								  qdGameObjectState const* state, 
								  NamedObjectListIndex* index)
{
	NamedObjectList& list = (*index)[state];
	IsObjectInCondition::run(gameDisp, state, list);
	IsObjectInCounter::run(gameDisp, state, list);
}

void StatObjectDependencies::stat(qdGameDispatcher const* gameDisp, 
								  qdGridZone const* zone, 
								  NamedObjectListIndex* index)
{
	IsObjectInCondition::run(gameDisp, zone, (*index)[zone]);
}

void StatObjectDependencies::stat(qdGameDispatcher const* gameDisp, 
								  qdMiniGame const* miniGame, 
								  NamedObjectListIndex* index)
{
	IsObjectInCondition::run(gameDisp, miniGame, (*index)[miniGame]);
}

void StatObjectDependencies::stat(qdGameDispatcher const* gameDisp,
				 qdCounter const* counter, NamedObjectListIndex* index)
{
	IsObjectInCondition::run(gameDisp, counter, (*index)[counter]);
}
