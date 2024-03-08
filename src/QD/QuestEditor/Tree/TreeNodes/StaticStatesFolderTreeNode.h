#pragma once
#include "foldertreenode.h"

/// Папка статических состояний персонажа
class StaticStatesFolderTreeNode : public FolderTreeNode
{
public:
	StaticStatesFolderTreeNode();
	~StaticStatesFolderTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
};
