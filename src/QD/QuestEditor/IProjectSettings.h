/********************************************************************
	created:	2003/01/27
	created:	27:1:2003   18:46
	filename: 	iprojectsettings.h
	file path:	d:\projects\quest\questeditor
	file base:	iprojectsettings
	file ext:	h
	author:		Илюха
	
	purpose:	интерфаейс к настройкам проекта
*********************************************************************/
#pragma once

interface IProjectSettings
{
	/// Возвращает целочисленное значение параметра
	virtual int GetIntSetting(const CString& strSection, 
							const CString& strSettingName, int def_val) = 0;
	/// Возвращает строковое значение параметра
	virtual const CString GetStrSetting(const CString& strSection, 
							const CString& strSettingName, const CString& def_val) = 0;
	/// Возвращает действительное значение параметра
	virtual float GetFloatSetting(const CString& strSection, 
							const CString& strSettingName, float def_val) = 0;
	/// Записывает целочисленное значение параметра
	virtual bool WriteIntSetting(const CString& strSection, 
							const CString& strSettingName, int Val) = 0;
	/// Записывает строковое значение параметра
	virtual bool WriteStrSetting(const CString& strSection, 
							const CString& strSettingName, const CString& Val) = 0;
	/// Записывает действительное значение параметра
	virtual bool WriteFloatSetting(const CString& strSection, 
							const CString& strSettingName, float Val) = 0;
	/// Удаляет значения параметров относящиеся к данной секции
	virtual void ClearSection(const CString& section) = 0;
	/// Сохраняет параметры
	virtual void Save() = 0;
};