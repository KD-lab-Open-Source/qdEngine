#pragma once
#include "interfaceeventrepresbase.h"
/**
	Представляет событие "Активация персонажа"
 */
class ActivatePersonageEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ActivatePersonageEventRepres(void);
	virtual ~ActivatePersonageEventRepres(void);
	/// Находит первое попавшегося перса и возвращает его имя
	virtual LPCTSTR getDefaultValue() const;
	/// Загружает уникальный список имен персонажей. Собирает имена со всех сцен проекта
	virtual void loadData(CWnd*pwnd) const;
};
