#include "StdAfx.h"
#include "Timer.h"
#include "kdw/Win32/Window.h"
#include "kdw/Widget.h"

namespace kdw{

class TimerImpl : public Win32::TimerInterface{
public:
	TimerImpl(Timer* owner)
	: owner_(owner)
	{}
	void onTimer(){
		owner_->signalTimer().emit();
	}
protected:
	Timer* owner_;
};

Timer::Timer(Widget* owner, float interval)
{
	impl_ = new TimerImpl(this);
	owner->_window()->attachTimer(impl_, round(interval * 1000.0f));
}

Timer::~Timer()
{
	impl_ = 0;
}

}
