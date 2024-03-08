#pragma once

class qdCondition;
class qdNamedObject;

/// Функтор. Проверяет используется ли объект в состоянии
class IsObjectInConditionTest
{
	IsObjectInConditionTest(qdNamedObject const* obj);
	~IsObjectInConditionTest();
	/// Возвращает может ли условие содержать объект
	static bool canConditionContainObject(qdCondition const* condition);
	/// По типу условия определяет какую функцию тестирования применить и применяет
	static bool generalTest(qdCondition const* condition, qdNamedObject const* obj);
	//! проверяет только OBJECT_NAME
	static bool test1(qdCondition* condition, qdNamedObject const* obj);
	//! проверяет OBJECT2_NAME и OBJECT2_NAME
	static bool test2(qdCondition* condition, qdNamedObject const* obj);
	//! проверяет OBJECT2_NAME, OBJECT2_NAME и 2
	static bool test3(qdCondition* condition, qdNamedObject const* obj);
public:
	/// Кликает run
	bool operator()(qdCondition const* cond) const;
	/// Кликает generalTest
	static bool run(qdCondition const* cond, qdNamedObject const* obj);
private:
	/// Тестируемый объект
	qdNamedObject const* obj_;
};
