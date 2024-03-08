#pragma once
#include ".\conditionalobjecttreenode.h"

/// Класс узла, соответствующего конечному экрану. 
class GameEndObjectTreeNode : public ConditionalObjectTreeNode
{
public:
	GameEndObjectTreeNode(qdNamedObject* object);
	~GameEndObjectTreeNode(void);
	virtual bool load(ITreeNodeFactory* factory,
						TreeLogic& treeLogic, 
						ITreeNode * parent, 
						qdNamedObjectBase const* owner,
						ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	virtual bool canDrag(TreeLogic& logic);
	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
