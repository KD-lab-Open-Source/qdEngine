#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\musicfoldertreenode.h"

#include "ISceneViewFacade.h"
#include "qd_named_object.h"

MusicFolderTreeNode::MusicFolderTreeNode()
: FolderTreeNode(QD_NAMED_OBJECT_MUSIC_TRACK)
{
}

MusicFolderTreeNode::~MusicFolderTreeNode(void)
{
}

bool MusicFolderTreeNode::load(ITreeNodeFactory* factory, 
							   TreeLogic& treeLogic, 
							   ITreeNode * parent, 
							   qdNamedObjectBase const* owner,
							   ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_MUSIC_TRACKS_NODE);
	CTreeListItem* const item = insertToTree(treeLogic, str, parent, 
										II_MUSIC_TRACKS_NODE,
										getFolderId(), insertAfter);
	if (!item)
		return false;

	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD MusicFolderTreeNode::getMenuResId() const{
	return IDR_MUSIC_TRACKS_NODE;
}

bool MusicFolderTreeNode::onStateChange(TreeLogic& logic, 
										DWORD oldState, 
										DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	if (newState&NS_FOCUSED)
	{
		logic.getSceneView()->SetViewState(VS_EDIT_SCENE);
		logic.getSceneView()->SetSubTools(SSTE_NONE);
	}

	return false;
}
