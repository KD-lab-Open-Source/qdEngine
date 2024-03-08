#include "stdafx.h"
#include ".\InventorySetter.h"

#include "qd_game_object_animated.h"

InventorySetter::InventorySetter()
: name_(NULL)
{
}

InventorySetter::InventorySetter(LPCTSTR name)
: name_(name)
{
}

void InventorySetter::operator()(qdGameObject* object)
{
	static_cast<qdGameObjectAnimated*>(object)->set_inventory_name(name_);
}
