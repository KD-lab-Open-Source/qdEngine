#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\staticobjecttreenode.h"

#include "paralaxpage.h"
#include "staticobjpage.h"

#include "menu_helpers.h"

#include "qd_game_object_static.h"
#include "ISceneViewFacade.h"

StaticObjectTreeNode::StaticObjectTreeNode(qdNamedObject* object)
: GameObjectTreeNode(object)
{
}

StaticObjectTreeNode::~StaticObjectTreeNode(void)
{
}

int StaticObjectTreeNode::selectPicture(qdNamedObject const* object)
{
	if (object->check_flag(QD_OBJ_DISABLE_MOVEMENT_FLAG))
		return II_NOT_MOV_STATIC_OBJ_NODE;
	return II_STATIC_OBJ_NODE;
}

bool StaticObjectTreeNode::load(ITreeNodeFactory* factory, 
								TreeLogic& treeLogic, 
								ITreeNode * parent, 
								qdNamedObjectBase const* owner,
								ITreeNode* insertAfter)
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();
	if (!insertToTree(treeLogic, 
									getObject()->name(), 
									parent, 
									selectPicture(getObject()),
									reinterpret_cast<DWORD_PTR>(getObject()), 
									insertAfter))
	{
		return false;
	}
	showCheckBox(treeLogic, true);
	setCheck(treeLogic, !getObject()->check_flag(QD_OBJ_HIDDEN_FLAG));
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD StaticObjectTreeNode::getMenuResId() const{
	return IDR_STATIC_OBJ_NODE;
}

void StaticObjectTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if (treeLogic.getGameDispatcher()->get_active_scene() 
		!= static_cast<qdGameScene*>(getObject()->owner()))
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES);

	if (!treeLogic.isSceneViewActive())
		menu_h::disableMenuItem(pm, IDC_CREATE_MASK);
}

bool StaticObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::auto_ptr<StaticObjPage> pPage ( new StaticObjPage);
	pPage->set_data(logic.getGameDispatcher(),
				static_cast<qdGameObjectStatic*>(getObject()));
	ptrPages[0] = pPage.get();

	std::auto_ptr<ParalaxPage> pParalax ( new ParalaxPage);
	pParalax->set_data(static_cast<qdGameObject*>(getObject()));
	ptrPages[1] = pParalax.get();

	return showPropDlg(ptrPages, iActive, IDS_PPCAP_STATIC_OBJ_PROPERTIES);
}

void StaticObjectTreeNode::updateItemImage(TreeLogic& treeLogic)
{
	treeLogic.getTreeListCtrl().SetItemImage(
		getTreeListItem(), selectPicture(getObject()));
}

bool StaticObjectTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, 
										 DWORD newState)
{
	if (!getObject())
		return false;
	bool bSelect = newState&NS_FOCUSED;
	ISceneViewFacade* sceneFacade = logic.getSceneView();
	if (bSelect) {
		sceneFacade->SelectObj(static_cast<qdGameObject*>(getObject()));
		sceneFacade->SetViewState(VS_EDIT_SCENE);
		return true;
	}
	else 
	{	
		if (sceneFacade->GetViewState() == VS_EDIT_MASK) 
			sceneFacade->OnAbortStateMaskEdit(false);
		else
			sceneFacade->SelectObj(NULL);
	}
	return false;
}
