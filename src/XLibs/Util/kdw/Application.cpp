#include "StdAfx.h"
#include "kdw/Application.h"
#include "kdw/Win32/Window.h"

#ifdef KDW_DLL
#include "Serialization\SerializationLib.h"
#include "XMath\XMathLib.h"
#endif


namespace kdw{

Application::Application(HINSTANCE instance)
{
#ifndef KDW_DLL
	if(instance)
		Win32::_setGlobalInstance(instance);
#endif
}

}


#ifdef KDW_DLL
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
   UNREFERENCED_PARAMETER(lpReserved);

   if(dwReason == DLL_PROCESS_ATTACH){
	   // загрузка DLL
	   Win32::_setGlobalInstance(hInstance);
   }
   else if (dwReason == DLL_PROCESS_DETACH){
	   // выгрузка DLL
   }
   return TRUE;
}
#endif