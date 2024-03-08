#include "stdafx.h"
#include ".\gameobjecttreenode.h"

#include "ISceneViewFacade.h"
#include "qd_game_object.h"
#include "qd_game_dispatcher.h"

#include "name_op.h"
#include "Main/ObjectTraits.h"

GameObjectTreeNode::GameObjectTreeNode(qdNamedObject* object):
  NamedObjectTreeNode(object)
{
}

GameObjectTreeNode::~GameObjectTreeNode(void)
{
}


bool GameObjectTreeNode::onCheckNode(TreeLogic& logic, bool bCheck)
{
	if (bCheck)
		getObject()->drop_flag(QD_OBJ_HIDDEN_FLAG);
	else
		getObject()->set_flag(QD_OBJ_HIDDEN_FLAG);
	return true;
}

bool GameObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false; 

	if(removeObject(logic)){
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}

	return false;
}

bool GameObjectTreeNode::removeObject(TreeLogic& logic)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());
	logic.getSceneView()->SelectObj(NULL);
	if (scene->remove_object(static_cast<qdGameObject*>(getObject())))
	{
		delete getObject();
//		setObject(NULL);
		return true;
	}
	return false;
}

//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
// при его удаление(сотояния, когда удаляется анимированный объект. 
//Все объекты, когда удаляется сцена)
bool GameObjectTreeNode::deleteAsSubObject(TreeLogic& logic)
{
	return removeObject(logic);
}

bool GameObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameScene const* scene = static_cast<qdGameScene*>(getObject()->owner());

	return uniq_name(newName, scene->object_list());
}

//! Возвращает true, если удалось переимновать, false в противном случае
bool GameObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;

	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());
	return scene->rename_object(static_cast<qdGameObject*>(getObject()), newName);
}
//! Можно ли сменить имя узлу
bool GameObjectTreeNode::canRename(TreeLogic& logic)
{
	return !ObjectTraits::isCopyOfGlobal(logic.getGameDispatcher(), 
		static_cast<qdGameObject*>(getObject()));
}
