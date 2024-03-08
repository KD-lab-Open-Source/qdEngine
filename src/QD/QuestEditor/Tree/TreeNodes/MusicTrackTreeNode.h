#pragma once
#include ".\conditionalobjecttreenode.h"

/// Узел музыкального трека
class MusicTrackTreeNode :
	public ConditionalObjectTreeNode
{
public:
	MusicTrackTreeNode(qdNamedObject* object);
	~MusicTrackTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
						ITreeNode * parent, 
						qdNamedObjectBase const* owner,
						ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
	// при его удаление(сотояния, когда удаляется анимированный объект. 
	//Все объекты, когда удаляется сцена)
	virtual bool deleteAsSubObject(TreeLogic& logic);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
