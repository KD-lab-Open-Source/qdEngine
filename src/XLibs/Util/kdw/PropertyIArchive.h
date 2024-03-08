#ifndef __KDW_PROPERTY_I_ARCHIVE_H_INCLUDED__
#define __KDW_PROPERTY_I_ARCHIVE_H_INCLUDED__

#include "Serialization/Serialization.h"

namespace kdw{

class PropertyRow;
class PropertyTreeModel;

class PropertyIArchive : public ::Archive{
public:
	PropertyIArchive(PropertyTreeModel* model);

	bool isEdit() const{ return true; }
	bool isOutput() const{ return false; }
	bool isInput() const{ return true; }

protected:

	bool processEnum(int& value, const EnumDescriptor& descriptor, const char* name, const char* nameAlt);
	bool processBitVector(int& flags, const EnumDescriptor& descriptor, const char* name, const char* nameAlt);
	bool processValue(std::string& value, const char* name, const char* nameAlt);
	bool processValue(std::wstring& value, const char* name, const char* nameAlt);
    bool processValue(bool& value, const char* name, const char* nameAlt);
    bool processValue(char& value, const char* name, const char* nameAlt);
    
    // Signed types
    bool processValue(signed char& value, const char* name, const char* nameAlt);
    bool processValue(signed short& value, const char* name, const char* nameAlt);
    bool processValue(signed int& value, const char* name, const char* nameAlt);
    bool processValue(signed long& value, const char* name, const char* nameAlt);
    // Unsigned types
    bool processValue(unsigned char& value, const char* name, const char* nameAlt);
    bool processValue(unsigned short& value, const char* name, const char* nameAlt);
    bool processValue(unsigned int& value, const char* name, const char* nameAlt);
    bool processValue(unsigned long& value, const char* name, const char* nameAlt);

    bool processValue(float& value, const char* name, const char* nameAlt);
    bool processValue(double& value, const char* name, const char* nameAlt);
    bool processValue(ComboListString& value, const char* name, const char* nameAlt);

    virtual bool openContainer(void* array, int& number, const char* name, const char* nameAlt,
							   const char* typeName, const char* elementTypeName, int elementSize, bool readOnly);
    virtual void closeContainer (const char* name);    

    bool openStructInternal(void* object, int size, const char* name, const char* nameAlt, const char* typeName, bool polymorphic);
    void closeStruct(const char* name);

    int openPointer(void*& object, const char* name, const char* nameAlt,
					 const char* baseName,
                     const char* derivedName, const char* derivedNameAlt);
    void closePointer(const char* name, const char* typeName, const char* derivedName);

    bool openBlock(const char* name, const char* nameAlt);
    void closeBlock();

	bool needDefaultArchive(const char* baseName) const { return false; }
private:
	bool openRow(const char* name, const char* nameAlt, const char* typeName);
	void closeRow(const char* name);

	PropertyTreeModel* model_;
	PropertyRow* currentNode_;
	PropertyRow* lastNode_;
};

}

#endif
