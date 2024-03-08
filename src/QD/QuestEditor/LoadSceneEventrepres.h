#pragma once
#include "interfaceeventrepresbase.h"

/**
	Представляет событие "Загрузить сцену"
 */
class LoadSceneEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	LoadSceneEventRepres(UINT_PTR descr_id);
	virtual ~LoadSceneEventRepres(void);
	/// Имя первой попавшейся сцены
	virtual LPCTSTR getDefaultValue() const;
	/// Загружает список имен сцен
	virtual void loadData(CWnd* pwnd) const;
private:
	/**
		Функтор для загрузки сцен в комбобокс
	 */
	class AddToCombo;
};
