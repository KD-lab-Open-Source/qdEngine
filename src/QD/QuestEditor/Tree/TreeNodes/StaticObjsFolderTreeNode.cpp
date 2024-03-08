#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\staticobjsfoldertreenode.h"

#include "ISceneViewFacade.h"

#include "menu_helpers.h"

#include "qd_named_object.h"

StaticObjsFolderTreeNode::StaticObjsFolderTreeNode()
: FolderTreeNode(QD_NAMED_OBJECT_STATIC_OBJ)
{
}

StaticObjsFolderTreeNode::~StaticObjsFolderTreeNode(void)
{
}

bool StaticObjsFolderTreeNode::load(ITreeNodeFactory* factory, 
									TreeLogic& treeLogic, 
									ITreeNode * parent, 
									qdNamedObjectBase const* owner,
									ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_STATIC_OBJS_NODE);
	CTreeListItem* const item = 
		insertToTree(treeLogic, str, parent, II_STATIC_OBJS_NODE, 
					getFolderId(), insertAfter);
	return (item != NULL);
}
//! Возвращает идентификатор ресурса меню для узла
DWORD StaticObjsFolderTreeNode::getMenuResId() const{
	return IDR_STATIC_OBJS_NODE;
}

void StaticObjsFolderTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if(treeLogic.getCurrentSceneNode() == NULL||!treeLogic.isSceneViewActive())
		menu_h::disableMenuItem(pm, IDC_CRT_STATIC_OBJ);
}

bool StaticObjsFolderTreeNode::onStateChange(TreeLogic& logic, 
											 DWORD oldState, 
											 DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	bool bSelect = newState&NS_FOCUSED;
	ISceneViewFacade* sceneFacade = logic.getSceneView();
	if (bSelect) {
		sceneFacade->SelectObj(NULL);
		sceneFacade->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	return false;
}
