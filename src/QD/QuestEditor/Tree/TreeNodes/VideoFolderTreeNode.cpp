#include "stdafx.h"
#include "resource.h"
#include ".\videofoldertreenode.h"
#include "ptree_consts.h"
#include "ISceneViewFacade.h"

VideoFolderTreeNode::VideoFolderTreeNode(void)
: FolderTreeNode(QD_NAMED_OBJECT_VIDEO)
{
}

VideoFolderTreeNode::~VideoFolderTreeNode(void)
{
}

bool VideoFolderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
							   ITreeNode * parent, qdNamedObjectBase const* owner,
							   ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_VIDEO_CATALOG);
	CTreeListItem* const item = insertToTree(treeLogic,
											str,
											parent,
											II_VIDEO_CATALOG,
											getFolderId(), insertAfter);
	return (item != NULL);
}
//! Возвращает идентификатор ресурса меню для узла
DWORD VideoFolderTreeNode::getMenuResId() const{
	return IDR_VIDEOS_NODE;
}

bool VideoFolderTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	if (newState&NS_FOCUSED)
		logic.getSceneView()->SetViewState(VS_NONE);
	return newState&NS_FOCUSED;
}