#include "stdafx.h"
#include "resource.h"
#include ".\gameendobjecttreenode.h"

#include "ISceneViewFacade.h"

#include "ptree_consts.h"
#include "qd_game_end.h"

#include "name_op.h"

GameEndObjectTreeNode::GameEndObjectTreeNode(qdNamedObject* object)
: ConditionalObjectTreeNode(object)
{
}

GameEndObjectTreeNode::~GameEndObjectTreeNode(void)
{
}

bool GameEndObjectTreeNode::load(ITreeNodeFactory* factory,
				  TreeLogic& treeLogic, 
				  ITreeNode * parent, 
				  qdNamedObjectBase const* owner,
				  ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
										getObject()->name(),
										parent,
										II_GAME_END_NODE,
										reinterpret_cast<DWORD_PTR>(getObject()), 
										insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD GameEndObjectTreeNode::getMenuResId() const{
	return IDR_GAME_END_NODE;
}

bool GameEndObjectTreeNode::onStateChange(TreeLogic& logic, 
										  DWORD oldState, 
										  DWORD newState)
{
	if (!getObject())
		return false;
	ISceneViewFacade *sceneView = logic.getSceneView();

	if (newState&NS_FOCUSED)
	{
		qdGameEnd* pge = static_cast<qdGameEnd*>(getObject());
		sceneView->SetViewState(VS_EDIT_INTERFACE);
		sceneView->SetSubTools(SSTE_NONE);
		return !logic.selectInterfaceScreen(pge->interface_screen());
	}
	return false;
}

bool GameEndObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false;
	if (logic.getGameDispatcher()->remove_game_end(
		static_cast<qdGameEnd*>(getObject())))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		delete getObject();
//		setObject(NULL);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

bool GameEndObjectTreeNode::canDrag(TreeLogic& logic){
	return true;
}

bool GameEndObjectTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool GameEndObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return gameDisp->rename_game_end(static_cast<qdGameEnd*>(getObject()), newName);
}

bool GameEndObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return uniq_name(newName, gameDisp->game_end_list());
}
