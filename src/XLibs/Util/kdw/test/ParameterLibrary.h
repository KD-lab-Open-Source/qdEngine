#pragma once
#include "XMath/Colors.h"

#include "Serialization/StringTable.h"
#include "Serialization/StringTableBase.h"
#include "Serialization/StringTableReference.h"

struct Style : public StringTableBase{
public:
	Style(const char* name = "");
	void serialize(Archive& ar);
protected:
	Color4c color_;
};

typedef StringTable<Style> StyleLibrary;
typedef StringTableReference<Style, false> StyleReference;


struct ParameterType : public StringTableBase{
public:
	ParameterType(const char* name = "");
	void serialize(Archive& ar);
protected:
	std::string description_;
	StyleReference style_;
};

typedef StringTable<ParameterType> ParameterTypeLibrary;
typedef StringTableReference<ParameterType, false> ParameterTypeReference;



struct Parameter : public StringTableBase{
public:
	Parameter(const char* name = "");
	void serialize(Archive& ar);
	static bool editorAllowDrag(){ return true; }
protected:
	float value_;
	ParameterTypeReference type_;
};

typedef StringTable<Parameter> ParameterLibrary;
typedef StringTableReference<Parameter, false> ParameterReference;



void saveAllLibraries();
void loadAllLibraries();
