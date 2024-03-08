#pragma once

class qdTriggerProfiler;
/**
	Данные из профайлера записывает в файл
 */
class ProfilerDataToFile
{
	ProfilerDataToFile(void);
	~ProfilerDataToFile(void);
public:
	/**
		Запрашивает имя файла.
		Записывает туда данные
	 */
	static bool run(qdTriggerProfiler const& profiler);
};
