/********************************************************************
	created:	2003/01/27
	created:	27:1:2003   18:46
	filename: 	iprojectsettings.h
	file path:	d:\projects\quest\questeditor
	file base:	iprojectsettings
	file ext:	h
	author:		�����
	
	purpose:	���������� � ���������� �������
*********************************************************************/
#pragma once

interface IProjectSettings
{
	/// ���������� ������������� �������� ���������
	virtual int GetIntSetting(const CString& strSection, 
							const CString& strSettingName, int def_val) = 0;
	/// ���������� ��������� �������� ���������
	virtual const CString GetStrSetting(const CString& strSection, 
							const CString& strSettingName, const CString& def_val) = 0;
	/// ���������� �������������� �������� ���������
	virtual float GetFloatSetting(const CString& strSection, 
							const CString& strSettingName, float def_val) = 0;
	/// ���������� ������������� �������� ���������
	virtual bool WriteIntSetting(const CString& strSection, 
							const CString& strSettingName, int Val) = 0;
	/// ���������� ��������� �������� ���������
	virtual bool WriteStrSetting(const CString& strSection, 
							const CString& strSettingName, const CString& Val) = 0;
	/// ���������� �������������� �������� ���������
	virtual bool WriteFloatSetting(const CString& strSection, 
							const CString& strSettingName, float Val) = 0;
	/// ������� �������� ���������� ����������� � ������ ������
	virtual void ClearSection(const CString& section) = 0;
	/// ��������� ���������
	virtual void Save() = 0;
};