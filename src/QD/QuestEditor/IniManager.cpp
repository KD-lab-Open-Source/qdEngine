#include "stdafx.h"
#include "inimanager.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
int get_int(const CString& str){
	return _tstoi(str);
}

void set_int(CString* str, int val){
	str->Format(_T("%d"), val);
}

float get_float(const CString& str){
	return _tstof(str);
}

void set_float(CString* str, float val){
	str->Format(_T("%.3f"), val);
}

/// Сохраняет пару ключ значение в ини файл
class save2ini
{
	const CString& file_;
public:
	save2ini(const CString& file):file_(file){}
	void operator()(IniManager::Settings_t::value_type& p) const{
		WritePrivateProfileString(p.first, p.second.first, p.second.second, file_);
	}
};

IniManager::IniManager(void)
{
}

IniManager::~IniManager(void)
{
}

void IniManager::SetFileName(const CString& file_name)
{
	file_name_ = file_name;
	load_ini();
}

CString IniManager::getFileName() const
{
	return file_name_;
}

int IniManager::GetIntSetting(const CString& strSection, 
											const CString& strSettingName, int def_val)
{
	Settings_t::iterator itr = find_setting(strSection, strSettingName);
	if (itr == settings_.end())
		return def_val;
	return get_int(itr->second.second);
}

const CString IniManager::GetStrSetting(const CString& strSection, 
								const CString& strSettingName, const CString& def_val)
{
	Settings_t::iterator itr = find_setting(strSection, strSettingName);
	if (itr == settings_.end())
		return def_val;
	return itr->second.second;
}

float IniManager::GetFloatSetting(const CString& strSection, 
									const CString& strSettingName, float def_val)
{
	Settings_t::iterator itr = find_setting(strSection, strSettingName);
	if (itr == settings_.end())
		return def_val;
	return get_float(itr->second.second);
}

bool IniManager::WriteIntSetting(const CString& strSection, 
											const CString& strSettingName, int Val)
{
	Settings_t::iterator itr = find_setting(strSection, strSettingName);
	if (itr == settings_.end())
	{
		itr = insert_value(strSection, strSettingName, CString());
		if (itr == settings_.end())
			return false;
	}
	set_int(&itr->second.second, Val);
	return true;
}

bool IniManager::WriteStrSetting(const CString& strSection, 
											const CString& strSettingName, const CString& Val)
{
	Settings_t::iterator itr = find_setting(strSection, strSettingName);
	if (itr == settings_.end())
	{
		itr = insert_value(strSection, strSettingName, CString());
		if (itr == settings_.end())
			return false;
	}
	itr->second.second = Val;
	return true;
}

bool IniManager::WriteFloatSetting(const CString& strSection, 
											const CString& strSettingName, float Val)
{
	Settings_t::iterator itr = find_setting(strSection, strSettingName);
	if (itr == settings_.end())
	{
		itr = insert_value(strSection, strSettingName, CString());
		if (itr == settings_.end())
			return false;
	}
	set_float(&itr->second.second, Val);
	return true;
}

IniManager::Settings_t::iterator IniManager::find_setting(const CString& section, 
											const CString& setting_name)
{
	std::pair<Settings_t::iterator,Settings_t::iterator> p = 
		settings_.equal_range(section);
	Settings_t::iterator res = 
		std::find_if(p.first, p.second, eql_setting_name(setting_name));
	if (res != p.second)
		return res;
	return settings_.end();
}

void IniManager::load_ini()
{
	settings_.clear();

	const int INIT_SIZE = 1024;
	CString buff;
	int size = 1024;
	int ret_chars = 0;
	while ((ret_chars = GetPrivateProfileSectionNames(buff.
		GetBufferSetLength(size), size, file_name_)) == (size - 2)) 
		size += 128;

	buff.ReleaseBuffer(ret_chars);
	LPCTSTR sections = buff;
	while (int len = _tcslen(sections)) {
		load_section(sections);
		sections += len+1;
	}
}

void IniManager::load_section(LPCTSTR section)
{
	const int INIT_SIZE = 1024;
	CString buff;
	int size = 1024;
	int ret_chars = 0;
	while ((ret_chars = GetPrivateProfileSection(section,
		buff.GetBufferSetLength(size), size, file_name_)) == (size - 2)) 
		size += 128;
	buff.ReleaseBuffer(ret_chars);
	LPCTSTR value = buff;
	CString strSection(section);
	while (int len = _tcslen(value)) {
		load_value(strSection, value);
		value += len+1;
	}
}

void IniManager::load_value(const CString& section, LPCTSTR value)
{
	LPCTSTR pos = _tcschr(value, _T('='));
	if (pos) 
	{
		CString strKey(value, pos - value);
		CString strVal(pos + 1);
		strKey.Trim();
		strVal.Trim();
		insert_value(section, strKey, strVal);
	}
}

IniManager::Settings_t::iterator IniManager::insert_value(const CString& strSec, 
										const CString& strKey, const CString& strVal)
{
	return settings_.insert(std::make_pair(strSec, std::make_pair(strKey, strVal)));
}

void IniManager::ClearSection(const CString& section)
{
	std::pair<Settings_t::iterator,Settings_t::iterator> p = 
											settings_.equal_range(section);
	settings_.erase(p.first, p.second);
}

void IniManager::Save()
{
	DeleteFile(file_name_);
	std::for_each(settings_.begin(), settings_.end(), save2ini(file_name_));
}
