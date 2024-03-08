#pragma once

#include <map>
#include <list>

typedef std::list<qdNamedObject const*> NamedObjectList;
class qdNamedObject;


class FilterObjectDependencies
{
public:

	enum eDependenciesBound{
		ACROSS_OBJECT, ///< зависимости подобъектов выходят за пределы объекта
		///< Пр.:(состояния - подобъекты, сцена объект)
		IN_OBJECT///< зависимости подобъектов не выходят за пределы
	};

	FilterObjectDependencies(void);
	~FilterObjectDependencies(void);

	static bool run(qdNamedObject const* object,
					std::map<qdNamedObject const*, 
								NamedObjectList>* ptrObjectsDependencies);
};
