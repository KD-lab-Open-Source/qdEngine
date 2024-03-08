#pragma once
#include ".\interfaceelementtreenode.h"

/// Узел сейва
class InterfaceSaveTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceSaveTreeNode(qdInterfaceObjectBase* object);
	~InterfaceSaveTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
