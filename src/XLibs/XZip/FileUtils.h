#ifndef __FILE_UTILS_H_INCLUDED__
#define __FILE_UTILS_H_INCLUDED__

#include <string>
#include <list>

namespace file_utils {

std::string extractFileBase(const char* pathName);
std::string extractFileExt(const char* pathName);
std::string extractFileName(const char* pathName);
std::string extractFilePath(const char* pathName);
bool compareFileName(const char* lhs, const char* rhs);
std::string normalizePath(const char* patch);
const char* localizePath(const char* path);

bool isFileExists(const char* fileName);

class DirIterator{
public:
    DirIterator(const char* path = 0);
	~DirIterator();

    DirIterator& operator++();
    const DirIterator operator++(int);
    const char* c_str() const;
    const char* operator* () const{ return c_str(); }

    bool isDirectory() const;
    bool isFile() const;
    bool operator==(const DirIterator& rhs);
    bool operator!=(const DirIterator& rhs);
private:
    WIN32_FIND_DATA findFileData_;
    HANDLE handle_;
};

bool getFileList(const char* path, const char* file_name_mask, std::list<std::string>& list);

}; // namespace file_utils

#endif
