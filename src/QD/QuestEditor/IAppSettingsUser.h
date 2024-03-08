#pragma once

class AppSettingsBasePage;

/**
	 ласс, который хочет добавить свою страничку в диалог настроек приложени€,
	по идее, должен реализовать этот интерефейс.
 */
interface IAppSettingsUser
{
	/// ¬озвращает указатель на станичку, которую надо вставить
	virtual boost::shared_ptr<AppSettingsBasePage> getPage() = 0;
};
