#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\movingobjsfoldertreenode.h"

#include "ISceneViewFacade.h"

#include "menu_helpers.h"

#include "qd_named_object.h"

MovingObjsFolderTreeNode::MovingObjsFolderTreeNode()
: FolderTreeNode(QD_NAMED_OBJECT_MOVING_OBJ)
{
}

MovingObjsFolderTreeNode::~MovingObjsFolderTreeNode(void)
{
}

bool MovingObjsFolderTreeNode::load(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic, 
									ITreeNode * parent, 
									qdNamedObjectBase const* owner,
									ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_MOVING_OBJS_NODE);
	CTreeListItem* const item = insertToTree(treeLogic, str, 
								parent, II_MOVING_OBJS_NODE, getFolderId(), insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD MovingObjsFolderTreeNode::getMenuResId() const{
	return IDR_MOVING_OBJS_NODE;
}

void MovingObjsFolderTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if(treeLogic.getGameDispatcher() == NULL||!treeLogic.isSceneViewActive())
		menu_h::disableMenuItem(pm, IDC_CRT_MOVING_OBJ);
}

bool MovingObjsFolderTreeNode::onStateChange(TreeLogic& logic, 
											 DWORD oldState, 
											 DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	bool bSelect = newState&NS_FOCUSED;
	ISceneViewFacade* sceneFacade = logic.getSceneView();
	if (bSelect) {
		if(logic.getCurrentSceneNode())
			sceneFacade->SetViewState(VS_EDIT_SCENE);
		else
			sceneFacade->SetViewState(VS_NONE);
		sceneFacade->SelectObj(NULL);
		return true;
	}

	return false;
}