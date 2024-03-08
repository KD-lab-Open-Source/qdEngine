#pragma once

class qdCondition;
class qdNamedObject;

/// �������. ��������� ������������ �� ������ � ���������
class IsObjectInConditionTest
{
	IsObjectInConditionTest(qdNamedObject const* obj);
	~IsObjectInConditionTest();
	/// ���������� ����� �� ������� ��������� ������
	static bool canConditionContainObject(qdCondition const* condition);
	/// �� ���� ������� ���������� ����� ������� ������������ ��������� � ���������
	static bool generalTest(qdCondition const* condition, qdNamedObject const* obj);
	//! ��������� ������ OBJECT_NAME
	static bool test1(qdCondition* condition, qdNamedObject const* obj);
	//! ��������� OBJECT2_NAME � OBJECT2_NAME
	static bool test2(qdCondition* condition, qdNamedObject const* obj);
	//! ��������� OBJECT2_NAME, OBJECT2_NAME � 2
	static bool test3(qdCondition* condition, qdNamedObject const* obj);
public:
	/// ������� run
	bool operator()(qdCondition const* cond) const;
	/// ������� generalTest
	static bool run(qdCondition const* cond, qdNamedObject const* obj);
private:
	/// ����������� ������
	qdNamedObject const* obj_;
};
