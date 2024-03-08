#include "stdafx.h"

#include "qd_named_object.h"
#include ".\filterobjectdependencies.h"

#include <boost/rtl.hpp>
namespace {
	//! Возвращает истину,если owner напрямую или опосредованно
	//! является "хозяином" object
	class TestOwner{
	public:
		TestOwner(qdNamedObject const* owner):owner_(owner){}
		bool operator()(qdNamedObject const* object) const{
			do {
				if (owner_ == object)
					return true;
			} while(object = object->owner());
			return false;
		}
	private:
		qdNamedObject const* owner_;
	};
}

FilterObjectDependencies::FilterObjectDependencies(void)
{
}

FilterObjectDependencies::~FilterObjectDependencies(void)
{
}

bool FilterObjectDependencies::run(qdNamedObject const* object,
				std::map<qdNamedObject const*, 
							NamedObjectList>* ptrObjectsDependencies)
{
	boost::rtl::crange<
		std::map<qdNamedObject const*, 
				NamedObjectList> 
					> r(*ptrObjectsDependencies);
	bool res = false;
	for(;r; ++r)
	{
		NamedObjectList &lst = (*r).second;
		boost::rtl::crange<NamedObjectList> objs = 
			boost::rtl::rng::remove_if(lst, TestOwner(object));
		boost::rtl::rng::erase(lst, objs);
		if (!lst.empty())
			res = true;
		lst.sort();
		boost::rtl::rng::erase(lst, boost::rtl::rng::unique(lst));
	}
	return res;
}
