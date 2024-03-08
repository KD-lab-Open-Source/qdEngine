#pragma once
#include "foldertreenode.h"

/// Папка инвентори
class InventoryFolderTreeNode : public FolderTreeNode
{
public:
	InventoryFolderTreeNode(void);
	~InventoryFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
};
