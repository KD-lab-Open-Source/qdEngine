#pragma once
#include "interfaceobjecttreenode.h"

/// Узел интерфейсного элемента
class InterfaceElementTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceElementTreeNode(qdInterfaceObjectBase* object);
	~InterfaceElementTreeNode(void);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic);
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);
protected:
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
