#pragma once
#include "foldertreenode.h"

/// Папка миниигр
class MinigameFolderTreeNode : public FolderTreeNode
{
public:
	MinigameFolderTreeNode(void);
	~MinigameFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
};
