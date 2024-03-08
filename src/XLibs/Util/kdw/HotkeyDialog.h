#ifndef __KDW_HOTKEY_DIALOG_INCLUDED__
#define __KDW_HOTKEY_DIALOG_INCLUDED__

#include "kdw/Dialog.h"
#include "sKey.h"

namespace kdw{

class HotkeyButton;

class HotkeyDialog : public Dialog{
public:
	HotkeyDialog(Widget* owner, sKey key = sKey());
	HotkeyDialog(HWND owner, sKey key = sKey());
	sKey& get(){ return key_; }

	void onChanged();

	void onKeyDefault();
	void onKeyCancel();
protected:
	HotkeyButton* button_;
	sKey key_;
};

}

#endif
