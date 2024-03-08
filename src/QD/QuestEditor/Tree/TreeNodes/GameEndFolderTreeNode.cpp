#include "stdafx.h"
#include ".\gameendfoldertreenode.h"
#include "resource.h"
#include "ptree_consts.h"

GameEndFolderTreeNode::GameEndFolderTreeNode(void)
:FolderTreeNode(QD_NAMED_OBJECT_GAME_END)
{
}

GameEndFolderTreeNode::~GameEndFolderTreeNode(void)
{
}

bool GameEndFolderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
								 ITreeNode * parent, qdNamedObjectBase const* owner,
								 ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_GAME_ENDS_NODE_NAME);
	CTreeListItem* const item = insertToTree(treeLogic,
											str,
											parent,
											II_GAME_ENDS_NODE, 
											getFolderId(),
											insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD GameEndFolderTreeNode::getMenuResId() const{
	return IDR_GAME_ENDS_NODE;
}
