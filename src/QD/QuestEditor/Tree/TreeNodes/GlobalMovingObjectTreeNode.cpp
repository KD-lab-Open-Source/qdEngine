#include "stdafx.h"
#include "resource.h"
#include ".\globalmovingobjecttreenode.h"

#include "animobjstates.h"
#include "characterproppage.h"

#include "qd_game_object_moving.h"
#include "ISceneViewFacade.h"
#include "ITriggerView.h"

#include "menu_helpers.h"
#include "Main/Utils/EnumCopyOfGlobal.h"
#include "Main/Utils/UpdateTriggerElementTitles.h"

#include "Main/ObjectTraits.h"
#include "name_op.h"


GlobalMovingObjectTreeNode::GlobalMovingObjectTreeNode(qdNamedObject* object)
:MovingObjectTreeNode(object)
{
}

GlobalMovingObjectTreeNode::~GlobalMovingObjectTreeNode(void)
{
}

//! Возвращает идентификатор ресурса меню для узла
DWORD GlobalMovingObjectTreeNode::getMenuResId() const{
	return IDR_GLOBAL_MOVING_OBJ_NODE;
}

void GlobalMovingObjectTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
//	MovingObjectTreeNode::modifyMenu(treeLogic, pm);
}

bool GlobalMovingObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<CharacterGaitsPage> ptrCPP ( new CharacterGaitsPage);
	ptrCPP->set_data(logic.getGameDispatcher(), (qdGameObjectMoving*)getObject());
	ptrPages[0] = ptrCPP.get();
	
	std::auto_ptr<AnimObjStates> ptrAOS 
		( new AnimObjStates(AnimObjStates::IDD, IDS_PROPPAGE_CAPTION_STOPS));
	ptrAOS->set_data(logic.getGameDispatcher(), (qdGameObjectAnimated*)getObject());
	ptrPages[1] = ptrAOS.get();
/*
	std::auto_ptr<InventoryPPForPers> ptrIPP( new InventoryPPForPers);
	ptrIPP->set_data(m_ptrGameDisp, (qdGameObjectMoving*)ptrObj);
	ptrIPP->set_global_obj();
	ptrPages[2] = ptrIPP.get();
*/				
//				std::auto_ptr<CharBound> ptrCB ( new CharBound);
//				ptrCB->set_data(m_ptrGameDisp, (qdGameObjectMoving*)ptrObj);
//				ptrPages[3] = ptrCB.get();

	return showPropDlg(ptrPages, iActive, IDS_PPCAP_MOVING_OBJ_PROPERTIES);
}

bool GlobalMovingObjectTreeNode::onStateChange(TreeLogic& logic, 
											   DWORD oldState, 
											   DWORD newState)
{
	if (!getObject())
		return false;
	bool const bSelect = newState&NS_FOCUSED;
	if (bSelect) {
		logic.getSceneView()->SelectObj(NULL);
		logic.getSceneView()->SetViewState(VS_NONE);
		return true;
	}

	return false;
}

bool GlobalMovingObjectTreeNode::deleteObject(TreeLogic& logic, 
											  bool ask, 
											  bool statDepends)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	qdGameObjectMoving* object = static_cast<qdGameObjectMoving*>(getObject());

	if (!preDelete(logic, ask, statDepends))
		return false;

	gameDisp->split_global_objects(object);
	if (!removeEmptyCopyOfGlobal(logic, object))
	{
		gameDisp->merge_global_objects(object);
		return false;
	}

	if (gameDisp->remove_global_object(object))
	{
		delete getObject();
//		setObject(NULL);
		logic.setFocusedNode(logic.getPrevVisibleNode(this));
		logic.deleteNode(this);
		return true;
	}
	return false;
}

/*!
	Получает список пустных(без состояний) копий \a pobj в сценах
	Удаляет все эти объекты 
 	\remarks Копия считается пустой, если у нее нет состояний.
	\param pobj -  - глобальный объект, копии которого будем искать и удалять
*/
bool GlobalMovingObjectTreeNode::removeEmptyCopyOfGlobal(TreeLogic& logic,
														 qdGameObject* pobj)
{
	qdGameObjectList list;
	EnumCopyOfGlobal::enumerate(logic.getGameDispatcher(), 
								pobj->name(), 
								(EnumCopyOfGlobal::CHECK_FUN)ObjectTraits::hasNoStates,
								&list);

	if (!list.empty()) {
		boost::rtl::crange<qdGameObjectList const> r(list);
		for(; r; ++r)
		{
			ITreeNode* node = *logic.getObjectNodes(*r);
			if (!static_cast<NamedObjectTreeNode*>(node)->preDelete(logic, false, true))
				return false;
			if (!node->deleteAsSubObject(logic))
				return false;

			logic.deleteNode(node);
		}
	}
	return true;
}

bool GlobalMovingObjectTreeNode::canDrag(TreeLogic& logic){
	return true;
}

DROPEFFECT GlobalMovingObjectTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	if (ITreeNode* sceneNode = logic.isNodeUnderScene(overNode)) {
		qdGameScene* pscene = 
			static_cast<qdGameScene*>(sceneNode->getContent().gameObject);
		if (!pscene->get_object(getObject()->name())) {
			logic.selectDropTarget(sceneNode);
			return DROPEFFECT_COPY;
		}
	}
	logic.selectDropTarget(NULL);
	return DROPEFFECT_NONE;
}

/*!
 	Определяет под какую сцену бросили и добавляет копию этого объекта в сцену
*/

DROPEFFECT GlobalMovingObjectTreeNode::onDropData(TreeLogic& logic, 
											ITreeNode* dropTarget, 
											DROPEFFECT dropEffect)
{
	qdGameScene* pscene = static_cast<qdGameScene*>(dropTarget->getContent().gameObject);

	CString name(getObject()->name());
	DROPEFFECT res = DROPEFFECT_NONE;
	if (logic.getProject()->AddPersonage(pscene, name, false))
		res = DROPEFFECT_COPY;

	logic.selectDropTarget(NULL);
	return res;
}

bool GlobalMovingObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName){
	return uniq_name(newName, logic.getGameDispatcher()->global_object_list());
}
//! Возвращает true, если удалось переимновать, false в противном случае
bool GlobalMovingObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	std::string oldName(getObject()->name());
	bool res = logic.getGameDispatcher()->rename_global_object(
		static_cast<qdGameObject*>(getObject()), newName);
	if (res)
	{
		renameScale(getObject(), oldName.c_str(), newName);
		renameCopies(logic, oldName.c_str(), newName);
		updateTriggerElementTitles(logic.getGameDispatcher(), 
			static_cast<qdGameObjectAnimated const*>(getObject()));
		logic.getTriggerView()->Redraw();
	}
	return res;
}

void GlobalMovingObjectTreeNode::renameCopies(TreeLogic& logic, 
											  LPCTSTR oldName,
											  LPCTSTR newName)
{
	qdGameObjectList objs;
	EnumCopyOfGlobal::enumerate(logic.getGameDispatcher(), 
		oldName,
		(EnumCopyOfGlobal::CHECK_FUN)ObjectTraits::is_obj_moving, 
		&objs);
	boost::rtl::crange<qdGameObjectList const> r(objs);
	for(; r; ++r)
	{
		ITreeNode* node = *logic.getObjectNodes(*r);
		logic.setNodeText(node, newName);
		doRename(logic, *r, newName);
		updateTriggerElementTitles(logic.getGameDispatcher(), 
			static_cast<qdGameObjectAnimated const*>(*r));
	}
}
