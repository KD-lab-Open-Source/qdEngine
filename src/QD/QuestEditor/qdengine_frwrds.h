/**
	\file
	Предварительное декларирование слассов и типов движка
 */
#ifndef _QDENGINE_FORWARDS_
#define _QDENGINE_FORWARDS_

class qdGameObjectMoving;
class qdGameObjectAnimated;
class qdGameObjectStatatic;
class qdGameObjectMouse;
class qdGameObject;

class qdGameDispatcher;
class qdGameScene;

class qdInventory;
class qdInventoryCellSet;
class qdInventoryCell;

class qdGridZone;

class qdCamera;
class sGridCell;
class qdNamedObject;

class qdGameObjectState;
class qdGameObjectStateStatic;
class qdGameObjectStateWalk;
class qdGameObjectStateMask;

class qdAnimation;
class qdCoordsAnimation;
class qdAnimationSet;

class qdAnimationSetPreview;
class qdSprite;

class qdVideo;

class qdTriggerElement;
class qdTriggerChain;

typedef std::vector<Vect2s> grid_contour_t;
typedef std::list<qdVideo*> qdVideoList;
typedef std::list<qdTriggerChain*> qdTriggerChainList;
typedef std::list<qdGameObject*> qdGameObjectList;
typedef std::list<qdGridZone*> qdGridZoneList;

#endif _QDENGINE_FORWARDS_