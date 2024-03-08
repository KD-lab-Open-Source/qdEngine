#ifndef __PROPERTY_EDITOR_EDITOR_DIALOG_H_INCLUDED__
#define __PROPERTY_EDITOR_EDITOR_DIALOG_H_INCLUDED__

#include "kdw/PropertyEditor.h"
#include "kdw/Dialog.h"

namespace kdw{

class PropertyTree;

class EditorDialog : public kdw::Dialog{
public:
	EditorDialog(Serializer& serializer, const char* stateFileName, int flags, kdw::Widget*);
	EditorDialog(Serializer& serializer, const char* stateFileName, int flags, HWND parent);

	void onResponse(int response);
	
	void serialize(Archive& ar);
protected:
	void init(Serializer& ser, const char* treeStateFileName, int flags);

	kdw::PropertyTree* tree_;
	std::string stateFileName_;
};
}

#endif