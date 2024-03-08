#include "stdafx.h"
#include "resource.h"
#include ".\minigamefoldertreenode.h"

#include "ISceneViewFacade.h"

#include "ptree_consts.h"

MinigameFolderTreeNode::MinigameFolderTreeNode(void)
: FolderTreeNode(QD_NAMED_OBJECT_MINIGAME)
{
}

MinigameFolderTreeNode::~MinigameFolderTreeNode(void)
{
}

bool MinigameFolderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
								  ITreeNode * parent, qdNamedObjectBase const* owner,
								  ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_MINIGAMES_NODE);
	CTreeListItem* const item = insertToTree(treeLogic,
											str,
											parent,
											II_MINIGAMES_NODE, 
											getFolderId(), insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD MinigameFolderTreeNode::getMenuResId() const{
	return IDR_MINIGAMES_NODE;
}

bool MinigameFolderTreeNode::onStateChange(TreeLogic& logic, 
										   DWORD oldState, 
										   DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	if (newState&NS_FOCUSED) {
		logic.getSceneView()->SetViewState(VS_NONE);
		return true;
	} 
	return false;
}
