// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>


#include <string>
#include <list>
#include "XUtil.h"
#include "my_STL.h"
using namespace std;

#define ASSERT(expression) assert(expression)
#ifndef VERIFY
#ifdef NDEBUG
# define VERIFY(expression) (expression)
#else
# define VERIFY(expression) { bool result = (expression) ? true : false; ASSERT(result && #expression ); }
#endif
#endif 

#include "Resource.h"