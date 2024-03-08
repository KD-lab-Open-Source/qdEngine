#ifndef __KDW_APPLIACATION_H_INCLUDED__
#define __KDW_APPLIACATION_H_INCLUDED__

#include "kdw/Win32/MessageLoop.h"

namespace kdw{

void initialize();

class KDW_API Application : public Win32::MessageLoop 
{
public:
	Application(HINSTANCE instance = 0);
	void quit() { __super::quit(); }
};

};

#endif
