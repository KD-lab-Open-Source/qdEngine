#include "stdafx.h"
#include ".\interfaceelementtreenode.h"

#include "ISceneViewFacade.h"

#include "qd_interface_screen.h"
#include "qd_interface_element.h"

#include "name_op.h"

InterfaceElementTreeNode::InterfaceElementTreeNode(qdInterfaceObjectBase* object)
: InterfaceObjectTreeNode(object)
{
}

InterfaceElementTreeNode::~InterfaceElementTreeNode(void)
{
}

bool InterfaceElementTreeNode::onStateChange(TreeLogic& logic, 
											 DWORD oldState, 
											 DWORD newState)
{
	if (!getObject())
		return false;

	if (!logic.isSceneViewActive())
		return false;

	if (newState&NS_FOCUSED)
	{
		qdInterfaceScreen* const pscr = 
			static_cast<qdInterfaceScreen*>(getObject()->owner());
		pscr->select_element(static_cast<qdInterfaceElement*>(getObject()), true);

		logic.getSceneView()->SetViewState(VS_EDIT_INTERFACE);
		logic.getSceneView()->SetSubTools(SSTE_NONE);
		return !logic.selectInterfaceScreen(getObject()->owner()->name());
	}
	else if (oldState&NS_FOCUSED){
		qdInterfaceScreen* const pscr = 
			static_cast<qdInterfaceScreen*>(getObject()->owner());
		pscr->select_element(static_cast<qdInterfaceElement*>(getObject()), false);
	}
	return false;
}

bool InterfaceElementTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask)) 
		return false;
	if (static_cast<qdInterfaceScreen*>(getObject()->owner())->
		remove_element(static_cast<qdInterfaceElement*>(getObject())))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		delete getObject();
//		setObject(NULL);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

bool InterfaceElementTreeNode::nameTest(TreeLogic& logic, 
										 LPCTSTR newName)
{
	qdInterfaceScreen* const pscr = static_cast<qdInterfaceScreen*>(getObject()->owner());
	return uniq_name(newName, pscr->element_list());
}

//! Возвращает true, если удалось переимновать, false в противном случае
bool InterfaceElementTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;

	qdInterfaceScreen* const pscr = static_cast<qdInterfaceScreen*>(getObject()->owner());
	return pscr->rename_element(static_cast<qdInterfaceElement*>(getObject()), newName);
}

//! Можно ли сменить имя узлу
bool InterfaceElementTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool InterfaceElementTreeNode::onCheckNode(TreeLogic& logic, bool bCheck)
{
	qdInterfaceScreen* const pscr = 
		static_cast<qdInterfaceScreen*>(getObject()->owner());
	if (bCheck)
		pscr->show_element(getObject()->name());
	else
		pscr->hide_element(getObject()->name(), false);
	return true;
}
