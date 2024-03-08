#pragma once
#include "interfaceeventrepresbase.h"

/**
	Представляет событие "Показать/скрыть элемент"
 */
class HideShowElementEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	HideShowElementEventRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual ~HideShowElementEventRepres(void);
	/// Имя первого попавшегося элемента в выбранном экране
	virtual LPCTSTR getDefaultValue() const;
	/// Список элементов из выбранного экрана
	virtual void loadData(CWnd* pwnd) const;
};
