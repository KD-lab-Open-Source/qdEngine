#include "stdafx.h"
#include "resource.h"
#include ".\gridzonestatetreenode.h"

#include "ptree_consts.h"

#include "qd_grid_zone_state.h"
#include "qd_grid_zone.h"

#include <boost/rtl.hpp>

#include "ITriggerView.h"
#include "ptree_te_filters.h"

GridZoneStateTreeNode::GridZoneStateTreeNode(LPCTSTR name, qdNamedObject* object)
: ConditionalObjectTreeNode(object)
, name_(name)
{
}

GridZoneStateTreeNode::~GridZoneStateTreeNode(void)
{
}

LPCTSTR GridZoneStateTreeNode::getName() const{
	return name_;
}

bool GridZoneStateTreeNode::load(ITreeNodeFactory* factory, 
								 TreeLogic& treeLogic, 
								 ITreeNode * parent, 
								 qdNamedObjectBase const* owner,
								 ITreeNode* insertAfter)
{
	CTreeListItem* item = insertToTree(treeLogic, getName(), 
										parent, II_GZONE_STATE_NODE,
										reinterpret_cast<DWORD_PTR>(getObject()), 
										insertAfter);
	if (!item)
		return false;

	qdGridZoneState* const state = static_cast<qdGridZoneState*>(getObject());

	if (state->is_in_triggers())
		setINTriggerFlag(treeLogic,  TIC_SET_NOT_ACTIVE);

	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD GridZoneStateTreeNode::getMenuResId() const{
	return IDR_GRID_ZONE_STATE_NODE;
}

void GridZoneStateTreeNode::modifyMenu(TreeLogic& treeLogic, CMenu* pm) const
{
}

bool GridZoneStateTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	return false;
}

bool GridZoneStateTreeNode::onStateChange(TreeLogic& logic, 
										  DWORD oldState, 
										  DWORD newState)
{
	if (!getObject())
		return false;
	ITreeNode* parent = logic.getParentNode(this);
	if (!logic.isSceneViewActive())
	{
		if (newState&NS_FOCUSED
			&&logic.getShowTriggerElement()
			&&getObject()->is_in_triggers())
		{
			logic.getTriggerView()->ShowTrigger(getObject());
			if (logic.getFilterScheme() == TreeLogic::FS_NONE)
			{
				logic.getTriggerView()->SelectByFilter(tef_by_obj(getObject()), 
					ITriggerView::SA_SELECT,
					ITriggerView::SM_EXCLUSIVE);
			}
		}
	}
	if (parent)
		return parent->onStateChange(logic, oldState, newState);
	return false;
}

bool GridZoneStateTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	setObject(0);
	logic.deleteNode(this);
	return true;
}
