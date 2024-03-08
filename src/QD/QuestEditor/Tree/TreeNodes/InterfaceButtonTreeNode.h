#pragma once
#include ".\interfaceelementtreenode.h"

/// Класс интерфейсной кнопки
class InterfaceButtonTreeNode : public InterfaceElementTreeNode
{
public:
	InterfaceButtonTreeNode(qdInterfaceObjectBase* object);
	~InterfaceButtonTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int activePage);
protected:
	/// Загружает в дерево состояния кнопки
	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic);
};
