#include "stdafx.h"
#include "ptree_consts.h"
#include "resource.h"
#include ".\animatedobjecttreenode.h"
#include "menu_helpers.h"

#include "objposproppage.h"
#include "animobjstates.h"
#include "paralaxpage.h"
#include "inventorypp.h"

#include "qd_game_object_animated.h"

#include "ITriggerView.h"
#include "ISceneViewFacade.h"

#include "Tree/NodeTraits.h"
#include "ptree_hlprs.hpp"

#include "ptree_te_filters.h"

#include <boost/rtl.hpp>

#include "Main/Utils/UpdateTriggerElementTitles.h"

class AnimatedObjectTreeNode::InsertState
{
public:
	InsertState(ITreeNodeFactory* factory, 
		TreeLogic& treeLogic, 
		ITreeNode * parent,
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter)
		: factory_(factory)
		, treeLogic_(treeLogic)
		, parent_(parent)
		, owner_(owner)
		, insertAfter_(insertAfter)
	{
	}
	bool operator()(qdNamedObject* object)
	{
		std::auto_ptr<ITreeNode> node(factory_->createGameObjectNode(object));
		if (!node.get()||!node->load(factory_, treeLogic_, parent_, owner_, insertAfter_))
			return false;
		node.release();
		return true;
	}
private:
	ITreeNodeFactory* factory_;
	TreeLogic& treeLogic_;
	ITreeNode * parent_;
	qdNamedObjectBase const* owner_;
	ITreeNode* insertAfter_;
};

AnimatedObjectTreeNode::AnimatedObjectTreeNode(qdNamedObject* object)
: GameObjectTreeNode(object)
{
}

AnimatedObjectTreeNode::~AnimatedObjectTreeNode(void)
{
}

//! Возвращает идентификатор ресурса меню для узла
DWORD AnimatedObjectTreeNode::getMenuResId() const{
	return IDR_ANIMATED_OBJ_NODE;
}

int AnimatedObjectTreeNode::selectPicture(qdNamedObject const* object)
{
	if (object->check_flag(QD_OBJ_DISABLE_MOUSE_FLAG)) 
		return II_DISABLE_MOUSE_ANIMATED_OBJ_NODE;
	else
		return II_ANIMATED_OBJ_NODE;
}

bool AnimatedObjectTreeNode::load(ITreeNodeFactory* factory, 
								  TreeLogic& treeLogic, 
								  ITreeNode* parent, 
								  qdNamedObjectBase const* owner,
								  ITreeNode* insertAfter)
{
	if (!insertToTree(treeLogic, getObject()->name(), 
					parent, selectPicture(getObject()),
					reinterpret_cast<DWORD_PTR>(getObject()), 
					insertAfter))
	{
		return false;
	}

	showCheckBox(treeLogic, true);
	setCheck(treeLogic, !getObject()->check_flag(QD_OBJ_HIDDEN_FLAG));

	loadStates(factory, treeLogic);
	treeLogic.expand(this, TreeLogic::EC_EXPAND);
	return true;
}

bool AnimatedObjectTreeNode::loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic)
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();

	qdGameObjectAnimated* const object = static_cast<qdGameObjectAnimated*>(getObject());
	boost::rtl::rng::for_each(object->state_vector(), 
						InsertState(factory, treeLogic, 
								this, getObject(), TN_INSERT_SORT));
	return true;
}

bool AnimatedObjectTreeNode::reload(ITreeNodeFactory* factory, 
									TreeLogic& logic, 
									ITreeNode const* parent, 
									qdNamedObjectBase const* owner)
{
	if (getTreeListItem())
	{
		logic.removeSubnodes(this);
		return loadStates(factory, logic);
	}
	else
		return load(factory, logic, const_cast<ITreeNode*>(parent), owner, TN_INSERT_SORT);
}

void AnimatedObjectTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	menu_h::checkMenuItem(pm, IDC_DISABLE_MOUSE, 
		getObject()->check_flag(QD_OBJ_DISABLE_MOUSE_FLAG));

	if (treeLogic.getGameDispatcher()->get_active_scene() 
		!= static_cast<qdGameScene*>(getObject()->owner()))
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES_IN_STATES);
}

bool AnimatedObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	qdGameObjectAnimated* pao = 
		static_cast<qdGameObjectAnimated*>(getObject());
	std::auto_ptr<ObjPosPropPage> pAPage ( new ObjPosPropPage);
	pAPage->set_data(logic.getGameDispatcher(), pao);
	ptrPages[0] = pAPage.get();

	std::auto_ptr<AnimObjStates> ptrAOS (new AnimObjStates);
	ptrAOS->set_data(logic.getGameDispatcher(), pao);
	ptrPages[1] = ptrAOS.get();

	std::auto_ptr<ParalaxPage> pParalax ( new ParalaxPage);
	pParalax->set_data(pao);
	ptrPages[2] = pParalax.get();

	std::auto_ptr<InventoryPP> pIPP(new InventoryPP);
	pIPP->set_data(logic.getGameDispatcher(), pao);
	ptrPages[3] = pIPP.get();
	return showPropDlg(ptrPages, iActive, IDS_PPCAP_ANIMATED_OBJ_PROPERTIES);
}

void AnimatedObjectTreeNode::updateItemImage(TreeLogic& treeLogic)
{
	treeLogic.getTreeListCtrl().SetItemImage(
		getTreeListItem(), selectPicture(getObject()));

}

bool AnimatedObjectTreeNode::onSelectInTriggerView(TreeLogic& logic, bool bSelect)
{
	if (bSelect)
	{
		switch(logic.getFilterScheme()) {
		case TreeLogic::FS_NONE:
			logic.getTriggerView()->SelectByFilter(tef_by_owner(getObject()), 
				ITriggerView::SA_SELECT,
				ITriggerView::SM_EXCLUSIVE);
			break;
		}
		return true;
	}
	return false;
}

bool AnimatedObjectTreeNode::onSelectInSceneView(TreeLogic& logic, bool bSelect)
{
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

		sceneFacade->SelectObj(NULL);
	}

	return false;
}

bool AnimatedObjectTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, 
										   DWORD newState)
{
	if (!getObject())
		return false;
	bool bSelect = newState&NS_FOCUSED;
	if (logic.isSceneViewActive())
		return onSelectInSceneView(logic, bSelect);
	else
		return onSelectInTriggerView(logic, bSelect);
	return false;
}


bool AnimatedObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!__super::preDelete(logic, ask, statDepends))
		return false;
	if (deleteAsSubObject(logic))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

//! Удаляем все состояния объекта
void AnimatedObjectTreeNode::deleteStates(TreeLogic& logic)
{
	qdGameObjectAnimated* obj = static_cast<qdGameObjectAnimated*>(getObject());

	qdGameObjectStateVector const& v = obj->state_vector();
	while (!v.empty())
	{
		ITreeNode* node = *logic.getObjectNodes(v.front());
		assert(node);
		node->deleteAsSubObject(logic);
	}
}

//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
// при его удаление(сотояния, когда удаляется анимированный объект. 
//Все объекты, когда удаляется сцена)
bool AnimatedObjectTreeNode::deleteAsSubObject(TreeLogic& logic)
{
	deleteStates(logic);
	return __super::deleteAsSubObject(logic);
}

bool AnimatedObjectTreeNode::canDrag(TreeLogic& logic){
	return true;
}

DROPEFFECT AnimatedObjectTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	ITreeNode* catalog = NULL;
	if ((catalog = logic.isNodeUnderAnimatedCatalog(overNode))
		||(catalog = logic.isNodeUnderMovingCatalog(overNode))) 
	{
		ITreeNode *sceneNode = logic.getParentNode(catalog);
		if (TreeNodeTraits::isSceneNode(sceneNode))
		{
			qdGameScene* pscene = 
				static_cast<qdGameScene*>(sceneNode->getContent().gameObject);
			if (pscene == getObject()->owner()) {
				logic.selectDropTarget(catalog);
				if (logic.isNodeUnderAnimatedCatalog(catalog)
					||ptree_helpers::IsKeyPressed(VK_CONTROL))
					return DROPEFFECT_COPY;
				else
					return DROPEFFECT_MOVE;
			}
		}
		else
		{

		}
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

DROPEFFECT AnimatedObjectTreeNode::onDropData(TreeLogic& logic, 
							  ITreeNode* dropTarget, 
							  DROPEFFECT dropEffect)
{
	ITreeNode* parent = logic.getParentNode(dropTarget);
	if (TreeNodeTraits::isSceneNode(parent))
	{
		qdGameScene* const pscene = 
			static_cast<qdGameScene*>(parent->getContent().gameObject);

		qdGameObjectAnimated* pobj = static_cast<qdGameObjectAnimated*>(getObject());
		if (logic.isNodeUnderAnimatedCatalog(dropTarget))
		{
			if (logic.copyAnimatedToAnimated(dropTarget, pobj, pscene))
				return DROPEFFECT_COPY;
			else
				return DROPEFFECT_NONE;
		}
		else if (logic.isNodeUnderMovingCatalog(dropTarget))
		{
			BOOL const res = logic.copyAnimatedToMoving(dropTarget, pobj, pscene);
			if (dropEffect == DROPEFFECT_MOVE)
			{
				if (statObjectDependencies(logic, getObject()))
					return DROPEFFECT_COPY;

				if (deleteAsSubObject(logic))
					return DROPEFFECT_MOVE;
				else
					return DROPEFFECT_COPY;
			}
			return DROPEFFECT_COPY;

		}
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

void AnimatedObjectTreeNode::updateTriggerElementTitles(qdGameDispatcher const* gameDisp, 
									   qdGameObjectAnimated const* object)
{
	UpdateTriggerElementTitles::updateForAnimated(gameDisp->trigger_chain_list(), object);
}

//! Возвращает true, если удалось переимновать, false в противном случае
bool AnimatedObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	CString oldName(getObject()->name());
	if (__super::rename(logic, newName))
	{
		updateTriggerElementTitles(logic.getGameDispatcher(), 
			static_cast<qdGameObjectAnimated *>(getObject()));
		logic.getTriggerView()->Redraw();
		return true;
	}
	return false;
}
