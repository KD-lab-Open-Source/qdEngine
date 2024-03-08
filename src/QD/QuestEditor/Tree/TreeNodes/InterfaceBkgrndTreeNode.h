#pragma once
#include ".\interfaceelementtreenode.h"

/// Класс бэкграунда
class InterfaceBkgrndTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceBkgrndTreeNode(qdInterfaceObjectBase* object);
	~InterfaceBkgrndTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
