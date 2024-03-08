#pragma once

class AppSettingsBasePage;

/**
	�����, ������� ����� �������� ���� ��������� � ������ �������� ����������,
	�� ����, ������ ����������� ���� ����������.
 */
interface IAppSettingsUser
{
	/// ���������� ��������� �� ��������, ������� ���� ��������
	virtual boost::shared_ptr<AppSettingsBasePage> getPage() = 0;
};
