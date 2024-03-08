#pragma once

#include <list>
typedef std::list<qdNamedObject const*> NamedObjectList;

class qdGameDispatcher;
class qdGameObject;

/// Проверяет ялвяется ли объект относительным для какой-либо КА
/**
	Находи объекты, у которых КА имеет флаг QD_COORDS_ANM_RELATIVE_FLAG и
	object является стартовым объектом
 */ 
class IsObjectCARelativeObject
{
public:
	IsObjectCARelativeObject(void);
	~IsObjectCARelativeObject(void);
	/// Пробегает по списку сцен и вызывает для каждой сцены test
	static bool run(qdGameDispatcher const* gameDisp, 
		qdGameObject const* object, NamedObjectList& lst);
protected:
	///  \brief Пробегается по всем анимированным объектам и находит нужные объекты
	/**
		@param scene - сцена, по персонажам которой пробегаем
		@param object - тестируемый объект
		@param lst	- список, в который складываются объекты КА, которых 
					имеет флаг QD_COORDS_ANM_RELATIVE_FLAG и стартовый объект object
	*/
	static void test(qdGameScene const*scene, 
					qdGameObject const* object, 
					NamedObjectList& lst);
	/// проверяет имеет ли анимация флаг QD_COORDS_ANM_RELATIVE_FLAG
	static bool stateHasRelativeCA(qdGameObjectState const* state);
};
