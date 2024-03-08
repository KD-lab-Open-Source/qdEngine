#ifndef __PROPERTY_EDITOR_H_INCLUDED__
#define __PROPERTY_EDITOR_H_INCLUDED__

#include "kdw/API.h"
#include "Serialization/Serializer.h"
#include "Win32/Types.h"

namespace kdw{
class Widget;

enum EditFlags{
	ONLY_TRANSLATED   = 1 << 0,
	IMMEDIATE_UPDATE  = 1 << 1,
	COMPACT		      = 1 << 2,
	EXPAND_ALL		  = 1 << 3
};

bool KDW_API edit(Serializer& ser, const char* stateFileName,
				  int flags = IMMEDIATE_UPDATE | ONLY_TRANSLATED,
				  Widget* parent = 0, const char* title = 0);
bool KDW_API edit(Serializer& ser, const char* stateFileName,
				  int flags, HWND parent, const char* title = 0);

}

#endif
