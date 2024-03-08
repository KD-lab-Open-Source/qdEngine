#include "StdAfx.h"
#include "kdw/HotkeyContext.h"
#include "kdw/Serialization.h"
#include "XTL/sigslot.h"
//#include "SystemUtil.h"

namespace kdw{

// ---------------------------------------------------------------------------
HotkeyContext::HotkeyContext(HotkeyContext* parent)
{
	setParent(parent);
}

HotkeyContext::~HotkeyContext()
{
}


void HotkeyContext::lockSticky()
{
	signalStickyLocked_.emit();
}

void HotkeyContext::setParent(HotkeyContext* parent)
{
	parent_ = parent;
}

HotkeySignal& HotkeyContext::signalPressed(sKey key)
{
	return signalsPressed_[key];
}

HotkeySignal& HotkeyContext::signalReleased(sKey key)
{
	return signalsReleased_[key];
}

bool HotkeyContext::injectPress(sKey key)
{
	lastKey_ = key;

	bool filtered = false;
	signalPressedAny_.emit(key, filtered);
	if(filtered)
		return true;
	HotkeySignals::iterator it = signalsPressed_.find(key);
	if(it != signalsPressed_.end()){
		it->second.emit();
		return true;
	}
	else if(parent_)
		return parent_->injectPress(key);
	return false;
}

bool HotkeyContext::injectRelease(sKey key)
{
	bool filtered = false;
	signalReleasedAny_.emit(key, filtered);
	if(filtered)
		return true;
	HotkeySignals::iterator it = signalsReleased_.find(key);
	if(it != signalsReleased_.end()){
		it->second.emit();
		return true;
	}
	else if(parent_)
		return parent_->injectRelease(key);
	return false;
}

}