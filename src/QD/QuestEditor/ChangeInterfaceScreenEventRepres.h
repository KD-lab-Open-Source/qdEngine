#pragma once
#include "interfaceeventrepresbase.h"

/**
	Событие "Сменить интерфейсный экран"
 */
class ChangeInterfaceScreenEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ChangeInterfaceScreenEventRepres(void);
	virtual ~ChangeInterfaceScreenEventRepres(void);
	/// Имя первого попавшегося экрана
	virtual LPCTSTR getDefaultValue() const;
	/// Список имен экранов
	virtual void loadData(CWnd* pwnd) const;
};
