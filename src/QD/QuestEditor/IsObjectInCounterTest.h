#pragma once

class qdCounter;
class qdNamedObject;

/// �������. ��������� ������������ �� ������ � ��������
class IsObjectInCounterTest
{
public:
	IsObjectInCounterTest(qdNamedObject const* testObject);
	~IsObjectInCounterTest(void);
	/// ���� ����� ��������� �������� obj
	static bool run(qdCounter const* counter, qdNamedObject const* obj);
	/// �������� run
	bool operator() (qdCounter const* counter) const;
private:
	/// ����������� ������
	qdNamedObject const* const testObject_;
};
