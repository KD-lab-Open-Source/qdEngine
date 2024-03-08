#pragma once
#include "interfaceeventrepresbase.h"

/**
	Событие "Включить определенное состояние кнопки"
	Вроде бы на момент моего ухода это было не сделано в движке
 */
class ButtonStateEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ButtonStateEventRepres(void);
	virtual ~ButtonStateEventRepres(void);
	/// Возвращае NULL
	virtual LPCTSTR getDefaultValue() const;
	/// ничего не делает
	virtual void loadData(CWnd* pwnd) const;
};
