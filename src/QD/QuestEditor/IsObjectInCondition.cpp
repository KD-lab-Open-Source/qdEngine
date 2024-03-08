#include "stdafx.h"
#include "resource.h"
#include ".\isobjectincondition.h"
#include ".\isobjectinconditiontest.h"

#include "qd_game_object_animated.h"
#include "qd_minigame.h"
#include "qd_video.h"
#include "qd_game_end.h"
#include "qd_grid_zone.h"
#include "qd_music_track.h"

#include "obj_type_detectors.h"
#include <boost/iterator/filter_iterator.hpp>
#include <boost/rtl.hpp>

class ptr_compare_with
{
public:
	ptr_compare_with(void const* lhs):lhs_(lhs){}
	bool operator()(void const* rhs) const{
		return (lhs_ != rhs);
	}
private:
	void const* lhs_;
};
class IsObjectInCondition::TestInConditionalObj
{
public:
	TestInConditionalObj(qdNamedObject const* obj,
						NamedObjectList& lst):obj_(obj), lst_(lst){}
	static bool run(qdConditionalObject const* testObject, 
				qdNamedObject const* obj, NamedObjectList& lst){
		int const count = testObject->conditions_count();
		for( int i = 0; i < count; ++i)
			if (IsObjectInConditionTest::run(testObject->get_condition(i), obj))
			{
				lst.push_back(testObject);
				return true;
			}
		return false;
	}
	bool operator()(qdConditionalObject const* testObject) const{
		return run(testObject, obj_, lst_);
	}
private:
	qdNamedObject const* obj_;
	NamedObjectList& lst_;
};

class IsObjectInCondition::TestInGridZone
{
public:
	TestInGridZone(qdNamedObject const* obj, NamedObjectList& lst):obj_(obj), lst_(lst){}
	static bool run(qdGridZone const* zone, 
					qdNamedObject const* obj,
					NamedObjectList& lst)
	{
		qdGridZone* z = const_cast<qdGridZone*>(zone);
		CString strZoneName;
		strZoneName.LoadString(IDS_NODE_NAME_ZONE_ON);
		if (TestInConditionalObj::run(z->get_state(strZoneName), obj, lst))
			return true;
		strZoneName.LoadString(IDS_NODE_NAME_ZONE_OFF);
		if (TestInConditionalObj::run(z->get_state(strZoneName), obj, lst))
			return true;
		return false;
	}
	bool operator()(qdGridZone const* zone) const{
		return run(zone, obj_, lst_);
	}
private:
	qdNamedObject const* obj_;
	NamedObjectList& lst_;
};
class IsObjectInCondition::TestInAnimatedObject
{
public:
	TestInAnimatedObject(qdNamedObject const* obj, 
		NamedObjectList& lst):obj_(obj), lst_(lst){};
	static bool run(qdGameObject const* testObject, 
		qdNamedObject const* obj, NamedObjectList& lst)
	{
		if (!ptree_helpers::is_animated(testObject)) 
			return false;
		qdGameObjectAnimated const* animObj = 
			static_cast<qdGameObjectAnimated const*>(testObject);
		return IsObjectInCondition::testConditionalObjects(
			animObj->state_vector(), obj, lst);
	}
	bool operator()(qdGameObject const* testObject) const{
		return run(testObject, obj_, lst_);
	}
private:
	qdNamedObject const* obj_;
	NamedObjectList& lst_;
};

class IsObjectInCondition::TestInScene
{
public:
	TestInScene(qdNamedObject const* obj, NamedObjectList& lst):
	  obj_(obj), lst_(lst)
	  {}
	static bool run(qdGameScene const* scene, 
					qdNamedObject const* obj,
					NamedObjectList& lst)
	{
		using namespace boost::rtl;
		bool res = false;
		if (testConditionalObjects(scene->music_track_list(), obj, lst))
			res = true;
		{
			boost::rtl::crange<qdGridZoneList const> zones(scene->grid_zone_list());
			while (zones = rng::find_if(zones, TestInGridZone(obj, lst)))
			{
				res = true;
				++zones;
			}
		}
		{
			boost::rtl::crange<qdGameObjectList const> objs(scene->object_list());
			while (objs = rng::find_if(objs,
				IsObjectInCondition::TestInAnimatedObject(obj, lst))) {
				res = true;
				++objs;
			}
		}
		return res;
	}

	  bool operator()(qdGameScene const* scene) const{
		  return run(scene, obj_, lst_);
	  }
private:
	qdNamedObject const* obj_;
	NamedObjectList& lst_;
};


IsObjectInCondition::IsObjectInCondition()
{
}

IsObjectInCondition::~IsObjectInCondition()
{
}

bool IsObjectInCondition::run(qdGameDispatcher const* pdisp, qdNamedObject const* obj, 
							  NamedObjectList& lst)
{
	if (!canBeInCondition(obj))
		return false;
	bool res = false;
	qdGameScene const* objScene = getObjectScene(obj);
	if (objScene&&objScene != obj)
	{
		if (TestInScene::run(objScene, obj, lst))
			res = true;
	}
	
	if (testInProject(pdisp, objScene, obj, lst))
		res = true;
	return res;
}

qdGameScene const* IsObjectInCondition::getObjectScene(qdNamedObject const* obj)
{
	do {
		if (ptree_helpers::is_obj_scene(obj)) 
			return static_cast<qdGameScene const*>(obj);
	} while(obj = obj->owner());
	return NULL;
}

bool IsObjectInCondition::canBeInCondition(qdNamedObject const* obj)
{
	return ptree_helpers::is_game_obj(obj)
		|| ptree_helpers::is_obj_state(obj)
		|| ptree_helpers::is_obj_scene(obj)
		|| ptree_helpers::is_obj_zone(obj)
		|| ptree_helpers::is_obj_counter(obj);
}

template<class InputIterator, class Test>
bool IsObjectInCondition::test(InputIterator begin, InputIterator end, Test const& test)
{
	bool res = false;
	while ((begin = std::find_if(begin, end, test)) != end){
		res = true;
		++begin;
	}
	return res;
}

template<class Container>
bool IsObjectInCondition::testConditionalObjects(Container const& lst, 
									qdNamedObject const* obj, NamedObjectList& objLst)
{
	return test(lst.begin(), lst.end(), TestInConditionalObj(obj, objLst));
}

bool IsObjectInCondition::testInProject(qdGameDispatcher const* disp,
										qdGameScene const* gameSceneExclude,
										qdNamedObject const*obj, 
										NamedObjectList& namedObjectList)
{
	bool res = false;
	using namespace boost::rtl;
	{
		qdGameSceneList const& lst = disp->scene_list();
		typedef boost::filter_iterator<ptr_compare_with,
						qdGameSceneList::const_iterator> iterator;
		iterator i = boost::make_filter_iterator(ptr_compare_with(gameSceneExclude),
					lst.begin(), lst.end());
		iterator e = boost::make_filter_iterator(ptr_compare_with(gameSceneExclude),
					lst.end(), lst.end());
		if (test(i, e, TestInScene(obj, namedObjectList)))
			res = true;
		if (test(lst.begin(), lst.end(), TestInConditionalObj(obj, namedObjectList)))
			res = true;
	}
//	if (testConditionalObjects(disp->minigame_list(), obj, namedObjectList))
//		res = true;
	if (testConditionalObjects(disp->video_list(), obj, namedObjectList))
		res = true;
	if (testConditionalObjects(disp->game_end_list(), obj, namedObjectList))
		res = true;
	{
		qdGameObjectList const& lst = disp->global_object_list();
		boost::rtl::crange<qdGameObjectList const> objs(disp->global_object_list());

		while (objs = rng::find_if(objs, 
			IsObjectInCondition::TestInAnimatedObject(obj, namedObjectList)))
		{
			res = true;
			++objs;
		}
	}
	return res;
}

