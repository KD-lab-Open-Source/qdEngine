#pragma once

/**
	���������, ������� ����� ������ �����������, ����� �������� 
	��������� �� ��������� �������� ����������.
	� SettingsChanged ������������� ���������. ����� �����������
 */
interface IAppSettingsChangeListener
{
	virtual void SettingsChanged() = 0;
};