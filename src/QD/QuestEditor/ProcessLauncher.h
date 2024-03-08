#pragma once

/// \brief Вспомогательный классик, который помогает запустить нам процесс и проверить
///	жив ли он
class ProcessLauncher
{
public:
	ProcessLauncher(void);
	~ProcessLauncher(void);

	/// Создает процесс по пути, командной строке и флагам
	bool createProcess(LPCTSTR appPath, LPTSTR commandLine, DWORD startupFlags = 0);
	/// Проверяет жив ли
	bool isStillAlive();
protected:
	/// Закрывает хендлы на процесс. Вместе с хендлами и процесс закроется
	void close();
private:
	/// Собствнно описател процесса
	PROCESS_INFORMATION pi_;
};
