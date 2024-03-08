#pragma once
#include "namedobjecttreenode.h"

/// Узел цепочки триггеров
class TriggerChainTreeNode : public NamedObjectTreeNode
{
public:
	TriggerChainTreeNode(qdNamedObject* object);
	~TriggerChainTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent,
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepnds);
	virtual bool canDrag(TreeLogic& logic);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, ITreeNode* dropTarget, 
		DROPEFFECT dropEffect);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// Выбрали(сняли выбор) при активном редакторе сцены
	bool onSelectInTriggerView(TreeLogic& logic, bool bSelect);
	/// Выбрали(сняли выбор) при активном редакторе триггеров
	bool onSelectInSceneView(TreeLogic& logic, bool bSelect);
	/// Выполняет действия после удаления
	void postDelete(TreeLogic& logic);

	/// Объеденяет триггера
	BOOL MergeTriggers(TreeLogic& logic, qdTriggerChain const* from, qdTriggerChain* to);
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
