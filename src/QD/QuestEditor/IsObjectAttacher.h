#pragma once

#include <list>
typedef std::list<qdNamedObject const*> NamedObjectList;

class qdGameDispatcher;
class qdNamedObject;
class qdGameScene;

/// ��������� �������� �� ������ ���������.
class IsObjectAttacher
{

	/// ��������� �� ���������� ����� � ��������� �� �������� �� ������ ���������
	/**
		@param scene - �����, �� ���������� ������� ���������
		@param object - ����������� ������
		@param lst	- ������, � ������� ������������ �������, ��� ������� object 
					�������� ���������
	 */
	static bool test(qdGameScene const*scene, 
				qdNamedObject const* object, 
				NamedObjectList& lst);
public:
	/// ��������� �� ���� ������ � ��� ������ ������� test
	static bool run(qdGameDispatcher const& gameDisp,
					qdNamedObject const* object, NamedObjectList& lst);
};