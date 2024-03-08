#ifndef __KDW_SERIALIZATION_H_INCLUDED__
#define __KDW_SERIALIZATION_H_INCLUDED__

#ifndef TRANSLATE
# define TRANSLATE(x) x
#endif

#include "XTL/SafeCast.h"
#include "Serialization/Serialization.h"

namespace kdw{
	class Widget;
	class Container;

	void _ensureChildren(kdw::Container* container, kdw::Widget* widget);

	template<class T, class Object>
	class _Property{
	public:
		typedef void(Object::*Setter)(T);
		_Property(T& value, Object* object, Setter setter)
		: object_(object)
		, setter_(setter)
		, value_(value)
		{
		}
		bool serialize(Archive& ar, const char* name, const char* nameAlt){
			T value(value_);
			bool result = ar.serialize(value, name, nameAlt);
			if(ar.isInput() && value != value_)
				(object_->*setter_)(value);
			return result;
		}
		T& value_;
		Object* object_;
		Setter setter_;
	};
	template<class T, class Object>
	class _PropertyConstRef{
	public:
		typedef void(Object::*Setter)(const T&);
		_PropertyConstRef(T& value, Object* object, Setter setter)
		: object_(object)
		, setter_(setter)
		, value_(value)
		{
		}
		bool serialize(Archive& ar, const char* name, const char* nameAlt){
			T value(value_);
			bool result = ar.serialize(value, name, nameAlt);
			if(ar.isInput() && value != value_)
				(object_->*setter_)(value);
			return result;
		}
		T& value_;
		Object* object_;
		Setter setter_;
	};

	template<class T, class Object>
	_Property<T, Object> _property(T& value, Object* object, void(Object::*setter)(T)){
		return _Property<T, Object>(value, object, setter);
	}
	template<class T, class Object>
	_PropertyConstRef<T, Object> _property(T& value, Object* object, void(Object::*setter)(const T&)){
		return _Property<T, Object>(value, object, setter);
	}
}


#endif
