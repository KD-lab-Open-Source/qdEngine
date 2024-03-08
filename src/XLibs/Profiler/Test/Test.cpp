// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "StaticMap.h"
#include <memory.h>
#include <xutil.h>
#include <Windows.h>
#include <process.h>
#include <math.h>
#include <d3dx9math.h>
#include "Profiler\Profiler.h"
#include "Serialization\SerializationLib.h"
#include "kdw/PropertyEditor.h"
#include "kdw/kdWidgetsLib.h"
#include "MTSection.h"
#include "XMath\xmath.h"
#include "XMath\Mat4f.h"
#include "XMath\XMathLib.h"


XStream ff("log", XS_OUT);

static int getCPUID()
{
	unsigned int id;
	__asm {
		mov eax, 1
		CPUID
		shr ebx, 24
		mov id, ebx
	}
	return id;
}

static MTSection lock_;
int time_;

int totalMemoryUsed();

template<class T>
struct Fqq {
	void add()
	{
		start_timer_auto();
		t += t;
	}

	T t;

};
Fqq<int> fqq;

int aaa;
int bbb;
float r;
Mat4f m1,m2,m3;
Vect3f v1, v2;
float f1, f2 = 23450200, f3 = 328974;
int i1, i2 = 28397433, i3 = 87483;

const int bufferSize = 100000;
int staticBuffer[bufferSize];
int* dynamicBuffer = new int[bufferSize];

void foo1()
{
	MTAuto lock(lock_);
	//D3DXMatrixPerspectiveLH(m1, 1, 1, 1, 100);

	m1 = Mat4f::ID;

	for(int i = 0; i < 100; i++){
		{
		start_timer_auto1(static);
		for(int i = 0; i < bufferSize; i++)
			aaa = staticBuffer[i];
		}

		{
		start_timer_auto1(dynamic);
		for(int i = 0; i < bufferSize; i++)
			aaa = dynamicBuffer[i];
		}
	}
}

void foo()
{
	for(int i = 0; i < 100; i++){
		//	MTAuto lock(lock_);
		foo1();
		//ff <= getCPUID() < "\t" <= xclock() < "\n";
		profiler_quant();
		//statistics_add(sss, random(10));
		//	Sleep(10);
	}
}

void __cdecl logic_thread( void * argument)
{
	//SetThreadAffinityMask(GetCurrentProcess(), 2);

	foo();
//	for(;;){
//		MTAuto lock(lock_);
//		ff <= getCPUID() < "\t" <= xclock() < "\n";
//		Sleep(10);
//	}
}

int main(int argc, const char* argv[])
{
	for(int i = 0; i < 100; i++){
		for(int i = 0; i < bufferSize; i++)
			staticBuffer[i] = 0;
		for(int i = 0; i < bufferSize; i++)
			dynamicBuffer[i] = 0;
	}

	profiler_start_stop(PROFILER_ACCURATE);

	//_beginthread(::logic_thread, 1000000,NULL);

	//SetThreadAffinityMask(GetCurrentProcess(), 1);

	foo();
	profiler_start_stop();

	return 0;
}

