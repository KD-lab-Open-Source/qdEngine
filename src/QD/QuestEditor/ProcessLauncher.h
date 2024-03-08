#pragma once

/// \brief ��������������� �������, ������� �������� ��������� ��� ������� � ���������
///	��� �� ��
class ProcessLauncher
{
public:
	ProcessLauncher(void);
	~ProcessLauncher(void);

	/// ������� ������� �� ����, ��������� ������ � ������
	bool createProcess(LPCTSTR appPath, LPTSTR commandLine, DWORD startupFlags = 0);
	/// ��������� ��� ��
	bool isStillAlive();
protected:
	/// ��������� ������ �� �������. ������ � �������� � ������� ���������
	void close();
private:
	/// ��������� �������� ��������
	PROCESS_INFORMATION pi_;
};
