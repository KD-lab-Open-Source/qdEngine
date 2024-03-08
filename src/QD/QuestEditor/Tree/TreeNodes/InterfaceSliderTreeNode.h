#pragma once
#include ".\interfaceelementtreenode.h"

/// Узел слайдера
class InterfaceSliderTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceSliderTreeNode(qdInterfaceObjectBase* object);
	~InterfaceSliderTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
};
