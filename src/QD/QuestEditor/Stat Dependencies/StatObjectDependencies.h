#pragma once

#include <map>
#include <list>

#include "qd_fwd.h"
class qdNamedObject;

typedef std::list<qdNamedObject const*> NamedObjectList;
typedef std::map<qdNamedObject const*, NamedObjectList> NamedObjectListIndex;
class StatObjectDependencies
{
	static void stat(qdGameDispatcher const* gameDisp, qdGameScene const* scene, 
					NamedObjectListIndex* index);
	static void stat(qdGameDispatcher const* gameDisp, 
		qdGameObject const* object, NamedObjectListIndex* index);
	static void stat(qdGameDispatcher const* gameDisp,
		qdGameObjectState const* state, NamedObjectListIndex* index);
	static void stat(qdGameDispatcher const* gameDisp,
		qdGridZone const* zone, NamedObjectListIndex* index);
	static void stat(qdGameDispatcher const* gameDisp,
		qdMiniGame const* miniGame, NamedObjectListIndex* index);
	static void stat(qdGameDispatcher const* gameDisp,
		qdCounter const* counter, NamedObjectListIndex* index);
	StatObjectDependencies(void);
	~StatObjectDependencies(void);
public:
	static void run(qdGameDispatcher* gameDisp,
					qdNamedObject const* begin, 
					NamedObjectListIndex* index);
};
