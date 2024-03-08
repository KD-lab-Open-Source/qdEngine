#ifndef __LIBRARY_ELEMENT_CREATE_DIALOG_H_INCLUDED__
#define __LIBRARY_ELEMENT_CREATE_DIALOG_H_INCLUDED__

#include "kdw/Dialog.h"
#include "kdw/Label.h"
#include "kdw/Entry.h"
#include "kdw/CheckBox.h"

namespace kdw{

class LibraryElementCreateDialog : public Dialog{
public:
	LibraryElementCreateDialog(Widget* parent, bool canBePasted, bool paste, const char* title)
	: kdw::Dialog(parent)
	, paste_(paste)
	{
		addButton(TRANSLATE("ОК"), RESPONSE_OK);
		addButton(TRANSLATE("Отмена"), RESPONSE_CANCEL);

		add(new Label(TRANSLATE("Название"), true));
		nameEntry_ = new Entry(title);
		add(nameEntry_);

		pasteCheck_ = new CheckBox(TRANSLATE("Вставить из буффера обмена"));
		pasteCheck_->setSensitive(canBePasted);
		add(pasteCheck_);
	}
	const char* name() const{ return nameEntry_->text(); }    
	bool paste() const{ return pasteCheck_->status(); }
protected:
	Entry* nameEntry_;
	CheckBox* pasteCheck_;

	bool canBePasted_;
	bool paste_;
};

}
#endif
