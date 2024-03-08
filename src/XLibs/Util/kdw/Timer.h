#ifndef __KDW_TIMER_H_INCLUDED__
#define __KDW_TIMER_H_INCLUDED__

#include "XTL/sigslot.h"

namespace kdw{
class Widget;
class TimerImpl;
class Timer : public ShareHandleBase{
public:
	Timer(Widget* owner, float interval = 0.1f);
	virtual ~Timer();
	sigslot::signal0& signalTimer(){ return signalTimer_; }
protected:
	sigslot::signal0 signalTimer_;
	ShareHandle<TimerImpl> impl_;
};

}

#endif
