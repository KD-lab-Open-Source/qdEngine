#include "stdafx.h"
#include "resource.h"
#include ".\InterfaceElementStateTreeNode.h"
#include "ptree_consts.h"

#include "qd_interface_element_state.h"
#include "qd_interface_button.h"
#include "InterfaceBtnStateProp.h"

#include "name_op.h"

bool EditInterfaceBtnState(qdInterfaceElementState* pstate);
InterfaceElementStateTreeNode::InterfaceElementStateTreeNode(
											qdInterfaceObjectBase* object)
: InterfaceObjectTreeNode(object)
{
}

InterfaceElementStateTreeNode::~InterfaceElementStateTreeNode(void)
{
}

bool InterfaceElementStateTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
										 ITreeNode * parent, 
										 qdNamedObjectBase const* owner,
										 ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
											getObject()->name(),
											parent,
											II_INTERFACE_ELEMENT_STATE,
											reinterpret_cast<DWORD_PTR>(getObject()), 
											insertAfter);
	loadModes(factory, treeLogic);
	treeLogic.expand(this, TreeLogic::EC_EXPAND);
	return (item != NULL);
}

bool InterfaceElementStateTreeNode::loadModes(ITreeNodeFactory* factory, 
											  TreeLogic& treeLogic) 
{
	qdInterfaceElementState* const state = 
		static_cast<qdInterfaceElementState*>(getObject());
	std::auto_ptr<ITreeNode> p = factory->createInterfaceElementStateModeNode(
									state, qdInterfaceElementState::DEFAULT_MODE);
	if (!p.get()||!p->load(factory, treeLogic, this, state))
		return false;
	p.release();
	p = factory->createInterfaceElementStateModeNode(
									state, qdInterfaceElementState::MOUSE_HOVER_MODE);
	if (!p.get()||!p->load(factory, treeLogic, this, state))
		return false;
	p.release();
	p = factory->createInterfaceElementStateModeNode(
									state, qdInterfaceElementState::EVENT_MODE);
	if (!p.get()||!p->load(factory, treeLogic, this, state))
		return false;
	p.release();
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InterfaceElementStateTreeNode::getMenuResId() const{
	return IDR_INTERFACE_ELE_STATE_NODE;
}

/*
bool InterfaceElementStateTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	qdInterfaceElementState* state = static_cast<qdInterfaceElementState*>(getObject());
	return EditInterfBtnState(state,qdInterfaceElementState::DEFAULT_MODE);
}
*/
bool InterfaceElementStateTreeNode::deleteObject(TreeLogic& logic, 
												 bool ask, 
												 bool statDepends)
{
	if (!preDelete(logic, ask))
		return false;
	qdInterfaceButton* const pbtn = 
		static_cast<qdInterfaceButton*>(getObject()->owner());

	bool res = false;
	int const num_states = pbtn->num_states();
	for(int i = 0; i < num_states; ++i)
	{
		if (getObject() == static_cast<qdInterfaceObjectBase*>(pbtn->get_state(i)))
		{
			res = pbtn->erase_state(i);
			if (res)
				break;
		}
	}
	if (res&&!postDelete(logic))
	{
		logic.setFocusedNode(logic.getPrevVisibleNode(this));
		logic.deleteNode(this);
	}
	return res;
}

bool InterfaceElementStateTreeNode::postDelete(TreeLogic& logic)
{
	qdInterfaceButton* const pbtn = static_cast<qdInterfaceButton*>(getObject()->owner());
	if (pbtn->num_states())
		return false;

	ITreeNode* node = *logic.getObjectNodes(pbtn);
	ITreeNode* prevVisible = logic.getPrevVisibleNode(node);
	if (node->deleteObject(logic, false, false)) 
	{
		logic.setFocusedNode(prevVisible);
//		logic.deleteNode(node);
//		getObject()->set_owner(NULL);
		return true;
	}
	return false;
}


bool InterfaceElementStateTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	const qdInterfaceButton* const pbtn = 
		static_cast<qdInterfaceButton*>(getObject()->owner());
	int const cnt = pbtn->num_states();
	for(int i = 0; i < cnt; ++i)
	{
		const qdInterfaceElementState* const pstate = pbtn->get_state(i);
		if (getObject() != static_cast<const qdInterfaceObjectBase*>(pstate))
		{
			LPCTSTR name = pstate->name();
			if (name&&!_tcsicmp(newName, name))
				return false;
		}
	}
	return true;
}

//! Возвращает true, если удалось переимновать, false в противном случае
bool InterfaceElementStateTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	getObject()->set_name(newName);
	return true;
}

//! Можно ли сменить имя узлу
bool InterfaceElementStateTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool InterfaceElementStateTreeNode::onStateChange(TreeLogic& logic, 
													  DWORD oldState, 
													  DWORD newState)
{
	if (!getObject() || !getObject()->owner() || !getObject()->owner()->owner())
		return false;
	if (!logic.isSceneViewActive())
		return false;
	return !logic.selectInterfaceScreen(getObject()->owner()->owner()->name());
}

bool InterfaceElementStateTreeNode::showProperties(TreeLogic& logic, 
												   int iActive)
{
	return EditInterfaceBtnState(
		static_cast<qdInterfaceElementState*>(getObject()));
}
