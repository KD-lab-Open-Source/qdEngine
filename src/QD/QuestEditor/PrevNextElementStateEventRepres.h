#pragma once
#include "interfaceeventrepresbase.h"

/**
	Включить предыдущее/следующее состояние кнопки
 */
class PrevNextElementStateEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	PrevNextElementStateEventRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual ~PrevNextElementStateEventRepres(void);
	/// Возвращает имя первой попавшейся кнопки из выбранного экрана
	virtual LPCTSTR getDefaultValue() const;
	/// Загружает список имен кнопок из выбранного экрана
	virtual void loadData(CWnd* pwnd) const;
};
