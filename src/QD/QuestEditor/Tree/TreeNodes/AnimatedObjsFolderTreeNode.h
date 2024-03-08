#pragma once
#include "foldertreenode.h"

/// Папка анимированных объектов
class AnimatedObjsFolderTreeNode : public FolderTreeNode
{
public:
	AnimatedObjsFolderTreeNode();
	~AnimatedObjsFolderTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);

	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
};
