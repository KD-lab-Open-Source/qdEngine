#pragma once
#include "interfaceobjecttreenode.h"

/// Узел состояния интерфейсного элемента
class InterfaceElementStateTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceElementStateTreeNode(qdInterfaceObjectBase* object);
	~InterfaceElementStateTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	/// Загружает режимы в дерево
	bool loadModes(ITreeNodeFactory* factory, TreeLogic& treeLogic) ;

	/// Удаляет кнопку, если в ней нет больше состояний
	bool postDelete(TreeLogic& logic);
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
