#include "StdAfx.h"
#include "kdw/PropertyEditor.h"
#include "kdw/Plug.h"
#include "kdw/PlugDialog.h"
#include "kdw/Win32/Window.h"
#include "EditorDialog.h"

namespace kdw{

bool KDW_API edit(Serializer& ser, const char* stateFileName, int flags, Widget* parent, const char* title)
{
	return edit(ser, stateFileName, flags, parent ? *_findWindow(parent) : HWND(0), title);
}

bool KDW_API edit(Serializer& ser, const char* stateFileName, int flags, HWND parent, const char* title)
{
	bool result = false;
	const char* typeName = ser.typeName();
	kdw::PlugBase* plug = 0;
	if(typeName && (plug = PlugFactory::instance().create(typeName, true))){
		plug->assignFrom(ser);
		PlugDialog dialog(parent, plug);
		result = dialog.showModal() == kdw::RESPONSE_OK;
		if(result)
			plug->assignTo(ser);
		delete plug;
	}
	else{
		EditorDialog dialog(ser, stateFileName, flags, parent);
		if(title)
			dialog.setTitle(title);
		result = dialog.showModal() == kdw::RESPONSE_OK;
	}
	return result;
}
}
