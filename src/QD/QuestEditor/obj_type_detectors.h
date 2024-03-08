#pragma once
namespace ptree_helpers
{
/**
* \defgroup ObjectTypeDetectors ‘ункции-детекторы типов объектов.
*  ќпредел€ют тип дл€ объектов, унаследованных от qdNamedObject
*
* @{
*/
inline bool is_game_dispatcher(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_DISPATCHER);
}
inline bool is_obj_static(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_STATIC_OBJ);
}
inline bool is_obj_animated(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ);
}
inline bool is_obj_moving(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ);
}
inline bool is_obj_mouse(const qdNamedObject* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MOUSE_OBJ);
}
inline bool is_game_obj(const qdNamedObject* p){
	return is_obj_static(p)||is_obj_animated(p)||is_obj_moving(p)||is_obj_mouse(p);
}
inline bool is_obj_state(qdNamedObject const * p){
	return (p->named_object_type() == QD_NAMED_OBJECT_OBJ_STATE);
}
inline bool is_obj_zone(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_GRID_ZONE);
}
inline bool is_obj_zone_state(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_GRID_ZONE_STATE);
}
inline bool is_obj_scene(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_SCENE);
}
inline bool is_obj_inventory(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_INVENTORY);
}
inline bool is_obj_minigame(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MINIGAME);
}
inline bool is_obj_music_track(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_MUSIC_TRACK);
}
inline bool is_obj_sound(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_SOUND);
}
inline bool is_obj_game_end(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_GAME_END);
}
inline bool is_obj_counter(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_COUNTER);
}
inline bool is_obj_font_info(qdNamedObject const* p){
	return (p->named_object_type() == QD_NAMED_OBJECT_FONT_INFO);
}
//! да если это QD_NAMED_OBJECT_ANIMATED_OBJ или QD_NAMED_OBJECT_MOVING_OBJ 
inline bool is_animated(qdNamedObject const* p){
	return (is_obj_animated(p)||is_obj_moving(p));
}
inline bool IsGameObject(qdNamedObject* ptrObj){
	switch(ptrObj->named_object_type())
	{
	case QD_NAMED_OBJECT_STATIC_OBJ:
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
	case QD_NAMED_OBJECT_MOVING_OBJ:
		return true;
	}
	return false;
}

/*@}*/

}