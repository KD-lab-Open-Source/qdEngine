#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\gridzonetreenode.h"

#include "menu_helpers.h"
#include "zoneproppage.h"

#include "qd_game_object.h"
#include "qd_grid_zone.h"

#include "Main/Utils/GridZoneUtils.h"
#include "Main/Utils/UpdateTriggerElementTitles.h"

#include "Tree/NodeTraits.h"
#include "name_op.h"

#include "ISceneViewFacade.h"

boost::scoped_ptr<qdGridZone> GridZoneTreeNode::zoneForEdit_;

GridZoneTreeNode::GridZoneTreeNode(qdNamedObject* zone)
: NamedObjectTreeNode(zone)
{
}

GridZoneTreeNode::~GridZoneTreeNode(void)
{
}

int GridZoneTreeNode::selectPicture(qdNamedObject const* pgz)
{
	if (pgz->check_flag(qdGridZone::ZONE_EXIT_FLAG))
		return II_GZONE_NEED_CUR_NODE;
	else
		return II_ZONE_NODE;
}



bool GridZoneTreeNode::load(ITreeNodeFactory* factory, 
							TreeLogic& treeLogic, 
							ITreeNode * parent, 
							qdNamedObjectBase const* owner,
							ITreeNode* insertAfter)
{
	if (!insertToTree(treeLogic, 
							getObject()->name(), 
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

bool GridZoneTreeNode::loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic)
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();

	qdGridZone* const zone = static_cast<qdGridZone*>(getObject());
	std::auto_ptr<ITreeNode> node = 
		factory->createGridZoneStateNode(qdGridZoneState::ZONE_STATE_OFF_NAME,
								zone->get_state(qdGridZoneState::ZONE_STATE_OFF_NAME));

	if (!node.get()||!node->load(factory, treeLogic, this, NULL))
		return false;

	node.release();
	node = factory->createGridZoneStateNode(qdGridZoneState::ZONE_STATE_ON_NAME,
						zone->get_state(qdGridZoneState::ZONE_STATE_ON_NAME));
	if (!node.get()||!node->load(factory, treeLogic, this, NULL))
		return false;

	node.release();
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD GridZoneTreeNode::getMenuResId() const{
	return IDR_GRID_ZONE_NODE;
}

void GridZoneTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
	if(treeLogic.getCurrentSceneNode() == NULL
		||!treeLogic.isSceneViewActive())
		menu_h::disableMenuItem(pm, IDC_CRT_GRID_ZONE);
}

void GridZoneTreeNode::updateItemImage(TreeLogic& treeLogic)
{
	qdGridZone* const zone = static_cast<qdGridZone*>(getObject());

	treeLogic.getTreeListCtrl().SetItemImage(getTreeListItem(), selectPicture(zone));
}

bool GridZoneTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	ASSERT(getObject());
	qdGridZone& gridZone = *static_cast<qdGridZone*>(getObject());

	if (!zoneForEdit_)
		zoneForEdit_.reset(new qdGridZone(gridZone));

	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<ZonePropPage> ptrZPP ( new ZonePropPage);
	ptrPages[0] = ptrZPP.get();

	ptrZPP->set_data(logic.getGameDispatcher(), &gridZone);
	if(showPropDlg(ptrPages, 1, IDS_GRID_ZONE_PROPERTIES))
	{
		*zoneForEdit_ = gridZone;
		ZonePropPage::ReapplyZones(static_cast<qdGameScene*>(gridZone.owner()));
		return true;
	}
	ZonePropPage::ReapplyZones(static_cast<qdGameScene*>(gridZone.owner()));
	return false;
}

bool GridZoneTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!getObject())
		return false;
	if (!logic.isSceneViewActive())
		return false;
	ISceneViewFacade* sceneView = logic.getSceneView();
	if (newState&NS_FOCUSED) {
		sceneView->SetViewState(VS_EDIT_NET);
		sceneView->SetSubTools(SSTE_EDIT_ZONE);
		qdGridZone* ptrZone = static_cast<qdGridZone*>(getObject());

		if (!zoneForEdit_) 
			zoneForEdit_.reset(new qdGridZone);

		*zoneForEdit_ = *ptrZone;
		sceneView->OnEditGridZone(ptrZone, this);
		return true;
	}
	else if (oldState&NS_FOCUSED)
	{
		sceneView->OnAbortGridZoneEdit(false);
		qdGridZone* ptrZone = static_cast<qdGridZone*>(getObject());
		*ptrZone = *zoneForEdit_;
		zoneForEdit_.reset();
	}
	return false;
}

bool GridZoneTreeNode::onSave(bool bSave)
{
	if (!zoneForEdit_)
		return false;

	qdGridZone * ptrZone = static_cast<qdGridZone*>(getObject());
	if (bSave) {
		*zoneForEdit_ = *ptrZone;
	}
	else
		*ptrZone = *zoneForEdit_;
	return true;
}

bool GridZoneTreeNode::onCheckNode(TreeLogic& logic, bool bCheck)
{
	qdGridZone* const pzone = static_cast<qdGridZone*>(getObject());
	ITreeNode* scene = logic.isNodeUnderScene(this);
	ISceneViewFacade* const sceneView = logic.getSceneView();

	if (bCheck) 
		pzone->drop_flag(QD_OBJ_HIDDEN_FLAG);
	else
		pzone->set_flag(QD_OBJ_HIDDEN_FLAG);

	//если это зона, то надо убрать выделение с сетки и контур этой зоны
	if (sceneView->GetViewState()==VS_EDIT_NET) {

		if (sceneView->GetSubTools()==SSTE_NONE
			||sceneView->GetSubTools()==SSTE_EDIT_ZONE_AND_SHOW_ALL) 
		{
			//если рисуем все зоны
			if(!bCheck)
				pzone->select(false);

			if (scene == logic.getCurrentSceneNode())
			{
				GridZoneUtils::reselectZones(
					*static_cast<qdGameScene*>(scene->getContent().gameObject));
			}
		}
		else if (sceneView->GetSubTools() == SSTE_EDIT_ZONE) 
		{
			if(zoneForEdit_&&zoneForEdit_->name()
				&&!_tcscmp(zoneForEdit_->name(), pzone->name()))
			{
				//если галочку убрали у зоны, 
				//которая в данный момент редактируется,то...
				pzone->select(!pzone->check_flag(QD_OBJ_HIDDEN_FLAG));

				if(bCheck)
					zoneForEdit_->drop_flag(QD_OBJ_HIDDEN_FLAG);
				else
					zoneForEdit_->set_flag(QD_OBJ_HIDDEN_FLAG);
			}
		}
	}
	return true;
}


bool GridZoneTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!getObject()) 
		return true;

	if (!preDelete(logic, ask, statDepends)) 
		return false;
	logic.getSceneView()->OnAbortGridZoneEdit(false);
	if (deleteAsSubObject(logic))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

/*!
Удаляет состояния зоны из триггеров
*/
void GridZoneTreeNode::removeGridZoneFromTriggers(qdGridZone *zone, 
												  qdGameDispatcher* gameDisp)
{
	qdGridZoneState const* state = zone->get_state(qdGridZoneState::ZONE_STATE_OFF_NAME);
	if (state->is_in_triggers())
		removeObjectFromTriggers(state, gameDisp);
	state = zone->get_state(qdGridZoneState::ZONE_STATE_ON_NAME);
	if (state->is_in_triggers())
		removeObjectFromTriggers(state, gameDisp);
}

//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
// при его удаление(сотояния, когда удаляется анимированный объект. 
//Все объекты, когда удаляется сцена)
bool GridZoneTreeNode::deleteAsSubObject(TreeLogic& logic)
{
	if (!getObject()) 
		return true;

	qdGridZone* const zone = static_cast<qdGridZone*>(getObject());
	zone->set_state(false);
	removeGridZoneFromTriggers(zone, logic.getGameDispatcher());

	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());

	if (scene->remove_grid_zone(zone))
	{
		qdGridZoneState const* state = 
			zone->get_state(qdGridZoneState::ZONE_STATE_OFF_NAME);
		ITreeNode* node = *logic.getObjectNodes(state);
		node->deleteObject(logic, false, false);

		state = zone->get_state(qdGridZoneState::ZONE_STATE_ON_NAME);

		node = *logic.getObjectNodes(state);
		node->deleteObject(logic, false, false);

		delete getObject();
//		setObject(NULL);
		return true;
	}
	return false;
}

bool GridZoneTreeNode::canDrag(TreeLogic& logic){
	return true;
}

/*!
 	Сеточные зоны можем только переносить внутри их каталога, т.е. менять зоны местами.
	Переностиь зоны в другую сцену нельзя
	\param overNode			- Узел, над которым бросили
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
*/

DROPEFFECT GridZoneTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	logic.selectDropTarget(NULL);
	if (overNode == this)
		return DROPEFFECT_NONE;

	if (logic.getParentNode(overNode) == logic.getDraggedParentNode()
		||overNode == logic.getDraggedParentNode())
	{
		logic.selectDropTarget(overNode);
		return DROPEFFECT_MOVE;
	}
	else
		return DROPEFFECT_NONE;
}

/*!
 	Зоны не копируются. Меняется только их порядок в дереве.
	Сначала добавляет копию перетаскиваемой зоны, а затем удаляет исходную
	\retval TRUE - если процесс завершился удачно
*/

DROPEFFECT GridZoneTreeNode::onDropData(TreeLogic& logic, ITreeNode* dropTarget, 
							  DROPEFFECT dropEffect)
{
	qdGameScene* pscene = static_cast<qdGameScene*>(getObject()->owner());


	qdGridZone* before = *pscene->grid_zone_list().begin();

	ITreeNode* insertAfter = dropTarget;
	if (!TreeNodeTraits::isNet(dropTarget))//навели не на узел "Сетка"
	{
		ITreeNode* nodeBefore = logic.getNextSiblingNode(dropTarget);
		if (!nodeBefore)
			before = NULL;
		else
			before = static_cast<qdGridZone*>(nodeBefore->getContent().gameObject);
	}
	else
		insertAfter = TN_INSERT_FIRST;

	qdGridZone* zone = static_cast<qdGridZone*>(getObject());

	setObject(NULL);

	bool res = logic.moveZone(pscene, 
		zone, 
		before, 
		insertAfter, logic.getDraggedParentNode());

	return res ? DROPEFFECT_MOVE : DROPEFFECT_NONE;
}

bool GridZoneTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool GridZoneTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;

	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());
	if (scene->rename_grid_zone(static_cast<qdGridZone*>(getObject()), newName))
	{
		if (zoneForEdit_)
			zoneForEdit_->set_name(newName);

		UpdateTriggerElementTitles::updateForZone(
			logic.getGameDispatcher()->trigger_chain_list(),
			static_cast<qdGridZone*>(getObject()));
		return true;
	}
	return false;
}

bool GridZoneTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameScene* scene = static_cast<qdGameScene*>(getObject()->owner());

	return uniq_name(newName, scene->grid_zone_list());
}
