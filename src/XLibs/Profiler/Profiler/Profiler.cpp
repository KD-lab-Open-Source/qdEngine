#include <my_stl.h>
#include <functional>
#include <algorithm>
#include <windows.h>
#include <process.h>
#include <crtdbg.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>
#include <math.h>
#include "Lmcons.h"

#include "Profiler.h"
#include "MTSection.h"
#include "kdw/PropertyEditor.h"
//#include "kdw/kdWidgetsLib.h"
#include "Serialization\XPrmArchive.h"
#include "Serialization\Decorators.h"

#pragma warning (disable: 4073)
#pragma init_seg(lib)

struct SerializeAll{
	SerializeAll(Profiler* profiler)
		: profiler_(profiler)
	{}

	void serialize(Archive& ar){
		profiler_->serializeAll(ar);
	}
	Profiler* profiler_;
};

__int64 getRDTSC();
int totalMemoryUsed(); 
int __cdecl allocationTrackingHook(  int  nAllocType,  void   * pvData,  unsigned int   nSize,  int      nBlockUse,  long     lRequest,  const unsigned char * szFileName,  int      nLine  );

AllocationData AllocationData::dbgHookData_;

ProfilerMode ProfilerInterface::profilerMode_ = PROFILER_ACCURATE;
__declspec(thread) class Profiler* ProfilerInterface::profiler_;
Profiler* ProfilerInterface::profilerForSerialization_;
MTSection ProfilerInterface::lock_;
bool ProfilerInterface::serializeMax_;

bool Profiler::started = false;
int Profiler::milliseconds = 0;
__int64 Profiler::start_ticks = 0;
__int64 Profiler::ticks = 0;
double Profiler::time_factor = 0;

bool Profiler::autoExit_;
int Profiler::startLogicQuant_;
int Profiler::endLogicQuant_;
string Profiler::title_;
string Profiler::profileFile_;

Profiler::Profilers Profiler::profilers_;

Profiler& ProfilerInterface::profiler() 
{ 
	if(!profiler_) 
		profiler_ = new Profiler();
	return *profiler_;
}

TimerData::TimerData(const char* title) 
{ 
	title_ = title; 
	clear(); 
	profiler().attach(this);
}

void TimerData::start() 
{
	if(profilerMode_){
		profiler_->startBuildTree(this);
#ifdef _DEBUG
		if(profilerMode_ == PROFILER_MEMORY)
			last_alloc = AllocationData::dbgHookData_;
#endif
	}
	
	if(!startCounter_)
		t0 = getRDTSC(); 
	startCounter_++;
}

void TimerData::stop() 
{
	if(!--startCounter_){
		__int64 t = getRDTSC();
		__int64 dt = t - t0; 
		dt_sum += dt;

		if(dt_max < dt){
			dt_max = dt;
			t_max = t;
		}
		n++;
	}
	else if(startCounter_ < 0)
		startCounter_ = 0;

	if(profilerMode_){
		profiler_->stopBuildTree();
#ifdef _DEBUG
		if(profilerMode_ == PROFILER_MEMORY){
			accumulated_alloc += AllocationData::dbgHookData_;
			accumulated_alloc -= last_alloc;
		}
#endif
	}
}

void TimerData::clear()
{
	t0 = 0;
	dt_sum = 0;
	n = 0;
	dt_max = 0;
	t_max = 0;
	startCounter_ = 0;
	serializing_ = false;
	accumulated_alloc = AllocationData();
	if(profilerMode_)
		children_.clear();
}

struct lessAvr
{																	       
	bool operator()(const TimerData* c1, const TimerData* c2) const
	{
		return c1->dt_sum > c2->dt_sum;
	}
};

struct lessMax
{																	       
	bool operator()(const TimerData* c1, const TimerData* c2) const
	{
		return c1->dt_max > c2->dt_max;
	}
};

struct lessMemory
{																	       
	bool operator()(const TimerData* c1, const TimerData* c2) const
	{
		return c1->accumulated_alloc.size > c2->accumulated_alloc.size;
	}
};


void TimerData::serialize(Archive& ar)
{
	Profiler& profiler = *profilerForSerialization_;
	XBuffer buf;
	buf.SetDigits(4);
	if(profilerMode_ != PROFILER_MEMORY)
		buf <= dt_sum*100./profiler.ticks < " %, " <= (n ? (double)dt_sum*profiler.time_factor/n : 0) < " ms, " 
			<= n*1000./profiler.milliseconds < " cps, " <= double(n)/profiler.frames < " cpq, max = " <= (double)dt_max*profiler.time_factor < " (" <= profiler.ticks2time(t_max) < ")";
	else 
		buf < "size = " <= accumulated_alloc.size < ", blocks = " <= accumulated_alloc.blocks < ", operations = " <= accumulated_alloc.operations;

	string str = buf.c_str();
	ar.serialize(str, title_, "^!");

	if(!children_.empty()){
		serializing_ = true;
		if(profilerMode_ == PROFILER_MEMORY)
			sort(children_.begin(), children_.end(), lessMemory());
		else if(serializeMax_)
			sort(children_.begin(), children_.end(), lessMax());
		else 
			sort(children_.begin(), children_.end(), lessAvr());
		Timers::iterator i;
		FOR_EACH(children_, i){
			if((*i)->empty())
				continue;
			if(!(*i)->serializing_)
				ar.serialize(*i, (*i)->title_, (*i)->title_);
			else{
				string message = "Рекурсивный вызов";
				ar.serialize(message, (*i)->title_, (*i)->title_);
			}
		}
		//ar.serialize(HLineDecorator(), "hline", "<");
		serializing_ = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				StatisticalData
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatisticalData::StatisticalData(char* title) 
{
	title_ = title; 
	clear();
	profiler().attach(this);
}

void StatisticalData::clear() 
{ 
	n = 0; 
	x_sum = x2_sum = 0; 
	x_max = -1e15; 
	x_min = 1e15; 
	t_max = t_min = 0; 
}

bool StatisticalData::serialize(Archive& ar, const char* name, const char* nameAlt)
{ 
	if(empty())
		return false;
	Profiler& profiler = *profilerForSerialization_;
	XBuffer buf;
	buf.SetDigits(4);
	buf <= avr() < " ± " <= (avr() ? sigma()*100/avr() : 0) < ", max = " <= x_max < " (" <= profiler.ticks2time(t_max) < "), min = " <= x_min < " (" <= profiler.ticks2time(t_min) < "), sampling: " <= n;
	string str = buf.c_str();
	return ar.serialize(str, title_, title_);
}

double StatisticalData::sigma() const 
{ 
	double d2 = (x2_sum - x_sum*x_sum/n); 
	return n > 1 && d2 > 0 ? sqrt(d2/((double)n*(n - 1))) : 0; 
}

void StatisticalData::add(double x) 
{
	n++; 
	x_sum += x; 
	if(x_max < x){ 
		x_max = x; 
		t_max = getRDTSC(); 
	} 
	if(x_min > x){ 
		x_min = x; 
		t_min = getRDTSC(); 
	} 
	x2_sum += x*x;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				Profiler
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
Profiler::Profiler()
{
	MTAutoInternal lock(lock_);

	profileFile_ < "profile";

	clear();

	profilers_.push_back(this);
}

void Profiler::attach(TimerData* timer)
{
	MTAutoInternal lock(lock_);

	timers_.push_back(timer);
}

void Profiler::attach(StatisticalData* stats)
{
	MTAutoInternal lock(lock_);

	statistics_.push_back(stats);
}

void Profiler::clear()
{
	MTAutoInternal lock(lock_);

	stack_.clear();

	if(profilerMode_)
		roots_.clear();

	Timers::iterator i;
	FOR_EACH(timers_, i)
		(*i)->clear();

	Statistics::iterator si;
	FOR_EACH(statistics_, si)
		(*si)->clear();

	frames = 0;
	//memory = totalMemoryUsed();
}

void Profiler::start_stop(ProfilerMode profilerMode)
{
	MTAutoInternal lock(lock_);

	if(!started){
		started = 1;
		profilerMode_ = profilerMode;

#ifdef _DEBUG
		if(profilerMode == PROFILER_MEMORY)
			_CrtSetAllocHook( &allocationTrackingHook );			
#endif

		Profilers::iterator i;
		FOR_EACH(profilers_, i)
			(*i)->clear();

		milliseconds = clock();
		start_ticks = getRDTSC();
	}
	else{
		milliseconds = clock() - milliseconds;
		ticks = getRDTSC() - start_ticks;

		if(!endLogicQuant_){
			if(kdw::edit(Serializer(SerializeAll(this)), 0)){
				if(!profileFile_.empty())
					serializeAll(XPrmOArchive(profileFile_.c_str(), false));
			}
		}
		else {
			/*
			XStream ff(0);
			if(ff.open(profileFile_, XS_OUT | XS_APPEND | XS_NOSHARING)){
				const int BUF_CN_SIZE=MAX_COMPUTERNAME_LENGTH + 1;
				DWORD cns = BUF_CN_SIZE;
				char cname[BUF_CN_SIZE];
				GetComputerName(cname, &cns);

				SYSTEMTIME localTime;
				::GetLocalTime(&localTime);

				ff < "\r\n";
				ff < cname < "\t" <= localTime.wYear < "." <= localTime.wMonth < "." <= localTime.wDay < " " <= localTime.wHour < ":" <=  localTime.wMinute < " " <= localTime.wSecond < "\r\n";
				ff < title_.c_str() < "\r\n";

				ff.write(buf.buffer(), buf.tell());
			}
			*/
		}
		started = 0;
		profilerMode_ = PROFILER_ACCURATE;
	}
}

void Profiler::quant(unsigned long curLogicQuant)
{
	frames++;

	if(endLogicQuant_){
		if(!started){
			if(curLogicQuant >= startLogicQuant_)
                start_stop(profilerMode_);
		}
		else if(curLogicQuant >= endLogicQuant_){
			start_stop(profilerMode_);
			if(autoExit_)
				ErrH.Exit();
			startLogicQuant_ = endLogicQuant_ = 0;
			started = 0;
		}
	}
}

void Profiler::setAutoMode(int startLogicQuant, int endLogicQuant, const char* title, const char* profileFile, bool autoExit) 
{
	startLogicQuant_ = startLogicQuant; 
	endLogicQuant_ = endLogicQuant;
	autoExit_ = autoExit;
	title_ = title;
	profileFile_ = profileFile;
}

void Profiler::serializeAll(Archive& ar)
{
	time_factor = (double)milliseconds/ticks;

	string mode;
	switch(profilerMode_){
		case PROFILER_ACCURATE:
			mode = !roots_.empty() ? "Точное измерение без перестройки дерева" :
				"Точное измерение без перестройки дерева. Дерево не было построено!";
			break;
		case PROFILER_REBUILD:
			mode = "Неточное измерение с перестройкой дерева";
			break;
		case PROFILER_MEMORY:
#ifdef _DEBUG
			mode = "Измерение выделения памяти с перестройкой дерева";
#else
			mode = "Ошибка! Измерение выделения памяти работает только в DEBUG-версии";
#endif
			break;
	}
	ar.serialize(mode, "Mode", "!Режим");

	ar.serialize(milliseconds, "Time_interval_mS", "!Time interval, mS");
	char total_name[2048];
	string  ticksName = _i64toa(ticks, total_name, 10);
	ar.serialize(ticksName, "Ticks", "!Ticks");
	ar.serialize((double)ticks/(milliseconds*1000.), "CPU_MHz", "!CPU, MHz");
	//ar.serialize(memory, "Memory start", "Memory start");
	//ar.serialize(totalMemoryUsed(), "Memory end", "Memory end");

	Profilers::iterator end = remove_if(profilers_.begin(), profilers_.end(), mem_fun(Profiler::empty));
	profilers_.erase(end, profilers_.end());

	serializeMax_ = false;
	ar.serialize(profilers_, "ThreadsByAvr", "!Сортировка по среднему");

	serializeMax_ = true;
	ar.serialize(profilers_, "ThreadsByMax", "!Сортировка по максимуму");

	if(ar.isEdit())
		ar.serialize(profileFile_, "profileFile", "Записать профайл, имя файла");
}

void Profiler::serialize(Archive& ar)
{
	profilerForSerialization_ = this;
	if(profilerMode_ != PROFILER_MEMORY){
		ar.serialize(frames, "Frames", "!Frames");
		ar.serialize(frames*1000./milliseconds, "FPS", "!FPS");
	}

	if(!milliseconds)
		milliseconds = 1;
	if(!frames)
		frames = 1;
	if(!ticks)
		ticks = 1;

	Timers& list = !roots_.empty() ? roots_ : timers_;
	if(profilerMode_ == PROFILER_MEMORY)
		sort(list.begin(), list.end(), lessMemory());
	else if(serializeMax_)
		sort(list.begin(), list.end(), lessMax());
	else
		sort(list.begin(), list.end(), lessAvr());

	Timers::iterator i;
	FOR_EACH(list, i)
		ar.serialize(*i, (*i)->title_, (*i)->title_);

	ar.serialize(statistics_, "Stat", "!Статистика");
}

void Profiler::startBuildTree(TimerData* timer)
{
	MTAutoInternal lock(lock_);

	if(!stack_.empty()){
		Timers& timers = stack_.back()->children_;
		if(find(timers.begin(), timers.end(), timer) == timers.end())
			timers.push_back(timer);
	}
	else if(find(roots_.begin(), roots_.end(), timer) == roots_.end())
		roots_.push_back(timer);
	stack_.push_back(timer);
}

void Profiler::stopBuildTree() 
{ 
	if(!stack_.empty())
		stack_.pop_back(); 
	else
		roots_.clear();
}

bool Profiler::empty() const
{
	Timers::const_iterator ti;
	FOR_EACH(timers_, ti)
		if(!(*ti)->empty())
			return false;

	Statistics::const_iterator si;
	FOR_EACH(statistics_, si)
		if(!(*si)->empty())
			return false;

	return true;
}


///////////////////////////////////////////////////////////////////////
//	Memory Hook
///////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define nNoMansLandSize 4
typedef struct _CrtMemBlockHeader
{
	struct _CrtMemBlockHeader * pBlockHeaderNext;
	struct _CrtMemBlockHeader * pBlockHeaderPrev;
	char *                      szFileName;
	int                         nLine;
	size_t                      nDataSize;
	int                         nBlockUse;
	long                        lRequest;
	unsigned char               gap[nNoMansLandSize];
	/* followed by:
	*  unsigned char           data[nDataSize];
	*  unsigned char           anotherGap[nNoMansLandSize];
	*/
} _CrtMemBlockHeader;

#define pHdr(pbData) (((_CrtMemBlockHeader *)pbData)-1)

int __cdecl allocationTrackingHook(  int  nAllocType,  void   * pvData,  unsigned int   nSize,  int      nBlockUse,  long     lRequest,  const unsigned char * szFileName,  int      nLine  )
{
	AllocationData::dbgHookData_.operations++;
	switch(nAllocType){
		case _HOOK_REALLOC:
			{
				_CrtMemBlockHeader *pHead=pHdr(pvData);
				int dSize=(int)nSize-(int)pHead->nDataSize;
				AllocationData::dbgHookData_.size += dSize;
				break;
			}
		case _HOOK_ALLOC:   
			{
				AllocationData::dbgHookData_.size += nSize;
				AllocationData::dbgHookData_.blocks++;
				break;
			}
		case _HOOK_FREE:   
			{
				_CrtMemBlockHeader *pHead = pHdr(pvData);
				nSize = (unsigned int)pHead->nDataSize;
				AllocationData::dbgHookData_.size -= nSize;
				AllocationData::dbgHookData_.blocks--;
				break;
			}
		default:{
			_CrtMemBlockHeader *pHead=pHdr(pvData);
			break;
				}

	}

	return 1;         // Allow the memory operation to proceed
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////
//	Count by pages
///////////////////////////////////////////////////////////////////////

int totalMemoryUsed()
{
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	int size = 0;
	MEMORY_BASIC_INFORMATION Buffer;
	VirtualQuery(SystemInfo.lpMinimumApplicationAddress, &Buffer,  sizeof(Buffer) );
	while(Buffer.BaseAddress < SystemInfo.lpMaximumApplicationAddress){
		if(Buffer.State == MEM_COMMIT && !(Buffer.Type & MEM_MAPPED) && Buffer.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READ) )
			size += Buffer.RegionSize;
		void* prev_address = Buffer.BaseAddress; 
		VirtualQuery((char*)Buffer.BaseAddress + Buffer.RegionSize, &Buffer,  sizeof(Buffer) );
		if(prev_address == Buffer.BaseAddress)
			break;
	}
	return size;
}
