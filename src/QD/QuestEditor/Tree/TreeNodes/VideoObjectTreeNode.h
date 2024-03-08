#pragma once
#include ".\conditionalobjecttreenode.h"

/// Узел видео
class VideoObjectTreeNode : public ConditionalObjectTreeNode
{
public:
	VideoObjectTreeNode(qdNamedObject* object);
	~VideoObjectTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
		ITreeNode* dropTarget, 
		DROPEFFECT dropEffect);
	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
