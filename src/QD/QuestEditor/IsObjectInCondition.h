#pragma once
#include <list>
class qdGameDispatcher;
class qdGameScene;
class qdNamedObject;

typedef std::list<qdNamedObject const*> NamedObjectList;

///	Утилита, проверяющая факт, что некоторый объект используется в условиях
class IsObjectInCondition
{
	IsObjectInCondition(void);
	~IsObjectInCondition(void);
	//! возвращает сцену, которой объект принадлежит
	static qdGameScene const* getObjectScene(qdNamedObject const* obj);
	//! Проверяет, может ли объект быть в условии
	static bool canBeInCondition(qdNamedObject const* obj);
	//! Делает проверку для всех сцен в проекте
	static bool testInProject(qdGameDispatcher const* disp,
							  qdGameScene const* gameSceneExclude,
							  qdNamedObject const*obj, NamedObjectList& lst);
	template<class InputIterator, class Test>
	static bool test(InputIterator begin, InputIterator end, Test const& test);
	template<class Container>
	static bool testConditionalObjects(Container const& lst, 
				qdNamedObject const* obj, NamedObjectList& objLst);
	//! Делает проверку для сцены
	class TestInScene;
	//! Делает проверку в объекте
	class TestInAnimatedObject;
	//! Проверка в зоне на сетке
	class TestInGridZone;
	//! Делает проверку в состоянии
	class TestInConditionalObj;
public:
	/// Входная функция
	static bool run(qdGameDispatcher const* pdisp, qdNamedObject const* obj, 
					NamedObjectList& lst);
};
