#include "StdAfx.h"
#include "ParameterLibrary.h"
#include "Serialization/StringTableImpl.h"
#include "Serialization/Serialization.h"
#include "kdw/PropertyRowReference.h"

WRAP_LIBRARY(StyleLibrary, "StyleLibrary", "Стили", "StyleLibrary", 1, LIBRARY_EDITABLE);
REGISTER_REFERENCE(StyleReference);

Style::Style(const char* name)
: StringTableBase(name) 
, color_(255, 0, 0, 255)
{
}

void Style::serialize(Archive& ar)
{
	__super::serialize(ar);
	ar.serialize(color_, "color", "Цвет");
}

// --------------------------------------------------------------------------------
WRAP_LIBRARY(ParameterLibrary, "ParameterLibrary", "Параметры", "ParameterLibrary", 1, LIBRARY_EDITABLE);
REGISTER_REFERENCE(ParameterReference);

Parameter::Parameter(const char* name)
: StringTableBase(name)
, value_(0.0f)
{
}

void Parameter::serialize(Archive& ar)
{
	__super::serialize(ar);
	ar.serialize(value_, "value", "Значение");
	ar.serialize(type_, "type", "Тип");
}

// --------------------------------------------------------------------------------

WRAP_LIBRARY(ParameterTypeLibrary, "ParameterTypeLibrary", "Типы параметров", "ParameterTypeLibrary", 1, LIBRARY_EDITABLE);
REGISTER_REFERENCE(ParameterTypeReference);

ParameterType::ParameterType(const char* name)
: StringTableBase(name)
, description_("")
{
}

void ParameterType::serialize(Archive& ar)
{
	__super::serialize(ar);
	ar.serialize(description_, "description", "Описание");
	ar.serialize(style_, "style", "Стиль");
}


void saveAllLibraries()
{
	ParameterLibrary::instance().saveLibrary();
	ParameterTypeLibrary::instance().saveLibrary();
	StyleLibrary::instance().saveLibrary();
}

void loadAllLibraries()
{
	ParameterLibrary::instance().loadLibrary();
	ParameterTypeLibrary::instance().loadLibrary();
	StyleLibrary::instance().loadLibrary();
}
