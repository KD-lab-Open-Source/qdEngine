#pragma once
#include "foldertreenode.h"

/// Папка походочных состояний персонажа
class WalkStatesFolderTreeNode : public FolderTreeNode
{
public:
	WalkStatesFolderTreeNode();
	~WalkStatesFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
};
