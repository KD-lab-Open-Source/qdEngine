#pragma once
#include "interfaceobjecttreenode.h"

/// Узел интерфейсного диспетчера 
class InterfaceDispTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceDispTreeNode(qdInterfaceObjectBase* object);
	~InterfaceDispTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic);
protected:
	/// Загружет интерфейсные экраны. Экраны грузят все остальное
	bool loadScreens(ITreeNodeFactory* factory, TreeLogic& treeLogic);
};
