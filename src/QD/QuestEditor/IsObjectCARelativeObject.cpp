#include "stdafx.h"
#include ".\isobjectcarelativeobject.h"

#include "obj_type_detectors.h"
#include <boost/rtl.hpp>
#include "qd_game_dispatcher.h"
#include "qd_game_scene.h"
#include "qd_game_object_animated.h"

IsObjectCARelativeObject::IsObjectCARelativeObject(void)
{
}

IsObjectCARelativeObject::~IsObjectCARelativeObject(void)
{
}

bool IsObjectCARelativeObject::run(qdGameDispatcher const* gameDisp, 
				qdGameObject const* object, NamedObjectList& lst)
{
	bool res = true;
	boost::rtl::crange<qdGameSceneList const> r(gameDisp->scene_list());
	for(; r; ++r)
		test(*r, object, lst);
	return res;
}

bool IsObjectCARelativeObject::stateHasRelativeCA(qdGameObjectState const* state)
{
	return (!state->coords_animation()->is_empty()
		&&state->coords_animation()->check_flag(QD_COORDS_ANM_RELATIVE_FLAG));
}

void IsObjectCARelativeObject::test(qdGameScene const*scene, 
				 qdGameObject const* object, 
				 NamedObjectList& lst)
{
	boost::rtl::filtered_range<
		qdGameObjectList const,
		bool (*)(qdNamedObject const*) 
	> r(scene->object_list(), ptree_helpers::is_animated);
	for(; r; ++r)
	{
		qdGameObjectAnimated const* obj = static_cast<qdGameObjectAnimated const*>(*r);
		boost::rtl::filtered_range<
			qdGameObjectStateVector const,
			bool (*)(qdGameObjectState const*)
			> sr(obj->state_vector(), stateHasRelativeCA);
		for(; sr;++sr)
		{
			qdGameObjectState const* state = *sr;
			if (state->coords_animation()->start_object() == object)
				lst.push_back(state);
		}
	}
}
