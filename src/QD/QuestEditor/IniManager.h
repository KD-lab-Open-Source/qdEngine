#pragma once
#include "iprojectsettings.h"

#include <map>

class save2ini;

/// Класс реализует IProjectSettings в плане записи/чтения в/из ини файл(а)
class IniManager :public IProjectSettings
{
	friend class save2ini;
	/// Индекс параметров. 
	/**
		Ключ имя секции.
		Значение пара строк - имя параметра:значение
	 */
	typedef std::multimap<CString, std::pair<CString, CString> > Settings_t;
	/// Функтор для сравнения заданной строки с именем параметра
	class eql_setting_name
	{
		const CString& setting_name_;
	public:
		eql_setting_name(const CString& setting_name):setting_name_(setting_name){}
		bool operator()(const Settings_t::value_type& vt){
			return (vt.second.first == setting_name_);
		}
	};
public:
	IniManager(void);
	~IniManager(void);

	/// Устанавливает имя ини-файла
	void SetFileName(const CString& file_name);
	/// Возвращает имя ини-файла
	CString getFileName() const;

	/// Возвращает целочисленное значение параметра
	int GetIntSetting(const CString& strSection, const CString& strSettingName, int def_val);
	/// Возвращает строковое значение параметра
	const CString GetStrSetting(const CString& strSection, 
		const CString& strSettingName, const CString& def_val);
	/// Возвращает действительное значение параметра
	float GetFloatSetting(const CString& strSection, const CString& strSettingName, float def_val);

	/// Записывает целочисленное значение параметра
	bool WriteIntSetting(const CString& strSection, const CString& strSettingName, int Val);
	/// Записывает строковое значение параметра
	bool WriteStrSetting(const CString& strSection, const CString& strSettingName, const CString& Val);
	/// Записывает действительное значение параметра
	bool WriteFloatSetting(const CString& strSection, const CString& strSettingName, float Val);

	/// Удаляет значения параметров относящиеся к данной секции
	void ClearSection(const CString& section);
	/// Сохраняет все в ини-файл
	void Save();
protected:
	/// В секции находит параметр с указанным именем
	Settings_t::iterator find_setting(const CString& section, const CString& setting_name);

	/// Загружает ини
	/**
		Получает список имен секций. Затем грузит каждую в отдельности
	 */
	void load_ini();
	/// Загружает секцию из ини файла
	/**
		Получает список значений и загружает их
	 */
	void load_section(LPCTSTR section);

	/// Загружает значение из ини-файла
	void load_value(const CString& section, LPCTSTR value);

	/// Вставляет значение в общий список
	Settings_t::iterator  insert_value(const CString& strSec, 
			const CString& strKey,const CString& strVal);
private:
	/// Список параметров
	Settings_t settings_;
	/// Имя ини-файла
	CString file_name_;
};
