#include "stdafx.h"
#include "resource.h"
#include ".\triggerchaintreenode.h"
#include "ptree_consts.h"

#include "qd_trigger_chain.h"
#include "ISceneViewFacade.h"
#include "ITriggerView.h"
#include "TriggerJoinerCondsChecker.h"
#include "name_op.h"

TriggerChainTreeNode::TriggerChainTreeNode(qdNamedObject* object)
: NamedObjectTreeNode(object)
{
}

TriggerChainTreeNode::~TriggerChainTreeNode(void)
{
}

bool TriggerChainTreeNode::load(ITreeNodeFactory* factory, 
								TreeLogic& treeLogic, 
								ITreeNode * parent,
								qdNamedObjectBase const* owner,
								ITreeNode* insertAfter)
{
	CTreeListItem* item = insertToTree(treeLogic, getObject()->name(),
										parent, II_TRIGGER_NODE,
										reinterpret_cast<DWORD_PTR>(getObject()), 
										insertAfter);
	if (!item)
		return false;
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD TriggerChainTreeNode::getMenuResId() const{
	return IDR_TRIGGER_NODE;
}


bool TriggerChainTreeNode::onSelectInTriggerView(TreeLogic& logic, bool bSelect)
{
	if (bSelect) {
		qdTriggerChain* ptrChain = static_cast<qdTriggerChain*>(getObject());

		logic.getTriggerView()->ShowTriggers(&ptrChain, 1);
		CWaitCursor wait;
		logic.setActiveTriggerIcon(static_cast<qdTriggerChain*>(getObject()), TRUE);
		return true;
	}
	return false;
}

bool TriggerChainTreeNode::onSelectInSceneView(TreeLogic& logic, bool bSelect)
{
	CWaitCursor __wait__;

	if (bSelect) {
		logic.getSceneView()->SetViewState(VS_NONE);
		CWaitCursor wait;
		logic.setActiveTriggerIcon(static_cast<qdTriggerChain*>(getObject()), TRUE);

		if (logic.isSceneViewActive())
			logic.setActiveSceneView(false);
		return true;
	}
	return false;
}

bool TriggerChainTreeNode::onStateChange(TreeLogic& logic, 
										 DWORD oldState, 
										 DWORD newState)
{
	if (!getObject())
		return false;
	if (logic.isSceneViewActive())
	{
		if (newState&NS_FOCUSED)
			onSelectInSceneView(logic, true);
		else if (oldState&NS_FOCUSED)
			onSelectInSceneView(logic, false);
	}
	else
	{
		if (newState&NS_FOCUSED)
			onSelectInTriggerView(logic, true);
		else if (oldState&NS_FOCUSED)
			onSelectInTriggerView(logic, false);
	}
	return false;
}

bool TriggerChainTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepnds)
{
	if (!preDelete(logic, ask, statDepnds))
		return false;
	if (logic.getGameDispatcher()->remove_trigger_chain(
		static_cast<qdTriggerChain*>(getObject())))
	{
		postDelete(logic);
		delete getObject();
//		setObject(NULL);
		logic.setFocusedNode(logic.getPrevVisibleNode(this));
		logic.deleteNode(this);
		return true;
	}
	return false;
}

void TriggerChainTreeNode::postDelete(TreeLogic& logic)
{
	logic.setActiveTriggerIcon(static_cast<qdTriggerChain*>(getObject()), FALSE);
}

bool TriggerChainTreeNode::canDrag(TreeLogic& logic){
	return true;
}

/*!
	Цепочку триггеров можно копировать только на другую цепочку
	\param overNode			- Узел, над которым бросили
	\retval DROPEFFECT_COPY - можем скопировать
	\retval DROPEFFECT_MOVE - можем перенести
	\retval DROPEFFECT_NONE - нельзя скопировать
 	
*/

DROPEFFECT TriggerChainTreeNode::canDrop(TreeLogic& logic, ITreeNode* overNode)
{
	//кинуть можем только на другой триггер.
	if (overNode == this
		||logic.getParentNode(overNode) != logic.getDraggedParentNode())
	{
		logic.selectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}
	logic.selectDropTarget(overNode);
	return DROPEFFECT_COPY;
}

/*!
 	Определяет на какую цепочку бросли и делает объединение 2х цепочек
	(MergeTriggers())
*/

DROPEFFECT TriggerChainTreeNode::onDropData(TreeLogic& logic, 
											ITreeNode* dropTarget, 
											DROPEFFECT dropEffect)
{
	qdTriggerChain* ptarget = 
		static_cast<qdTriggerChain*>(dropTarget->getContent().gameObject);
	logic.selectDropTarget(NULL);

	qdTriggerChain * pch = static_cast<qdTriggerChain*>(getObject());
	return MergeTriggers(logic, pch, ptarget) ? DROPEFFECT_COPY : DROPEFFECT_NONE;
}

/*!
 	Проверяет можно ли объединить триггера(TriggerJoinerCondsChecker::doCheck())
	Если можно, то объеденяем триггера (ITriggerView::MergeTriggers())
	\retval TRUE - если триггера объеденены
*/
BOOL TriggerChainTreeNode::MergeTriggers(TreeLogic& logic, 
										 qdTriggerChain const* from, 
										 qdTriggerChain* to)
{
	TriggerJoinerCondsChecker check(logic.getTriggerView(), logic.getViewMgr());
	if (!check.doCheck(from, to))
		return FALSE;
	return logic.getTriggerView()->MergeTriggers(from, to);
}

bool TriggerChainTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool TriggerChainTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return gameDisp->rename_trigger_chain(
		static_cast<qdTriggerChain*>(getObject()), newName);
}

bool TriggerChainTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return uniq_name(newName, gameDisp->trigger_chain_list());
}
