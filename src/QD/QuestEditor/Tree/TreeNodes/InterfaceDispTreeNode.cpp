#include "stdafx.h"
#include ".\interfacedisptreenode.h"
#include "resource.h"

#include "Tree/LoadTreeAssist.h"
#include "ptree_consts.h"

#include "qd_interface_dispatcher.h"
#include "qd_interface_screen.h"

#include "ISceneViewFacade.h"

#include <boost/rtl.hpp>

InterfaceDispTreeNode::InterfaceDispTreeNode(qdInterfaceObjectBase* object)
: InterfaceObjectTreeNode(object)
{
}

InterfaceDispTreeNode::~InterfaceDispTreeNode(void)
{
}

bool InterfaceDispTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
								 ITreeNode * parent, qdNamedObjectBase const* owner,
								 ITreeNode* insertAfter)
{
	CString str((LPTSTR)IDS_INTERFACE_NODE);
	CTreeListItem* const item = insertToTree(treeLogic, str, 
												parent, 
												II_INTERFACE_NODE, 
												reinterpret_cast<DWORD_PTR>(getObject()), 
												insertAfter);
	if (!item)
		return false;
	loadScreens(factory, treeLogic);
	return true;
}

bool InterfaceDispTreeNode::loadScreens(ITreeNodeFactory* factory, 
										TreeLogic& treeLogic) 
{
	qdInterfaceDispatcher* const disp = static_cast<qdInterfaceDispatcher*>(getObject());
	boost::rtl::rng::for_each(disp->screen_list(),
		load_tree_assist::InsertInterfaceObject(factory, 
									treeLogic, this, NULL, TN_INSERT_SORT));
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InterfaceDispTreeNode::getMenuResId() const{
	return IDR_INTERFACE_DISPATCHER;
}

bool InterfaceDispTreeNode::onStateChange(TreeLogic& logic, 
										  DWORD oldState, 
										  DWORD newState)
{
	if (!getObject())
		return false;
	if (newState&NS_FOCUSED)
	{
		logic.getSceneView()->SetViewState(VS_EDIT_INTERFACE);
		logic.getSceneView()->SetSubTools(SSTE_NONE);

		return true;
	}
	return false;
}

//! Можно ли сменить имя узлу
bool InterfaceDispTreeNode::canRename(TreeLogic& logic)
{
	return false;
}
