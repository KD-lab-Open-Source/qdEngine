#pragma once
#include "foldertreenode.h"

/// Папка последних экранов
class GameEndFolderTreeNode :
	public FolderTreeNode
{
public:
	GameEndFolderTreeNode(void);
	~GameEndFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
};
