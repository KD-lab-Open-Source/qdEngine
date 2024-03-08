#include <my_stl.h>
#include <windows.h>

#include <vector>

#include "FileUtils.h"

#define xassert(a)

namespace file_utils {

typedef std::list<std::string> StringList;

void splitFileList(std::list<std::string>& file_list, const char* ptr, char delimeter)
{
	file_list.clear();

	if(!*ptr)
		return;

	const char* token_start = ptr;
	const char* token_end = ptr;

	for(;;) {
		if(*ptr != delimeter) {
			token_end = ptr+1;
			if(*token_end == '\0'){
				file_list.push_back(std::string(token_start, token_end));
				break;
			}
		}
		else {
			file_list.push_back(std::string(token_start, token_end));
			token_start = ptr+1;
			token_end = token_start;
		}
		++ptr;
	}
}

bool isFileExists(const char* fileName)
{
	DWORD desiredAccess = GENERIC_READ;
	DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD creationDeposition = OPEN_EXISTING;
	DWORD flagsAndAttributues = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS;
	HANDLE file = CreateFile(fileName, desiredAccess, shareMode, 0, creationDeposition, flagsAndAttributues, 0);
	if(file == INVALID_HANDLE_VALUE)
		return false;
	else{
		CloseHandle(file);
		return true; 
	}
}

std::string extractFileBase(const char* pathName)
{
    std::vector<char> nameBuf(_MAX_FNAME);

    _splitpath (pathName, 0, 0, &nameBuf[0], 0);
    std::string fileName (&nameBuf [0]);
    return fileName;
}

std::string extractFileExt(const char* pathName)
{
    std::vector<char> extBuf(_MAX_EXT);

    _splitpath (pathName, 0, 0, 0, &extBuf[0]);
    std::string fileExt (&extBuf [0]);
    return fileExt;
}

std::string extractFileName(const char* pathName)
{
    std::vector<char> nameBuf(_MAX_FNAME);
    std::vector<char> extBuf(_MAX_EXT);

    _splitpath (pathName, 0, 0, &nameBuf[0], &extBuf[0]);
    std::string fileName (&nameBuf [0]);
    std::string fileExt (&extBuf [0]);
    return (fileName + fileExt);
}

std::string extractFilePath(const char* pathName)
{
    std::vector<char> driveBuf(_MAX_FNAME);
    std::vector<char> pathBuf(_MAX_FNAME);
    _splitpath(pathName, &driveBuf[0], &pathBuf[0], 0, 0);
	return std::string(&driveBuf[0]) + std::string(&pathBuf[0]);

}

bool compareFileName(const char* lhs, const char* rhs)
{
	char buffer1[_MAX_PATH];
	char buffer2[_MAX_PATH];
	if(_fullpath(buffer1, lhs, _MAX_PATH) == 0)
		return stricmp(lhs, rhs) == 0;
	if(_fullpath(buffer2, rhs, _MAX_PATH) == 0)
		return stricmp(lhs, rhs) == 0;
	return stricmp(buffer1, buffer2) == 0;
}

std::string normalizePath(const char* patch)
{
	std::string result;
	if(!patch){
		xassert(patch);
		return std::string();
	}

	if(patch[0] == '.' && (patch[1] == '\\' || patch[1] == '/'))
		patch += 2;

	result.reserve(strlen(patch));
	for(const char* p = patch; *p ;p++){
		if(p == patch) // Для сетевых путей, типа \\CENTER\MODELS\...
		{
			result += *p;
			continue;
		}
		if(*p != '\\')
			result += *p;
		else{
			while(p[1] == '\\')
				++p;
			result += *p;
		}
	}
	char* loweredString = strlwr(strdup(result.c_str()));
	result = loweredString;
	free(loweredString);
	xassert(result.size() < _MAX_PATH);
	return result;
}

const char* localizePath(const char* path)
{
	static char cur_path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, cur_path);

	char full_path[MAX_PATH];
	_fullpath(full_path, cur_path, MAX_PATH);
	_fullpath(cur_path, path, MAX_PATH);

	strlwr(cur_path);
	strlwr(full_path);

	if(!strncmp(full_path, cur_path, strlen(full_path)))
		return cur_path + strlen(full_path) + 1;
	else
		return cur_path;
}

// --------------------------------------------------------------------------
DirIterator::DirIterator(const char* path)
{
	ZeroMemory(&findFileData_, sizeof(findFileData_));
	if(path)
		handle_ = FindFirstFile(path, &findFileData_);
	else
		handle_ = INVALID_HANDLE_VALUE;
}

DirIterator::~DirIterator()
{
	if(handle_ != INVALID_HANDLE_VALUE)
		FindClose (handle_);
}

DirIterator& DirIterator::operator++()
{
	xassert(handle_ && handle_ != INVALID_HANDLE_VALUE && "Incrementing bad DirIterator!");
	if (FindNextFile (handle_, &findFileData_) == false) {
		handle_ = INVALID_HANDLE_VALUE;
	}
	return *this;
}

const DirIterator DirIterator::operator++(int)
{
	DirIterator oldValue (*this);
	++(*this);
	return oldValue;
}

const char* DirIterator::c_str() const
{
	xassert (handle_ != INVALID_HANDLE_VALUE && "Dereferencing bad DirIterator!");
	return findFileData_.cFileName;
}

bool DirIterator::isDirectory() const
{
	return (findFileData_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool DirIterator::operator==(const DirIterator& rhs)
{
	return handle_ == rhs.handle_;
}

bool DirIterator::operator!=(const DirIterator& rhs)
{
	return handle_ != rhs.handle_;
}

bool DirIterator::isFile() const
{
	return !(findFileData_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool getFileList(const char* path, const char* file_name_mask, std::list<std::string>& list)
{
	DirIterator end;

	StringList path_list;
	splitFileList(path_list, path, ';');

	StringList mask_list;
	splitFileList(mask_list, file_name_mask, ';');

	StringList::const_iterator path_it;
	for(path_it = path_list.begin(); path_it != path_list.end(); ++path_it){
		for(StringList::const_iterator mask_it = mask_list.begin(); mask_it != mask_list.end(); ++mask_it){
			DirIterator dir_it((*path_it + "\\" + *mask_it).c_str());
			while(dir_it != end){
				if(dir_it.isFile())
					list.push_back(*path_it + "\\" + dir_it.c_str());
				++dir_it;
			}
		}
	}

	for(path_it = path_list.begin(); path_it != path_list.end(); ++path_it){
		DirIterator dir_it((*path_it + "\\*.*").c_str());
		while(dir_it != end){
			if(dir_it.isDirectory() && *dir_it.c_str() != '.')
				getFileList((*path_it + "\\" + dir_it.c_str()).c_str(), file_name_mask, list);
			++dir_it;
		}
	}

	return true;
}

}; // namespace file_utils
