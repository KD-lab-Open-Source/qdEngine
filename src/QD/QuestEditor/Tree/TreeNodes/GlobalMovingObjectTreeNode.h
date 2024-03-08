#pragma once
#include "movingobjecttreenode.h"

/// Класс узла, принадлежащего глобальному персонажу
class GlobalMovingObjectTreeNode :
	public MovingObjectTreeNode
{
public:
	GlobalMovingObjectTreeNode(qdNamedObject* object);
	~GlobalMovingObjectTreeNode(void);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
	bool showProperties(TreeLogic& logic, int iActive);

	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	virtual bool canDrag(TreeLogic& logic);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// Удаляет копии глобального объекта, у которых нет состояний
	/// Возвращает true, если newName уникально
	bool removeEmptyCopyOfGlobal(TreeLogic& logic, qdGameObject* pobj);
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
	//! Переименовывает копии глобального перса в сценах
	void renameCopies(TreeLogic& logic, LPCTSTR oldName, LPCTSTR newName);
};
