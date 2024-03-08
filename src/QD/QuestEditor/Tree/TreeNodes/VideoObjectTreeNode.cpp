#include "stdafx.h"
#include "resource.h"
#include "menu_helpers.h"

#include "VideoPage.h"
#include "ISceneViewFacade.h"

#include "qd_video.h"

#include ".\videoobjecttreenode.h"
#include "Tree/NodeTraits.h"
#include "name_op.h"

#include "ptree_consts.h"

VideoObjectTreeNode::VideoObjectTreeNode(qdNamedObject* object)
: ConditionalObjectTreeNode(object)
{
}

VideoObjectTreeNode::~VideoObjectTreeNode(void)
{
}

bool VideoObjectTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
							   ITreeNode * parent, qdNamedObjectBase const* owner,
							   ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
											 getObject()->name(),
											 parent,
											 II_VIDEO_NODE,
											 reinterpret_cast<DWORD_PTR>(getObject()),
											  insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD VideoObjectTreeNode::getMenuResId() const{
	return IDR_VIDEO_NODE;
}

void VideoObjectTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if(!treeLogic.isSceneViewActive()){
		menu_h::disableMenuItem(pm, IDC_STOP_VIDEO);
		menu_h::disableMenuItem(pm, IDC_PLAY_STOP_VIDEO);
	}
}

bool VideoObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<VideoPage> ptrPage(new VideoPage);
	ptrPage->set_data(logic.getGameDispatcher(), static_cast<qdVideo*>(getObject()));
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, iActive,IDS_PPCAP_VIDEO_PROPERTIES);
}

bool VideoObjectTreeNode::onStateChange(TreeLogic& logic, 
										DWORD oldState, 
										DWORD newState)
{
	if (!getObject())
		return false;

	if (!logic.isSceneViewActive())
		return false;

	if (newState&NS_FOCUSED) {
		logic.getSceneView()->SetViewState(VS_NONE);
		logic.playStopVideo(static_cast<qdVideo*>(getObject()));
		return true;
	}
	else if (oldState&NS_FOCUSED)
		logic.stopVideo(static_cast<qdVideo*>(getObject()));

	return false;
}

bool VideoObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false;

	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	if(gameDisp->is_video_playing(static_cast<qdVideo*>(getObject())))
	{
		gameDisp->stop_video();
		gameDisp->close_video();
	}
	if (gameDisp->remove_video(static_cast<qdVideo*>(getObject())))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		delete getObject();
//		setObject(NULL);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

DROPEFFECT VideoObjectTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	if (overNode != this
		&&(TreeNodeTraits::isVideoCatalog(overNode)
		||TreeNodeTraits::isVideoCatalog(logic.getParentNode(overNode)))
		)
	{
		logic.selectDropTarget(overNode);
		return DROPEFFECT_MOVE;
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

DROPEFFECT VideoObjectTreeNode::onDropData(TreeLogic& logic, 
							  ITreeNode* dropTarget, 
							  DROPEFFECT dropEffect)
{
	ITreeNode* insertAfter = TN_INSERT_FIRST;
	ITreeNode* videoCatalog = NULL;
	qdVideo const* before = NULL;
	qdGameDispatcher * gameDisp = logic.getGameDispatcher();
	if (TreeNodeTraits::isVideoCatalog(dropTarget))
	{
		videoCatalog = dropTarget;
		//Вставляем первым
		if (!gameDisp->video_list().empty())
			before = gameDisp->video_list().front();
	}
	else
	{
		videoCatalog = logic.getParentNode(dropTarget);
		insertAfter = dropTarget;
		if (ITreeNode* beforeNode = logic.getNextSiblingNode(insertAfter)) 
			before = static_cast<qdVideo*>(beforeNode->getContent().gameObject);
	}

	bool res = logic.moveVideo(static_cast<qdVideo*>(getObject()), 
								before, insertAfter, videoCatalog);
	logic.selectDropTarget(NULL);
	return res ? DROPEFFECT_MOVE : DROPEFFECT_NONE;
}

bool VideoObjectTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool VideoObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return gameDisp->rename_video(
		static_cast<qdVideo*>(getObject()), newName);
}

bool VideoObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return uniq_name(newName, gameDisp->video_list());
}
