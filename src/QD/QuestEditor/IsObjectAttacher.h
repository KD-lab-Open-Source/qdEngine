#pragma once

#include <list>
typedef std::list<qdNamedObject const*> NamedObjectList;

class qdGameDispatcher;
class qdNamedObject;
class qdGameScene;

/// Проверяет является ли объект аттачером.
class IsObjectAttacher
{

	/// пробегает по персонажам сцены и проверяет не является ли объект аттачером
	/**
		@param scene - сцена, по персонажам которой пробегаем
		@param object - тестируемый объект
		@param lst	- список, в который складываются объекты, для которых object 
					является аттачером
	 */
	static bool test(qdGameScene const*scene, 
				qdNamedObject const* object, 
				NamedObjectList& lst);
public:
	/// Пробегает по всем сценам и для каждой кликает test
	static bool run(qdGameDispatcher const& gameDisp,
					qdNamedObject const* object, NamedObjectList& lst);
};