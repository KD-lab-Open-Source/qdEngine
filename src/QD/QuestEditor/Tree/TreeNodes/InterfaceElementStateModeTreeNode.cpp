#include "stdafx.h"
#include ".\InterfaceElementStateModeTreeNode.h"
#include "ptree_consts.h"
#include "resource.h"

#include "qd_interface_button.h"

bool EditInterfBtnState(qdInterfaceElementState* pstate,
						qdInterfaceElementState::state_mode_t state_mode);

InterfaceElementStateModeTreeNode::
	InterfaceElementStateModeTreeNode(qdInterfaceElementState* object,
								  qdInterfaceElementState::state_mode_t state_mode)
: InterfaceObjectTreeNode(object)
, stateMode_(state_mode)
{
}

InterfaceElementStateModeTreeNode::~InterfaceElementStateModeTreeNode(void)
{
}

qdInterfaceElementState::state_mode_t 
InterfaceElementStateModeTreeNode::getStateMode() const{
	return stateMode_;
}

CString const InterfaceElementStateModeTreeNode::selectName(
	qdInterfaceElementState::state_mode_t stateMode)
{
	CString result;
	switch(stateMode) {
	case qdInterfaceElementState::DEFAULT_MODE:
		result.LoadString(IDS_INTRF_ELE_STATE_MODE_DEFAULT);
		break;
	case qdInterfaceElementState::MOUSE_HOVER_MODE:
		result.LoadString(IDS_INTRF_ELE_STATE_MODE_MOUSE_OVER);
		break;
	case qdInterfaceElementState::EVENT_MODE:
		result.LoadString(IDS_INTRF_ELE_STATE_MODE_EVENT);
		break;
	default:
		ASSERT(0);
	}
	return result;
}

bool InterfaceElementStateModeTreeNode::load(ITreeNodeFactory* factory, 
											 TreeLogic& treeLogic,
											 ITreeNode * parent, 
											 qdNamedObjectBase const* owner,
											 ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
											selectName(getStateMode()),
											parent,
											II_INTERFACE_ELEMENT_NODE, 
											reinterpret_cast<DWORD_PTR>(getObject()), 
											insertAfter);

	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InterfaceElementStateModeTreeNode::getMenuResId() const{
	return IDR_ELEMENT_STATE_MODE;
}

bool InterfaceElementStateModeTreeNode::showProperties(TreeLogic& logic, int activePage)
{
	return EditInterfBtnState(static_cast<qdInterfaceElementState*>(getObject()), 
		stateMode_);
}

bool InterfaceElementStateModeTreeNode::onStateChange(TreeLogic& logic, 
													  DWORD oldState, 
													  DWORD newState)
{
	if (!getObject())
		return false;
	if (!logic.isSceneViewActive())
		return false;

	if (newState&NS_FOCUSED)
	{
		qdInterfaceButton* btn = static_cast<qdInterfaceButton*>(getObject()->owner());
		logic.selectInterfaceScreen(btn->owner()->name());
		static_cast<qdInterfaceElementState*>(getObject())->set_state_mode(stateMode_);
		btn->set_state(static_cast<qdInterfaceElementState*>(getObject()));
//		btn->init();

		return true;
	}
	else if (oldState&NS_FOCUSED)
	{
		static_cast<qdInterfaceElementState*>(getObject())->
			set_state_mode(qdInterfaceElementState::DEFAULT_MODE);
//		static_cast<qdInterfaceButton*>(getObject()->owner())->init();
		qdInterfaceButton* btn = static_cast<qdInterfaceButton*>(getObject()->owner());
		btn->set_state(static_cast<qdInterfaceElementState*>(getObject()));
		btn->init();
		return true;
	}
	return false;
}
