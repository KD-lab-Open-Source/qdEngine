#include "stdafx.h"
#include ".\copyofglobalupdater.h"

#include "qd_game_dispatcher.h"
#include "qd_game_scene.h"
#include "Tree/TreeLogic.h"
#include "qd_game_object.h"

#include <boost/rtl.hpp>

CopyOfGlobalUpdater::CopyOfGlobalUpdater(void)
{
}

CopyOfGlobalUpdater::~CopyOfGlobalUpdater(void)
{
}

void CopyOfGlobalUpdater::update(TreeLogic& logic, qdGameObject const* globalObject)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	boost::rtl::crange<qdGameSceneList const> r(gameDisp->scene_list());
	
	for(; r; ++r)
	{
		qdGameScene* scene = *r;
		qdGameObject* obj = scene->get_object(globalObject->name());
		ITreeNode* sceneNode = *logic.getObjectNodes(scene);
		if (obj)
		{
			(*logic.getObjectNodes(obj))->reload(logic.getTreeNodeFactory(), 
												logic, 
												sceneNode, 
												scene);
		}
	}
}