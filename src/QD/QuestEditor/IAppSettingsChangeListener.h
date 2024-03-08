#pragma once

/**
	Интерфейс, который класс должен реализовать, чтобы получать 
	сообщения об изменении настроек приложения.
	В SettingsChanged запрашиваются параметры. Затем применяются
 */
interface IAppSettingsChangeListener
{
	virtual void SettingsChanged() = 0;
};