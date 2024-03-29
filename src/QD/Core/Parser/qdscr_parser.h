
#ifndef __QDSCR_PARSER_H__
#define __QDSCR_PARSER_H__

#include "xml_fwd.h"

//! �������������� ����� �������.
enum qdscrTagID
{
	QDSCR_ROOT = 1,

	// Common
	QDSCR_ID,
	QDSCR_X,
	QDSCR_Y,
	QDSCR_SX,
	QDSCR_SY,
	QDSCR_POS2D,
	QDSCR_POS3D,
	QDSCR_SRC_POS,
	QDSCR_DEST_POS,
	QDSCR_FILE,
	QDSCR_START_TIME,
	QDSCR_PERIOD,
	QDSCR_LENGTH,
	QDSCR_SPEED,
	QDSCR_ANIMATION_SPEED,
	QDSCR_INTERPOLATION_TIME,
	QDSCR_SCALE,
	QDSCR_NAME,
	QDSCR_FLAG,
	QDSCR_RND,
	QDSCR_MASK_SIZE,
	QDSCR_MASK_ATTRIBUTES,
	QDSCR_MASK_HEIGHTS,
	QDSCR_BOUND,
	QDSCR_TYPE,
	QDSCR_SIZE,
	QDSCR_STATE,
	QDSCR_TEXT,
	QDSCR_SHORT_TEXT,
	QDSCR_VALUE,
	QDSCR_TIME,
	QDSCR_CD,
	QDSCR_ALIGN,
	QDSCR_VALIGN,
	QDSCR_ORIENTATION,
	QDSCR_COLOR,
	QDSCR_COMMENT,

	QDSCR_SCREEN_SIZE,
	QDSCR_TEXT_SET,

	// Text
	QDSCR_TEXT_COLOR,
	QDSCR_TEXT_HOVER_COLOR,
	QDSCR_TEXT_ALIGN,

	// Fonts
	QDSCR_FONT_INFO,

	// Contours
	QDSCR_CONTOUR_RECTANGLE,
	QDSCR_CONTOUR_CIRCLE,
	QDSCR_CONTOUR_POLYGON,

	// Named Object Reference
	QDSCR_NAMED_OBJECT,
	QDSCR_NAMED_OBJECT_TYPES,

	// Grid zone
	QDSCR_GRID_ZONE,
	QDSCR_GRID_ZONE_CELLS,
	QDSCR_GRID_ZONE_HEIGHT,
	QDSCR_GRID_ZONE_CONTOUR,
	QDSCR_GRID_ZONE_STATE,
	QDSCR_GRID_ZONE_SHADOW_COLOR,
	QDSCR_GRID_ZONE_SHADOW_ALPHA,

	// Trigger Element
	QDSCR_TRIGGER_ELEMENT,
	QDSCR_TRIGGER_ELEMENT_LINK,
	QDSCR_TRIGGER_ELEMENT_PARENT_LINKS,
	QDSCR_TRIGGER_ELEMENT_CHILD_LINKS,
	QDSCR_TRIGGER_ELEMENT_LINK_AUTO_RESTART,
	QDSCR_TRIGGER_ELEMENT_LINK_OFFSETS,
	QDSCR_TRIGGER_ELEMENT_TITLE,
	QDSCR_TRIGGER_ELEMENT_CELL_NUMBER,
	QDSCR_TRIGGER_PARENT_LINK_OWNER_OFFSET,
	QDSCR_TRIGGER_PARENT_LINK_CHILD_OFFSET,
	QDSCR_TRIGGER_CHILD_LINK_OWNER_OFFSET,
	QDSCR_TRIGGER_CHILD_LINK_CHILD_OFFSET,
	QDSCR_TRIGGER_START_ELEMENT,

	// Trigger Chain
	QDSCR_TRIGGER_CHAIN,
	QDSCR_TRIGGER_CHAIN_ROOT,
	QDSCR_TRIGGER_CHAIN_WORK_AREA,
	QDSCR_TRIGGER_CHAIN_LAYOUT,

	QDSCR_TRIGGER_BOUND,

	// Sound
	QDSCR_SOUND,
	QDSCR_SOUND_VOLUME,

	// Music
	QDSCR_MUSIC_TRACK,
	QDSCR_MUSIC_TRACK_CYCLED,

	// Video
	QDSCR_VIDEO,
	QDSCR_VIDEO_POSITION,
	QDSCR_VIDEO_BACKGROUND,

	// Minigame
	QDSCR_MINIGAME,
	QDSCR_MINIGAME_DLL_NAME,
	QDSCR_MINIGAME_GAME_NAME,
	QDSCR_MINIGAME_CONFIG_FILE,
	QDSCR_MINIGAME_STATE,
	QDSCR_MINIGAME_SCORE,
	QDSCR_MINIGAME_CONFIG_PARAMETER,

	// Animation
	QDSCR_ANIMATION_FRAME,
	QDSCR_ANIMATION_INFO,
	QDSCR_ANIMATION_FILE,
	QDSCR_ANIMATION,

	// AnimationSet
	QDSCR_ANIMATION_SET,
	QDSCR_ANIMATION_SET_START_ANGLE,
	QDSCR_ANIMATION_SET_TURN,

	// Coords Animation
	QDSCR_COORDS_ANIMATION_POINT,
	QDSCR_COORDS_ANIMATION,
	QDSCR_ANIMATION_PHASE,

	// Game Object
	QDSCR_PARALLAX_OFFSET,

	// Static Object
	QDSCR_STATIC_OBJECT,

	// Animated Object
	QDSCR_ANIMATED_OBJECT,
	QDSCR_OBJECT_STATE,
	QDSCR_OBJECT_MOVEMENT_STATES,
	QDSCR_OBJECT_MOVEMENT_STATE,

	QDSCR_OBJECT_DIRECTION,
	QDSCR_OBJECT_DEFAULT_POS,
	QDSCR_OBJECT_DEFAULT_DIRECTION,

	QDSCR_OBJECT_STATE_STATIC,
	QDSCR_OBJECT_STATE_WALK,
	QDSCR_OBJECT_STATE_MASK,
	QDSCR_OBJECT_STATE_MASK_POS,
	QDSCR_OBJECT_STATE_MASK_SIZE,
	QDSCR_OBJECT_STATE_MASK_DATA,
	QDSCR_OBJECT_STATE_MASK_PARENT,
	QDSCR_OBJECT_STATE_MASK_CONTOUR,
	QDSCR_OBJECT_STATE_CENTER_OFFSET,
	QDSCR_OBJECT_STATE_DELAY,
	QDSCR_OBJECT_STATE_SOUND_DELAY,
	QDSCR_OBJECT_STATE_TEXT_DELAY,
	QDSCR_OBJECT_STATE_CURSOR_ID,
	QDSCR_OBJECT_STATE_ACCELERATION,
	QDSCR_OBJECT_STATE_RND_MOVE,

	QDSCR_OBJECT_STATE_WALK_SOUND_FREQUENCY,

	QDSCR_STATE_CENTER_OFFSETS,
	QDSCR_STATE_STATIC_CENTER_OFFSETS,
	QDSCR_STATE_START_CENTER_OFFSETS,
	QDSCR_STATE_STOP_CENTER_OFFSETS,

	// State conditions
	QDSCR_CONDITION,
	QDSCR_CONDITION_DATA_INT,
	QDSCR_CONDITION_DATA_FLOAT,
	QDSCR_CONDITION_DATA_STRING,
	QDSCR_CONDITION_INVERSE,
	QDSCR_CONDITIONS_MODE,

	QDSCR_CONDITION_GROUP,
	QDSCR_CONDITION_OBJECT,

	// Counter
	QDSCR_COUNTER,
	QDSCR_COUNTER_ELEMENT,
	QDSCR_COUNTER_INC_VALUE,
	QDSCR_COUNTER_LIMIT,
	QDSCR_COUNTER_DIGITS,
	QDSCR_COUNTER_NAME,

	// Object Scale
	QDSCR_OBJ_SCALE_INFO,

	// Mouse object
	QDSCR_MOUSE_OBJECT,
	QDSCR_MOUSE_DEFAULT_CURSORS,

	// Moving Object
	QDSCR_MOVEMENT_ANIMATION_NUMBER,
	QDSCR_MOVEMENT_ANIMATIONS,
	QDSCR_STATIC_ANIMATIONS,
	QDSCR_MOVING_OBJECT,
	QDSCR_COLLISION_RADIUS,
	QDSCR_COLLISION_DELAY,
	QDSCR_COLLISION_PATH,
	QDSCR_FOLLOW_MIN_RADIUS,
	QDSCR_FOLLOW_MAX_RADIUS,
	QDSCR_ATTACH_SHIFT,
	QDSCR_PERSONAGE_CONTROL,
	QDSCR_PERSONAGE_MOVEMENT_TYPE,
	QDSCR_ROTATION_ANGLE_PER_QUANT,

	// Inventory
	QDSCR_INVENTORY_CELL_TYPE,
	QDSCR_INVENTORY_CELL_SET,
	QDSCR_INVENTORY_CELL_SET_SIZE,
	QDSCR_INVENTORY_CELL_SET_ADDITIONAL_CELLS,
	QDSCR_INVENTORY_CELL_SET_POS,
	QDSCR_INVENTORY,

	// Camera & grid
	QDSCR_CAMERA,
	QDSCR_CAMERA_GRID_SIZE,
	QDSCR_CAMERA_GRID_HEIGHT,
	QDSCR_CAMERA_GRID_ATTRIBUTES,
	QDSCR_CAMERA_GRID_CENTER,
	QDSCR_CAMERA_CELL_SIZE,
	QDSCR_CAMERA_FOCUS,
	QDSCR_CAMERA_ANGLES,
	QDSCR_CAMERA_SCREEN_SIZE,
	QDSCR_CAMERA_SCREEN_OFFSET,
	QDSCR_CAMERA_SCREEN_CENTER,
	QDSCR_CAMERA_MODE,
	QDSCR_CAMERA_SCROLLING_SPEED,
	QDSCR_CAMERA_SCROLLING_DIST,
	QDSCR_CAMERA_SMOOTH_SWITCH,
	QDSCR_CAMERA_SCALE_POW,
	QDSCR_CAMERA_SCALE_Z_OFFSET,

	// Game end
	QDSCR_GAME_END,
	QDSCR_GAME_END_SCREEN,
	
	// Interface
	QDSCR_INTERFACE,

	QDSCR_INTERFACE_SCENE_REDRAW,

	QDSCR_INTERFACE_MAIN_MENU,
	QDSCR_INTERFACE_INGAME_SCREEN0,
	QDSCR_INTERFACE_INGAME_SCREEN1,
	QDSCR_INTERFACE_HIGHSCORE_NAME_SCREEN,
	QDSCR_INTERFACE_SAVE_PROMPT_SCREEN,
	QDSCR_INTERFACE_SAVE_NAME_SCREEN,

	QDSCR_INTERFACE_NEED_SAVE_SCREENSHOT,
	QDSCR_INTERFACE_NEED_SHOW_SAVE_NAME,
	QDSCR_INTERFACE_NEED_SHOW_SAVE_TIME,
	QDSCR_INTERFACE_SAVE_FONT_TYPE,
	QDSCR_INTERFACE_SAVE_FONT_COLOR,
	QDSCR_INTERFACE_SAVE_IS_AUTOSAVE,

	QDSCR_INTERFACE_SCREEN,
	QDSCR_INTERFACE_SCREEN_HIDE_TIME,
	QDSCR_INTERFACE_SCREEN_HIDE_OFFSET,
	QDSCR_INTERFACE_ELEMENT,

	QDSCR_INTERFACE_ELEMENT_STATE,
	QDSCR_INTERFACE_ELEMENT_STATE_MODE,
	QDSCR_INTERFACE_ELEMENT_POS,

	QDSCR_INTERFACE_OPTION_ID,

	QDSCR_INTERFACE_EVENT,
	QDSCR_INTERFACE_EVENT_DATA,
	QDSCR_INTERFACE_EVENT_BEFORE_ANIMATION,
	QDSCR_INTERFACE_EVENT_ACTIVATION_TYPE,

	QDSCR_INTERFACE_ACTION,
	QDSCR_INTERFACE_ACTION_DATA,

	QDSCR_INTERFACE_ANIMATION_FLAGS,
	QDSCR_INTERFACE_SLIDER_RECTANGLE,
	QDSCR_INTERFACE_SLIDER_ORIENTATION,
	QDSCR_INTERFACE_BACKGROUND_OFFSET,

	QDSCR_INTERFACE_THUMBNAIL_SIZE,
	QDSCR_INTERFACE_TEXT_SHIFT,

	QDSCR_TEXT_WINDOW_BORDER_CORNER,
	QDSCR_TEXT_WINDOW_BORDER_LINE_H,
	QDSCR_TEXT_WINDOW_BORDER_LINE_V,
	QDSCR_TEXT_WINDOW_BORDER_BACK,
	QDSCR_TEXT_WINDOW_SLIDER,
	QDSCR_TEXT_WINDOW_TYPE,
	QDSCR_TEXT_WINDOW_MAX_STRING_LENGTH,
	QDSCR_TEXT_WINDOW_INFO_TYPE,
	QDSCR_TEXT_WINDOW_PLAYER_ID,
	
	// Scene
	QDSCR_SCENE,
	QDSCR_SCENE_SAVE_SLOT,
	QDSCR_STARTUP_SCENE,
	QDSCR_SCENE_RESTART_MINIGAME,

	// Location
	QDSCR_LOCATION,

	QDSCR_GAME_TITLE,
	QDSCR_TEXT_DB,
	QDSCR_CD_KEY,

	// For qdScreenTextFormat
	QDSCR_SCREEN_TEXT_FORMAT,
	QDSCR_FONT_TYPE,

	QDSCR_DEFAULT_FONT,
	QDSCR_GLOBAL_DEPEND,
	QDSCR_INVERSE_DIRECTION,
	QDSCR_ALPHA,
	QDSCR_ENABLE_BACKGROUND,
	QDSCR_FADE_TIME,

	QDSCR_HALL_OF_FAME_SIZE,

	QDSCR_RESOURCE_COMPRESSION,

	QDSCR_SCREEN_TRANSFORM,

	QDSCR_MAX_KEYWORD_ID
};

xml::parser& qdscr_XML_Parser();
const char* qdscr_XML_string(const char* p);

#endif /* __QDSCR_PARSER_H__ */
