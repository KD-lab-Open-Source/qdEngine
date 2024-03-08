#pragma once
#include "treenodestube.h"

class qdInterfaceObjectBase;

/// Базовый класс для узлов, соответствующих интерфейсным объектам
/**
	Содережит объект соответствующий узлу и минимальную базовую 
	функциональность по удалению объекта
 */
class InterfaceObjectTreeNode : public TreeNodeStube
{
public:
	InterfaceObjectTreeNode(qdInterfaceObjectBase* object);
	~InterfaceObjectTreeNode(void);
	virtual ContentClassificator getContentClassificator() const;
	virtual void onDeleteItem(TreeLogic& logic);
	NodeContent getContent() const;
protected:
	/// Возвращает соответствующий узлу объект
	qdInterfaceObjectBase* getObject() const;
	/// Назначает объект, соответствующий узлу
	void setObject(qdInterfaceObjectBase* obj);
	/// Спрашивает у пользователя, можно ли удалить объект
	bool preDelete(TreeLogic& logic, bool ask);
private:
	/// Интерфейсный объект, соответствующий узлу
	qdInterfaceObjectBase* object_;
};
