#pragma once
#include "InterfaceEventRepresBase.h"

/**
	Событие "Сменить интерфейсный экран"
 */
class ShowInterfaceScreenAsModalEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	ShowInterfaceScreenAsModalEventRepres(void);
	virtual ~ShowInterfaceScreenAsModalEventRepres(void);
	/// Имя первого попавшегося экрана
	virtual LPCTSTR getDefaultValue() const;
	/// Список имен экранов
	virtual void loadData(CWnd* pwnd) const;
};
