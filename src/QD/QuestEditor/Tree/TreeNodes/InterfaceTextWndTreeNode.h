#pragma once
#include ".\interfaceelementtreenode.h"

/// Узел окна с текстом
class InterfaceTextWndTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceTextWndTreeNode(qdInterfaceObjectBase* object);
	~InterfaceTextWndTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
};
