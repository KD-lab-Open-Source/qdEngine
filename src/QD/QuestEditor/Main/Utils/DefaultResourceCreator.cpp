#include "stdafx.h"
#include ".\defaultresourcecreator.h"
#include "resource.h"
#include "RelativePath.h"
namespace
{
	/// Создает структуру директорий по имени файла
	bool CreateDirStruct(CString file_name)
	{
		CStringArray ar;
		r_p::DecompositePath(file_name,ar);
		int size = ar.GetSize();
		CString dir;
		for(int i = 0; i < size; ++i)
		{
			dir += ar.GetAt(i);
			if(!CreateDirectory(dir, NULL))
			{
				if(ERROR_FILE_EXISTS != GetLastError()&&
					ERROR_ALREADY_EXISTS != GetLastError())
					return false;
			}
			dir += "\\";
		}
		return true;
	}

	/// Достает ресурс из ресурсов exe-шника и сохраняет их в файл
	bool SaveResource(CString strFileName, DWORD rid, LPCTSTR strType)
	{
		HRSRC hrsc0	= FindResource(NULL,MAKEINTRESOURCE(rid),strType);
		if (!hrsc0) 
			return false;
		DWORD buf_sz = SizeofResource(NULL,(HRSRC)hrsc0);
		if(!buf_sz)
			return false;
		HGLOBAL hrsc1 = LoadResource(NULL,(HRSRC)hrsc0);
		if(!hrsc1)
			return false;

		char* buf = (char*)LockResource(hrsc1);	
		if(!buf)
			return false;

		if(!CreateDirStruct(strFileName))
			return false;

		HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL, 
			CREATE_NEW, FILE_FLAG_WRITE_THROUGH, NULL); 
		if(hFile == INVALID_HANDLE_VALUE)
		{
			//если файл с таким именем уже есть, считаем что он правильный
			if(ERROR_FILE_EXISTS == GetLastError()||
				ERROR_ALREADY_EXISTS == GetLastError())
				return true;

			return false;
		}
		DWORD nWritten = 0;
		BOOL res = WriteFile(hFile, buf, buf_sz, &nWritten, NULL);
		res = (buf_sz == nWritten);

		CloseHandle(hFile);
		return static_cast<bool>(res);
	}
}

DefaultResourceCreator::DefaultResourceCreator(void)
{
}

DefaultResourceCreator::~DefaultResourceCreator(void)
{
}

bool DefaultResourceCreator::create()
{
	CString file_name;
	file_name.LoadString(IDS_DEF_CURSOR_FILE_NAME);
	if(!SaveResource(file_name, IDR_DEFAULT_CURSOR, "TGA"))
	{
		if(ERROR_ALREADY_EXISTS != GetLastError())
			return false;
	}
	file_name.LoadString(IDS_DEF_FONT_FILE_NAME);
	if(!SaveResource(file_name, IDR_DEFAULT_FONT, "TGA"))
	{
	}
	file_name.LoadString(IDS_DEF_FONT_IDX_FILE_NAME);
	if(!SaveResource(file_name, IDR_DEFAULT_FONT_IDX, "IDX"))
	{
		if(ERROR_ALREADY_EXISTS != GetLastError())
			return false;
	}
	return true;

}