#include "stdafx.h"
#include ".\treenodefactory.h"

#include "TreeNodes/GameDispTreeNode.h"
#include "TreeNodes/GameSceneTreeNode.h"
#include "TreeNodes/NetFolderTreeNode.h"
#include "TreeNodes/GridZoneTreeNode.h"
#include "TreeNodes/GridZoneStateTreeNode.h"
#include "TreeNodes/MusicFolderTreeNode.h"
#include "TreeNodes/MusicTrackTreeNode.h"

#include "TreeNodes/StaticObjectTreeNode.h"
#include "TreeNodes/AnimatedObjectTreeNode.h"
#include "TreeNodes/MovingObjectTreeNode.h"

#include "TreeNodes/StaticObjsFolderTreeNode.h"
#include "TreeNodes/AnimatedObjsFolderTreeNode.h"
#include "TreeNodes/MovingObjsFolderTreeNode.h"
#include "TreeNodes/ObjectStateTreeNode.h"
#include "TreeNodes/StaticStatesFolderTreeNode.h"
#include "TreeNodes/WalkStatesFolderTreeNode.h"
#include "TreeNodes/TriggerFolderTreeNode.h"
#include "TreeNodes/TriggerChainTreeNode.h"
#include "TreeNodes/MouseTreeNode.h"
#include "TreeNodes/InventoryFolderTreeNode.h"
#include "TreeNodes/InventoryObjectTreeNode.h"
#include "TreeNodes/CounterFolderTreeNode.h"
#include "TreeNodes/CounterObjectTreeNode.h"
#include "TreeNodes/FontFolderTreeNode.h"
#include "TreeNodes/FontObjectTreeNode.h"
#include "TreeNodes/VideoFolderTreeNode.h"
#include "TreeNodes/VideoObjectTreeNode.h"

#include "TreeNodes/MiniGameFolderTreeNode.h"
#include "TreeNodes/MiniGameObjectTreeNode.h"

#include "TreeNodes/GlobalMovingObjectTreeNode.h"

#include "TreeNodes/GameEndFolderTreeNode.h"
#include "TreeNodes/GameEndObjectTreeNode.h"
#include "TreeNodes/InterfaceDispTreeNode.h"
#include "TreeNodes/InterfaceScreenTreeNode.h"
#include "TreeNodes/InterfaceBkgrndTreeNode.h"
#include "TreeNodes/InterfaceCounterTreeNode.h"
#include "TreeNodes/InterfaceSliderTreeNode.h"
#include "TreeNodes/InterfaceSaveTreeNode.h"
#include "TreeNodes/InterfaceButtonTreeNode.h"
#include "TreeNodes/InterfaceTextWndTreeNode.h"
#include "TreeNodes/InterfaceElementStateTreeNode.h"
#include "TreeNodes/InterfaceElementStateModeTreeNode.h"

#include "qd_named_object.h"
#include "qd_interface_dispatcher.h"
#include "qd_interface_element.h"
#include "qd_interface_element_state.h"

#include "ptree_consts.h"

#include <typeinfo>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TreeNodeFactory::TreeNodeFactory(void)
{
}

TreeNodeFactory::~TreeNodeFactory(void)
{
}

std::auto_ptr<ITreeNode> TreeNodeFactory::createGlobalGameObjectNode(
													qdNamedObject* object)
{
	assert(object->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ);
	return std::auto_ptr<ITreeNode>(new GlobalMovingObjectTreeNode(object));
}

std::auto_ptr<ITreeNode> TreeNodeFactory::createGameObjectNode(qdNamedObject* object)
{
	DBGCHECK;

#define CREATE_OBJ(ID, CRT_TYPE)\
	case ID:\
		result.reset(new CRT_TYPE);\
	break\

	std::auto_ptr<ITreeNode> result;
	switch(object->named_object_type()) {
	CREATE_OBJ(QD_NAMED_OBJECT_DISPATCHER,GameDispTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_SCENE, GameSceneTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_GRID_ZONE, GridZoneTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_MUSIC_TRACK, MusicTrackTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_STATIC_OBJ, StaticObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_ANIMATED_OBJ, AnimatedObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_MOVING_OBJ, MovingObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_OBJ_STATE, ObjectStateTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_TRIGGER_CHAIN, TriggerChainTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_MOUSE_OBJ, MouseTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_INVENTORY, InventoryObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_COUNTER, CounterObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_FONT_INFO, FontObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_VIDEO, VideoObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_MINIGAME, MiniGameObjectTreeNode(object));
	CREATE_OBJ(QD_NAMED_OBJECT_GAME_END, GameEndObjectTreeNode(object));
	}
	return result;
#undef CREATE_OBJ
}

std::auto_ptr<ITreeNode> TreeNodeFactory::createInterfaceObjectNode(
														qdInterfaceObjectBase* object)
{

	std::auto_ptr<ITreeNode> result;
	type_info const& ti = typeid(*object);
	if (ti == typeid(qdInterfaceDispatcher))
		result.reset(new InterfaceDispTreeNode(object));
	else if (ti == typeid(qdInterfaceScreen))
		result.reset(new InterfaceScreenTreeNode(object));
	else if (ti == typeid(qdInterfaceBackground))
		result.reset(new InterfaceBkgrndTreeNode(object));
	else if (ti == typeid(qdInterfaceCounter))
		result.reset(new InterfaceCounterTreeNode(object));
	else if (ti == typeid(qdInterfaceSlider))
		result.reset(new InterfaceSliderTreeNode(object));
	else if (ti == typeid(qdInterfaceButton))
		result.reset(new InterfaceButtonTreeNode(object));
	else if (ti == typeid(qdInterfaceSave))
		result.reset(new InterfaceSaveTreeNode(object));
	else if (ti == typeid(qdInterfaceTextWindow))
		result.reset(new InterfaceTextWndTreeNode(object));
	else if (ti == typeid(qdInterfaceElementState))
		result.reset(new InterfaceElementStateTreeNode(object));
//	else if (ti == typeid(qdInterfaceElementStateMode))
//		result.reset(new InterfaceElementStateModeTreeNode(object));
	return result;
}

std::auto_ptr<ITreeNode> TreeNodeFactory::createFolderNode(DWORD id)
{
#define CREATE_OBJ(ID, CRT_TYPE)\
	case ID:\
	result.reset(new CRT_TYPE);\
	break\

	std::auto_ptr<ITreeNode> result;
	switch(id) {
		CREATE_OBJ(QD_NAMED_OBJECT_GRID_ZONE, NetFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_MUSIC_TRACK, MusicFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_STATIC_OBJ, StaticObjsFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_ANIMATED_OBJ, AnimatedObjsFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_MOVING_OBJ, MovingObjsFolderTreeNode);
		CREATE_OBJ(QE_STATE_CATALOG, StaticStatesFolderTreeNode);
		CREATE_OBJ(QE_MOVEMENT_STATE_CATALOG, WalkStatesFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_TRIGGER_CHAIN, TriggerFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_INVENTORY, InventoryFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_COUNTER, CounterFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_FONT_INFO, FontFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_VIDEO, VideoFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_MINIGAME, MinigameFolderTreeNode);
		CREATE_OBJ(QD_NAMED_OBJECT_GAME_END, GameEndFolderTreeNode);
	}
	return result;
#undef CREATE_OBJ
}

std::auto_ptr<ITreeNode> TreeNodeFactory::createGridZoneStateNode(LPCTSTR name, 
															 qdNamedObject* object)
{
	return std::auto_ptr<ITreeNode>(new GridZoneStateTreeNode(name, object));
}

std::auto_ptr<ITreeNode> TreeNodeFactory::createInterfaceElementStateModeNode(
	qdInterfaceElementState* object, DWORD modeId)
{
	return std::auto_ptr<ITreeNode>(new InterfaceElementStateModeTreeNode(object, 
		static_cast<qdInterfaceElementState::state_mode_t>(modeId)));
}
