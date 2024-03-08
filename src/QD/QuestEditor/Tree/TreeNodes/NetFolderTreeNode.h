#pragma once
#include "foldertreenode.h"

/// Папка зон на сетке
class NetFolderTreeNode : public FolderTreeNode
{
public:
	NetFolderTreeNode();
	virtual ~NetFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, 
		TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	/// Управляет методом отрисовки сетки
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
};
