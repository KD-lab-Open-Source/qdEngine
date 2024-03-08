#pragma once
#include <list>
class qdGameDispatcher;
class qdGameScene;
class qdNamedObject;

typedef std::list<qdNamedObject const*> NamedObjectList;

///	�������, ����������� ����, ��� ��������� ������ ������������ � ��������
class IsObjectInCondition
{
	IsObjectInCondition(void);
	~IsObjectInCondition(void);
	//! ���������� �����, ������� ������ �����������
	static qdGameScene const* getObjectScene(qdNamedObject const* obj);
	//! ���������, ����� �� ������ ���� � �������
	static bool canBeInCondition(qdNamedObject const* obj);
	//! ������ �������� ��� ���� ���� � �������
	static bool testInProject(qdGameDispatcher const* disp,
							  qdGameScene const* gameSceneExclude,
							  qdNamedObject const*obj, NamedObjectList& lst);
	template<class InputIterator, class Test>
	static bool test(InputIterator begin, InputIterator end, Test const& test);
	template<class Container>
	static bool testConditionalObjects(Container const& lst, 
				qdNamedObject const* obj, NamedObjectList& objLst);
	//! ������ �������� ��� �����
	class TestInScene;
	//! ������ �������� � �������
	class TestInAnimatedObject;
	//! �������� � ���� �� �����
	class TestInGridZone;
	//! ������ �������� � ���������
	class TestInConditionalObj;
public:
	/// ������� �������
	static bool run(qdGameDispatcher const* pdisp, qdNamedObject const* obj, 
					NamedObjectList& lst);
};
