#pragma once

/**
	��������� ������������� ����������� �������� � ������ 
	��� ��������� ��� ����� ������ 
 */
interface INameSetter
{
	virtual const CString& get_string() const = 0;
	virtual void set_string(const CString& str) = 0;
};

/**
	��������� �������� � ������, ������������ ������ ���������, ���������� ������
	���������� �� ������ �������
 */
interface INameTester
{
	virtual const CString get_unique_name(const CString& test) const = 0;
};