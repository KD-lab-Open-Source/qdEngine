#ifndef __KDW_LIBRARY_EDITOR_DIALOG_H_INCLUDED__
#define __KDW_LIBRARY_EDITOR_DIALOG_H_INCLUDED__

#include "kdw/Dialog.h"

namespace kdw{

class LibraryEditor;

class LibraryEditorDialog : public Dialog{
public:
	LibraryEditorDialog(Widget* owner);
	LibraryEditorDialog(HWND owner);

	int showModal(const char* libraryName = "");
	void serialize(Archive& ar);
protected:
	void init();
	LibraryEditor* libraryEditor_;
};

}

#endif
