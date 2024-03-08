#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\movingobjecttreenode.h"
#include "menu_helpers.h"

#include "animobjstates.h"
#include "characterproppage.h"
#include "inventoryppforpers.h"
#include "objposproppage.h"
#include "PersonageControlPropPage.h"
#include "charbound.h"

#include "ISceneViewFacade.h"
#include "ITriggerView.h"

#include <boost/rtl.hpp>

#include "Main\StateTraits.h"
#include "Tree/NodeTraits.h"

#include "qd_game_object_moving.h"
#include "qd_game_dispatcher.h"

#include "Main/ObjectTraits.h"

/// Функтор. Вставляет состояние персонажа в дерево
class MovingObjectTreeNode::InsertState
{
public:
	InsertState(ITreeNodeFactory* factory, 
				TreeLogic& treeLogic, 
				ITreeNode * staticStateCatalog, 
				ITreeNode * walkStateCatalog,
				qdNamedObjectBase const* owner,
				ITreeNode* insertAfter)
				: factory_(factory)
				, treeLogic_(treeLogic)
				, staticStateCatalog_(staticStateCatalog)
				, walkStateCatalog_(walkStateCatalog)
				, owner_(owner)
				, insertAfter_(insertAfter)
	{
	}
	bool operator()(qdGameObjectState* state)
	{
		if (StateTraits::isWalkState(state))
			return insert(state, walkStateCatalog_);
		return insert(state, staticStateCatalog_);
	}
protected:
	/// Производит вставку
	bool insert(qdGameObjectState* state, ITreeNode * parent)
	{
		std::auto_ptr<ITreeNode> p = factory_->createGameObjectNode(state);
		if (!p.get()||!p->load(factory_, treeLogic_, parent, owner_, insertAfter_))
			return false;
		p.release();
		return true;
	}
private:
	/// Фабрика узлов
	ITreeNodeFactory* factory_;
	/// Логика дерева
	TreeLogic& treeLogic_;
	/// Узел папки станических состояний
	ITreeNode * staticStateCatalog_;
	/// Узел папки походочных состояний
	ITreeNode * walkStateCatalog_;
	/// Владелец состояния
	qdNamedObjectBase const* owner_;
	/// После этого узла вставлять
	ITreeNode* insertAfter_;
};

MovingObjectTreeNode::MovingObjectTreeNode(qdNamedObject* object)
: AnimatedObjectTreeNode(object)
{
}

MovingObjectTreeNode::~MovingObjectTreeNode(void)
{
}

bool MovingObjectTreeNode::load(ITreeNodeFactory* factory, 
								TreeLogic& treeLogic, 
								ITreeNode * parent, 
								qdNamedObjectBase const* owner,
								ITreeNode* insertAfter)
{
	if (!insertToTree(treeLogic, getObject()->name(), 
							parent, II_MOVING_OBJ_NODE,
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

bool MovingObjectTreeNode::loadStates(ITreeNodeFactory* factory, 
									  TreeLogic& treeLogic) 
{
	std::auto_ptr<ITreeNode> p = factory->createFolderNode(QE_STATE_CATALOG);
	if (!p.get()||!p->load(factory, treeLogic, this, NULL))
		return false;
	ITreeNode* staticStates = p.release();
	p = factory->createFolderNode(QE_MOVEMENT_STATE_CATALOG);
	if (!p.get()||!p->load(factory, treeLogic, this, NULL))
		return false;
	ITreeNode* walkStates = p.release();

	return loadStates(factory, treeLogic, staticStates, walkStates);
}

bool MovingObjectTreeNode::loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic,
				ITreeNode* staticStates, ITreeNode* walkStates) 
{
	boost::rtl::rng::for_each(
		static_cast<qdGameObjectMoving*>(getObject())->state_vector(),
		InsertState(factory, treeLogic, 
		staticStates,
		walkStates,
		getObject(),
		TN_INSERT_SORT));
	treeLogic.expand(staticStates, TreeLogic::EC_EXPAND);
	treeLogic.expand(walkStates, TreeLogic::EC_EXPAND);
	return true;
}

bool MovingObjectTreeNode::reload(ITreeNodeFactory* factory, TreeLogic& logic, 
			ITreeNode const* parent, qdNamedObjectBase const* owner)
{
	if (getTreeListItem())
	{
		ITreeNode* staticStates = logic.getSubfolder(this, QE_STATE_CATALOG);
		ITreeNode* walkStates = logic.getSubfolder(this, QE_MOVEMENT_STATE_CATALOG);
		ASSERT(staticStates&&walkStates);
		logic.removeSubnodes(staticStates);
		logic.removeSubnodes(walkStates);
		return loadStates(factory, logic, staticStates, walkStates);
	}
	else
	{
		ASSERT(parent);
		return load(factory, logic, const_cast<ITreeNode*>(parent), owner, TN_INSERT_SORT);
	}
}

//! Возвращает идентификатор ресурса меню для узла
DWORD MovingObjectTreeNode::getMenuResId() const{
	return IDR_MOVING_OBJ_NODE;
}

void MovingObjectTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	menu_h::checkMenuItem(pm, IDC_NOT_CONTROLED_PERS, 
		getObject()->check_flag(QD_OBJ_NON_PLAYER_PERSONAGE_FLAG));
	if (treeLogic.getGameDispatcher()->get_active_scene() != getObject()->owner())
		menu_h::disableMenuItem(pm, IDC_REMOVE_EDGES_IN_STATES);
}

bool MovingObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<ObjPosPropPage> pAPage (new ObjPosPropPage);
	qdGameObjectMoving* const movingObj = static_cast<qdGameObjectMoving*>(getObject());
	pAPage->set_data(logic.getGameDispatcher(), movingObj);
	ptrPages[0] = pAPage.get();

	std::auto_ptr<CharBound> ptrCB ( new CharBound);
	ptrCB->set_data(logic.getGameDispatcher(), movingObj);
	ptrPages[1] = ptrCB.get();

	std::auto_ptr<InventoryPPForPers> ptrIPP( new InventoryPPForPers);
	ptrIPP->set_data(logic.getGameDispatcher(), movingObj);
	ptrPages[2] = ptrIPP.get();

	std::auto_ptr<PersonageControlPropPage> ptrPCP( new PersonageControlPropPage);
	ptrPCP->set_data(movingObj);
	ptrPages[3] = ptrPCP.get();

	return showPropDlg(ptrPages, iActive, IDS_PPCAP_MOVING_OBJ_PROPERTIES);
}

bool MovingObjectTreeNode::onStateChange(TreeLogic& logic, 
										 DWORD oldState, 
										 DWORD newState)
{
	if (!getObject())
		return false;
	bool bSelect = newState&NS_FOCUSED;
	if (bSelect) {
		if (logic.isSceneViewActive())
		{
			ISceneViewFacade* sceneFacade = logic.getSceneView();
			sceneFacade->SelectObj(static_cast<qdGameObject*>(getObject()));
			sceneFacade->SetViewState(VS_EDIT_SCENE);
		}
		return true;
	}
	else 
	{
		if (logic.isSceneViewActive())
			logic.getSceneView()->SelectObj(NULL);
	}

	return false;
}

bool MovingObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
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

//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
// при его удаление(сотояния, когда удаляется анимированный объект. 
//Все объекты, когда удаляется сцена)
bool MovingObjectTreeNode::deleteAsSubObject(TreeLogic& logic)
{
	qdGameScene *pscene = static_cast<qdGameScene*>(getObject()->owner());

	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	qdGameObject* pGlobalObj = 
		ObjectTraits::isCopyOfGlobal(gameDisp, static_cast<qdGameObject*>(getObject()));
	if (pGlobalObj) 
		gameDisp->split_global_objects(pGlobalObj);

	pscene->set_active_personage(NULL);
	deleteStates(logic);
	bool res = removeObject(logic);
	if (pGlobalObj)
		gameDisp->merge_global_objects(pGlobalObj);
	return res;
}

DROPEFFECT MovingObjectTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	logic.selectDropTarget(NULL);
	if (ITreeNode* catalog = logic.isNodeUnderMovingCatalog(overNode)) 
	{
		ITreeNode* sceneNode = logic.getParentNode(catalog);
		if (TreeNodeTraits::isSceneNode(sceneNode))
		{
			qdGameScene* pscene = 
				static_cast<qdGameScene*>(sceneNode->getContent().gameObject);
			if (pscene == getObject()->owner()) {
				logic.selectDropTarget(catalog);
				return DROPEFFECT_COPY;
			}
		}
	}
	return DROPEFFECT_NONE;
}

/*!
 	Если персонаж - копия глобального пероснажа, то убираем у него глобальные состояния.
	Затем созадем копию (copyAnimatedToMoving()). Восстанавливаем глобальные состояния
	\retval TRUE - если процесс завершился удачно
*/

DROPEFFECT MovingObjectTreeNode::onDropData(TreeLogic& logic, ITreeNode* dropTarget, 
							  DROPEFFECT dropEffect)
{
	ITreeNode* parent = logic.getParentNode(dropTarget);
	if (TreeNodeTraits::isSceneNode(parent))
	{
		qdGameScene* pscene = static_cast<qdGameScene*>(parent->getContent().gameObject);
		bool bSplitting = false;
		qdGameDispatcher* gameDisp = logic.getGameDispatcher();
		if (ObjectTraits::isCopyOfGlobal(gameDisp, 
						static_cast<qdGameObject*>(getObject())))
		{
			pscene->split_global_objects(gameDisp->get_global_object(getObject()->name()));
			bSplitting = true;
		}
		bool res = logic.copyAnimatedToMoving(dropTarget, 
							static_cast<qdGameObjectAnimated*>(getObject()), pscene);
		if (bSplitting)
			pscene->merge_global_objects(gameDisp->get_global_object(getObject()->name()));

		return res ? DROPEFFECT_COPY : DROPEFFECT_NONE;
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

void MovingObjectTreeNode::renameScale(qdNamedObject const* obj, 
									   LPCTSTR oldName, 
									   LPCTSTR newName)
{
	//убираем и добавляем заново масштаб
	qdGameScene* pscene = dynamic_cast<qdGameScene*>(obj->owner());
	if (pscene)
	{
		pscene->remove_object_scale(oldName);
		pscene->set_object_scale(newName,
			static_cast<qdGameObjectMoving const*>(obj)->scale());
	}
}

bool MovingObjectTreeNode::doRename(TreeLogic& logic, 
									qdNamedObject* object, 
									LPCTSTR newName)
{
	renameScale(object, object->name(), newName);
	return static_cast<qdGameScene*>(object->owner())->
		rename_object(static_cast<qdGameObject*>(object), newName);
}

void MovingObjectTreeNode::updateItemImage(TreeLogic& treeLogic)
{

}
