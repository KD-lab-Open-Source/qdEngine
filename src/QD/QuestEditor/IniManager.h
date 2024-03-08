#pragma once
#include "iprojectsettings.h"

#include <map>

class save2ini;

/// ����� ��������� IProjectSettings � ����� ������/������ �/�� ��� ����(�)
class IniManager :public IProjectSettings
{
	friend class save2ini;
	/// ������ ����������. 
	/**
		���� ��� ������.
		�������� ���� ����� - ��� ���������:��������
	 */
	typedef std::multimap<CString, std::pair<CString, CString> > Settings_t;
	/// ������� ��� ��������� �������� ������ � ������ ���������
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

	/// ������������� ��� ���-�����
	void SetFileName(const CString& file_name);
	/// ���������� ��� ���-�����
	CString getFileName() const;

	/// ���������� ������������� �������� ���������
	int GetIntSetting(const CString& strSection, const CString& strSettingName, int def_val);
	/// ���������� ��������� �������� ���������
	const CString GetStrSetting(const CString& strSection, 
		const CString& strSettingName, const CString& def_val);
	/// ���������� �������������� �������� ���������
	float GetFloatSetting(const CString& strSection, const CString& strSettingName, float def_val);

	/// ���������� ������������� �������� ���������
	bool WriteIntSetting(const CString& strSection, const CString& strSettingName, int Val);
	/// ���������� ��������� �������� ���������
	bool WriteStrSetting(const CString& strSection, const CString& strSettingName, const CString& Val);
	/// ���������� �������������� �������� ���������
	bool WriteFloatSetting(const CString& strSection, const CString& strSettingName, float Val);

	/// ������� �������� ���������� ����������� � ������ ������
	void ClearSection(const CString& section);
	/// ��������� ��� � ���-����
	void Save();
protected:
	/// � ������ ������� �������� � ��������� ������
	Settings_t::iterator find_setting(const CString& section, const CString& setting_name);

	/// ��������� ���
	/**
		�������� ������ ���� ������. ����� ������ ������ � �����������
	 */
	void load_ini();
	/// ��������� ������ �� ��� �����
	/**
		�������� ������ �������� � ��������� ��
	 */
	void load_section(LPCTSTR section);

	/// ��������� �������� �� ���-�����
	void load_value(const CString& section, LPCTSTR value);

	/// ��������� �������� � ����� ������
	Settings_t::iterator  insert_value(const CString& strSec, 
			const CString& strKey,const CString& strVal);
private:
	/// ������ ����������
	Settings_t settings_;
	/// ��� ���-�����
	CString file_name_;
};
