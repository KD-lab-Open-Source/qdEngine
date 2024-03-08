#pragma once
#include "treenodestube.h"
#include "Stat Dependencies/filterobjectdependencies.h"

class qdNamedObject;

class NamedObjectTreeNode : public TreeNodeStube
{
public:
	typedef std::map<qdNamedObject const*, NamedObjectList> DependsIndex;
	NamedObjectTreeNode(qdNamedObject* object);
	~NamedObjectTreeNode(void);

	qdNamedObject* getObject() const;
	virtual ContentClassificator getContentClassificator() const;
	virtual void onDeleteItem(TreeLogic& logic);
	virtual NodeContent getContent() const;
	//! выполняет действия необхдимые перед удалением объекта
	virtual bool preDelete(TreeLogic& logic, bool ask, bool statDepends);
protected:
	void setINTriggerFlag(TreeLogic& logic, eTriggerIconCommand value);

	//! Составляет список зависимостей объекта(вхождение в условия и базовый объект КА)
	bool statObjectDependencies(TreeLogic& logic, qdNamedObject const* object);
	//! Отбрасывает все ссылки на подъобъекты объекта(состояния для анимированных, 
	//! все объекты для сцены), не выходящие за пределы объекта
	FilterObjectDependencies::eDependenciesBound 
		filterDepedenciesBound(TreeLogic& logic, 
								qdNamedObject const* object,
								DependsIndex & statistic);
	void fillObjectList(TreeLogic& logic,
						DependsIndex const& statistic);

	void setObject(qdNamedObject* object);

	/*!
		Пробегает по всем цепочкам. Усли находит элемент, который содержит \a pno, удаляет
		его
	*/
	static void removeObjectFromTriggers(qdNamedObject const*, qdGameDispatcher* disp);

	virtual bool canRename(TreeLogic& logic);
private:
	qdNamedObject* object_;
};
