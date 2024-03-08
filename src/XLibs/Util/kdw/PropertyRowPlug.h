#ifndef __PROPERTY_ROW_PLUG_H_INCLUDED__
#define __PROPERTY_ROW_PLUG_H_INCLUDED__

#include "kdw/PropertyTree.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/PropertyRow.h"
#include "kdw/PlugDialog.h"
#include <crtdbg.h>

namespace kdw{

template<class Type, class PlugType>
class PropertyRowPlug : public PropertyRowImpl<Type, PropertyRowPlug>{
public:
	PropertyRowPlug(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
	: PropertyRowImpl<Type, PropertyRowPlug>(object, size, name, nameAlt, typeName)
	{}

	PropertyRowPlug() {}

	bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow){
		return onActivate(tree);
	}

	bool onActivate(PropertyTree* tree){
		ShareHandle<PlugType> plug = new PlugType();
		PlugDialog dialog(tree, &*plug);
		plug->assignFrom((void*)&value(), sizeof(Type));
		if(dialog.showModal() == RESPONSE_OK){
			plug->assignTo((void*)&value(), sizeof(Type));
			tree->model()->rowChanged(this);
		}
		return true;
	}

	std::string valueAsString() const{ 
		return PlugType::valueAsString(value());
	}
};

}

#endif
