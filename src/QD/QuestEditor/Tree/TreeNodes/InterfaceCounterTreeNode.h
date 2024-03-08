#pragma once
#include ".\interfaceelementtreenode.h"

/// Класс бэкграунда
class InterfaceCounterTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceCounterTreeNode(qdInterfaceObjectBase* object);
	~InterfaceCounterTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
