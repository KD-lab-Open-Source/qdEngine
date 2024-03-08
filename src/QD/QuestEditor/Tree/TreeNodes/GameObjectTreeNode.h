#pragma once
#include "NamedObjectTreeNode.h"


class GameObjectTreeNode : public NamedObjectTreeNode
{
public:
	GameObjectTreeNode(qdNamedObject* object);
	~GameObjectTreeNode(void);
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
	// при его удаление(сотояния, когда удаляется анимированный объект. 
	//Все объекты, когда удаляется сцена)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic);
protected:
	//! Удаляет объект из сцены и высвобождает память занимаемую объектом
	virtual bool removeObject(TreeLogic& logic);
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
