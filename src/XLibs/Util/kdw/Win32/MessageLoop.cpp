#include "StdAfx.h"
#include "kdw/Win32/MessageLoop.h"

#include <algorithm>
#include <windows.h>
#include <windowsx.h>
#include <CrtDbg.h>

namespace Win32{

sigslot::signal0 MessageLoop::signalIdle_;

int MessageLoop::run()
{
	MSG msg;
	while(true){
		if(!PeekMessage(&msg, 0, 0, 0, TRUE)){
			signalIdle_.emit();
			Sleep(10);
		}
		else{
			if(msg.message == WM_QUIT)
				return int(msg.wParam);

			//_CrtCheckMemory();
			TranslateMessage(&msg); // генерит WM_CHAR из WM_KEYDOWN и т.п.
			DispatchMessage(&msg);
			//_CrtCheckMemory();
		}
	}
}

void MessageLoop::quit()
{
	onQuit();
	::PostQuitMessage(0);
}

MessageLoop& MessageLoop::instance()
{
	static MessageLoop messageLoop;
	return messageLoop;
}

}
