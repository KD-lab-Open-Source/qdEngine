#pragma once
#include "foldertreenode.h"

/// Папка счетчиков
class CounterFolderTreeNode : public FolderTreeNode
{
public:
	CounterFolderTreeNode(void);
	~CounterFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
};
