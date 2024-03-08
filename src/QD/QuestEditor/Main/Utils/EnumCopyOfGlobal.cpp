#include "stdafx.h"
#include ".\enumcopyofglobal.h"

#include "qd_game_dispatcher.h"

#include <boost/rtl.hpp>

EnumCopyOfGlobal::EnumCopyOfGlobal(void)
{
}

EnumCopyOfGlobal::~EnumCopyOfGlobal(void)
{
}

void EnumCopyOfGlobal::enumerate(qdGameDispatcher const* gameDisp, 
					  LPCTSTR name, 
					  CHECK_FUN f, qdGameObjectList* pres)
{
	boost::rtl::crange<qdGameSceneList const> r(gameDisp->scene_list());
	for(; r; ++r)
	{
		qdGameScene* pscene = *r;
		if (qdGameObject* p = pscene->get_object(name)) {
			if (f(p)) 
				pres->push_back(p);
		}
	}

}
