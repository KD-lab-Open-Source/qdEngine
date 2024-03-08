#include "stdafx.h"
#include ".\objecttraits.h"

#include "qd_named_object.h"
#include "qd_game_object.h"
#include "qd_game_dispatcher.h"
#include "qd_game_object_animated.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_object_base.h"

#include "StateTraits.h"

#include <boost/rtl.hpp>

bool ObjectTraits::isGlobal(qdNamedObjectBase const& object)
{ 
	if (const qdNamedObject* pno = dynamic_cast<const qdNamedObject*>(&object))
		return (!pno->owner()
				||pno->owner()->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);
	return false;
}

qdGameObject* ObjectTraits::isCopyOfGlobal(qdGameDispatcher* disp, 
									qdGameObject const* obj)
{
	return disp->get_global_object(obj->name());
}

bool ObjectTraits::hasWalkState(qdGameObjectAnimated const* object)
{
	return (boost::rtl::rng::find_if(object->state_vector(), StateTraits::isWalkState));
}

bool ObjectTraits::is_game_dispatcher(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);
}
bool ObjectTraits::is_obj_static(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_STATIC_OBJ);
}
bool ObjectTraits::is_obj_animated(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ);
}
bool ObjectTraits::is_obj_moving(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ);
}
bool ObjectTraits::is_obj_mouse(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ);
}
bool ObjectTraits::is_game_obj(const qdNamedObject* p){
	return is_obj_static(p)||is_obj_animated(p)||is_obj_moving(p)||is_obj_mouse(p);
}
bool ObjectTraits::is_obj_state(qdNamedObject const * p){
	return (p->named_object_type() == QD_NAMED_OBJECT_OBJ_STATE);
}
bool ObjectTraits::is_obj_zone(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_GRID_ZONE);
}
bool ObjectTraits::is_obj_zone_state(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_GRID_ZONE_STATE);
}
bool ObjectTraits::is_obj_scene(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_SCENE);
}
bool ObjectTraits::is_obj_inventory(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_INVENTORY);
}
bool ObjectTraits::is_obj_minigame(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MINIGAME);
}
bool ObjectTraits::is_obj_music_track(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MUSIC_TRACK);
}
bool ObjectTraits::is_obj_sound(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_SOUND);
}
bool ObjectTraits::is_obj_game_end(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_GAME_END);
}
bool ObjectTraits::is_obj_counter(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_COUNTER);
}
bool ObjectTraits::is_obj_font_info(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_FONT_INFO);
}
//! да если это QD_NAMED_OBJECT_ANIMATED_OBJ или QD_NAMED_OBJECT_MOVING_OBJ 
bool ObjectTraits::is_animated(qdNamedObject const* p){
	return (is_obj_animated(p)||is_obj_moving(p));
}
bool ObjectTraits::is_trigger(qdNamedObject const* p)
{
	return (p->named_object_type() == QD_NAMED_OBJECT_TRIGGER_CHAIN);
}

bool ObjectTraits::hasNoStates(qdGameObject* pno)
{
	ASSERT(is_animated(pno));

	return (static_cast<qdGameObjectAnimated*>(pno)->state_vector().empty());
}

bool ObjectTraits::is_obj_interface_disp(qdInterfaceObjectBase const* p)
{
	return (typeid(*p) == typeid(qdInterfaceDispatcher));
}
