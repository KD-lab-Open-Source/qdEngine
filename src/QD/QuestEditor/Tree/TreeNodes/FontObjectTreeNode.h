#pragma once
#include "namedobjecttreenode.h"

/// Класс узла, соответствующий объекту фонта
class FontObjectTreeNode : public NamedObjectTreeNode
{
public:
	FontObjectTreeNode(qdNamedObject* object);
	~FontObjectTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
