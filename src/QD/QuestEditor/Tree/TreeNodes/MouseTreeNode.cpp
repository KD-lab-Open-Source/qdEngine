#include "stdafx.h"
#include "resource.h"
#include ".\mousetreenode.h"

#include "animobjstates.h"
#include "ISceneViewFacade.h"
#include "ptree_consts.h"

#include "qd_game_object_mouse.h"

MouseTreeNode::MouseTreeNode(qdNamedObject* object)
: AnimatedObjectTreeNode(object)
{
}

MouseTreeNode::~MouseTreeNode(void)
{
}

bool MouseTreeNode::load(ITreeNodeFactory* factory, 
						 TreeLogic& treeLogic, 
						 ITreeNode * parent,
						 qdNamedObjectBase const* owner,
						 ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_MOUSE_OBJ_NODE);
	CTreeListItem* item = insertToTree(treeLogic,
									str,
									parent,
									II_MOUSE_NODE,
									reinterpret_cast<DWORD_PTR>(getObject()), 
									insertAfter);
	if (!item)
		return false;
	loadStates(factory, treeLogic);
	return true;
}

bool MouseTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<AnimObjStates> ptrAOS ( new AnimObjStates);
	ptrAOS->set_data(logic.getGameDispatcher(), 
			static_cast<qdGameObjectAnimated*>(getObject()));
	ptrPages[0] = ptrAOS.get();
	return showPropDlg(ptrPages, iActive, IDS_PPCAP_MOUSE_OBJ_PROPERTIES);
}

bool MouseTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!getObject())
		return false;
	if (newState&NS_FOCUSED) {
		logic.getSceneView()->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	return false;
}

DWORD MouseTreeNode::getMenuResId() const{
	return IDR_MOUSE_NODE;
}

bool MouseTreeNode::canDrag(TreeLogic& logic){
	return false;
}

//! Возвращает true, если удалось переимновать, false в противном случае
bool MouseTreeNode::canRename(TreeLogic& logic){
	return false;
}
