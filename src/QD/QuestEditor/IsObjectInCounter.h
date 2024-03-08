#pragma once

#include <list>

class qdNamedObject;
class qdGameDispatcher;

typedef std::list<qdNamedObject const*> NamedObjectList;

/// ���������� ������������ �� ������ � �����-���� ��������
class IsObjectInCounter
{
	IsObjectInCounter(void);
	~IsObjectInCounter(void);
public:
	///  \brief ��������� �� ���� ��������� � ���� � ��� obj.
	/// ���� �����, �� ��������� ������� � objectList
	static bool run(qdGameDispatcher const* disp, 
					qdNamedObject const* obj, 
					NamedObjectList& objectList);
};
