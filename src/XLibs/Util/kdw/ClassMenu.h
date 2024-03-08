#ifndef __KDW_INTERN_CLASS_MENU_H_INCLUDED__
#define __KDW_INTERN_CLASS_MENU_H_INCLUDED__

#include "Serialization/ComboStrings.h"

namespace kdw{

struct ClassMenuItemAdder
{
	virtual void operator()(PopupMenuItem& root, int index, const char* text){
		root.add(text);
	}

	void generateMenu(PopupMenuItem& createItem, const ComboStrings& comboStrings, bool includeEmpty = true);
};


}

#endif
