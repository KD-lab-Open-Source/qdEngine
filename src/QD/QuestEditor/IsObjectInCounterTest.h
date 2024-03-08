#pragma once

class qdCounter;
class qdNamedObject;

/// Функтор. Проверяет используется ли объект в счетчике
class IsObjectInCounterTest
{
public:
	IsObjectInCounterTest(qdNamedObject const* testObject);
	~IsObjectInCounterTest(void);
	/// Ищет среди элементов счетчика obj
	static bool run(qdCounter const* counter, qdNamedObject const* obj);
	/// Вызывает run
	bool operator() (qdCounter const* counter) const;
private:
	/// Тестируемый объект
	qdNamedObject const* const testObject_;
};
