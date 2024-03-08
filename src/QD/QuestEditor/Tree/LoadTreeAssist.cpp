#include "stdafx.h"
#include "LoadTreeAssist.h"
#include "Tree/ITreeNodeFactory.h"
#include "Tree/ITreeNode.h"
#include "Main/StateTraits.h"

#include "qd_game_object_state.h"
#include "qd_game_object_animated.h"

namespace load_tree_assist
{
InsertObject::InsertObject(ITreeNodeFactory* factory, 
						   TreeLogic& treeLogic, 
						   ITreeNode * parent,
						   qdNamedObjectBase const* owner,
						   ITreeNode* insertAfter)
: factory_(factory)
, treeLogic_(treeLogic)
, parent_(parent)
, owner_(owner)
, insertAfter_(insertAfter)
	{
	}
bool InsertObject::operator() (qdNamedObject* object){
	return operator()(object, parent_);
}

bool InsertObject::operator()(qdNamedObject* object, ITreeNode * parent){
	std::auto_ptr<ITreeNode> node(factory_->createGameObjectNode(object));
	if (!node.get()||!node->load(factory_, treeLogic_, parent, owner_, insertAfter_))
		return false;
	node.release();
	return true;
}

InsertGameObject::InsertGameObject(ITreeNodeFactory* factory, TreeLogic& treeLogic,
				ITreeNode * staticObjs, 
				ITreeNode * animatedObjs, 
				ITreeNode * movingObjs,
				qdNamedObjectBase const* owner,
				ITreeNode* insertAfter)
: objInserter_(factory, treeLogic, NULL, owner, insertAfter)
, staticObjs_(staticObjs)
, animatedObjs_(animatedObjs)
, movingObjs_(movingObjs)
{
}
void InsertGameObject::operator()(qdGameObject* object)
{
	switch(object->named_object_type()) {
	case QD_NAMED_OBJECT_STATIC_OBJ:
		objInserter_(object, staticObjs_);
		break;
	case QD_NAMED_OBJECT_ANIMATED_OBJ:
		objInserter_(object, animatedObjs_);
		break;
	case QD_NAMED_OBJECT_MOVING_OBJ:
		objInserter_(object, movingObjs_);
		break;
	}
}

InsertInterfaceObject::InsertInterfaceObject(ITreeNodeFactory* factory, 
						   TreeLogic& treeLogic, 
						   ITreeNode * parent,
						   qdNamedObjectBase const* owner,
						   ITreeNode* insertAfter)
						   : factory_(factory)
						   , treeLogic_(treeLogic)
						   , parent_(parent)
						   , owner_(owner)
						   , insertAfter_(insertAfter)
{
}

bool InsertInterfaceObject::operator()(qdInterfaceObjectBase* object){
	std::auto_ptr<ITreeNode> node(factory_->createInterfaceObjectNode(object));
	if (!node.get()||!node->load(factory_, treeLogic_, parent_, owner_, insertAfter_))
		return false;
	node.release();
	return true;
}


bool own_walk::operator()(const qdGameObjectState* ps) const{
	return (!StateTraits::isGlobalForObj(*ps, *powner)
		&&ps->state_type() == qdGameObjectState::STATE_WALK);
}

bool own_not_walk::operator()(const qdGameObjectState* ps) const{
	return (!StateTraits::isGlobalForObj(*ps, *powner)
		&&ps->state_type() != qdGameObjectState::STATE_WALK);
}

}