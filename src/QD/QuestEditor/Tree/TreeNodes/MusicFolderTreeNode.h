#pragma once
#include "foldertreenode.h"

/// Папка музыкальных треков
class MusicFolderTreeNode : public FolderTreeNode
{
public:
	MusicFolderTreeNode();
	~MusicFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
};
