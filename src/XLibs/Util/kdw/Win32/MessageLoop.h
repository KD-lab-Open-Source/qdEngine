#ifndef __KDW_WIN32_MESSAGE_LOOP_H_INCLUDED__
#define __KDW_WIN32_MESSAGE_LOOP_H_INCLUDED__

#include "kdw/API.h"
#include "XTL/sigslot.h"

namespace Win32{

class KDW_API MessageLoop : public sigslot::has_slots
{
public:
	int run();

	sigslot::signal0& signalIdle() { return signalIdle_; }
	sigslot::signal0& signalQuit() { return signalQuit_; }

	void quit();
	virtual void onQuit() { signalQuit().emit(); }

	static MessageLoop& instance();

protected:
	static sigslot::signal0 signalIdle_;
	sigslot::signal0 signalQuit_;
};

}

#endif
