#pragma once
/// ������� ������������� ���������
/*!
��������� �� ���� �������� �� ���� ������ � ��������� ������������ �� � ���
���������. ���� ��, �� ������ �������� � ������. 
*/
class EnumInventoryUsers
{
	EnumInventoryUsers(void);
	~EnumInventoryUsers(void);
public:
	static void enumerate(qdGameDispatcher const* gameDisp, 
		qdInventory const* pInv, 
		qdGameObjectList* plist4Remove);
};
