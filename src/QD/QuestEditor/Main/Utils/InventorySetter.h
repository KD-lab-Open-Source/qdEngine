#pragma once

/// �������. ��������� ���������� ������� ���������
class InventorySetter
{
public:
	InventorySetter();
	InventorySetter(LPCTSTR name);
	void operator()(qdGameObject* object);
private:
	/// ��� ���������
	LPCTSTR name_;
};
