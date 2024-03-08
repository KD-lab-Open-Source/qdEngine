#pragma once

class qdGameDispatcher;
class qdGameObject;

#include "qd_fwd.h"

/// Находит все копии глобального объекта, удовлетворяющие функции проверки
class EnumCopyOfGlobal
{
public:
	EnumCopyOfGlobal(void);
	~EnumCopyOfGlobal(void);
	typedef bool (*CHECK_FUN)(qdGameObject const* p);
/*!
 	Пробегает по всем сценам. Ищет копию глобального объекта, указанного именем.
	Если для найденной копии функция-условие возвращает true, то она(копия) добавляется
	в список
	\param name	- имя глобального объекта
	\param f	- указатель на функцию-фильтр для объектов-копий
	\param pres	- указатель на список, куда будут сложены все копии прошедшие фильтрацию
*/
	static void enumerate(qdGameDispatcher const* gameDisp, 
							LPCTSTR name, 
							CHECK_FUN f, qdGameObjectList* pres);
};
