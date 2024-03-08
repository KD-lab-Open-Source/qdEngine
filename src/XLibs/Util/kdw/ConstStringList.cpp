#include "StdAfx.h"
#include "kdw/ConstStringList.h"
#include "Serialization/Serialization.h"

ConstStringList globalConstStringList;

const char* ConstStringList::findOrAdd(const char* string)
{
	// TODO: попробовать вектор стрингов по указателям с сортировкой
	Strings::iterator it = std::find(strings_.begin(), strings_.end(), string);
	if(it == strings_.end()){
		strings_.push_back(string);
		return strings_.back().c_str();
	}
	else{
		return it->c_str();
	}
}


ConstStringWrapper::ConstStringWrapper(ConstStringList* list, const char*& string)
: list_(list ? list : &globalConstStringList)
, string_(string)
{
	ASSERT(string_);
}

bool ConstStringWrapper::serialize(Archive& ar, const char* name, const char* nameAlt)
{
	if(ar.isOutput()){
		ASSERT(string_);
		std::string out = string_;
		return ar.serialize(out, name, nameAlt);
	}
	else{
		std::string in;
		bool result = ar.serialize(in, name, nameAlt);

		string_ = list_->findOrAdd(in.c_str());
		return result;
	}
}
