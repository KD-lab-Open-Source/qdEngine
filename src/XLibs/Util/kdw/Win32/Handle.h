#ifndef __KDW_WIN32_HANDLE_H_INCLUDED__
#define __KDW_WIN32_HANDLE_H_INCLUDED__

//#include "kdw/Win32/Types.h"
#include <windows.h>

namespace Win32{

template<class HANDLE_TYPE = HANDLE>
class Handle{
public:
	Handle(HANDLE_TYPE handle = 0)
	: handle_(handle)
	{
	}

	operator HANDLE_TYPE() const{ return handle_; }

	HANDLE_TYPE detach(){
		HANDLE_TYPE result = handle_;
		handle_ = 0;
		return result;
	}

	void set(HANDLE_TYPE handle){
		if(handle_)
			::DeleteObject(handle_);
        handle_ = handle;
	}

	Handle& operator=(HANDLE_TYPE rhs){
		set(rhs);
		return *this;
	}

	~Handle(){
		set(0);
	}
protected:
	HANDLE_TYPE handle_;
private:
	void operator=(const Handle& rhs){
	}
};

class AutoSelector{
public:
	AutoSelector(HDC dc, HGDIOBJ object)
	: object_(object)
	, dc_(dc)
	{
		oldObject_ = ::SelectObject(dc_, object_);
	}

	~AutoSelector(){
		HGDIOBJ object = ::SelectObject(dc_, oldObject_);
		xassert(object == object_);
		::DeleteObject(object_);
	}
	operator HGDIOBJ() const{ return oldObject_; }
protected:
	bool deleteObject_;
	HDC dc_;
	HGDIOBJ object_;
	HGDIOBJ oldObject_;
};

class StockSelector{
public:
	StockSelector(HDC dc, HGDIOBJ object)
	: object_(object)
	, dc_(dc)
	{
		oldObject_ = ::SelectObject(dc_, object_);
	}

	~StockSelector(){
		HGDIOBJ object = ::SelectObject(dc_, oldObject_);
		xassert(object == object_);
	}
	operator HGDIOBJ() const{ return oldObject_; }
protected:
	HDC dc_;
	HGDIOBJ object_;
	HGDIOBJ oldObject_;
};

}

#endif
