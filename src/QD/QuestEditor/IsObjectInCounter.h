#pragma once

#include <list>

class qdNamedObject;
class qdGameDispatcher;

typedef std::list<qdNamedObject const*> NamedObjectList;

/// Определяет используется ли объект в каком-либо счетчике
class IsObjectInCounter
{
	IsObjectInCounter(void);
	~IsObjectInCounter(void);
public:
	///  \brief Пробегает по всем счетчикам и ищет в них obj.
	/// Если нашла, то добавляет счетчик в objectList
	static bool run(qdGameDispatcher const* disp, 
					qdNamedObject const* obj, 
					NamedObjectList& objectList);
};
