#include "stdafx.h"
#include ".\staticstatesfoldertreenode.h"
#include "resource.h"

#include "ptree_consts.h"

StaticStatesFolderTreeNode::StaticStatesFolderTreeNode()
: FolderTreeNode(QE_STATE_CATALOG)
{
}

StaticStatesFolderTreeNode::~StaticStatesFolderTreeNode(void)
{
}

bool StaticStatesFolderTreeNode::load(ITreeNodeFactory* factory, 
									  TreeLogic& treeLogic, 
									  ITreeNode * parent, 
									  qdNamedObjectBase const* owner,
									  ITreeNode* insertAfter)
{
	CString str((LPTSTR)IDS_STOPS_CATALOG);
	CTreeListItem* const item = 
		insertToTree(treeLogic, str, parent, II_STATES_CATALOG, getFolderId(), insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD StaticStatesFolderTreeNode::getMenuResId() const{
	return IDR_MOVING_OBJ_ANIMATION_NODE;
}
