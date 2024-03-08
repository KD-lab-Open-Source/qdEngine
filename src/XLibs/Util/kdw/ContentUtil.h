#ifndef __KDW_CONTENT_UTIL_H_INCLUDED__
#define __KDW_CONTENT_UTIL_H_INCLUDED__

#include <string>

namespace kdw{

class Widget;

std::string selectAndCopyResource(const char* resourceHomePath, const char** filter,
								  const char* defaultName, const char* title, Widget* parent = 0);

std::string selectAndCopyModel(const char* resourceHomePath, const char** filter,
							   const char* defaultName, const char* title, Widget* parent = 0);

typedef bool (*GetAllTextureNamesFunc)(const char* filename, vector<string>& names);
extern GetAllTextureNamesFunc getAllTextureNamesFunc;

}

#endif
