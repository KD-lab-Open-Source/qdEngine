#ifndef __CONST_STRING_LIST_H_INCLUDED__
#define __CONST_STRING_LIST_H_INCLUDED__

class Archive;
class ConstStringList{
public:
	const char* findOrAdd(const char* string);
protected:
	typedef std::list<std::string> Strings;
	Strings strings_;
};

class ConstStringWrapper{
public:
	ConstStringWrapper(ConstStringList* list, const char*& string);
	bool serialize(Archive& ar, const char* name, const char* nameAlt);
protected:
	ConstStringList* list_;
	const char*& string_;
};

extern ConstStringList globalConstStringList;

#endif
