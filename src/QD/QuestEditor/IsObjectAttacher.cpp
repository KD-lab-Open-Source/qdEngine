#include "stdafx.h"
#include "IsObjectAttacher.h"
#include "obj_type_detectors.h"

#include "qd_game_object_moving.h"

#include <boost/rtl.hpp>

bool IsObjectAttacher::run(qdGameDispatcher const& gameDisp,
				qdNamedObject const* object, NamedObjectList& lst)
{
	bool res = false;
	boost::rtl::crange<qdGameSceneList const> r(gameDisp.scene_list());

	for(; r; ++r)
	{
		if (test(*r, object, lst))
			res = true;
	}
	return res;
}

bool IsObjectAttacher::test(qdGameScene const*scene, 
							qdNamedObject const* object, 
							NamedObjectList& lst)
{
	bool res = false;
	boost::rtl::filtered_range<
		qdGameObjectList const,
		bool (*)(qdNamedObject const*) 
	> r(scene->object_list(), ptree_helpers::is_obj_moving);
	for(; r; ++r)
	{
		qdGameObjectMoving const* obj = static_cast<qdGameObjectMoving const*>(*r);
		if (obj->attacher() == object)
		{
			lst.push_back(obj);
			res = true;
		}
	}
	return res;
}
