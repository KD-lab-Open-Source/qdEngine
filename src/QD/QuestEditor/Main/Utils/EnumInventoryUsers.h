#pragma once
/// Находит пользователей инвентори
/*!
Пробегает по всем объектам во всех сценах и проверяет используется ли в них
инвентори. Если да, то объект кладется в список. 
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
