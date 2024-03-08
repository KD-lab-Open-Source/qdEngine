#include "stdafx.h"
#include ".\walkstatesfoldertreenode.h"
#include "resource.h"
#include "ptree_consts.h"

WalkStatesFolderTreeNode::WalkStatesFolderTreeNode()
: FolderTreeNode(QE_MOVEMENT_STATE_CATALOG)
{
}

WalkStatesFolderTreeNode::~WalkStatesFolderTreeNode(void)
{
}

bool WalkStatesFolderTreeNode::load(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic, 
									ITreeNode * parent, 
									qdNamedObjectBase const* owner,
									ITreeNode* insertAfter)
{
	CString str((LPTSTR)IDS_GAITS_CATALOG);
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();
	CTreeListItem* const item = 
		insertToTree(treeLogic, str, parent, 
					II_STATES_CATALOG, getFolderId(), insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD WalkStatesFolderTreeNode::getMenuResId() const{
	return IDR_MOVING_OBJ_GAITS_NODE;
}
