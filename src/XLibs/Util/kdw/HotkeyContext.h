#ifndef __KDW_HOTKEY_DISPATCHER_H_INCLUDED__
#define __KDW_HOTKEY_DISPATCHER_H_INCLUDED__

#include "kdw/API.h"
//#include "XTL/StaticMap.h"
#include <map>
#include "XTL/sigslot.h"
#include "sKey.h"

namespace kdw{

typedef sigslot::signal0 HotkeySignal;
//typedef StaticMap<sKey, HotkeySignal> HotkeySignals;
typedef map<sKey, HotkeySignal> HotkeySignals;

class KDW_API HotkeyContext : public ShareHandleBase{
public:
	HotkeyContext(HotkeyContext* parent = 0);
	~HotkeyContext();

	bool injectPress(sKey key);
	bool injectRelease(sKey key);

	HotkeySignal& signalPressed(sKey key);
	HotkeySignal& signalReleased(sKey key);

	sigslot::signal2<sKey, bool&>& signalPressedAny(){ return signalPressedAny_; }
	sigslot::signal2<sKey, bool&>& signalReleasedAny(){ return signalReleasedAny_; }
	sigslot::signal0& signalStickyLocked(){ return signalStickyLocked_; }

	void lockSticky();

	void setParent(HotkeyContext* parent);
protected:
	sigslot::signal2<sKey, bool&> signalPressedAny_;
	sigslot::signal2<sKey, bool&> signalReleasedAny_;
	sKey lastKey_;

	sigslot::signal0 signalStickyLocked_;

	HotkeySignals signalsPressed_;
	HotkeySignals signalsReleased_;

	HotkeyContext* parent_;
};

}

#endif
