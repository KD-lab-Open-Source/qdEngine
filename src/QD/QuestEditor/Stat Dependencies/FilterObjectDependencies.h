#pragma once

#include <map>
#include <list>

typedef std::list<qdNamedObject const*> NamedObjectList;
class qdNamedObject;


class FilterObjectDependencies
{
public:

	enum eDependenciesBound{
		ACROSS_OBJECT, ///< ����������� ����������� ������� �� ������� �������
		///< ��.:(��������� - ����������, ����� ������)
		IN_OBJECT///< ����������� ����������� �� ������� �� �������
	};

	FilterObjectDependencies(void);
	~FilterObjectDependencies(void);

	static bool run(qdNamedObject const* object,
					std::map<qdNamedObject const*, 
								NamedObjectList>* ptrObjectsDependencies);
};
