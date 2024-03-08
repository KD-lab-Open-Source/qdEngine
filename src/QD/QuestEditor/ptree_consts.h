#pragma once
/************************************************************************/
/* Константы                                                            */
/************************************************************************/

/**
	@defgroup ImageIds Идентификаторы имиджей для узлов дерева
	@{
	Номера соответствуют тому в каком порядке были добавлены
	бмпшки.
	Отдельным узлам могут соответствовать несколько id. В этом случае
	имиджи отражают разные состояния объектов, соответствющих узлу
 */
const int II_GLOBAL_NODE						= 0;
const int II_LOCATIONS_NODE						= 1;
const int II_LOCATION_NODE						= 2;
const int II_SCENES_NODE						= 3;
const int II_SCENE_NODE							= 4;
const int II_NET_NODE							= 5;
const int II_STATIC_OBJS_NODE					= 6;
const int II_ANIMATED_OBJS_NODE					= 7;
const int II_MOVING_OBJS_NODE					= 8;
const int II_MOUSE_NODE							= 9;
const int II_TRIGGERS_NODE						= 10;
const int II_STATIC_OBJ_NODE					= 11;
const int II_ANIMATED_OBJ_NODE					= 12;
const int II_MOVING_OBJ_NODE					= 13;
const int II_STATE_NODE							= 14;
const int II_GLOBAL_STATE_NODE					= 17;
const int II_CA_STATE_NODE						= 18;
const int II_GLOBAL_OBJ_STATE_NODE				= 19;
const int II_INVENTORY_STATE_NODE				= 20;
const int II_TRIGGER_NODE						= 15;
const int II_STATES_CATALOG						= 16;
const int II_DIALOG_STATE_NODE					= 21;
const int II_MINIGAME_NODE						= 22;
const int II_MINIGAME_STATE_NODE				= 23;
const int II_MINIGAMES_NODE						= 24;
const int II_MUSIC_TRACKS_NODE					= 25;
const int II_MUSIC_TRACK_NODE					= 26;
const int II_GZONE_STATE_NODE					= 27;
const int II_INTERFACE_NODE						= 28;
const int II_INTERFACE_SCEREEN_NODE				= 29;
const int II_INTERFACE_ELEMENT_NODE				= 30;
const int II_CURSOR_FOR_DYN_OBJ					= 31;
const int II_CURSOR_FOR_ZONES					= 32;
const int II_CURSOR_FOR_DYN_OBJ_AND_ZONES		= 33;
const int II_CURSOR_NODE						= 34;
const int II_ZONE_NODE							= 35;
const int II_VIDEO_CATALOG						= 36;
const int II_VIDEO_NODE							= 37;
const int II_INVENTORY_OBJS_NODE				= 38;
const int II_INVENTORY_OBJ_NODE					= 39;
const int II_DISABLE_MOUSE_ANIMATED_OBJ_NODE	= 40;
const int II_NOT_MOV_STATIC_OBJ_NODE			= 41;
const int II_INTERFACE_ELEMENT_STATE			= 42;
const int II_CURSOR_FOR_ALL_NODE				= 43;
const int II_GZONE_NEED_CUR_NODE				= 44;
const int II_STATE4INVENTORY_NODE				= 45;
const int II_CURSOR_FOR_MAIN_MENU				= 46;
const int II_CURSOR_FOR_INGAME_INTERF			= 47;
const int II_GAME_END_NODE						= 48;
const int II_GAME_ENDS_NODE						= 49;
const int II_COUNTER_NODE						= 50;
const int II_COUNTERS_NODE						= 51;
const int II_DIALOGS_FRAME_NODE					= 52;
const int II_FONTS_CATALOG						= 53;
const int II_FONT_NODE							= 54;
const int II_CURSOR_FOR_OBJ_IN_INVENTORY		= 55;

const int TIMER_EVENT_PLAY_VIDEO	 = 1;

#ifdef USE_NEW_PROJ
const int II_ACTIVE_TRIGGER = 1;
const int II_NOT_ACTIVE_TRIGGER = 2;
const int II_DEFAULT_STATE = 3;
#else
const int II_ACTIVE_TRIGGER = 0;
const int II_NOT_ACTIVE_TRIGGER = 1;
const int II_DEFAULT_STATE = 2;
#endif

/**
@}
*/

//! Глубина по умлочанию для фона в сцене
const float DEF_BACKGROUND_DEPTH			= 800.f;
//! Глубина по умлочанию для статических объектов
const float START_DEPTH_FOR_STATIC_OBJ		= 759.f;
//! Глубина по умлочанию для динамических объектов
const float START_DEPTH_FOR_ANIM_OBJ		= 500.f;
//! Глубина по умлочанию для персонажей
const float START_DEPTH_FOR_MOVING_OBJ		= 300.f;
//! Индекс состояния, который означает, что никакое состояние не установлено
const int NO_STATE						= -1;

const DWORD QE_FIRST								= 0xfff;
//! Идентификатор каталога обычных состояний у персонажа
const DWORD QE_STATE_CATALOG						= (DWORD)QE_FIRST - 1;
//! Идентификатор каталога походочных состояний у персонажа
const DWORD QE_MOVEMENT_STATE_CATALOG				= (DWORD)QE_FIRST - 2;

//////////////////////////////////////////////////////////////////////////
