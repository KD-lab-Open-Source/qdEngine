#ifndef __KDW_HOTKEY_BUTTON_H_INCLUDED__
#define __KDW_HOTKEY_BUTTON_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include "sKey.h"

namespace kdw{

class HotkeyButtonImpl;
class HotkeyButton : public _WidgetWithWindow{
public:
	HotkeyButton(int border = 0);
	sigslot::signal0& signalChanged(){ return signalChanged_; }

	sKey get() const{ return key_; }
protected:
	sigslot::signal0 signalChanged_;
	HotkeyButtonImpl& impl();
	sKey key_;
	friend class HotkeyButtonImpl;
};

}

#endif
