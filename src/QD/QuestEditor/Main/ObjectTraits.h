#pragma once

class qdGameDispatcher;
class qdGameObject;
class qdGameObjectAnimated;
class qdNamedObject;
class qdInterfaceObjectBase;

class ObjectTraits
{
public:
	static bool isGlobal(qdNamedObjectBase const& object);
	static qdGameObject* isCopyOfGlobal(qdGameDispatcher* disp, 
		qdGameObject const* obj);
	static bool hasWalkState(qdGameObjectAnimated const* object);

	static bool is_game_dispatcher(qdNamedObject const* p);
	static bool is_obj_static(qdNamedObject const* p);
	static bool is_obj_animated(qdNamedObject const* p);
	static bool is_obj_moving(qdNamedObject const* p);
	static bool is_obj_mouse(qdNamedObject const* p);
	static bool is_game_obj(qdNamedObject const* p);
	static bool is_obj_state(qdNamedObject const * p);
	static bool is_obj_zone(qdNamedObject const* p);
	static bool is_obj_zone_state(qdNamedObject const* p);
	static bool is_obj_scene(qdNamedObject const* p);
	static bool is_obj_inventory(qdNamedObject const* p);
	static bool is_obj_minigame(qdNamedObject const* p);
	static bool is_obj_music_track(qdNamedObject const* p);
	static bool is_obj_sound(qdNamedObject const* p);
	static bool is_obj_game_end(qdNamedObject const* p);
	static bool is_obj_counter(qdNamedObject const* p);
	static bool is_obj_font_info(qdNamedObject const* p);
	//! да если это QD_NAMED_OBJECT_ANIMATED_OBJ или QD_NAMED_OBJECT_MOVING_OBJ 
	static bool is_animated(qdNamedObject const* p);
	static bool is_trigger(qdNamedObject const* p);

	static bool hasNoStates(qdGameObject* pno);

	static bool is_obj_interface_disp(qdInterfaceObjectBase const* p);
};
