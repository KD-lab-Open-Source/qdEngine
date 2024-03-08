#include "stdafx.h"
#include "processlauncher.h"

ProcessLauncher::ProcessLauncher(void)
{
	memset(&pi_, 0, sizeof(pi_));
}

ProcessLauncher::~ProcessLauncher(void)
{
	close();
}

bool ProcessLauncher::createProcess(LPCTSTR appPath, 
									LPTSTR commandLine,
									DWORD startupFlags)
{
	close();
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = startupFlags;
	if (CreateProcess(appPath, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi_))
	{
		CloseHandle(pi_.hThread);
		return true;
	}
	return false;
}

bool ProcessLauncher::isStillAlive()
{
	if (!pi_.hProcess)
		return false;
	DWORD dwExitCode = 0;
	if (!GetExitCodeProcess(pi_.hProcess, &dwExitCode))
		return false;
	return (dwExitCode == STILL_ACTIVE);
}

void ProcessLauncher::close()
{
	if (!pi_.hProcess)
		return;
	CloseHandle(pi_.hProcess);
}
