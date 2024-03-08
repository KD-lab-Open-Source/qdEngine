#pragma once
#include "foldertreenode.h"

/// Папка триггеров
class TriggerFolderTreeNode : public FolderTreeNode
{
public:
	TriggerFolderTreeNode(void);
	~TriggerFolderTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
};
