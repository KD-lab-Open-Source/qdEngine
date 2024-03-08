#pragma once
#include "interfaceeventrepresbase.h"

/**
	Представляет событие "Запустить видео"
 */
class PlayVideoEventRepres :
	public InterfaceEventWithDataRepres
{
public:
	PlayVideoEventRepres(void);
	virtual ~PlayVideoEventRepres(void);
	/// Имя первого папавшегося видео
	virtual LPCTSTR getDefaultValue() const;
	/// Загружает список имен видео
	virtual void loadData(CWnd* pwnd) const;
};
