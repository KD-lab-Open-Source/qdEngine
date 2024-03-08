#pragma once
#include ".\namedobjecttreenode.h"

/// Класс узла, принадлежащего миниигре
class MiniGameObjectTreeNode :
	public NamedObjectTreeNode
{
public:
	MiniGameObjectTreeNode(qdNamedObject* object);
	~MiniGameObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;

	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);

	virtual bool showProperties(TreeLogic& logic, int activePage);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCSTR newName);

};
