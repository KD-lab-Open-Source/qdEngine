#ifndef __KDW_PROPERTY_O_ARCHIVE_H_INCLUDED__
#define __KDW_PROPERTY_O_ARCHIVE_H_INCLUDED__

#include "Serialization/Serialization.h"

namespace kdw{

class PropertyRow;
class PropertyTreeModel;

class PropertyOArchive : public ::Archive{
public:
	PropertyOArchive(PropertyTreeModel* model);
	~PropertyOArchive();

	bool isEdit() const{ return true; }
	bool isOutput() const{ return true; }
	bool isInput() const{ return false; }

protected:
	PropertyOArchive(PropertyTreeModel* model, const char* typeName, const char* derivedTypeName, const char* derivedTypeNameAlt);

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

	Archive* openDefaultArchive(const char* typeName, const char* derivedTypeName, const char* derivedTypeNameAlt);
    void closeDefaultArchive(ShareHandle<Archive> base_ar, const char* typeName, const char* derivedTypeName, const char* derivedTypeNameAlt);


    bool openStructInternal(void* object, int size, const char* name, const char* nameAlt, const char* typeName, bool polymorphic);
    void closeStruct(const char* name);

    int openPointer(void*& object, const char* name, const char* nameAlt,
					 const char* baseName,
                     const char* derivedName, const char* derivedNameAlt);
    void closePointer (const char* name, const char* typeName, const char* derivedName);

    bool openBlock(const char* name, const char* nameAlt);
    void closeBlock();

	bool needDefaultArchive(const char* baseName) const { return true; }
private:
	PropertyRow* addRow(ShareHandle<PropertyRow> newRow, bool block = false, PropertyRow* previousNode = 0);
	void enterNode(PropertyRow* row); // sets currentNode
	PropertyRow* rootNode();

	bool updateMode_;
	PropertyTreeModel* model_;
	ShareHandle<PropertyRow> currentNode_;
	PropertyRow* lastNode_;

	// для defaultArchive
	ShareHandle<PropertyRow> rootNode_;
	std::string typeName_;
	const char* derivedTypeName_;
	std::string derivedTypeNameAlt_;
};

}

#endif
