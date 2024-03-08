#include "stdafx.h"
#include "resource.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_moving.h"

#include "SceneTreeView.h"

#include "qd_named_object_reference.h"

#include <boost/rtl.hpp>


void SceneTreeView::fillObjectList()
{
	if (!ptrObjectsDependencies_)
		return;
	objectList_->clearList();
	boost::rtl::crange<std::map<qdNamedObject const*, 
					NamedObjectList> > stats(*ptrObjectsDependencies_);
	for(; stats; ++stats)
	{
		boost::rtl::crange<NamedObjectList const> list((*stats).second);
		if (list)
		{
			qdNamedObjectReference objRef((*stats).first);
			HITEM objNode = objectList_->addObjectToList(&objRef);
			
			for(; list; ++list)
			{
				qdNamedObjectReference ref(*list);
				objectList_->addObjectToList(&ref, objNode);
			}
		}
	}
}