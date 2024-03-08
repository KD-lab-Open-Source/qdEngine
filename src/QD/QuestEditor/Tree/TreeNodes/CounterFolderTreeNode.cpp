#include "stdafx.h"
#include "resource.h"
#include ".\counterfoldertreenode.h"
#include "ptree_consts.h"

CounterFolderTreeNode::CounterFolderTreeNode(void)
: FolderTreeNode(QD_NAMED_OBJECT_COUNTER)
{
}

CounterFolderTreeNode::~CounterFolderTreeNode(void)
{
}

bool CounterFolderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
								 ITreeNode * parent, qdNamedObjectBase const* owner,
								 ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_COUNTERS_NODE_NAME);
	CTreeListItem* item = insertToTree(treeLogic, str, 
									parent, II_COUNTERS_NODE, getFolderId(), insertAfter);
	if (!item)
		return false;
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD CounterFolderTreeNode::getMenuResId() const{
	return IDR_COUNTERS_NODE;
}
