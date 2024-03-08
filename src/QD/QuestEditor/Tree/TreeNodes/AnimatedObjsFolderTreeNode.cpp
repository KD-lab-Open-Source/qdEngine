#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\AnimatedObjsFolderTreeNode.h"
#include "menu_helpers.h"

#include "ISceneViewFacade.h"

#include "qd_named_object.h"


AnimatedObjsFolderTreeNode::AnimatedObjsFolderTreeNode()
: FolderTreeNode(QD_NAMED_OBJECT_ANIMATED_OBJ)
{
}

AnimatedObjsFolderTreeNode::~AnimatedObjsFolderTreeNode(void)
{
}

bool AnimatedObjsFolderTreeNode::load(ITreeNodeFactory* factory, 
								  TreeLogic& treeLogic, 
								  ITreeNode * parent, 
								  qdNamedObjectBase const* owner,
								  ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_ANIMATED_OBJS_NODE);
	CTreeListItem* const item = insertToTree(treeLogic, str, parent, 
										II_ANIMATED_OBJS_NODE, 
										getFolderId(), insertAfter);
	return (item != NULL);
}

//! ¬озвращает идентификатор ресурса меню дл€ узла
DWORD AnimatedObjsFolderTreeNode::getMenuResId() const{
	return IDR_ANIMATED_OBJS_NODE;
}

void AnimatedObjsFolderTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if(treeLogic.getCurrentSceneNode() == NULL||!treeLogic.isSceneViewActive())
		menu_h::disableAllMenuItems(pm);
	// Ќет причин скрывать сворачивающие, раскрывающие пункты  - активируем их
	menu_h::enableMenuItem(pm, IDC_COLLAPSE_ALL);
	menu_h::enableMenuItem(pm, IDC_EXPAND_ALL);
}

bool AnimatedObjsFolderTreeNode::onStateChange(TreeLogic& logic, 
											   DWORD oldState, 
											   DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	bool bSelect = newState&NS_FOCUSED;
	if (logic.isSceneViewActive())
	{
		if (bSelect) {
			logic.getSceneView()->SelectObj(NULL);
			logic.getSceneView()->SetViewState(VS_EDIT_SCENE);
			return true;
		}
	}
	return false;
}
