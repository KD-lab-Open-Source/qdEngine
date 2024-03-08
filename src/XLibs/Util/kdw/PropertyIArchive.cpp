#include "StdAfx.h"

#include "kdw/Serialization.h"
#include "Serialization/EnumDescriptor.h"
#include "kdw/PropertyTreeModel.h"
#include "PropertyIArchive.h"
#include "kdw/_PropertyRowBuiltin.h"
#include "UnicodeConverter.h"

namespace kdw{

PropertyIArchive::PropertyIArchive(PropertyTreeModel* model)
: model_(model)
, currentNode_(0)
, lastNode_(0)
{
	//lastNode_ = model->root();
}

bool PropertyIArchive::processEnum(int& value, const EnumDescriptor& descriptor, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, descriptor.typeName())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else 
		return false;
}

bool PropertyIArchive::processBitVector(int& flags, const EnumDescriptor& descriptor, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, descriptor.typeName())){
		currentNode_->assignTo(flags);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(std::string& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(std::string).name())){
 		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(std::wstring& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(std::string).name())){
		string str;
		currentNode_->assignTo(str);
		closeRow(name);
		a2w(value, str);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(bool& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(bool).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(char& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(char).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

// Signed types
bool PropertyIArchive::processValue(signed char& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(signed char).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(signed short& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(signed short).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(signed int& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(signed int).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(signed long& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(signed long).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

// Unsigned types
bool PropertyIArchive::processValue(unsigned char& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(unsigned char).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(unsigned short& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(unsigned short).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(unsigned int& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(unsigned int).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(unsigned long& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(unsigned long).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(float& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(float).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(double& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(double).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::processValue(ComboListString& value, const char* name, const char* nameAlt)
{
	if(openRow(name, nameAlt, typeid(ComboListString).name())){
		currentNode_->assignTo(value);
		closeRow(name);
		return true;
	}
	else
		return false;
}

bool PropertyIArchive::openContainer(void* array, int& number, const char* name, const char* nameAlt,
						   const char* typeName, const char* elementTypeName, int elementSize, bool readOnly)
{
	if(openRow(name, nameAlt, typeName)){
		number = currentNode_->size();
		return true;
	}
	else
		return false;
}

void PropertyIArchive::closeContainer (const char* name)
{
	closeRow(name);
}

bool PropertyIArchive::openStructInternal(void* object, int size, const char* name, const char* nameAlt, const char* typeName, bool polymorphic)
{
	if(openRow(name, nameAlt, typeName)){
		if(currentNode_->isLeaf() && currentNode_ != model_->root()){
			currentNode_->assignTo(object, size);
			closeRow(name);
			return false;
		}
		return true;
	}
	else
		return false;
}

void PropertyIArchive::closeStruct(const char* name)
{
	closeRow(name);
}

int PropertyIArchive::openPointer(void*& object, const char* name, const char* nameAlt,
								  const char* baseName,
								  const char* baseNameComboList, const char* baseNameComboListAlt)
{
	if(openRow(name, nameAlt, baseName)){
		xassert(currentNode_);
		PropertyRowPointer* row = safe_cast<PropertyRowPointer*>(currentNode_);
		const char* value = row->derivedName();
		if(value && value[0] != '\0'){
			int result = indexInComboListString(baseNameComboList, value);
			if(result != NULL_POINTER)
				return result;

			ASSERT(0 && "Error: No such class registered");
		}
		closeRow(name);
	}
	return NULL_POINTER;
}

void PropertyIArchive::closePointer(const char* name, const char* typeName, const char* derivedName)
{
	closeRow(name);
}



bool PropertyIArchive::openBlock(const char* name, const char* nameAlt)
{
	if(openRow(nameAlt, nameAlt, "")){
		return true;
	}
	else
		return false;
}

void PropertyIArchive::closeBlock()
{
	closeRow("block");
}

bool PropertyIArchive::openRow(const char* name, const char* nameAlt, const char* typeName)
{
	if(!name)
		return false;

	if(!currentNode_/* && strlen(name) == 0*/){
		currentNode_ = model_->root();
		ASSERT(currentNode_);
		lastNode_ = currentNode_;
		return true;
	}

	ASSERT(currentNode_);
	if(currentNode_ && !currentNode_->empty()){
		if(currentNode_->isContainer()){
			if(lastNode_ == currentNode_){
				ASSERT(!currentNode_->empty());
				currentNode_ = static_cast<PropertyRow*>(currentNode_->front());
				ASSERT(currentNode_);
				lastNode_ = currentNode_;
				return true;
			}
			else{
				PropertyRow* node = lastNode_;
				ASSERT(node);
				while(node->parent() && currentNode_ != node->parent())
					node = node->parent();
				PropertyRow::iterator iter = std::find(currentNode_->begin(), currentNode_->end(), node);
				
				if(iter != currentNode_->end()){
					++iter;

					if(iter != currentNode_->end()){
						currentNode_ = static_cast<PropertyRow*>(&**iter);
						ASSERT(currentNode_);
						lastNode_ = currentNode_;
						return true;
					}
					else{
						return false;
					}
				}
				else{
					ASSERT(0);
					return false;
				}
			}
		}
		else{
			PropertyRow* node = currentNode_->find(name, 0, typeName);
			if(node && !node->multiValue()){
				currentNode_ = node;
				lastNode_ = currentNode_;
				return true;
			}
			else
				return false;
		}
	}
	else
		return false;
}

void PropertyIArchive::closeRow(const char* name)
{
	//if(currentNode_ && currentNode_->parent()/* && strlen(name) == 0*/)
	//	currentNode_ = 0;
	//else{
		ASSERT(currentNode_);
		if(currentNode_)
			currentNode_ = currentNode_->parent();
	//}

}

};
