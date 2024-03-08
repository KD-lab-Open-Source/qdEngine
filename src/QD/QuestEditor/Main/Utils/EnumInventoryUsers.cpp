#include "stdafx.h"
#include ".\enuminventoryusers.h"

#include "qd_game_object_animated.h"
#include "Main/ObjectTraits.h"

#include <boost/rtl.hpp>

EnumInventoryUsers::EnumInventoryUsers(void)
{
}

EnumInventoryUsers::~EnumInventoryUsers(void)
{
}

void EnumInventoryUsers::enumerate(qdGameDispatcher const* gameDisp, 
								   qdInventory const* pInv, 
								   qdGameObjectList* plist4Remove)
{
	boost::rtl::crange<qdGameSceneList const> r (gameDisp->scene_list());
	for(; r; ++r){
		qdGameScene* const ptrScene = *r;
		boost::rtl::crange<qdGameObjectList const> ol(ptrScene->object_list());
		for(; ol; ++ol){
			qdGameObject* const po = *ol;
			if (ObjectTraits::is_animated(po)){
				qdGameObjectAnimated* const pao = static_cast<qdGameObjectAnimated*>(po);
				if (pao->has_inventory_name()
					&&!_tcscmp(pInv->name(), pao->inventory_name()))
					plist4Remove->push_back(pao);
			}
		}
	}
}
