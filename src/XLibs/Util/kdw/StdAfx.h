#ifndef __KDW_STDAFX_H_INCLUDED__
#define  __KDW_STDAFX_H_INCLUDED__
#include <assert.h>

#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <windows.h>

#include "XUtil.h"

#include <string>
#include <vector>
#include <list>
#include "Handle.h"
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

#endif 
