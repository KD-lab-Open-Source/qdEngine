#ifndef __KDW_IDLE_LISTENER_H_INCLUDED__
#define __KDW_IDLE_LISTENER_H_INCLUDED__

namespace kdw{

class IdleListenerImpl;
class IdleListener{
public:
	IdleListener();
	virtual void onIdle() = 0;
protected:
	IdleListenerImpl* impl_;
};

}

#endif
