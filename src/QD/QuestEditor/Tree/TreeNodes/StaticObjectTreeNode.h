#pragma once
#include "gameobjecttreenode.h"

/// Узел статического объекта
class StaticObjectTreeNode : public GameObjectTreeNode
{
public:
	StaticObjectTreeNode(qdNamedObject* object);
	~StaticObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	/// Выдает картинку при узеле объекта в дереве
	static int selectPicture(qdNamedObject const* object);

	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
};
