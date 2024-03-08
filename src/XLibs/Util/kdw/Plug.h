#ifndef __KDW_PLUG_H_INCLUDED__
#define __KDW_PLUG_H_INCLUDED__

#include "Serialization/SerializationFactory.h"
#include "Serialization/BinaryArchive.h"
#include "Serialization/Factory.h"
#include "kdw/PropertyRowPlug.h"
#include "XTL/StaticString.h"

namespace kdw{
// Plug - интерфейс для всяческих окошек редактирования (может быть
// как встроен в виджет PlugHost(например, в редактор библиотек), так и показан
// с помощью PlugDialog)

class PlugBase : public ShareHandleBase{
public:
	template<class T>
	static std::string valueAsString(const T&){
		return " ... ";
	}
	PlugBase(const char* typeName)
	: typeName_(typeName)
	{}
	virtual ~PlugBase() {}

	virtual void assignTo(Serializer& ser) = 0;
	virtual void assignFrom(Serializer& ser) = 0;
	virtual void assignTo(void* data, std::size_t size) = 0;
	virtual void assignFrom(void* data, std::size_t size) = 0;
	virtual void onSet() {}
	virtual void detach() {}
	virtual Widget* asWidget() = 0;

	const char* typeName() const{ return typeName_; }
protected:
	const char* typeName_;
};

template<class Type, class Derived>
class Plug : public PlugBase{
public:
	Plug()
	: PlugBase(typeid(Type).name())
	{
	}
	Type& value(){ return value_; }

	void set(const Type& value){
		BinaryOArchive oa;
		oa.serialize(value, "value", 0);
		BinaryIArchive ia(oa);
		ia.serialize(value_, "value", 0);
		onSet();
	}
	const Type& get() const{ return value_; }
	void get(Type& value) const{
		BinaryOArchive oa;
		oa.serialize(value_, "value", 0);
		BinaryIArchive ia(oa);
		ia.serialize(value, "value", 0);
	}
	void assignTo(void* _data, std::size_t size){
		xassert(sizeof(value_) == size);
		Type& data = *reinterpret_cast<Type*>(_data);
		get(data);
	}
	void assignFrom(void* _data, std::size_t size){
		xassert(sizeof(value_) == size);
		Type& data = *reinterpret_cast<Type*>(_data);
		set(data);
	}
	void assignTo(Serializer& ser){
		BinaryOArchive oa;
		oa.serialize(value_, "value", "");
		BinaryIArchive ia(oa);
		ser.serialize(ia, "value", "");
	}
	void assignFrom(Serializer& ser){
		BinaryOArchive oa;
		ser.serialize(oa, "value", "");
		BinaryIArchive ia(oa);
		ia.serialize(value_, "value", "");
		onSet();
	}
	void detach(){ 
		value_ = Type(); 
		onSet(); 
	}
private:
	Type value_;
};

typedef Factory<StaticString, PlugBase> PlugFactory;

#define REGISTER_PLUG(Type, PlugType) \
	typedef kdw::PlugFactory kdwPlugFactory; \
	REGISTER_CLASS_IN_FACTORY(kdwPlugFactory, typeid(Type).name(), PlugType) \
	typedef kdw::PropertyRowPlug<Type, PlugType> PropertyRow##Type; \
	REGISTER_PROPERTY_ROW(Type, PropertyRow##Type) 

}

#endif
