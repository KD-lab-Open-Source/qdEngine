#include "stdafx.h"
#include "resource.h"
#include ".\triggerfoldertreenode.h"
#include "ptree_consts.h"

TriggerFolderTreeNode::TriggerFolderTreeNode(void)
: FolderTreeNode(QD_NAMED_OBJECT_TRIGGER_CHAIN)
{
}

TriggerFolderTreeNode::~TriggerFolderTreeNode(void)
{
}

bool TriggerFolderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
								 ITreeNode * parent, qdNamedObjectBase const* owner,
								 ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_TRIGGERS_NODE);
	CTreeListItem* item = insertToTree(treeLogic, str,
							parent, II_TRIGGERS_NODE, getFolderId(), insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD TriggerFolderTreeNode::getMenuResId() const{
	return IDR_TRIGGERS_NODE;
}
