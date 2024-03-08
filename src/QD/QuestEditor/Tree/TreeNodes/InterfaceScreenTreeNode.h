#pragma once
#include "interfaceobjecttreenode.h"

/// Узел интерфейсного экрана
class InterfaceScreenTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceScreenTreeNode(qdInterfaceObjectBase* object);
	~InterfaceScreenTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic);
protected:
	/// Загружает содержимое интерфейсного экрана
	bool loadContent(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCTSTR newName) const;
};
