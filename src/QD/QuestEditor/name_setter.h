#pragma once
#include "name_interf.h"

/// \brief Агрегирует некоторую структуру данных. И позволяет назначать ей строку. 
/// И получать из неё строку
template<class _Data>
class name_setter :
	public INameSetter
{
	_Data* pdata_;
public:

	explicit name_setter(_Data* pdata):
		pdata_(pdata)
	{
	}

	virtual const CString& get_string() const{
		return pdata_->name();
	}
	virtual void set_string(const CString& str){
		pdata_->set_name(str);
	}
};
